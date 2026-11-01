# tests/hw: the tests on real hardware

The other test categories boot QEMU with the root served by the build
host. These run on a real machine, from a USB stick, without network or
host: the stick boots 9front, runs the tests by itself and writes the
logs to its own 9fat partition, which any machine can read.

Target so far: Lenovo ThinkPad X1 Carbon (2015, amd64). The internal disks
are only read.

## The stick

	scripts/hw/mkstick.sh [image]		# $CLI_DIR/hwstick.amd64.img
	PACK=1 scripts/hw/mkstick.sh		# and a hand-off dir: packed image, collect.py, HOWTO.txt

The stick is written on another machine with dd (`HOWTO.txt`):
`zstd -dc hwstick.amd64.img.zst | sudo dd of=/dev/sdX bs=4M iflag=fullblock conv=fsync status=progress`.

`mkstick.sh` builds the image in a QEMU guest like `scripts/guest/mkdisk.sh`
(needs the amd64 build and `scripts/guest/build-boot.sh`):

- 9fat: 9bootfat, `plan9.ini`, the kernel, and `hwtest/` (configurations,
  base `plan9.ini`, `README.txt`, the logs);
- hjfs `fs`: the distribution, `/rc/bin/hwtest` (`hwtest.rc`),
  `/cfg/hwtest/termrc`, the categories in `/sys/lib/hwtest`, the guest
  test programs in `/tests` (tests/guest, and tests/*/root of dirs with a
  `hwstick` file).

`plan9.ini` boots without questions: `bootpart=fs` (bootrc takes the `fs`
partition of a USB disk, waiting for nusb; never an internal disk),
`user=glenda`, `service=terminal`, `sysname=hwtest` (termrc runs
`/cfg/hwtest/termrc`), `hwtest=1`.

## A run

`hwtest` mounts the 9fat of the stick it booted from and runs, for each
line of `hwtest/configs` (`name<tab>steps<tab>plan9.ini lines;...`), the
steps (categories `tests/hw/cat/<step>.rc`, each printing `ok …`,
`FAIL …: why`, `SKIP …` like the guest tests; a step has 600 s, or
`<step>.timeout`). Then `plan9.ini` becomes the next configuration and
the machine resets; after the last one `plan9.ini` is the base without
`hwtest` and the machine halts.

Logs: `hwtest/run<N>/<config>/<step>.log`, `summary.txt`, `kmesg.txt`;
`hwtest/state` is where the run is.

- A step running when the machine died is logged as `HANG` at the next
  boot, and the run goes on.
- A configuration whose kernel hangs while booting: power on, press a key
  when 9boot prints `plan9.ini`, type `hwskip=1` and `boot`: it is logged
  as `BOOTHANG` and the run goes on (9boot has no boot menu; `hwskip`
  must also make the kernel ignore the configuration's own options).

	scripts/hw/collect.py /dev/sdX1-or-image [dir]	# copies hwtest/, prints the summaries

Configurations (`tests/hw/configs`): `default` (all steps), `ncpu1`
(`*ncpu=1`: boot, guest, stress), `again` (boot after a reset).

## The dry run in QEMU

`bash tests/hw/run.sh` (category `hw`) makes the image with
`configs.qemu` and a serial console and boots it in QEMU as a USB stick on
xHCI, with KVM: the configurations with resets between them, a power-off
during the step `hangme` (HANG, then the run goes on), the logs read back
with `collect.py`, `plan9.ini` set back at the end. It also attaches an
IDE disk (`disk` only reads it: its checksum is compared afterwards)
and an e1000. `STICK=image` uses an
existing image.

## Categories

| step | what |
|---|---|
| sysinfo | kmesg, PCI, cpuid, archctl, sysstat, memory, disks, USB, ether, IRQs |
| boot | the system came up: processors, /bin, RTC, ps, sysname, root from USB, no panic |
| guest | tests/guest's unit tests (libc, libthread, auth, mp, pak, APE): `/tests/run` |
| disk | internal disks, only read: inquiry, size, partition tables (`fdisk -r`, `prep -r`), 64 MB read at start, middle, end, timed |
| usb | controllers and devices (`usbtree`), the stick as storage, 64 MB read from it |
| net | ethernet interfaces listed (not configured), ping and TCP over loopback |
| stress | all processors a minute on the same floating point sum (one result), a ramfs copy compared, temperatures (`#P/cputemp`) |
| gcc | the native gcc: `gccmk` builds a program from a 9front mkfile, it runs (SKIP without `/$objtype/gnu`) |
| hangme | hangs (dry run only) |
