/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/i2c.h (ours, not Linux's) */
#ifndef _KPI_LINUX_I2C_H
#define _KPI_LINUX_I2C_H

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/acpi.h>	/* 9front: as Linux's i2c.h (acpi_handle) */

/* I2C: buses the driver makes (GMBUS, DP AUX), used by itself (kpi/i2c.c) */
struct i2c_msg { u16 addr; u16 flags; u16 len; u8 *buf; };
#define I2C_M_RD		0x0001
#define I2C_M_TEN		0x0010
#define I2C_M_RECV_LEN		0x0400
#define I2C_M_NO_RD_ACK		0x0800
#define I2C_M_IGNORE_NAK	0x1000
#define I2C_M_REV_DIR_ADDR	0x2000
#define I2C_M_NOSTART		0x4000
#define I2C_M_STOP		0x8000
#define I2C_FUNC_I2C			0x00000001
#define I2C_FUNC_10BIT_ADDR		0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING	0x00000004
#define I2C_FUNC_SMBUS_PEC		0x00000008
#define I2C_FUNC_NOSTART		0x00000010
#define I2C_FUNC_SMBUS_QUICK		0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE	0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE	0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA	0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA	0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA	0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA	0x00400000
#define I2C_FUNC_SMBUS_BYTE		(I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE)
#define I2C_FUNC_SMBUS_BYTE_DATA	(I2C_FUNC_SMBUS_READ_BYTE_DATA | I2C_FUNC_SMBUS_WRITE_BYTE_DATA)
#define I2C_FUNC_SMBUS_WORD_DATA	(I2C_FUNC_SMBUS_READ_WORD_DATA | I2C_FUNC_SMBUS_WRITE_WORD_DATA)
#define I2C_FUNC_SMBUS_PROC_CALL	0x00800000
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL	0x00008000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA	0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA	0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK	0x04000000
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK	0x08000000
#define I2C_FUNC_SMBUS_I2C_BLOCK	(I2C_FUNC_SMBUS_READ_I2C_BLOCK | I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)
/* 9front: what an adapter can emulate with plain I2C (the value Linux's drivers expect) */
#define I2C_FUNC_SMBUS_EMUL		(I2C_FUNC_SMBUS_PEC | I2C_FUNC_SMBUS_QUICK | \
					 I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA | \
					 I2C_FUNC_SMBUS_WORD_DATA | I2C_FUNC_SMBUS_PROC_CALL | \
					 I2C_FUNC_SMBUS_I2C_BLOCK | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)
#define I2C_CLASS_HWMON		(1 << 0)
#define I2C_CLASS_DDC		(1 << 3)
#define I2C_CLASS_SPD		(1 << 7)
/* i2c_adapter_quirks.flags: what the adapter cannot do */
#define I2C_AQ_COMB			(1 << 0)	/* combined messages: */
#define I2C_AQ_COMB_WRITE_FIRST		(1 << 1)	/*  first a write */
#define I2C_AQ_COMB_READ_SECOND		(1 << 2)	/*  then a read */
#define I2C_AQ_COMB_SAME_ADDR		(1 << 3)	/*  both to one address */
#define I2C_AQ_COMB_WRITE_THEN_READ	(I2C_AQ_COMB | I2C_AQ_COMB_WRITE_FIRST | I2C_AQ_COMB_READ_SECOND | I2C_AQ_COMB_SAME_ADDR)
#define I2C_AQ_NO_CLK_STRETCH		(1 << 4)
#define I2C_AQ_NO_ZERO_LEN_READ		(1 << 5)
#define I2C_AQ_NO_ZERO_LEN_WRITE	(1 << 6)
#define I2C_AQ_NO_ZERO_LEN		(I2C_AQ_NO_ZERO_LEN_WRITE | I2C_AQ_NO_ZERO_LEN_READ)
#define I2C_AQ_NO_REP_START		(1 << 7)
#define I2C_NAME_SIZE		20
#define I2C_LOCK_ROOT_ADAPTER	0x01
#define I2C_LOCK_SEGMENT	0x02
struct i2c_adapter;
struct i2c_algorithm {
	int	(*master_xfer)(struct i2c_adapter *a, struct i2c_msg *msgs, int num);
	int	(*master_xfer_atomic)(struct i2c_adapter *a, struct i2c_msg *msgs, int num);
	int	(*smbus_xfer)(struct i2c_adapter *a, u16 addr, unsigned short flags, char rw, u8 cmd, int size, void *data);
	u32	(*functionality)(struct i2c_adapter *a);
};
struct i2c_lock_operations { void (*lock_bus)(struct i2c_adapter *a, unsigned int flags); int (*trylock_bus)(struct i2c_adapter *a, unsigned int flags); void (*unlock_bus)(struct i2c_adapter *a, unsigned int flags); };
struct i2c_adapter_quirks { u64 flags; int max_num_msgs; u16 max_write_len, max_read_len, max_comb_1st_msg_len, max_comb_2nd_msg_len; };
struct i2c_adapter {
	struct module	*owner;
	unsigned int	class;
	const struct i2c_algorithm	*algo;
	void	*algo_data;
	const struct i2c_lock_operations	*lock_ops;
	struct rt_mutex	*bus_lock_unused;
	struct mutex	bus_lock;
	int	timeout;
	int	retries;
	struct device	dev;
	int	nr;
	char	name[48];
	const struct i2c_adapter_quirks	*quirks;
};
struct i2c_client { unsigned short flags; unsigned short addr; char name[I2C_NAME_SIZE]; struct i2c_adapter *adapter; struct device dev; int irq; };
struct i2c_board_info { char type[I2C_NAME_SIZE]; unsigned short flags; unsigned short addr; void *platform_data; int irq; };
struct i2c_device_id { char name[I2C_NAME_SIZE]; unsigned long driver_data; };
struct i2c_driver { const char *name; int (*probe)(struct i2c_client *c); void (*remove)(struct i2c_client *c); const struct i2c_device_id *id_table; struct device_driver driver; };
#define I2C_BOARD_INFO(t, a)	.type = t, .addr = (a)
int	i2c_transfer(struct i2c_adapter *a, struct i2c_msg *msgs, int num);
int	__i2c_transfer(struct i2c_adapter *a, struct i2c_msg *msgs, int num);
int	i2c_add_adapter(struct i2c_adapter *a);
void	i2c_del_adapter(struct i2c_adapter *a);
struct i2c_client	*i2c_new_client_device(struct i2c_adapter *a, const struct i2c_board_info *info);
void	i2c_unregister_device(struct i2c_client *c);
struct i2c_adapter	*i2c_get_adapter(int nr);
void	i2c_put_adapter(struct i2c_adapter *a);
static inline void *i2c_get_adapdata(const struct i2c_adapter *a) { return dev_get_drvdata(&a->dev); }
static inline void i2c_set_adapdata(struct i2c_adapter *a, void *d) { dev_set_drvdata(&a->dev, d); }
static inline void *i2c_get_clientdata(const struct i2c_client *c) { return dev_get_drvdata(&c->dev); }
static inline void i2c_set_clientdata(struct i2c_client *c, void *d) { dev_set_drvdata(&c->dev, d); }
static inline u32 i2c_get_functionality(struct i2c_adapter *a) { return a->algo->functionality(a); }
static inline int i2c_check_functionality(struct i2c_adapter *a, u32 f) { return (f & i2c_get_functionality(a)) == f; }
#define to_i2c_adapter(d)	container_of((d), struct i2c_adapter, dev)
#define to_i2c_client(d)	container_of((d), struct i2c_client, dev)
#define to_i2c_driver(d)	container_of((d), struct i2c_driver, driver)
#define i2c_verify_client(d)	((struct i2c_client *)NULL)
#define i2c_client_has_driver(c)	((c) && (c)->dev.driver)
/* the adapter's lock_ops if it has them (drm_dp_helper, GMBUS), else bus_lock */
void	i2c_lock_bus(struct i2c_adapter *a, unsigned int flags);
void	i2c_unlock_bus(struct i2c_adapter *a, unsigned int flags);
int	i2c_trylock_bus(struct i2c_adapter *a, unsigned int flags);
#define i2c_register_driver(o, d)	0
#define i2c_add_driver(d)	0
#define i2c_del_driver(d)	do { } while (0)
#define i2c_acpi_find_adapter_by_handle(h)	((struct i2c_adapter *)NULL)
struct acpi_resource;
struct acpi_resource_i2c_serialbus;
/* no ACPI resources here (DSI panels described by ACPI) */
static inline bool i2c_acpi_get_i2c_resource(struct acpi_resource *r, struct acpi_resource_i2c_serialbus **sb) { return false; }
#define module_i2c_driver(d)	static int __unused_##d
s32	i2c_smbus_read_byte_data(const struct i2c_client *c, u8 cmd);
s32	i2c_smbus_write_byte_data(const struct i2c_client *c, u8 cmd, u8 v);

/* 9front: no device-managed resources: the adapter stays (Linux's devm_ form) */
#define devm_i2c_add_adapter(dev, a)	i2c_add_adapter(a)

#endif
