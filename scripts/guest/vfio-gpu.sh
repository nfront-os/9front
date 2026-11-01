#!/bin/bash
# The host's GPU for a 9front guest (single-GPU VFIO passthrough): what
# needs root, so that QEMU runs as the user afterwards.
#
# usage: sudo scripts/guest/vfio-gpu.sh bind [PID]
#            the card (GPU and its HDMI audio: a bus reset of one resets
#            both) from amdgpu/snd_hda_intel to vfio-pci; /dev/vfio/N
#            for $SUDO_USER; with PID: that process's memlock limit
#            unlimited (VFIO pins all guest memory; its later children,
#            QEMU, inherit it)
#        sudo scripts/guest/vfio-gpu.sh release
#            back to the host's drivers (amdgpu probes the card anew)
#        scripts/guest/vfio-gpu.sh status
#
# GPU and AUDIO (PCI addresses) default to the RX 7600 of this host.
# The host loses its screen while the card is the guest's (work over
# ssh); if the card does not come back from the guest, only a reboot
# of the host helps.
set -e
GPU=${GPU:-0000:09:00.0}
AUDIO=${AUDIO:-0000:09:00.1}
DEVS="$GPU $AUDIO"
sys=/sys/bus/pci/devices

die(){ echo "vfio-gpu: $*" >&2; exit 1; }
driver(){ [ -e "$sys/$1/driver" ] && basename "$(readlink "$sys/$1/driver")" || echo none; }
group(){ basename "$(readlink "$sys/$1/iommu_group")"; }

status(){
	for d in $DEVS; do
		[ -e "$sys/$d" ] || die "no PCI device $d"
		g=$(group "$d")
		echo "$d $(cat "$sys/$d/vendor"):$(cat "$sys/$d/device") driver $(driver "$d") iommu group $g ($(ls "/sys/kernel/iommu_groups/$g/devices" | tr '\n' ' '))$( [ -e /dev/vfio/$g ] && echo " /dev/vfio/$g: $(stat -c %U /dev/vfio/$g)")"
	done
}

case "$1" in
status)
	status
	;;
bind)
	[ "$(id -u)" = 0 ] || die "bind needs root (sudo)"
	user=${SUDO_USER:-root}
	if [ -n "$2" ]; then
		prlimit --pid "$2" --memlock=unlimited:unlimited || die "prlimit on $2 failed"
		echo "memlock of $2: $(prlimit --pid "$2" --memlock --noheadings -o SOFT,HARD)"
	fi
	modprobe vfio-pci
	for d in $DEVS; do
		g=$(group "$d")
		[ "$(ls /sys/kernel/iommu_groups/$g/devices | wc -l)" = 1 ] || die "$d shares IOMMU group $g with other devices"
		echo vfio-pci > "$sys/$d/driver_override"
		if [ "$(driver "$d")" != vfio-pci ]; then
			if [ -e "$sys/$d/driver" ]; then
				echo "$d" > "$sys/$d/driver/unbind" || die "$d: unbind from $(driver "$d") failed"
			fi
			echo "$d" > /sys/bus/pci/drivers/vfio-pci/bind || die "$d: vfio-pci bind failed (see dmesg)"
		fi
		[ "$(driver "$d")" = vfio-pci ] || die "$d: not bound to vfio-pci"
		chown "$user" "/dev/vfio/$g"
	done
	status
	;;
release)
	[ "$(id -u)" = 0 ] || die "release needs root (sudo)"
	for d in $DEVS; do
		echo > "$sys/$d/driver_override"
		if [ "$(driver "$d")" = vfio-pci ]; then
			echo "$d" > "$sys/$d/driver/unbind" || die "$d: unbind from vfio-pci failed"
		fi
		echo "$d" > /sys/bus/pci/drivers_probe || echo "vfio-gpu: $d: no host driver took it" >&2
	done
	status
	;;
*)
	sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//'
	exit 2
	;;
esac
