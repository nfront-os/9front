# What the LinuxKPI build compiles: Kbuild directories (kbuild.py chooses
# their files with kconfig) and single files from outside them.
dir	drivers/gpu/drm
#dir	drivers/gpu/drm/scheduler
# amdgpu: in the census (census.py -a), not yet in the build
#dir	drivers/gpu/drm/amd/amdgpu
file	drivers/video/hdmi.c
file	drivers/dma-buf/dma-resv.c
# dma-fence(-array, -chain, -unwrap), dma-buf, sync_file and lib/scatterlist
# are GPL in Linux: the LinuxKPI has them (kpi/linux/drm_linux.c, from
# OpenBSD's)
