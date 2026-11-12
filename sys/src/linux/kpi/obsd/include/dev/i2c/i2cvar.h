/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, dev/i2c/i2cvar.h: the i2c
 * controller interface of OpenBSD's linux/i2c.h (its bit-banging
 * adapters, drm_linux.c).
 */
#ifndef _OBSD_DEV_I2C_I2CVAR_H
#define _OBSD_DEV_I2C_I2CVAR_H

#include <sys/types.h>

typedef enum {
	I2C_OP_READ = 0,
	I2C_OP_READ_WITH_STOP = 1,
	I2C_OP_WRITE = 2,
	I2C_OP_WRITE_WITH_STOP = 3,
} i2c_op_t;
#define I2C_OP_READ_P(x)	(((int)(x) & 2) == 0)
#define I2C_OP_WRITE_P(x)	(!I2C_OP_READ_P(x))
#define I2C_OP_STOP_P(x)	(((int)(x) & 1) != 0)

typedef unsigned short	i2c_addr_t;

struct i2c_controller {
	void	*ic_cookie;
	int	(*ic_acquire_bus)(void *, int);
	void	(*ic_release_bus)(void *, int);
	int	(*ic_exec)(void *, i2c_op_t, i2c_addr_t, const void *, size_t, void *, size_t, int);
	int	(*ic_send_start)(void *, int);
	int	(*ic_send_stop)(void *, int);
	int	(*ic_initiate_xfer)(void *, i2c_addr_t, int);
	int	(*ic_read_byte)(void *, uint8_t *, int);
	int	(*ic_write_byte)(void *, uint8_t, int);
};
typedef struct i2c_controller	*i2c_tag_t;

#define I2C_F_POLL	0x01
#define I2C_F_STOP	0x02
#define I2C_F_LAST	0x04

#define iic_acquire_bus(ic, f)	(*(ic)->ic_acquire_bus)((ic)->ic_cookie, (f))
#define iic_release_bus(ic, f)	(*(ic)->ic_release_bus)((ic)->ic_cookie, (f))
#define iic_exec(ic, op, addr, cmd, cl, buf, bl, f) \
	(*(ic)->ic_exec)((ic)->ic_cookie, (op), (addr), (cmd), (cl), (buf), (bl), (f))

#endif
