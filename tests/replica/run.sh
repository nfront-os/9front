#!/bin/bash
# replica: updates over HTTP. The host publishes versions of a small tree
# (scripts/guest/proto.py -l, scripts/replica/publish.py: signed index,
# objects by SHA-256, replica's log) and serves them over HTTP; a guest
# (scripts/guest/boot-cli.sh) pulls them with replica/pull through
# replica/httpfs (tests/replica/guest.rc, cfg):
#   A  a new client takes version 1, then 2 (added, changed, removed,
#      a mode changed, an empty file, a name with a blank)
#   D  version 3's object altered on the server: refused, the file stays
#   E  an index not as signed: refused, nothing changes
#   B  a system installed from version 1's CD (the stage with the client db
#      publish.py -c writes) takes version 2 without a conflict
#   C  the same with a file its user changed: kept, reported
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
for f in $objtype/bin/replica/httpfs $objtype/bin/webfs $KBIN; do
	[ -f "$ROOT/$f" ] || { skip "replica" "no $f, run $BUILDHOW"; done_tests; exit; }
done
command -v openssl >/dev/null || { skip "replica" "no openssl"; done_tests; exit; }
PUB="python3 $ROOT/scripts/replica/publish.py"
W=$TMPD/w
X=$TMPD/extra/tests/replica
mkdir -p "$W" "$X"
port=$(python3 -c 'import socket; s=socket.socket(); s.bind(("127.0.0.1", 0)); print(s.getsockname()[1])')

# version n of the source tree: the stage proto.py makes of it, with its listing
src=$W/src
cat > "$W/proto" <<'EOF'
mode=ug+rw
mode=o-w
uid=sys
gid=sys
bin	d775
	a	775 glenda
	b	664
lib	d775
	+
EOF
stage(){ rm -rf "$W/stage$1"; python3 "$ROOT/scripts/guest/proto.py" -s "$src" -d "$W/stage$1" -l "$W/list$1" "$W/proto"; }
publish(){
	local n=$1 from=$2; shift 2
	[ -z "$from" ] || cp -a "$W/pub$from" "$W/pub$n"
	$PUB -k "$W/key.pem" "$@" "$W/stage$n" "$W/list$n" "$W/pub$n"
}
versions(){
	$PUB -g "$W/key.pem" > "$X/nfront.pub" 2>/dev/null &&
	mkdir -p "$src/bin" "$src/lib/sub" &&
	printf 'a1\n' > "$src/bin/a" && printf 'b1\n' > "$src/bin/b" &&
	printf 'x1\n' > "$src/lib/x" && : > "$src/lib/empty" &&
	printf 'y1\n' > "$src/lib/sub/y" && printf 's1\n' > "$src/lib/with blank" &&
	stage 1 && publish 1 '' -c "http://10.0.2.2:$port/pub1" && cp "$W/proto" "$X/proto1" || return 1
	# 2: lib/x changed, bin/b removed, lib/sub/z and lib/new/n added, bin/a's mode
	sleep 1
	printf 'x2\n' > "$src/lib/x" && rm "$src/bin/b" && printf 'z2\n' > "$src/lib/sub/z" &&
	mkdir "$src/lib/new" && printf 'n2\n' > "$src/lib/new/n" &&
	sed -i 's/^	a	775 glenda/	a	755 glenda/' "$W/proto" &&
	stage 2 && publish 2 1 || return 1
	# 3: lib/x again, its object then altered on the server
	sleep 1
	printf 'x3\n' > "$src/lib/x" && stage 3 && publish 3 2 || return 1
	local sha
	sha=$(awk '$1 == "lib/x" { print $2 }' "$W/pub3/sums")
	chmod u+w "$W/pub3/obj/${sha:0:2}/$sha" && printf 'x9\n' > "$W/pub3/obj/${sha:0:2}/$sha" || return 1
	# 4: version 2's index, one byte changed (bin/a's mode back), the signature kept
	cp -a "$W/pub2" "$W/pub4" &&
	sed -i 's/^root\/bin\/a 755 /root\/bin\/a 775 /' "$W/pub4/index" &&
	! cmp -s "$W/pub2/index" "$W/pub4/index" &&
	cp -a "$W/stage1" "$X/stage1"
}
check "publish versions 1 to 4 (scripts/replica/publish.py)" versions || { done_tests; exit; }
check "version 2's log: lib/x changed, bin/b removed, bin/a's mode, lib/sub/z added" bash -c "
	grep -q ' c lib/x ' '$W/pub2/log' && grep -q ' d bin/b ' '$W/pub2/log' &&
	grep -q ' m bin/a - 755 glenda ' '$W/pub2/log' && grep -q ' a lib/sub/z ' '$W/pub2/log'"
check "an unchanged file keeps the db's time in the stage (the CD's times are the replica's)" bash -c "
	[ \$(stat -c %Y '$W/stage1/lib/sub/y') = \$(stat -c %Y '$W/stage2/lib/sub/y') ]"

sed "1a u=http://10.0.2.2:$port" "$HERE/guest.rc" > "$X/guest.rc"
cp "$HERE/cfg" "$X/cfg"
python3 -m http.server --bind 127.0.0.1 --directory "$W" "$port" > "$TMPD/http.log" 2>&1 &
httpd=$!
trap 'kill $httpd 2>/dev/null; rm -rf "$TMPD"' EXIT
truncate -s 256M "$TMPD/gefs.img"
boot(){
	MEM=1024 DISK="$TMPD/gefs.img" UFS_EXTRA="$TMPD/extra" WAITFOR='REPLICA-DONE' WAITMAX=300 CMDWAIT=300 \
		"$ROOT/scripts/guest/boot-cli.sh" 'rc /tests/replica/guest.rc' > "$TMPD/boot" 2>&1
	tr -d '\r' < "$CLI_DIR/console.log" > "$TMPD/console"
	grep -q '^REPLICA-DONE' "$TMPD/console" || { tail -20 "$TMPD/console"; return 1; }
}
check "boot and pull (tests/replica/guest.rc)" boot || { cp "$TMPD/console" "$CLI_DIR/replica-fail.log" 2>/dev/null; done_tests; exit; }
cp "$TMPD/console" "$CLI_DIR/replica.log"

# the client tree a step left against a stage (modes from the listing's
# proto); C: lib/x as its user changed it
python3 - "$TMPD/console" "$W" > "$TMPD/cmp" <<'EOF'
import hashlib, os, sys
con, w = sys.argv[1:]
sums, modes, status, out, step = {}, {}, {}, {}, None
for line in open(con, errors='replace'):
	line = line.rstrip('\n')
	f = line.split(' ', 3)
	if f[0] == 'SUMS' and len(f) == 4:
		sums.setdefault(f[1], {})[os.path.normpath(f[3])] = f[2]
	elif f[0] == 'MODE' and len(f) == 4:
		m = f[3].split(' ', 2)	# uid gid path
		modes.setdefault(f[1], {})[os.path.normpath(m[-1])] = ' '.join([f[2]] + m[:-1])
	elif f[0] == 'STEP' and len(f) >= 2:
		step = f[1]
		out[step] = []
	elif f[0] == 'STATUS' and len(f) >= 2:
		status[f[1]] = line.split(' ', 2)[2] if len(f) > 2 else ''
		step = None
	elif step:
		out[step].append(line)
def tree(n, change=None):
	t = {}
	root = os.path.join(w, 'stage%d' % n)
	for d, _, fs in os.walk(root):
		for x in fs:
			p = os.path.relpath(os.path.join(d, x), root)
			if p.startswith('dist/'):
				continue
			b = open(os.path.join(d, x), 'rb').read()
			if change and p in change:
				b = change[p](b)
			t[p] = hashlib.sha1(b).hexdigest()
	return t
want = {'A1': tree(1), 'A2': tree(2), 'D': tree(2), 'E': tree(2), 'B': tree(2),
	'C': tree(2, {'lib/x': lambda b: b'x1\nlocal\n'}), 'F': tree(2), 'F0': tree(1)}
for s, t in want.items():
	print('%s %s %s' % (s, 'tree' , 'ok' if sums.get(s) == t else 'differs: got %r want %r' % (sums.get(s), t)))
	print('%s status %s' % (s, status.get(s, 'none')))
	print('%s out %s' % (s, ' | '.join(out.get(s, []))[:600]))
for s in ('A1', 'A2', 'B'):
	print('%s bin/a %s' % (s, modes.get(s, {}).get('bin/a')))
	print('%s lib/sub %s' % (s, modes.get(s, {}).get('lib/sub')))
EOF
st(){ grep -q "^$1 status $2\$" "$TMPD/cmp" || { grep "^$1 " "$TMPD/cmp"; return 1; }; }
tr_(){ grep -q "^$1 tree ok" "$TMPD/cmp" || { grep "^$1 " "$TMPD/cmp"; return 1; }; }
out(){ grep "^$1 out " "$TMPD/cmp" | grep -q "$2" || { grep "^$1 " "$TMPD/cmp"; return 1; }; }
nout(){ ! grep "^$1 out " "$TMPD/cmp" | grep -q "$2" || { grep "^$1 " "$TMPD/cmp"; return 1; }; }
check "A1: a new client takes version 1 (an empty file, a name with a blank)" bash -c "$(declare -f st tr_); TMPD='$TMPD'; st A1 '' && tr_ A1"
check "A1: modes as the proto gives them (bin/a 775, ug+rw)" grep -q '^A1 bin/a --rwxrwxr-x' "$TMPD/cmp"
check "A2: version 2: changed, added, removed" bash -c "$(declare -f st tr_); TMPD='$TMPD'; st A2 '' && tr_ A2"
check "A2: bin/a's new mode (m, 755)" grep -q '^A2 bin/a --rwxr-xr-x' "$TMPD/cmp"
check "D: an altered object is refused, the file stays version 2's" bash -c "$(declare -f st tr_ out); TMPD='$TMPD'; tr_ D && out D 'contents differ from the signed index'"
check "E: an index not as signed is refused" bash -c "$(declare -f tr_ out); TMPD='$TMPD'; ! grep -q '^E status \$' '$TMPD/cmp' && tr_ E && out E 'signature does not verify'"
check "B: a system installed from version 1 takes version 2, no conflict" bash -c "$(declare -f st tr_ nout); TMPD='$TMPD'; st B '' && tr_ B && nout B 'locally\|conflict'"
check "C: a file its user changed is kept and reported" bash -c "$(declare -f tr_ out); TMPD='$TMPD'; tr_ C && out C 'locally modified; will not'"
check "F: pull(1) on gefs snapshots main, then takes version 2" bash -c "$(declare -f st tr_ out); TMPD='$TMPD'; st F '' && tr_ F && out F 'pull: snapshot pull\.'"
check "F: the snapshot holds version 1, the way back" bash -c "$(declare -f tr_); TMPD='$TMPD'; tr_ F0"
done_tests
