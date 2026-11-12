// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: devices, drivers, buses, devres, power
 * management callbacks.  A file server drives one device; buses and
 * drivers are recorded, not matched against other devices.
 */
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/hwmon.h>
#include <linux/backlight.h>
#include <linux/mutex.h>
#include <linux/err.h>

const char *
dev_name(const struct device *d)
{
	if (d->kobj.name)
		return d->kobj.name;
	return d->init_name ? d->init_name : "";
}

int
dev_set_name(struct device *d, const char *fmt, ...)
{
	const char *old = d->kobj.name;
	va_list ap;

	va_start(ap, fmt);
	d->kobj.name = kvasprintf(GFP_KERNEL, fmt, ap);
	va_end(ap);
	kfree(old);
	return d->kobj.name ? 0 : -ENOMEM;
}

/* the attributes of a kobject: a kobj_attribute's show and store (as Linux's) */
static ssize_t
kobj_attr_show(struct kobject *k, struct attribute *a, char *buf)
{
	struct kobj_attribute *ka = container_of(a, struct kobj_attribute, attr);

	return ka->show ? ka->show(k, ka, buf) : -EIO;
}

static ssize_t
kobj_attr_store(struct kobject *k, struct attribute *a, const char *buf, size_t n)
{
	struct kobj_attribute *ka = container_of(a, struct kobj_attribute, attr);

	return ka->store ? ka->store(k, ka, buf, n) : -EIO;
}

const struct sysfs_ops kobj_sysfs_ops = {
	.show = kobj_attr_show,
	.store = kobj_attr_store,
};

void
kobject_init(struct kobject *k, const struct kobj_type *t)
{
	k->ktype = t;
	kref_init(&k->kref);
}

static int
kobject_vset_name(struct kobject *k, const char *fmt, va_list ap)
{
	const char *old = k->name;

	k->name = kvasprintf(GFP_KERNEL, fmt, ap);
	kfree(old);
	return k->name ? 0 : -ENOMEM;
}

int
kobject_set_name(struct kobject *k, const char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = kobject_vset_name(k, fmt, ap);
	va_end(ap);
	return r;
}

/* under its parent (else its kset's), in its kset's list */
static int
kobject_join(struct kobject *k, struct kobject *parent)
{
	if (!k->name)
		return -EINVAL;
	if (!parent && k->kset)
		parent = &k->kset->kobj;
	k->parent = kobject_get(parent);
	if (k->kset) {
		kobject_get(&k->kset->kobj);
		spin_lock(&k->kset->list_lock);
		list_add_tail(&k->entry, &k->kset->list);
		spin_unlock(&k->kset->list_lock);
	}
	return 0;
}

static int
kobject_vadd(struct kobject *k, struct kobject *parent, const char *fmt, va_list ap)
{
	int r;

	if (fmt) {
		r = kobject_vset_name(k, fmt, ap);
		if (r)
			return r;
	}
	return kobject_join(k, parent);
}

int
kobject_add(struct kobject *k, struct kobject *parent, const char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = kobject_vadd(k, parent, fmt, ap);
	va_end(ap);
	return r;
}

int
kobject_init_and_add(struct kobject *k, const struct kobj_type *t, struct kobject *parent, const char *fmt, ...)
{
	va_list ap;
	int r;

	kobject_init(k, t);
	va_start(ap, fmt);
	r = kobject_vadd(k, parent, fmt, ap);
	va_end(ap);
	return r;
}

struct kobject *
kobject_get(struct kobject *k)
{
	if (k)
		kref_get(&k->kref);
	return k;
}

static void
kobject_release(struct kref *r)
{
	struct kobject *k = container_of(r, struct kobject, kref);
	struct kobject *parent = k->parent;
	struct kset *ks = k->kset;
	const char *name = k->name;

	if (ks) {	/* out of the set before its type releases it */
		spin_lock(&ks->list_lock);
		list_del_init(&k->entry);
		spin_unlock(&ks->list_lock);
	}
	if (k->ktype && k->ktype->release)
		k->ktype->release(k);
	else if (k->kpi == (void *)kobject_release)	/* kobject_create_and_add's */
		kfree(k);
	kfree(name);
	kobject_put(parent);
	if (ks)
		kobject_put(&ks->kobj);
}

int
kset_register(struct kset *k)
{
	INIT_LIST_HEAD(&k->list);
	spin_lock_init(&k->list_lock);
	kref_init(&k->kobj.kref);
	return kobject_join(&k->kobj, k->kobj.parent);
}

void
kset_unregister(struct kset *k)
{
	if (k)
		kobject_put(&k->kobj);
}

void
kobject_put(struct kobject *k)
{
	if (k && kref_read(&k->kref))
		kref_put(&k->kref, kobject_release);
}

struct kobject *
kobject_create_and_add(const char *name, struct kobject *parent)
{
	struct kobject *k = kzalloc(sizeof(*k), GFP_KERNEL);

	if (!k)
		return NULL;
	kobject_init(k, NULL);
	k->kpi = (void *)kobject_release;
	if (kobject_add(k, parent, "%s", name)) {
		kfree(k);
		return NULL;
	}
	return k;
}

int
device_for_each_child(struct device *parent, void *data, int (*fn)(struct device *d, void *data))
{
	/* children are not recorded: the drivers' own (i2c, aux) are reached otherwise */
	return 0;
}

static DEFINE_SPINLOCK(reglock);
static const struct bus_type *buses[16];
static struct device_driver *drivers[32];

int
bus_register(const struct bus_type *b)
{
	int i, r = -ENOSPC;

	spin_lock(&reglock);
	for (i = 0; i < (int)ARRAY_SIZE(buses); i++)
		if (!buses[i]) {
			buses[i] = b;
			r = 0;
			break;
		}
	spin_unlock(&reglock);
	return r;
}

void
bus_unregister(const struct bus_type *b)
{
	int i;

	spin_lock(&reglock);
	for (i = 0; i < (int)ARRAY_SIZE(buses); i++)
		if (buses[i] == b)
			buses[i] = NULL;
	spin_unlock(&reglock);
}

int
driver_register(struct device_driver *drv)
{
	int i, r = -ENOSPC;

	spin_lock(&reglock);
	for (i = 0; i < (int)ARRAY_SIZE(drivers); i++)
		if (!drivers[i]) {
			drivers[i] = drv;
			r = 0;
			break;
		}
	spin_unlock(&reglock);
	return r;
}

void
driver_unregister(struct device_driver *drv)
{
	int i;

	spin_lock(&reglock);
	for (i = 0; i < (int)ARRAY_SIZE(drivers); i++)
		if (drivers[i] == drv)
			drivers[i] = NULL;
	spin_unlock(&reglock);
}

/*
 * devres: what a driver allocated or registered for its device, undone
 * in reverse order when the driver lets go of it (kpi_devres_release_all)
 */
struct devres {
	struct list_head	node;
	void	(*action)(void *);	/* an action, or NULL for memory */
	void	*data;
	u8	mem[] __aligned(16);
};

static DEFINE_SPINLOCK(devreslock);

static void
devres_init(struct device *d)
{
	if (!d->devres_head.next)
		INIT_LIST_HEAD(&d->devres_head);
}

static void
devres_add(struct device *d, struct devres *dr)
{
	spin_lock(&devreslock);
	devres_init(d);
	list_add_tail(&dr->node, &d->devres_head);
	spin_unlock(&devreslock);
}

void *
devm_kmalloc(struct device *d, size_t size, gfp_t gfp)
{
	struct devres *dr = kmalloc(sizeof(*dr) + size, gfp);

	if (!dr)
		return NULL;
	dr->action = NULL;
	dr->data = NULL;
	if (gfp & __GFP_ZERO)
		memset(dr->mem, 0, size);
	devres_add(d, dr);
	return dr->mem;
}

void *devm_kzalloc(struct device *d, size_t size, gfp_t gfp) { return devm_kmalloc(d, size, gfp | __GFP_ZERO); }

void
devm_kfree(struct device *d, const void *p)
{
	struct devres *dr;

	if (!p)
		return;
	dr = container_of((void *)p, struct devres, mem);
	spin_lock(&devreslock);
	list_del(&dr->node);
	spin_unlock(&devreslock);
	kfree(dr);
}

char *
devm_kstrdup(struct device *d, const char *s, gfp_t gfp)
{
	size_t n;
	char *p;

	if (!s)
		return NULL;
	n = strlen(s) + 1;
	p = devm_kmalloc(d, n, gfp);
	if (p)
		memcpy(p, s, n);
	return p;
}

char *
devm_kasprintf(struct device *d, gfp_t gfp, const char *fmt, ...)
{
	va_list ap;
	char *s, *p;

	va_start(ap, fmt);
	s = kvasprintf(gfp, fmt, ap);
	va_end(ap);
	if (!s)
		return NULL;
	p = devm_kstrdup(d, s, gfp);
	kfree(s);
	return p;
}

int
devm_add_action(struct device *d, void (*action)(void *), void *data)
{
	struct devres *dr = kmalloc(sizeof(*dr), GFP_KERNEL);

	if (!dr)
		return -ENOMEM;
	dr->action = action;
	dr->data = data;
	devres_add(d, dr);
	return 0;
}

int
devm_add_action_or_reset(struct device *d, void (*action)(void *), void *data)
{
	int r = devm_add_action(d, action, data);

	if (r)
		action(data);
	return r;
}

static struct devres *
findaction(struct device *d, void (*action)(void *), void *data)
{
	struct devres *dr;

	devres_init(d);
	list_for_each_entry_reverse(dr, &d->devres_head, node)
		if (dr->action == action && dr->data == data)
			return dr;
	return NULL;
}

void
devm_remove_action(struct device *d, void (*action)(void *), void *data)
{
	struct devres *dr;

	spin_lock(&devreslock);
	dr = findaction(d, action, data);
	if (dr)
		list_del(&dr->node);
	spin_unlock(&devreslock);
	WARN_ON(!dr);
	kfree(dr);
}

void
devm_release_action(struct device *d, void (*action)(void *), void *data)
{
	struct devres *dr;

	spin_lock(&devreslock);
	dr = findaction(d, action, data);
	if (dr)
		list_del(&dr->node);
	spin_unlock(&devreslock);
	if (WARN_ON(!dr))
		return;
	action(data);
	kfree(dr);
}

/* a group's start: an entry without action whose data is the group's id */
static void groupmark(void *data) { }

void *
devres_open_group(struct device *d, void *id, gfp_t gfp)
{
	struct devres *dr = kmalloc(sizeof(*dr), gfp);

	if (!dr)
		return NULL;
	dr->action = groupmark;
	dr->data = id ? id : dr;
	devres_add(d, dr);
	return dr->data;
}

void devres_close_group(struct device *d, void *id) { }

/* release, newest first, what came after the group's start (and the start) */
int
devres_release_group(struct device *d, void *id)
{
	struct devres *dr, *mark = NULL;
	int n = 0;

	spin_lock(&devreslock);
	devres_init(d);
	list_for_each_entry_reverse(dr, &d->devres_head, node)
		if (dr->action == groupmark && (!id || dr->data == id)) {
			mark = dr;
			break;
		}
	spin_unlock(&devreslock);
	if (!mark)
		return -ENOENT;
	for (;;) {
		spin_lock(&devreslock);
		dr = list_last_entry(&d->devres_head, struct devres, node);
		list_del(&dr->node);
		spin_unlock(&devreslock);
		if (dr == mark) {
			kfree(dr);
			break;
		}
		if (dr->action)
			dr->action(dr->data);
		kfree(dr);
		n++;
	}
	return n;
}

void
kpi_devres_release_all(struct device *d)
{
	struct devres *dr;

	for (;;) {
		spin_lock(&devreslock);
		devres_init(d);
		if (list_empty(&d->devres_head)) {
			spin_unlock(&devreslock);
			break;
		}
		dr = list_last_entry(&d->devres_head, struct devres, node);
		list_del(&dr->node);
		spin_unlock(&devreslock);
		if (dr->action && dr->action != groupmark)
			dr->action(dr->data);
		kfree(dr);
	}
}

/* backlights, by name */
static LIST_HEAD(backlights);
static DEFINE_MUTEX(backlightlock);

struct kpi_backlight {
	struct list_head	node;
	struct backlight_device	bd;
};

struct backlight_device *
backlight_device_register(const char *name, struct device *parent, void *data,
    const struct backlight_ops *ops, const struct backlight_properties *props)
{
	struct kpi_backlight *b;

	b = kzalloc(sizeof(*b), GFP_KERNEL);
	if (b == NULL)
		return ERR_PTR(-ENOMEM);
	mutex_init(&b->bd.update_lock);
	mutex_init(&b->bd.ops_lock);
	b->bd.ops = ops;
	b->bd.priv = data;
	if (props)
		b->bd.props = *props;
	b->bd.dev.parent = parent;
	b->bd.dev.init_name = kstrdup(name, GFP_KERNEL);
	mutex_lock(&backlightlock);
	list_add_tail(&b->node, &backlights);
	mutex_unlock(&backlightlock);
	return &b->bd;
}

void
backlight_device_unregister(struct backlight_device *bd)
{
	struct kpi_backlight *b;

	if (IS_ERR_OR_NULL(bd))
		return;
	b = container_of(bd, struct kpi_backlight, bd);
	mutex_lock(&backlightlock);
	list_del(&b->node);
	mutex_unlock(&backlightlock);
	mutex_lock(&bd->ops_lock);
	bd->ops = NULL;
	mutex_unlock(&bd->ops_lock);
	kfree(bd->dev.init_name);
	kfree(b);
}

static void
devm_backlight_release(void *b)
{
	backlight_device_unregister(b);
}

struct backlight_device *
devm_backlight_device_register(struct device *dev, const char *name, struct device *parent, void *data,
    const struct backlight_ops *ops, const struct backlight_properties *props)
{
	struct backlight_device *b;
	int r;

	b = backlight_device_register(name, parent, data, ops, props);
	if (IS_ERR(b))
		return b;
	r = devm_add_action_or_reset(dev, devm_backlight_release, b);
	return r ? ERR_PTR(r) : b;
}

struct backlight_device *
backlight_device_get_by_name(const char *name)
{
	struct kpi_backlight *b;
	struct backlight_device *r = NULL;

	mutex_lock(&backlightlock);
	list_for_each_entry(b, &backlights, node)
		if (b->bd.dev.init_name && strcmp(b->bd.dev.init_name, name) == 0) {
			r = &b->bd;
			break;
		}
	mutex_unlock(&backlightlock);
	return r;
}

int
backlight_device_set_brightness(struct backlight_device *bd, unsigned long brightness)
{
	int r = -ENXIO;

	mutex_lock(&bd->ops_lock);
	if (bd->ops) {
		if (brightness > bd->props.max_brightness)
			r = -EINVAL;
		else {
			bd->props.brightness = brightness;
			r = backlight_update_status(bd);
		}
	}
	mutex_unlock(&bd->ops_lock);
	return r;
}

/* hardware monitors: a device of their own, under the driver's, with its attribute groups */
struct device *
hwmon_device_register_with_groups(struct device *parent, const char *name, void *drvdata,
    const struct attribute_group **groups)
{
	struct device *d;

	d = kzalloc(sizeof(*d), GFP_KERNEL);
	if (d == NULL)
		return ERR_PTR(-ENOMEM);
	d->parent = parent;
	d->init_name = name;
	d->groups = groups;
	dev_set_drvdata(d, drvdata);
	return d;
}

void
hwmon_device_unregister(struct device *d)
{
	kfree(d);
}

/* power management: no system sleep here, the machine stays on */
suspend_state_t pm_suspend_target_state = PM_SUSPEND_ON;

/* the driver's callbacks */
#define PMCALL(name, op) \
int name(struct device *d) \
{ const struct dev_pm_ops *pm = d->driver ? d->driver->pm : NULL; return pm && pm->op ? pm->op(d) : 0; }
PMCALL(pm_generic_suspend, suspend)
PMCALL(pm_generic_resume, resume)
PMCALL(pm_generic_freeze, freeze)
PMCALL(pm_generic_thaw, thaw)
PMCALL(pm_generic_poweroff, poweroff)
PMCALL(pm_generic_restore, restore)
PMCALL(pm_generic_runtime_suspend, runtime_suspend)
PMCALL(pm_generic_runtime_resume, runtime_resume)
