#include "ext4_config.h"
#include "ext4.h"
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include <bio.h>
#include "ext4_inode.h"
#include "ext4_super.h"
#include "group.h"
#include "common.h"

/*
 * qid.vers changes with every change of a file, as with other file
 * servers: the kernel keeps the text of programs by qid, and a program
 * rebuilt in place (the linker writes it anew, an inode number comes
 * back) ran the old text with the new data. The inode's generation
 * stays the same on writes (checksums use it); its mtime goes in, and
 * a count of the changes since ext4srv started, per inode.
 */
enum { Nvtab = 4096 };
static struct { u32int ino, n; } vtab[Nvtab];

static u32int *
vslot(u32int ino)
{
	u32int i, k;

	for(k = 0, i = ino % Nvtab; k < Nvtab; k++, i = (i+1) % Nvtab)
		if(vtab[i].ino == ino || vtab[i].ino == 0){
			vtab[i].ino = ino;
			return &vtab[i].n;
		}
	return &vtab[ino % Nvtab].n;	/* full: shared, still counts */
}

static void
vbump(u32int ino)
{
	(*vslot(ino))++;
}

static u32int
qvers(u32int ino, struct ext4_inode *inode)
{
	return ext4_inode_get_generation(inode) + ext4_inode_get_modif_time(inode) +
		0x9e3779b9U * *vslot(ino);
}

/*
 * exclusive-use files (DMEXCL), as open(5) has them: one open at a
 * time, another open fails with "file is locked". Kept in the inode as
 * EXT4_INODE_FLAG_UNRM (chattr's u, not used by Linux), as DMTMP is
 * NODUMP. The opens are those of this ext4srv: a list of the inodes
 * the fids have open, an entry goes when its fid is clunked. A lock is
 * not broken after a time (open(5) allows it): a 9P connection that
 * goes clunks its fids.
 */
#define EXCLFLAG EXT4_INODE_FLAG_UNRM
static char Elocked[] = "file is locked";

typedef struct Excl Excl;
struct Excl {
	Part *p;
	u32int ino;
	Excl *next;
};
static Excl *excls;

static int
exclopen(Part *p, u32int ino)
{
	Excl *e;

	for(e = excls; e != nil; e = e->next)
		if(e->p == p && e->ino == ino){
			werrstr(Elocked);
			return -1;
		}
	e = emalloc9p(sizeof(*e));
	e->p = p;
	e->ino = ino;
	e->next = excls;
	excls = e;
	return 0;
}

static void
exclclose(Part *p, u32int ino)
{
	Excl **l, *e;

	for(l = &excls; (e = *l) != nil; l = &e->next)
		if(e->p == p && e->ino == ino){
			*l = e->next;
			free(e);
			return;
		}
}

#define MIN(a,b) ((a)<(b)?(a):(b))

int mainstacksize = 65536;

typedef struct Aux Aux;

struct Aux {
	Part *p;
	u32int uid;
	char *path;
	int doff;
	union {
		ext4_file *file;
		ext4_dir *dir;
	};
	int type;
	bool rclose;
	u32int exclino;	/* the DMEXCL file this fid has open */
};

enum {
	Adir,
	Afile,

	Root = 0,
};

static Opts opts = {
	.label = "",
};
static u8int zero[65536];
static char *srvname;
static char *device;
static Part *devpart;

static int
haveperm(Aux *a, int p, struct ext4_inode *inodeout)
{
	struct ext4_mountpoint *mp;
	struct ext4_inode inode;
	u32int ino, id;
	int m, fm;
	Group *g;

	switch(p & 3){
	case OREAD:
		p = AREAD;	
		break;
	case ORCLOSE:
	case OWRITE:
		p = AWRITE;
		break;
	case ORDWR:
		p = AREAD|AWRITE;
		break;
	case OEXEC:
		p = AEXEC;	
		break;
	default:
		return 0;
	}
	if(p & OTRUNC)
		p |= AWRITE;

	mp = &a->p->mp;
	if(ext4_raw_inode_fill(mp, a->path, &ino, &inode) != 0){
		werrstr("%s: %r", a->path);
		return -1;
	}

	if(inodeout != nil)
		memmove(inodeout, &inode, sizeof(inode));

	fm = ext4_inode_get_mode(a->p->sb, &inode);

	/* other */
	m = fm & 7;
	if((p & m) == p)
		return 1;

	/* owner */
	id = ext4_inode_get_uid(&inode);
	if(a->uid == Root || ((g = findgroupid(&a->p->groups, id)) != nil && ingroup(g, a->uid))){
		m |= (fm >> 6) & 7;
		if((p & m) == p)
			return 1;
	}

	/* group */
	id = ext4_inode_get_gid(&inode);
	if(a->uid == Root || ((g = findgroupid(&a->p->groups, id)) != nil && ingroup(g, a->uid))){
		m |= (fm >> 3) & 7;
		if((p & m) == p)
			return 1;
	}

	return 0;
}

static void
rattach(Req *r)
{
	Aux *a;

	if((a = calloc(1, sizeof(*a))) == nil)
		respond(r, "memory");
	if(r->ifcall.aname && *r->ifcall.aname){
		if((a->p = openpart(r->ifcall.aname, &opts)) == nil){
			free(a);
			responderror(r);
			return;
		}
	}else if((a->p = devpart) == nil){
		respond(r, "no file specified");
		return;
	}

	if(opts.asroot || findgroup(&a->p->groups, r->ifcall.uname, &a->uid) == nil)
		a->uid = Root;

	incref(a->p);
	a->type = Adir;
	a->path = estrdup9p("");
	r->ofcall.qid = a->p->qidmask;
	r->fid->qid = a->p->qidmask;
	r->fid->aux = a;
	respond(r, nil);
}

static u32int
toext4mode(u32int mode, u32int perm, int creat)
{
	u32int e;

	e = 0;
	mode &= ~OCEXEC;

	if(mode & OTRUNC)
		e |= O_TRUNC;

	mode &= 3;
	if(mode == OWRITE)
		e |= O_WRONLY;
	else if(mode == ORDWR)
		e |= O_RDWR;

	if(creat)
		e |= O_CREAT;

	if(perm & DMEXCL)
		e |= O_EXCL;
	if(perm & DMAPPEND)
		e |= O_APPEND;

	return e;
}

static void
ropen(Req *r)
{
	struct ext4_mountpoint *mp;
	struct ext4_inode inode;
	u32int ino;
	char *path;
	int res;
	Aux *a;

	a = r->fid->aux;
	mp = &a->p->mp;
	switch(a->type){
	case Adir:
		if(r->ifcall.mode != OREAD || !haveperm(a, r->ifcall.mode, nil)){
			respond(r, Eperm);
			return;
		}
		if(a->dir != nil){
			respond(r, "double open");
			return;
		}
		if((a->dir = malloc(sizeof(*a->dir))) == nil)
			goto Nomem;
		if((path = estrdup9p(a->path)) == nil){
			free(a->dir);
			a->dir = nil;
			goto Nomem;
		}
		res = ext4_dir_open(mp, a->dir, path);
		free(path);
		if(res != 0){
			free(a->dir);
			a->dir = nil;
			responderror(r);
			return;
		}
		break;

	case Afile:
		if(!haveperm(a, r->ifcall.mode, nil)){
			respond(r, Eperm);
			return;
		}
		if(a->file != nil){
			respond(r, "double open");
			return;
		}
		if(ext4_raw_inode_fill(mp, a->path, &ino, &inode) == 0 &&
		    (ext4_inode_get_flags(&inode) & EXCLFLAG) != 0){
			if(exclopen(a->p, ino) < 0){
				responderror(r);
				return;
			}
			a->exclino = ino;
		}
		if((a->file = malloc(sizeof(*a->file))) == nil)
			goto Nomem;
		if((path = estrdup9p(a->path)) == nil){
			free(a->file);
			a->file = nil;
			goto Nomem;
		}
		res = ext4_fopen2(mp, a->file, path, toext4mode(r->ifcall.mode, 0, 0));
		free(path);
		if(res != 0){
			free(a->file);
			a->file = nil;
			if(a->exclino != 0){
				exclclose(a->p, a->exclino);
				a->exclino = 0;
			}
			responderror(r);
			return;
		}
		a->rclose = (r->ifcall.mode & ORCLOSE) != 0;
		if(r->ifcall.mode & OTRUNC)
			vbump(a->file->inode);
		break;

Nomem:
		if(a->type == Afile && a->exclino != 0){
			exclclose(a->p, a->exclino);
			a->exclino = 0;
		}
		respond(r, "memory");
		return;
	}

	r->ofcall.iounit = 0;

	respond(r, nil);
}

static void
rcreate(Req *r)
{
	u32int perm, dirperm, t, iflags;
	struct ext4_mountpoint *mp;
	struct ext4_inode inode;
	int mkdir, isroot;
	long tm;
	char *s;
	Aux *a;

	a = r->fid->aux;
	mp = &a->p->mp;
	s = nil;

	if(a->file != nil || a->dir != nil){
		werrstr("double create");
		goto error;
	}
	if(!haveperm(a, OWRITE, &inode)){
		werrstr(Eperm);
		goto error;
	}

	/* first make sure this is a directory */
	isroot = r->fid->qid.path == a->p->qidmask.path;
	if(!isroot){
		t = ext4_inode_type(a->p->sb, &inode);
		if(t != EXT4_INODE_MODE_DIRECTORY){
			werrstr("create in non-directory");
			goto error;
		}
	}
	ext4_mode_get(mp, a->path, &dirperm);

	/* check if the entry already exists */
	s = isroot ? estrdup9p(r->ifcall.name) : smprint("%s/%s", a->path, r->ifcall.name);
	if(ext4_inode_exist(mp, s, EXT4_DE_UNKNOWN) == 0){
		werrstr("file already exists");
		goto error;
	}

	mkdir = r->ifcall.perm & DMDIR;
	perm = mkdir ? 0666 : 0777;
	perm = r->ifcall.perm & (~perm | (dirperm & perm));

	if(mkdir){
		a->type = Adir;
		if(ext4_dir_mk(mp, s) < 0)
			goto error;
		a->dir = emalloc9p(sizeof(*a->dir));
		if(ext4_dir_open(mp, a->dir, s) < 0){
			free(a->dir);
			a->dir = nil;
			goto ext4errorrm;
		}
	}else{
		a->type = Afile;
		a->file = emalloc9p(sizeof(*a->file));
		if(ext4_fopen2(mp, a->file, s, toext4mode(r->ifcall.mode, perm, 1)) < 0){
			free(a->file);
			a->file = nil;
			goto error;
		}
	}

	iflags = 0;
	if(r->ifcall.perm & DMAPPEND)
		iflags |= EXT4_INODE_FLAG_APPEND;
	if(r->ifcall.perm & DMTMP)
		iflags |= EXT4_INODE_FLAG_NODUMP;
	if(!mkdir && (r->ifcall.perm & DMEXCL) != 0){
		iflags |= EXCLFLAG;
		if(exclopen(a->p, a->file->inode) < 0)
			goto ext4errorrm;
		a->exclino = a->file->inode;
	}
	if(ext4_mode_set(mp, s, perm, iflags) < 0)
		goto ext4errorrm;
	ext4_owner_set(mp, s, a->uid, a->uid);
	tm = time(nil);
	ext4_atime_set(mp, s, tm);
	ext4_mtime_set(mp, s, tm);
	ext4_ctime_set(mp, s, tm);

	r->fid->qid.path = a->p->qidmask.path | a->file->inode;
	vbump(a->file->inode);
	r->fid->qid.vers = tm + 0x9e3779b9U * *vslot(a->file->inode);
	r->fid->qid.type = 0;
	if(iflags & EXCLFLAG)
		r->fid->qid.type |= QTEXCL;
	r->ofcall.qid = r->fid->qid;

	free(a->path);
	a->path = s;
	r->ofcall.iounit = 0;
	respond(r, nil);
	return;

ext4errorrm:
	if(mkdir)
		ext4_dir_rm(mp, s);
	else
		ext4_fremove(mp, s);
error:
	free(s);
	responderror(r);
}

static int
dirfill(Dir *dir, Aux *a, char *path)
{
	struct ext4_mountpoint *mp;
	u32int t, ino, id, iflags;
	struct ext4_inode inode;
	char tmp[16];
	Group *g;
	char *s;
	int r;

	memset(dir, 0, sizeof(*dir));
	mp = &a->p->mp;

	if(path == nil){
		r = ext4_raw_inode_fill(mp, a->path, &ino, &inode);
	}else{
		s = smprint("%s%s%s", a->path, *a->path ? "/" : "", path);
		r = ext4_raw_inode_fill(mp, s, &ino, &inode);
		free(s);
	}
	if(r < 0){
		werrstr("inode: %s: %r", path ? path : "/");
		return -1;
	}

	dir->mode = ext4_inode_get_mode(a->p->sb, &inode) & 0777;
	dir->qid.path = a->p->qidmask.path | ino;
	dir->qid.vers = qvers(ino, &inode);
	dir->qid.type = 0;
	t = ext4_inode_type(a->p->sb, &inode);
	if(t == EXT4_INODE_MODE_DIRECTORY){
		dir->qid.type |= QTDIR;
		dir->mode |= DMDIR;
	}else
		dir->length = ext4_inode_get_size(a->p->sb, &inode);

	iflags = ext4_inode_get_flags(&inode);
	if(iflags & EXT4_INODE_FLAG_APPEND){
		dir->qid.type |= QTAPPEND;
		dir->mode |= DMAPPEND;
	}
	if(iflags & EXT4_INODE_FLAG_NODUMP){
		dir->qid.type |= QTTMP;
		dir->mode |= DMTMP;
	}
	if(t != EXT4_INODE_MODE_DIRECTORY && (iflags & EXCLFLAG) != 0){
		dir->qid.type |= QTEXCL;
		dir->mode |= DMEXCL;
	}

	if(path != nil && (s = strrchr(path, '/')) != nil)
		path = s+1;
	dir->name = estrdup9p(path != nil ? path : "");
	dir->atime = ext4_inode_get_access_time(&inode);
	dir->mtime = ext4_inode_get_modif_time(&inode);

	sprint(tmp, "%ud", id = ext4_inode_get_uid(&inode));
	dir->uid = estrdup9p((g = findgroupid(&a->p->groups, id)) != nil ? g->name : tmp);

	sprint(tmp, "%ud", id = ext4_inode_get_gid(&inode));
	dir->gid = estrdup9p((g = findgroupid(&a->p->groups, id)) != nil ? g->name : tmp);

	return 0;
}

static int
dirgen(int n, Dir *dir, void *aux)
{
	const ext4_direntry *e;
	Aux *a;

	a = aux;
	if(n == 0 || n != a->doff){
		ext4_dir_entry_rewind(a->dir);
		a->doff = 0;
	}

	for(;;){
		do{
			if((e = ext4_dir_entry_next(a->dir)) == nil)
				return -1;
		}while(e->name == nil || strcmp((char*)e->name, ".") == 0 || strcmp((char*)e->name, "..") == 0);

		if(e->inode_type != EXT4_DE_REG_FILE && e->inode_type != EXT4_DE_DIR)
			continue;

		if(a->doff++ != n)
			continue;

		if(dirfill(dir, a, (char*)e->name) == 0)
			return 0;

		a->doff--;
	}
}

static void
rread(Req *r)
{
	usize n;
	Aux *a;

	a = r->fid->aux;
	if(a->type == Adir && a->dir != nil){
		dirread9p(r, dirgen, a);
	}else if(a->type == Afile && a->file != nil){
		if(ext4_fseek(a->file, r->ifcall.offset, 0) != 0)
			n = 0;
		else if(ext4_fread(a->file, r->ofcall.data, r->ifcall.count, &n) < 0){
			responderror(r);
			return;
		}

		r->ofcall.count = n;
	}

	respond(r, nil);
}

/*
 * a file's length from its inode: an ext4_file keeps the one it had
 * when it was opened, a wstat or write through another fid may have
 * changed it since.
 */
static u64int
filesize(ext4_file *f)
{
	struct ext4_inode_ref ref;

	if(ext4_fs_get_inode_ref(&f->mp->fs, f->inode, &ref) == 0){
		f->fsize = ext4_inode_get_size(&f->mp->fs.sb, ref.inode);
		ext4_fs_put_inode_ref(&ref);
	}
	return f->fsize;
}

static void
rwrite(Req *r)
{
	usize n, sz;
	Aux *a;

	a = r->fid->aux;
	if(a->type == Afile){
		filesize(a->file);
		while(ext4_fsize(a->file) < r->ifcall.offset){
			ext4_fseek(a->file, 0, 2);
			sz = MIN(r->ifcall.offset-ext4_fsize(a->file), sizeof(zero));
			if(ext4_fwrite(a->file, zero, sz, &n) < 0)
				goto error;
		}
		if((a->file->flags & O_APPEND) == 0){
			if(ext4_fseek(a->file, r->ifcall.offset, 0) < 0)
				goto error;
			if(ext4_ftell(a->file) != r->ifcall.offset){
				werrstr("could not seek");
				goto error;
			}
		}
		if(ext4_fwrite(a->file, r->ifcall.data, r->ifcall.count, &n) < 0)
			goto error;
		vbump(a->file->inode);

		assert(r->ifcall.count >= n);
		r->ofcall.count = n;
		respond(r, nil);
		return;
	}else{
		werrstr(Eperm);
	}

error:
	responderror(r);
}

static void
rremove(Req *r)
{
	struct ext4_mountpoint *mp;
	struct ext4_inode inode;
	const ext4_direntry *e;
	u32int ino, t, empty;
	ext4_dir dir;
	Group *g;
	Aux *a;

	a = r->fid->aux;
	mp = &a->p->mp;

	/* do not resolve links here as most likely it's JUST the link we want to remove */
	if(ext4_raw_inode_fill(mp, a->path, &ino, &inode) < 0)
		goto error;

	if(a->uid == Root || ((g = findgroupid(&a->p->groups, ext4_inode_get_uid(&inode))) != nil && g->id == a->uid)){
		t = ext4_inode_type(a->p->sb, &inode);
		if(t == EXT4_INODE_MODE_DIRECTORY && ext4_dir_open(mp, &dir, a->path) == 0){
			for(empty = 1; empty;){
				if((e = ext4_dir_entry_next(&dir)) == nil)
					break;
				empty = e->name == nil || strcmp((char*)e->name, ".") == 0 || strcmp((char*)e->name, "..") == 0;
			}
			ext4_dir_close(&dir);
			if(!empty){
				werrstr("directory not empty");
				goto error;
			}else if(ext4_dir_rm(mp, a->path) < 0)
				goto error;
		}else if(ext4_fremove(mp, a->path) < 0)
			goto error;
	}else{
		werrstr(Eperm);
		goto error;
	}

	respond(r, nil);
	return;

error:
	responderror(r);
}

static void
rstat(Req *r)
{
	Aux *a;

	a = r->fid->aux;
	if(dirfill(&r->d, a, nil) != 0)
		responderror(r);
	else
		respond(r, nil);
}

static void
rwstat(Req *r)
{
	int res, isdir, wrperm, isowner, n;
	usize wn;
	u32int bs;
	struct ext4_mountpoint *mp;
	struct ext4_inode inode;
	u32int uid, gid, iflags;
	ext4_file f;
	Aux *a, o;
	Group *g;
	char *s;

	a = r->fid->aux;
	mp = &a->p->mp;

	/* can't do anything to root, can't change the owner */
	if(*a->path == 0 || (r->d.uid != nil && r->d.uid[0] != 0)){
		werrstr(Eperm);
		goto error;
	}

	wrperm = haveperm(a, OWRITE, &inode);
	uid = ext4_inode_get_uid(&inode);
	isowner = a->uid == Root || a->uid == uid;

	/* permission to truncate */
	isdir = ext4_inode_type(a->p->sb, &inode) == EXT4_INODE_MODE_DIRECTORY;
	if(r->d.length >= 0 && (!wrperm || isdir)){
		werrstr(Eperm);
		goto error;
	}

	/* permission to change mode */
	if(r->d.mode != ~0){
		/* has to be owner and can't change dir bit */
		if(!isowner || (!!isdir != !!(r->d.mode & DMDIR))){
			werrstr(Eperm);
			goto error;
		}
	}

	/* permission to change mtime */
	if(r->d.mtime != ~0 && !isowner){
		werrstr(Eperm);
		goto error;
	}

	/* permission to change gid */
	if(r->d.gid != nil && r->d.gid[0] != 0){
		/* has to be the owner, group has to exist, must be in that group */
		if(!isowner || (g = findgroup(&a->p->groups, r->d.gid, &gid)) == nil || !ingroup(g, a->uid)){
			werrstr(Eperm);
			goto error;
		}
	}

	/* check for permission to rename and do the rename */
	if(r->d.name != nil && r->d.name[0] != 0){
		/* check parent write permission */
		o = *a;
		o.path = a->path;
		if(!haveperm(&o, OWRITE, nil)){
			werrstr(Eperm);
			goto error;
		}

		if((s = strrchr(a->path, '/')) != nil){
			n = s - a->path;
			s = emalloc9p(n + 1 + strlen(r->d.name) + 1);
			memmove(s, a->path, n);
			s[n++] = '/';
			strcpy(s+n, r->d.name);
		}else{
			s = estrdup9p(r->d.name);
		}

		if(ext4_frename(mp, a->path, s) < 0){
			free(s);
			goto error;
		}
		free(a->path);
		a->path = s;
	}

	/* truncate */
	if(r->d.length >= 0 && ext4_inode_can_truncate(a->p->sb, &inode)){
		if(ext4_fopen2(mp, &f, a->path, toext4mode(OWRITE, 0, 0)) < 0)
			goto error;
		/*
		 * a longer length extends the file with zeros (stat(5)),
		 * as rwrite does past the end; ext4_ftruncate only
		 * shortens.
		 */
		res = 0;
		while(res == 0 && ext4_fsize(&f) < r->d.length){
			ext4_fseek(&f, 0, 2);
			wn = MIN(r->d.length - ext4_fsize(&f), sizeof(zero));
			res = ext4_fwrite(&f, zero, wn, &wn);
		}
		/*
		 * shorter: zero the rest of the new last block first,
		 * ext4_ftruncate frees the blocks after it but leaves its
		 * old bytes, which a later extension showed again.
		 */
		bs = ext4_sb_get_block_size(&mp->fs.sb);
		if(res == 0 && r->d.length < ext4_fsize(&f) && r->d.length % bs != 0){
			wn = MIN(ext4_fsize(&f) - r->d.length, bs - r->d.length % bs);
			ext4_fseek(&f, r->d.length, 0);
			res = ext4_fwrite(&f, zero, wn, &wn);
		}
		if(res == 0)
			res = ext4_ftruncate(&f, r->d.length);
		vbump(f.inode);
		ext4_fclose(&f);
		if(res != 0)
			goto error;
	}

	/* mode */
	if(r->d.mode != ~0){
		iflags = 0;
		if(r->d.mode & DMAPPEND)
			iflags |= EXT4_INODE_FLAG_APPEND;
		if(r->d.mode & DMTMP)
			iflags |= EXT4_INODE_FLAG_NODUMP;
		if(r->d.mode & DMEXCL)
			iflags |= EXCLFLAG;
		if(ext4_mode_set(mp, a->path, r->d.mode & 0777, iflags) < 0)
			goto error;
	}

	/* mtime */
	if(r->d.mtime != ~0 && ext4_mtime_set(mp, a->path, r->d.mtime) < 0)
		goto error;

	/* gid */
	if(r->d.gid != nil && r->d.gid[0] != 0 && ext4_owner_set(mp, a->path, uid, gid) < 0)
		goto error;

	/* inode changed - update the time */
	ext4_ctime_set(mp, a->path, time(nil));

	respond(r, nil);
	return;

error:
	responderror(r);
}

static char *
rwalk1(Fid *fid, char *name, Qid *qid)
{
	struct ext4_mountpoint *mp;
	static char errbuf[ERRMAX];
	struct ext4_inode inode;
	u32int ino, t, iflags;
	Aux *a, dir;
	int isroot;
	char *s;

	a = fid->aux;
	mp = &a->p->mp;
	isroot = fid->qid.path == a->p->qidmask.path;
	s = nil;

	if(isroot && strcmp(name, "..") == 0){
		*qid = a->p->qidmask;
		fid->qid = a->p->qidmask;
		return nil;
	}

	if(ext4_raw_inode_fill(mp, a->path, &ino, &inode) < 0){
err:
		free(s);
		rerrstr(errbuf, sizeof(errbuf));
		return errbuf;
	}

	t = ext4_inode_type(a->p->sb, &inode);
	if(t != EXT4_INODE_MODE_DIRECTORY)
		return "not a directory";
	dir = *a;
	dir.path = a->path;
	if(!haveperm(&dir, OEXEC, nil))
		return Eperm;

	s = isroot ? estrdup9p(name) : smprint("%s/%s", a->path, name);
	cleanname(s);
	if(s[0] == '.' && s[1] == 0) /* special case - root */
		*s = 0;
	if(ext4_raw_inode_fill(mp, s, &ino, &inode) < 0)
		goto err;
	t = ext4_inode_type(a->p->sb, &inode);
	if(t != EXT4_INODE_MODE_FILE && t != EXT4_INODE_MODE_DIRECTORY){
		free(s);
		return "directory entry not found";
	}

	qid->type = 0;
	qid->path = a->p->qidmask.path | ino;
	qid->vers = qvers(ino, &inode);
	if(t == EXT4_INODE_MODE_DIRECTORY){
		qid->type |= QTDIR;
		a->type = Adir;
	}else
		a->type = Afile;
	iflags = ext4_inode_get_flags(&inode);
	if(iflags & EXT4_INODE_FLAG_APPEND)
		qid->type |= QTAPPEND;
	if(iflags & EXT4_INODE_FLAG_NODUMP)
		qid->type |= QTTMP;
	if(t != EXT4_INODE_MODE_DIRECTORY && (iflags & EXCLFLAG) != 0)
		qid->type |= QTEXCL;
	free(a->path);
	a->path = s;
	fid->qid = *qid;

	return nil;
}

static char *
rclone(Fid *oldfid, Fid *newfid)
{
	Aux *a, *c;

	a = oldfid->aux;

	if((c = malloc(sizeof(*c))) == nil)
		return "memory";
	memmove(c, a, sizeof(*c));
	c->path = estrdup9p(a->path);
	c->file = nil;
	c->dir = nil;
	c->rclose = false;
	c->exclino = 0;

	incref(c->p);
	newfid->aux = c;

	return nil;
}

static void
rdestroyfid(Fid *fid)
{
	Aux *a;

	a = fid->aux;
	if(a == nil)
		return;
	fid->aux = nil;

	if(a->type == Adir && a->dir != nil){
		ext4_dir_close(a->dir);
		free(a->dir);
	}else if(a->type == Afile && a->file != nil){
		ext4_fclose(a->file);
		free(a->file);
		if(a->rclose)
			ext4_fremove(&a->p->mp, a->path);
	}
	if(a->exclino != 0)
		exclclose(a->p, a->exclino);

	if(decref(a->p) < 1)
		closepart(a->p);
	free(a->path);
	free(a);
}

static int
note(void *, char *s)
{
	if(strncmp(s, "sys:", 4) != 0){
		closeallparts();
		close(0);
		return 1;
	}

	return 0;
}

static void
cmdsrv(void *)
{
	char s[32], *c, *a[4];
	int f, p[2], n;
	Biobuf b;

	if(pipe(p) < 0)
		sysfatal("%r");
	snprint(s, sizeof(s), "#s/%s.cmd", srvname);
	if((f = create(s, ORCLOSE|OWRITE, 0660)) < 0){
		remove(s);
		if((f = create(s, ORCLOSE|OWRITE, 0660)) < 0)
			sysfatal("%r");
	}
	if(fprint(f, "%d", p[0]) < 1)
		sysfatal("srv write");

	dup(p[1], 0);
	close(p[1]);
	close(p[0]);

	Binit(&b, 0, OREAD);
	for(; (c = Brdstr(&b, '\n', 1)) != nil; free(c)){
		if((n = tokenize(c, a, nelem(a))) < 1)
			continue;
		USED(n);
		if(strcmp(a[0], "stats") == 0 || strcmp(a[0], "df") == 0){
			statallparts();
		}else if(strcmp(a[0], "halt") == 0){
			closeallparts();
			close(0);
			threadexitsall(nil);
		}else if(strcmp(a[0], "sync") == 0){
			syncallparts();
		}else{
			print("unknown command: %s\n", a[0]);
		}
	}
}

static void
rstart(Srv *)
{
	threadnotify(note, 1);
	proccreate(cmdsrv, nil, mainstacksize);
}

static void
rend(Srv *)
{
	closeallparts();
	close(0);
	threadexitsall(nil);
}

static Srv fs = {
	.attach = rattach,
	.open = ropen,
	.create = rcreate,
	.read = rread,
	.write = rwrite,
	.remove = rremove,
	.stat = rstat,
	.wstat = rwstat,
	.walk1 = rwalk1,
	.clone = rclone,
	.destroyfid = rdestroyfid,
	.start = rstart,
	.end = rend,
};

static void
usage(void)
{
	fprint(2,
		"usage: %s [-Ss] [-f file] [-g groupfile] [-n srvname] [-r (2|3|4)]"
		" [-b blksize] [-I inodesize] [-L label]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char *gr;
	vlong sz;
	int f, stdio;

	rfork(RFNOTEG);

	stdio = 0;
	device = nil;
	ARGBEGIN{
	case 'D':
		chatty9p++;
		break;
	case 'd':
		ext4_dmask_set(strtoul(EARGF(usage()), nil, 0));
		break;
	case 'f':
		if(device != nil)
			usage();
		device = EARGF(usage());
		break;
	case 'g':
		gr = EARGF(usage());
		if((f = open(gr, OREAD)) < 0)
			sysfatal("%r");
		sz = seek(f, 0, 2);
		if(sz < 0)
			sysfatal("%s: invalid group file", gr);
		if((opts.group = malloc(sz+1)) == nil)
			sysfatal("memory");
		seek(f, 0, 0);
		if(readn(f, opts.group, sz) != sz)
			sysfatal("%s: read failed", gr);
		close(f);
		opts.group[sz] = 0;
		break;
	case 'n':
		if(stdio != 0)
			usage();
		srvname = EARGF(usage());
		break;
	case 's':
		stdio = 1;
		if(srvname != nil)
			usage();
		break;
	case 'S':
		opts.asroot = 1;
		break;

	case 'b':
		opts.blksz = atoi(EARGF(usage()));
		if(opts.blksz != 1024 && opts.blksz != 2048 && opts.blksz != 4096)
			usage();
		break;
	case 'I':
		opts.inodesz = atoi(EARGF(usage()));
		if(opts.inodesz < 128 || ((opts.inodesz-1) & opts.inodesz) != 0)
			usage();
		break;
	case 'L':
		opts.label = EARGF(usage());
		break;
	case 'r':
		if(opts.ream > 0)
			usage();
		opts.ream = atoi(EARGF(usage()));
		if(opts.ream < 2 || opts.ream > 4)
			usage();
		break;

	default:
		usage();
	}ARGEND

	if(argc != 0)
		usage();

	if(device == nil && opts.ream > 1)
		usage();
	if(device != nil && (devpart = openpart(device, &opts)) == nil)
		sysfatal("%r");

	if(stdio){
		fs.infd = 0;
		fs.outfd = 1;
		threadsrv(&fs);
	}else{
		if(srvname == nil)
			srvname = "ext4";
		threadpostsrv(&fs, srvname);
	}
	threadexits(nil);
}
