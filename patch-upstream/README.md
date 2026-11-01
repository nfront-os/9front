# Patches for upstream 9front

Fixes made on the `gcc` branch (the gcc port of 9front) that are not about
gcc: they apply to 9front as it is, built with its own compilers. Each one
is a patch for [9front](https://github.com/9front/9front) branch `front`,
in `git format-patch` form, one change per patch, without the port's
`/* GCC port: ... */` markers.

They stay on the `gcc` branch as well (marked there) until 9front has
taken or declined them.

- Base: `origin/front` at `350266104` (2026-10-31, "aux/acpi: battery
  charging control for thinkpads").
- Source: branch `upstream` of this repository (worktree
  `../9front-upstream`), one commit per patch on top of `origin/front`.
- Regenerate after a change or a rebase of `upstream` onto a newer
  `origin/front`:

	git -C ../9front-upstream rebase origin/front
	rm patch-upstream/0*.patch
	git -C ../9front-upstream format-patch -o $PWD/patch-upstream origin/front..upstream

## Applying

On Unix: `git am patch-upstream/0*.patch` on a checkout of `front`.
On 9front: `git/import` takes the files as they are.

All 17 apply as a series. Each applies alone too, except:

- 0009 needs 0007 (context: `vbump` in `rwstat`),
- 0010 needs 0007 and 0009,
- 0012 needs 0007 (context: `vbump` in `ropen` and `rcreate`).

The commit messages end with a `Co-Authored-By:` line (the changes were
made with an AI assistant); drop it if 9front does not want it.

## How they were tested

- **Running**: the same code (apart from comments, and in `tcp.c` the
  port's own changes around it) runs in the `gcc` branch's system, built by gcc for amd64 and 386, booted in QEMU. Each
  patch below says which test found the problem and which one shows the
  fix. Most were found by the Rust port for 9front (`../9front-rust`: Rust's
  std test suite, cargo and rustc running on 9front, tokio).
- **9front's compilers**: on the `upstream` branch's sources and headers
  (not the gcc branch's), in a 9front guest: every `.c` of ext4srv,
  `ndb/cs.c` and `9/ip/tcp.c` (from `pc64` and `pc`) with `6c` and `8c`
  `-FTVw` (the mkfiles' flags); APE's `arm64/notetramp.c` with
  `7c -FTVw -p` against the patched `ureg.h`; `9/arm64/main.c` (0016)
  with `7c -FTVw`; `9/pc/sd53c8xx.c` (0017) with `8c -FTVw` (pc) and
  `6c -FTVw` (from pc64). No errors, no warnings.
- **Not done**: a kernel and ext4srv built with 9front's compilers and run
  with these patches. The kernel conf (0014) and riostart (0015) are not
  compiled code.

## The patches

| # | Area | Kind |
|---|------|------|
| 0001 | ip/tcp: stale bypass pointer hangs up a reused conversation | bug fix |
| 0002 | ip/tcp: full listener queue: keep the call in limbo, no reset | behaviour change |
| 0003 | ndb/cs: errors cut to 63 bytes | bug fix |
| 0004 | ext4srv: holes read block 0 of the disk | bug fix (data) |
| 0005 | ext4srv: "file not found" instead of "file does not exist" | bug fix |
| 0006 | ext4srv: truncating to the same length fails | bug fix |
| 0007 | ext4srv: qid.vers never changes | bug fix |
| 0008 | ext4srv: rwrite uses a stale file length | bug fix (data) |
| 0009 | ext4srv: wstat to a longer length fails | bug fix (stat(5)) |
| 0010 | ext4srv: old bytes come back after shorten and extend | bug fix (data) |
| 0011 | ext4srv: atime of a new file is 0 | bug fix |
| 0012 | ext4srv: exclusive-use files (DMEXCL) | new feature |
| 0013 | ape: arm64 Ureg layout differs from the kernel's | bug fix |
| 0014 | pc: xhci controller never registered | bug fix |
| 0015 | riostart: stats window can take the console's focus | bug fix |
| 0016 | arm64: the last cpu never comes up (sev lost) | bug fix |
| 0017 | sd53c8xx: boot hangs 600 s in scsiverify with SMP | bug fix |

### 0001 ip/tcp: don't hang up a reused conversation through a stale bypass pointer

A spliced loopback connection has each side's `bypass` pointing to the
other. A side that reaches Closed clears only its own. When the other side
closed later, `tcpclose()` hung up the rq of whatever conversation the
stale pointer named, by then often a new one: a new listener on it got
"listen hungup", its clients "connection refused". Now it hangs up only a
peer that still points back, as `tcpbypass()` checks already.

Found by: tokio tests in the Rust port (echo servers on 127.0.0.1, one
after another in one program). Shown by: `../9front-rust/tests/async.sh`.

### 0002 ip/tcp: keep a call in limbo when the listener's queue is full

When the last ack of a handshake comes and the listener already has
`Maxincall` (10) calls queued, or no conversation is free,
`tcpincoming()` dropped the call and `tcpiput()` sent a reset. The client
has the connection as established at that point (its connect returned), so
it reads eof right away on a connection it was told is open.

Now the call stays in limbo and the ack is dropped, as BSD and Linux do with
a full backlog: `limborexmit()` resends the syn-ack (up to 5 times, about 4
s in all), the client acks again, and the call comes in once there is room.
Later segments of such a call (they ack the syn-ack, their seq is past
irs+1) are dropped too instead of answered with a reset; the client sends
them again. If no room comes in time the limbo entry expires as before and
the client's next segment gets the reset.

This changes behaviour only when a listener's queue is full or no
conversation is free. Limbo stays bounded by `Maxlimbo` (1000), and keeping
a call there still needs the peer to complete the handshake (know iss).

Found by: tokio's echo test, 20 clients at once ("early eof", now and then;
the console showed `Fsnewcall: incall queue full (10)`). Shown by: a listener
that takes no calls for a second while 30 clients connect
(`../9front-rust/tests/crates/tokioecho`): without the patch 20 of them read
eof at once, with it all 30 get their echo.

### 0003 ndb/cs: don't cut errors to 63 bytes

`rwrite()` read a failed lookup's error into `curerr[64]`, and `sendmsg()`
puts "cs: " in front: for a long name the reason was cut off
("cs: can't translate address no-such-host-abcdefghijklmnopqrstuvwxyz"
without ".invalid: dns: name does not exist"), so a program cannot tell a
missing name from other failures. Now `ERRMAX`.

Found by: the Rust port's DNS test (`tests/crates/tlsdns`). Shown by: `echo
-n 'tcp!no-such-host-abcdefghijklmnopqrstuvwxyz.invalid!80' >/net/cs`.

### 0004 ext4srv: read holes as zeros, not block 0 of the disk

`ext4_fread()` handled a hole (an unallocated block, physical block 0) only
in a partial first block. In the loop over whole blocks and in a partial
last block it read physical block 0 of the partition into the file, and a
run started with `if(!fblock_start)`, so after a hole the next data block
was read in the hole's place (the data after a hole came out shifted). Now
a run starts when it is empty and a hole reads as zeros.

Found by: the gcc port building itself on an ext4 image made by Linux's
`mkfs.ext4 -d`, which makes every all-zero block a hole: binaries, archives
and object files read wrong.

### 0005 ext4srv: say "file does not exist" as other file servers

A missing file gave "file not found". Plan 9 says "file does not exist"
(Enonexist), and programs look for it: APE maps only that to ENOENT, so a
POSIX program took a missing file on ext4 for a hard error.

Found by: gcc on ext4srv stopping at the first `-I` directory without the
header instead of trying the next.

### 0006 ext4srv: truncating a file to the length it has is no error

`ext4_ftruncate()` refused any length not below the file's ("space
preallocation not supported"), the same length too: a wstat of length 0 on
an empty file failed.

Found by: cargo on ext4srv (Rust's `File::set_len(0)` on a new file).

### 0007 ext4srv: change qid.vers with every change of a file

qid.vers was the inode's generation, which writes never change (0 for a new
file). The kernel caches a program's text by qid: a program rebuilt in place
(new contents, the same inode number) ran with the old text and the new data
("invalid opcode"). Now vers is generation + mtime + a count of the changes
(write, create, truncation) per inode since ext4srv started, so it differs
after every change, also within the same second.

Found by: `cargo build` on an ext4 image, running a program it had just
rebuilt in place.

### 0008 ext4srv: rwrite takes the file's length from its inode

An open file keeps the length it had at open. `rwrite()` fills the gap from
the end to the write's offset with zeros using that length: after another fid
had made the file longer, the fill wrote zeros over that fid's data; after
another fid had truncated it, the gap was not filled. Now the length is read
from the inode first.

Found by: Rust's std test suite on ext4srv (two handles to one file).

### 0009 ext4srv: a wstat to a longer length extends the file with zeros

stat(5): a wstat of length sets the file's length, and what is past the old
end reads as zeros. Every length went to `ext4_ftruncate()`, which only
shortens, so any longer length failed. Now zeros are written up to the new
length first, as `rwrite()` does past the end. Needs 0007.

Found by: Rust's std test suite (`File::set_len` to a larger size; POSIX
`ftruncate` through APE likewise).

### 0010 ext4srv: zero the rest of the last block when a file is shortened

`ext4_ftruncate()` frees the blocks past the new length but leaves the old
bytes in the rest of the new last block; extending the file again (0009)
showed them where it should read zeros. Now the rest of that block is zeroed
before truncating. Needs 0007, 0009.

Found by: Rust's std test suite (`set_len` shorter, then longer, then read).

### 0011 ext4srv: set atime of a new file

`rcreate()` set mtime and ctime but left atime 0 (1970).

Found by: Rust's std test suite (metadata of a new file).

### 0012 ext4srv: exclusive-use files (DMEXCL)

open(5): a DMEXCL file can be open by one fid at a time. ext4srv dropped the
bit, so locks built on it locked nothing. Now it is kept in the inode as
`EXT4_INODE_FLAG_UNRM` (chattr's `u`, unused by Linux), as DMTMP is kept as
NODUMP; reported in stat and in the qid (QTEXCL); set by create and wstat. A
second open of such a file fails with "file is locked" until the first fid
is clunked. Locks are not broken after a time (open(5) allows a server to). Needs 0007.

Found by: cargo and rustc on 9front, which lock their build directories with
DMEXCL files. Shown by: the Rust port's `tests/lock.rs` on `/tmp` of a guest
whose `/tmp` is ext4srv; `e2fsck` of the image afterwards is clean.

### 0013 ape: arm64 Ureg in the kernel's layout

`/arm64/include/ape/ureg.h` had r29 and sp in a union and type, psr, pc after
r30; the kernel's `/arm64/include/ureg.h` has r29, r30, sp, pc, psr, type.
APE's `notetramp.c` sets `u->pc` to continue a note in the signal handler:
it set psr instead, and every APE program on arm64 that caught a signal died
at pc 0 (built with 7c too). Now the kernel's layout.

Found by: the gcc port's APE signal test on arm64 (QEMU).

### 0014 pc: register the xhci controller (usbxhcipci first, as in pc64)

The `link` section of a kernel conf calls only the first name's link
function. pc had `usbxhci pci usbxhcipci`: `usbxhcilink()` is empty, so
`usbxhcipcilink()`, which registers the controller type, never ran and 9pc
found no xhci controller. pc64 has `usbxhcipci pci usbxhci`.

Found by: reading the 386 kernel's conf while porting it (pc64 has the
order that works). Not tested with an xhci device on 9pc.

### 0015 glenda's riostart: make the stats window first, not in the background

`window 0,0,161,117 stats` is the old syntax; window(1) makes such a window
from a background job, which can finish after the windows made below it. A
new window becomes the current one, so stats could take the focus from the
console window (console=0: rc on the serial line) and typed input went to
stats. `window -r 0 0 161 117 stats -lmisce` makes it at once through wctl.

Found by: the gcc port's disk and gui tests, which boot a terminal and type
into the serial console window: the input was lost on about every second
boot under KVM.

### 0016 arm64: wake each new cpu until it runs

`mpinit` starts the other processors with PSCI CPU_ON; they wait in
`_start`'s `wfe` for an event, and nothing sent one: on a QEMU that models
wfe exactly (QEMU 11) no other cpu came up and `synccycles()` waited
forever. A single `sev()` after CPU_ON (as bcm64 does) is not enough: when
it comes before the new cpu runs, it is lost; each cpu was then woken only
by the sev for the next one, and the last one never came up. Now `mpinit`
sends `sev()` until the new cpu has set `active.machs[i]` in `machinit`.

Found by: the gcc port's NIX branch, booting arm64 (QEMU virt, cortex-a72)
with 4 to 8 cpus: cpu3 of 4 never came up, also without NIX. Shown by: the
boot test with 4 cpus and the NIX tests with 6 and 8 (all cpus up).

### 0017 sd53c8xx: clear the SCSI reset interrupt in busreset

`busreset()` pulses SCSI RST, which raises a SCSI reset interrupt (SIST0
RST). `sd53c8xxenable` enables the interrupt only afterwards; the pending
interrupt then reaches `sd53c8xxinterrupt` late, with SMP on another
processor, after the SCRIPTS were started for the first command and wait
for a reselection. The handler sets `c->running = 0` for the old reset,
the next start (a write of DSP) is ignored by the running chip, and the
command times out after 600 seconds. `busreset` now reads ISTAT, SIST0,
SIST1 and DSTAT after the reset, which clears it.

Found by: the boot test with 4 cpus (QEMU lsi53c895a, KVM), which hung
in `scsiverify` now and then (kbdfs's open of the disk); traced with
QEMU's `lsi_*` trace events and gdb on the hung guest. Shown by: 21 boots
with 4 cpus without a hang, where it hung at the first before.

## Not included

- TCP half close (ctl `shutdown`, the gcc branch's `tcpshutdown`): a new
  interface added for the Rust port (`shutdown(Write)`), not a fix.
- Everything about gcc (types, `#pragma pack`, `volatile`, GAS ports,
  member paths, the build tool).
