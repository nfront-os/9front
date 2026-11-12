// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the I2C core and the bit-banging algorithm (ours:
 * Linux's drivers/i2c are GPL).  The buses are the driver's own (GMBUS,
 * DP AUX, DDC); clients are only recorded.
 */
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/idr.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>

static DEFINE_IDR(adapters);
static DEFINE_MUTEX(corelock);

void
i2c_lock_bus(struct i2c_adapter *a, unsigned int flags)
{
	if (a->lock_ops)
		a->lock_ops->lock_bus(a, flags);
	else
		mutex_lock(&a->bus_lock);
}

void
i2c_unlock_bus(struct i2c_adapter *a, unsigned int flags)
{
	if (a->lock_ops)
		a->lock_ops->unlock_bus(a, flags);
	else
		mutex_unlock(&a->bus_lock);
}

int
i2c_trylock_bus(struct i2c_adapter *a, unsigned int flags)
{
	if (a->lock_ops)
		return a->lock_ops->trylock_bus(a, flags);
	return mutex_trylock(&a->bus_lock);
}

int
__i2c_transfer(struct i2c_adapter *a, struct i2c_msg *msgs, int num)
{
	unsigned long start = jiffies;
	int r = -EAGAIN, try;

	if (!a->algo->master_xfer)
		return -EOPNOTSUPP;
	for (try = 0; try <= a->retries; try++) {
		r = a->algo->master_xfer(a, msgs, num);
		if (r != -EAGAIN || time_after(jiffies, start + a->timeout))
			break;
	}
	return r;
}

int
i2c_transfer(struct i2c_adapter *a, struct i2c_msg *msgs, int num)
{
	int r;

	if (!a->algo->master_xfer)
		return -EOPNOTSUPP;
	i2c_lock_bus(a, I2C_LOCK_SEGMENT);
	r = __i2c_transfer(a, msgs, num);
	i2c_unlock_bus(a, I2C_LOCK_SEGMENT);
	return r;
}

int
i2c_add_adapter(struct i2c_adapter *a)
{
	int nr;

	mutex_init(&a->bus_lock);
	if (!a->timeout)
		a->timeout = HZ;
	mutex_lock(&corelock);
	nr = idr_alloc(&adapters, a, 0, 0, GFP_KERNEL);
	mutex_unlock(&corelock);
	if (nr < 0)
		return nr;
	a->nr = nr;
	dev_set_name(&a->dev, "i2c-%d", nr);
	return 0;
}

void
i2c_del_adapter(struct i2c_adapter *a)
{
	mutex_lock(&corelock);
	if (idr_find(&adapters, a->nr) == a)
		idr_remove(&adapters, a->nr);
	mutex_unlock(&corelock);
}

struct i2c_adapter *
i2c_get_adapter(int nr)
{
	struct i2c_adapter *a;

	mutex_lock(&corelock);
	a = idr_find(&adapters, nr);
	mutex_unlock(&corelock);
	return a;
}

void i2c_put_adapter(struct i2c_adapter *a) { }

struct i2c_client *
i2c_new_client_device(struct i2c_adapter *a, const struct i2c_board_info *info)
{
	struct i2c_client *c = kzalloc(sizeof(*c), GFP_KERNEL);

	if (!c)
		return ERR_PTR(-ENOMEM);
	c->adapter = a;
	c->addr = info->addr;
	c->flags = info->flags;
	c->irq = info->irq;
	c->dev.platform_data = info->platform_data;
	c->dev.parent = &a->dev;
	strscpy(c->name, info->type, sizeof(c->name));
	dev_set_name(&c->dev, "%d-%04x", a->nr, c->addr);
	return c;
}

void
i2c_unregister_device(struct i2c_client *c)
{
	if (IS_ERR_OR_NULL(c))
		return;
	kfree(c->dev.kobj.name);
	kfree(c);
}

s32
i2c_smbus_read_byte_data(const struct i2c_client *c, u8 cmd)
{
	u8 v;
	struct i2c_msg m[2] = {
		{ .addr = c->addr, .flags = 0, .len = 1, .buf = &cmd },
		{ .addr = c->addr, .flags = I2C_M_RD, .len = 1, .buf = &v },
	};
	int r = i2c_transfer(c->adapter, m, 2);

	return r == 2 ? v : r < 0 ? r : -EIO;
}

s32
i2c_smbus_write_byte_data(const struct i2c_client *c, u8 cmd, u8 v)
{
	u8 b[2] = { cmd, v };
	struct i2c_msg m = { .addr = c->addr, .flags = 0, .len = 2, .buf = b };
	int r = i2c_transfer(c->adapter, &m, 1);

	return r == 1 ? 0 : r < 0 ? r : -EIO;
}

/*
 * bit banging: SDA and SCL driven by the adapter's callbacks; SCL high
 * waits for a device that stretches the clock (if SCL can be read)
 */
static inline struct i2c_algo_bit_data *bitdata(struct i2c_adapter *a) { return a->algo_data; }

static void
halfdelay(struct i2c_algo_bit_data *b)
{
	udelay((b->udelay + 1) / 2);
}

static void sdalo(struct i2c_algo_bit_data *b) { b->setsda(b->data, 0); halfdelay(b); }
static void sdahi(struct i2c_algo_bit_data *b) { b->setsda(b->data, 1); halfdelay(b); }
static void scllo(struct i2c_algo_bit_data *b) { b->setscl(b->data, 0); halfdelay(b); }

static int
sclhi(struct i2c_algo_bit_data *b)
{
	unsigned long start = jiffies;

	b->setscl(b->data, 1);
	if (b->getscl)
		while (!b->getscl(b->data)) {
			if (time_after(jiffies, start + b->timeout)) {
				if (b->getscl(b->data))
					break;
				return -ETIMEDOUT;
			}
			usleep_range(20, 50);
		}
	udelay(b->udelay);
	return 0;
}

static void
start(struct i2c_algo_bit_data *b)
{
	/* SDA falls while SCL is high */
	b->setsda(b->data, 0);
	udelay(b->udelay);
	scllo(b);
}

static int
repstart(struct i2c_algo_bit_data *b)
{
	sdahi(b);
	if (sclhi(b) < 0)
		return -ETIMEDOUT;
	b->setsda(b->data, 0);
	udelay(b->udelay);
	scllo(b);
	return 0;
}

static void
stop(struct i2c_algo_bit_data *b)
{
	/* SDA rises while SCL is high */
	sdalo(b);
	sclhi(b);
	b->setsda(b->data, 1);
	udelay(b->udelay);
}

/* 1: acknowledged, 0: not, <0: error */
static int
outbyte(struct i2c_algo_bit_data *b, u8 c)
{
	int i, ack;

	for (i = 7; i >= 0; i--) {
		b->setsda(b->data, (c >> i) & 1);
		udelay((b->udelay + 1) / 2);
		if (sclhi(b) < 0)
			return -ETIMEDOUT;
		scllo(b);
	}
	sdahi(b);
	if (sclhi(b) < 0)
		return -ETIMEDOUT;
	ack = !b->getsda(b->data);
	scllo(b);
	return ack;
}

static int
inbyte(struct i2c_algo_bit_data *b, bool ack)
{
	int i, c = 0;

	sdahi(b);
	for (i = 0; i < 8; i++) {
		if (sclhi(b) < 0)
			return -ETIMEDOUT;
		c = c << 1 | (b->getsda(b->data) ? 1 : 0);
		b->setscl(b->data, 0);
		udelay(i == 7 ? b->udelay / 2 : b->udelay);
	}
	/* acknowledge (SDA low) all but the last byte read */
	if (ack)
		b->setsda(b->data, 0);
	udelay((b->udelay + 1) / 2);
	if (sclhi(b) < 0) {
		sdahi(b);
		return -ETIMEDOUT;
	}
	scllo(b);
	sdahi(b);
	return c;
}

static int
address(struct i2c_adapter *a, struct i2c_msg *m)
{
	struct i2c_algo_bit_data *b = bitdata(a);
	u8 addr;
	int i, r = 0;

	if (m->flags & I2C_M_TEN)
		return -EOPNOTSUPP;
	addr = m->addr << 1;
	if (m->flags & I2C_M_RD)
		addr |= 1;
	if (m->flags & I2C_M_REV_DIR_ADDR)
		addr ^= 1;
	for (i = 0; i <= a->retries; i++) {
		r = outbyte(b, addr);
		if (r != 0)
			break;
		/* not acknowledged: stop, wait, start again */
		stop(b);
		udelay(b->udelay);
		if (i < a->retries)
			start(b);
	}
	if (r < 0)
		return r;
	return r ? 0 : -ENXIO;
}

static int
bit_xfer(struct i2c_adapter *a, struct i2c_msg *msgs, int num)
{
	struct i2c_algo_bit_data *b = bitdata(a);
	struct i2c_msg *m;
	int i, j, r = 0, c, len;
	bool nak_ok;

	if (b->pre_xfer && (r = b->pre_xfer(a)) < 0)
		return r;
	start(b);
	for (i = 0; i < num; i++) {
		m = &msgs[i];
		nak_ok = m->flags & I2C_M_IGNORE_NAK;
		if (!(m->flags & I2C_M_NOSTART)) {
			if (i && (r = repstart(b)) < 0)
				goto out;
			r = address(a, m);
			if (r && !nak_ok)
				goto out;
		}
		if (m->flags & I2C_M_RD) {
			len = m->len;
			for (j = 0; j < len; j++) {
				c = inbyte(b, j + 1 < len && !(m->flags & I2C_M_NO_RD_ACK));
				if (c < 0) {
					r = c;
					goto out;
				}
				m->buf[j] = c;
				if (j == 0 && (m->flags & I2C_M_RECV_LEN)) {
					if (c <= 0 || c > 32) {
						r = -EPROTO;
						goto out;
					}
					len += c;
					m->len += c;
				}
			}
		} else {
			for (j = 0; j < m->len; j++) {
				r = outbyte(b, m->buf[j]);
				if (r < 0)
					goto out;
				if (r == 0 && !nak_ok) {
					r = -EIO;
					goto out;
				}
			}
		}
	}
	r = num;
out:
	stop(b);
	if (b->post_xfer)
		b->post_xfer(a);
	return r;
}

static u32
bit_func(struct i2c_adapter *a)
{
	return I2C_FUNC_I2C | I2C_FUNC_NOSTART | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_SMBUS_READ_BLOCK_DATA |
		I2C_FUNC_SMBUS_BLOCK_PROC_CALL | I2C_FUNC_PROTOCOL_MANGLING;
}

const struct i2c_algorithm i2c_bit_algo = {
	.master_xfer = bit_xfer,
	.functionality = bit_func,
};

int
i2c_bit_add_bus(struct i2c_adapter *a)
{
	a->algo = &i2c_bit_algo;
	a->retries = 3;
	return i2c_add_adapter(a);
}

int i2c_bit_add_numbered_bus(struct i2c_adapter *a) { return i2c_bit_add_bus(a); }
