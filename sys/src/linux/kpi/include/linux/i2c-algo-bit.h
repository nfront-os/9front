/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/i2c-algo-bit.h (ours, not Linux's) */
#ifndef _KPI_LINUX_I2C_ALGO_BIT_H
#define _KPI_LINUX_I2C_ALGO_BIT_H

#include <linux/i2c.h>

/* I2C by toggling two lines (kpi/i2c.c: ours, Linux's is GPL) */
struct i2c_algo_bit_data {
	void	*data;
	void	(*setsda)(void *data, int state);
	void	(*setscl)(void *data, int state);
	int	(*getsda)(void *data);
	int	(*getscl)(void *data);
	int	(*pre_xfer)(struct i2c_adapter *a);
	void	(*post_xfer)(struct i2c_adapter *a);
	int	udelay;
	int	timeout;
	bool	can_do_atomic;
};
int	i2c_bit_add_bus(struct i2c_adapter *a);
int	i2c_bit_add_numbered_bus(struct i2c_adapter *a);
extern const struct i2c_algorithm i2c_bit_algo;

#endif
