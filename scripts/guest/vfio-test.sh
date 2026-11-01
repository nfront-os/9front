#!/bin/bash
# The first step on the host's GPU: does the card reach a 9front guest
# (VFIO passthrough) and come back?  Needs the card bound to vfio-pci and
# an unlimited memlock (sudo scripts/guest/vfio-gpu.sh bind PID), KVM.
# Boots the build's kernel with the GPU and its audio function passed
# through, lists the guest's PCI devices and the card's configuration
# space as the guest sees it.  Afterwards the card stays vfio-pci's:
# sudo scripts/guest/vfio-gpu.sh release gives it back to the host.
#
# usage: scripts/guest/vfio-test.sh [guest commands]
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

# by default: the PCI devices with their BARs (pci -bv), and the first
# words of the AMD functions' configuration space (vendor 1002)
cmds=${1:-'pci -bv; for(f in ''#$/pci/''*ctl) if(grep -s ''1002/'' $f){ echo $f; cat $f; xd -x4 `{echo $f | sed ''s/ctl$/raw/''} | sed 4q }'}
MEM=${MEM:-4096} CPU=${CPU:-host} QEMU_EXTRA="-device vfio-pci,host=$GPU,multifunction=on -device vfio-pci,host=$AUDIO $QEMU_EXTRA" \
	"$ROOT/scripts/guest/boot-cli.sh" "$cmds"
