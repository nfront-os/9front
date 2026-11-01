#!/bin/bash
# The first step on the host's GPU: does the card reach a 9front guest
# (VFIO passthrough) and come back?  Needs the card bound to vfio-pci and
# an unlimited memlock (sudo scripts/guest/vfio-gpu.sh bind PID), KVM.
# Boots the build's kernel with the GPU and its audio function passed
# through, lists the guest's PCI devices and the card's configuration
# space as the guest sees it.  Afterwards the card stays vfio-pci's:
# sudo scripts/guest/vfio-gpu.sh release gives it back to the host.
#
# The card's firmware (the host's /usr/lib/firmware/amdgpu, unpacked) is
# in the guest's /lib/firmware/amdgpu: aux/amdgpu loads it from there.
#
# usage: scripts/guest/vfio-test.sh [guest commands]
#   env: FWDIR  the host's amdgpu firmware (/usr/lib/firmware/amdgpu)
#        FW     its files for this card (Navi 33: GC 11.0.2, PSP and SMU
#               13.0.10, DCN 3.2.1, SDMA 6.0.3, VCN 4.0.4)
set -e
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
GPU=${GPU:-0000:09:00.0}
AUDIO=${AUDIO:-0000:09:00.1}
sys=/sys/bus/pci/devices

for d in $GPU $AUDIO; do
	drv=$(basename "$(readlink "$sys/$d/driver" 2>/dev/null)" 2>/dev/null)
	[ "$drv" = vfio-pci ] || { echo "vfio-test: $d is bound to ${drv:-nothing}, not vfio-pci (sudo scripts/guest/vfio-gpu.sh bind $PPID)" >&2; exit 1; }
	g=$(basename "$(readlink "$sys/$d/iommu_group")")
	[ -w /dev/vfio/$g ] || { echo "vfio-test: /dev/vfio/$g not writable" >&2; exit 1; }
done
lim=$(ulimit -l)
[ "$lim" = unlimited ] || { echo "vfio-test: memlock $lim KB: VFIO pins all guest memory (sudo scripts/guest/vfio-gpu.sh bind PID of this shell's parent)" >&2; exit 1; }

# the firmware into the root the guest gets (boot-cli.sh: UFS_EXTRA)
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
FWDIR=${FWDIR:-/usr/lib/firmware/amdgpu}
FW=${FW:-gc_11_0_2 psp_13_0_10 smu_13_0_10 dcn_3_2_1 sdma_6_0_3 vcn_4_0_4}
X=$CLI_DIR/vfio-extra
rm -rf "$X"
mkdir -p "$X/lib/firmware/amdgpu"
for f in $FW; do
	for z in "$FWDIR"/$f*.bin*; do
		[ -e "$z" ] || { echo "vfio-test: no firmware $FWDIR/$f*" >&2; exit 1; }
		b=$(basename "$z")
		case $b in
		*.zst)	zstd -q -d -c < "$z" > "$X/lib/firmware/amdgpu/${b%.zst}" ;;
		*.xz)	xz -d -c < "$z" > "$X/lib/firmware/amdgpu/${b%.xz}" ;;
		*)	cp "$z" "$X/lib/firmware/amdgpu/$b" ;;
		esac
	done
done

# by default: the PCI devices with their BARs (pci -bv), and the first
# words of the AMD functions' configuration space (vendor 1002)
cmds=${1:-"pci -bv; for(f in '#\$/pci/'*ctl) if(grep -s '1002/' \$f){ echo \$f; cat \$f; xd -x4 \`{echo \$f | sed 's/ctl\$/raw/'} | sed 4q }"}
UFS_EXTRA=$X MEM=${MEM:-4096} CPU=${CPU:-host} QEMU_EXTRA="-device vfio-pci,host=$GPU,multifunction=on -device vfio-pci,host=$AUDIO $QEMU_EXTRA" \
	"$ROOT/scripts/guest/boot-cli.sh" "$cmds"
