/* SPDX-License-Identifier: MIT */
/* libdrm on 9front: the part for AMD's GPUs (amdgpu9.c), as drm9.c calls it */
typedef struct Amd Amd;
Amd	*amd9attach(int fd, const char *dir, int ctx);	/* the context's arena */
void	amd9setfd(Amd *a, int fd);	/* its requests go to fd now */
int	amd9ioctl(Amd *a, unsigned long req, void *arg);	/* 0, or -1 and errno */
Amd	*amd9of(int fd);	/* drm9.c: the descriptor's, or NULL */
int	amd9syncexport(Amd *a, uint32_t handle, int *fd);	/* a syncobj as a descriptor: 0, or -errno */
int	amd9syncimport(Amd *a, int fd, uint32_t *handle);
