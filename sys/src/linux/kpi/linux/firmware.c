// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: firmware, the files under /lib/firmware (as on
 * Linux; the blobs come from linux-firmware).
 */
#include <linux/firmware.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <kpi/p9.h>

static int
load(const struct firmware **fwp, const char *name, bool warn)
{
	struct firmware *fw;
	char path[256];
	void *data;
	long size;

	*fwp = NULL;
	fw = kzalloc(sizeof(*fw), GFP_KERNEL);
	if (!fw)
		return -ENOMEM;
	snprintf(path, sizeof(path), "/lib/firmware/%s", name);
	if (p9_readfile(path, &data, &size) < 0) {
		if (warn)
			pr_warn("firmware %s: not found\n", path);
		kfree(fw);
		return -ENOENT;
	}
	fw->data = data;
	fw->size = size;
	fw->priv = data;
	*fwp = fw;
	return 0;
}

int request_firmware(const struct firmware **fw, const char *name, struct device *d) { return load(fw, name, true); }
int firmware_request_nowarn(const struct firmware **fw, const char *name, struct device *d) { return load(fw, name, false); }
int request_firmware_direct(const struct firmware **fw, const char *name, struct device *d) { return load(fw, name, false); }

int
request_firmware_into_buf(const struct firmware **fwp, const char *name, struct device *d, void *buf, size_t size)
{
	const struct firmware *fw;
	struct firmware *f;
	int r;

	r = load(&fw, name, true);
	if (r)
		return r;
	if (fw->size > size) {
		release_firmware(fw);
		return -EFBIG;
	}
	memcpy(buf, fw->data, fw->size);
	f = (struct firmware *)fw;
	p9_free(f->priv);
	f->priv = NULL;
	f->data = buf;
	*fwp = f;
	return 0;
}

void
release_firmware(const struct firmware *fw)
{
	if (!fw)
		return;
	if (fw->priv)
		p9_free(fw->priv);
	kfree(fw);
}

struct nowait {
	struct work_struct	work;
	const char	*name;
	struct device	*dev;
	void	*ctx;
	void	(*cont)(const struct firmware *fw, void *ctx);
};

static void
nowait_work(struct work_struct *w)
{
	struct nowait *n = container_of(w, struct nowait, work);
	const struct firmware *fw;

	load(&fw, n->name, true);
	n->cont(fw, n->ctx);
	kfree_const(n->name);
	kfree(n);
}

int
request_firmware_nowait(struct module *m, bool uevent, const char *name, struct device *d, gfp_t gfp, void *ctx,
	void (*cont)(const struct firmware *fw, void *ctx))
{
	struct nowait *n = kzalloc(sizeof(*n), gfp);

	if (!n)
		return -ENOMEM;
	n->name = kstrdup_const(name, gfp);
	n->dev = d;
	n->ctx = ctx;
	n->cont = cont;
	INIT_WORK(&n->work, nowait_work);
	schedule_work(&n->work);
	return 0;
}
