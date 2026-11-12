/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/device.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DEVICE_H
#define _KPI_LINUX_DEVICE_H

#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/pm.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/wait.h>
#include <linux/seq_file.h>
#include <linux/completion.h>

/* devices and drivers: one PCI device per file server (kpi/device.c) */
struct device;
struct device_driver;
struct dev_pm_ops {
	int	(*prepare)(struct device *d);
	void	(*complete)(struct device *d);
	int	(*suspend)(struct device *d);
	int	(*resume)(struct device *d);
	int	(*freeze)(struct device *d);
	int	(*thaw)(struct device *d);
	int	(*poweroff)(struct device *d);
	int	(*restore)(struct device *d);
	int	(*suspend_late)(struct device *d);
	int	(*resume_early)(struct device *d);
	int	(*freeze_late)(struct device *d);
	int	(*thaw_early)(struct device *d);
	int	(*poweroff_late)(struct device *d);
	int	(*restore_early)(struct device *d);
	int	(*suspend_noirq)(struct device *d);
	int	(*resume_noirq)(struct device *d);
	int	(*freeze_noirq)(struct device *d);
	int	(*thaw_noirq)(struct device *d);
	int	(*poweroff_noirq)(struct device *d);
	int	(*restore_noirq)(struct device *d);
	int	(*runtime_suspend)(struct device *d);
	int	(*runtime_resume)(struct device *d);
	int	(*runtime_idle)(struct device *d);
};

/* a power domain's callbacks (amdgpu embeds one): after dev_pm_ops */
/* a power domain's callbacks for its devices (runtime PM here only counts) */
struct dev_pm_domain {
	struct dev_pm_ops	ops;
	int	(*start)(struct device *d);
	void	(*detach)(struct device *d, bool power_off);
	int	(*activate)(struct device *d);
	void	(*sync)(struct device *d);
	void	(*dismiss)(struct device *d);
	int	(*set_performance_state)(struct device *d, unsigned int state);
};

struct dev_pm_info { bool is_prepared; bool is_suspended; int usage_count; int disable_depth; };
struct bus_type { const char *name; int (*match)(struct device *d, const struct device_driver *drv); int (*uevent)(const struct device *d, struct kobj_uevent_env *e);
	int (*probe)(struct device *d); void (*remove)(struct device *d); const struct dev_pm_ops *pm; };
struct class { const char *name; };
struct device_type { const char *name; const struct attribute_group **groups; void (*release)(struct device *d); };
struct device_driver { const char *name; const struct bus_type *bus; struct module *owner; const struct dev_pm_ops *pm;
	int (*probe)(struct device *d); int (*remove)(struct device *d); void (*shutdown)(struct device *d);
	const struct attribute_group **dev_groups; bool suppress_bind_attrs; };
struct fwnode_handle { struct fwnode_handle *secondary; const void *ops; struct device *dev; };
struct device_node;
struct dev_iommu;
struct device_dma_parameters { unsigned int max_segment_size; unsigned long segment_boundary_mask; };
struct device {
	struct kobject	kobj;
	struct device	*parent;
	const char	*init_name;
	const struct device_type	*type;
	const struct bus_type	*bus;
	struct device_driver	*driver;
	void	*platform_data;
	void	*driver_data;
	struct dev_pm_info	power;
	u64	*dma_mask;
	u64	coherent_dma_mask;
	struct device_dma_parameters	*dma_parms;
	struct device_node	*of_node;
	struct fwnode_handle	*fwnode;
	dev_t	devt;
	struct class	*class;
	const struct attribute_group	**groups;
	void	(*release)(struct device *d);
	struct list_head	devres_head;
	struct dev_iommu	*iommu;		/* none: no IOMMU */
	void	*kpi;	/* kpi/device.c */
	void	*acpi;	/* 9front: its ACPI node (struct acpi_device), KPI_ACPI_PCI, or NULL: kpi/linux/acpi.c */
};
const char	*dev_name(const struct device *d);
int	dev_set_name(struct device *d, const char *fmt, ...) __printf(2, 3);
static inline void *dev_get_drvdata(const struct device *d) { return d->driver_data; }
static inline void dev_set_drvdata(struct device *d, void *p) { d->driver_data = p; }
static inline void *dev_get_platdata(const struct device *d) { return d->platform_data; }
static inline const char *dev_driver_string(const struct device *d) { return d->driver ? d->driver->name : "?"; }
#define dev_to_node(d)			(-1)
#define dev_is_platform(d)		0
#define dev_pm_set_driver_flags(d, f)	do { } while (0)
#define dev_pm_domain_set(d, p)		do { } while (0)
#define get_device(d)			(d)
#define put_device(d)			do { } while (0)
#define device_initialize(d)		do { } while (0)
#define device_add(d)			0
#define device_del(d)			do { } while (0)
#define device_register(d)		0
#define device_unregister(d)		do { } while (0)
#define device_is_registered(d)		1
#define device_create_file(d, a)	0
#define device_remove_file(d, a)	do { } while (0)
#define device_create_bin_file(d, a)	0
#define device_remove_bin_file(d, a)	do { } while (0)
#define device_link_add(c, s, f)	((struct device_link *)NULL)
#define device_link_del(l)		do { } while (0)
#define device_set_wakeup_capable(d, c)	do { } while (0)
#define device_property_read_u32(d, n, v)	(-EINVAL)
#define device_property_present(d, n)	0
#define device_match_fwnode(d, f)	0
#define device_lock(d)			do { } while (0)
#define device_unlock(d)		do { } while (0)
#define device_find_child(d, data, m)	((struct device *)NULL)
#define of_driver_match_device(d, drv)	0
#define of_property_read_bool(n, p)	0
#define of_property_present(n, p)	0
#define of_parse_phandle(n, p, i)	((struct device_node *)NULL)
#define of_node_put(n)			do { } while (0)
#define of_node_get(n)			(n)
#define of_device_uevent_modalias(d, e)	(-ENODEV)
#define of_device_is_available(n)	0
#define bus_find_device_by_of_node(b, n)	((struct device *)NULL)
#define for_each_available_child_of_node(p, c)	for ((c) = NULL; (c); )
int	device_for_each_child(struct device *parent, void *data, int (*fn)(struct device *d, void *data));
int	bus_register(const struct bus_type *b);
void	bus_unregister(const struct bus_type *b);
int	driver_register(struct device_driver *drv);
void	driver_unregister(struct device_driver *drv);
#define bus_register_notifier(b, n)	0
#define bus_unregister_notifier(b, n)	0
#define device_link_remove(c, s)	do { } while (0)
#define BUS_NOTIFY_ADD_DEVICE		0x00000001
#define BUS_NOTIFY_DEL_DEVICE		0x00000002
#define BUS_NOTIFY_BOUND_DRIVER		0x00000004
#define BUS_NOTIFY_UNBIND_DRIVER	0x00000005
#define BUS_NOTIFY_DRIVER_NOT_BOUND	0x00000008
#define of_fwnode_handle(n)		((struct fwnode_handle *)NULL)
#define device_set_node(d, f)		do { (d)->fwnode = (f); } while (0)
#define device_iommu_mapped(d)		0
#define dev_fwnode(d)			((d)->fwnode)
#define dev_of_node(d)			((d)->of_node)
#define dev_coredumpm(d, o, data, dl, g, r, f)	do { } while (0)
struct device_link;
struct device_attribute { struct attribute attr; ssize_t (*show)(struct device *d, struct device_attribute *a, char *buf);
	ssize_t (*store)(struct device *d, struct device_attribute *a, const char *buf, size_t n); };
#define DEVICE_ATTR(n, m, s, st)	struct device_attribute dev_attr_##n = __ATTR(n, m, s, st)
#define DEVICE_ATTR_RO(n)		struct device_attribute dev_attr_##n = __ATTR_RO(n)
#define DEVICE_ATTR_RW(n)		struct device_attribute dev_attr_##n = __ATTR_RW(n)
#define DEVICE_ATTR_WO(n)		struct device_attribute dev_attr_##n = __ATTR_WO(n)
#define kobj_to_dev(k)			container_of(k, struct device, kobj)
/* devres: resources freed with the device (kpi/device.c) */
typedef void (*dr_release_t)(struct device *d, void *res);
void	*devm_kmalloc(struct device *d, size_t size, gfp_t gfp);
void	*devm_kzalloc(struct device *d, size_t size, gfp_t gfp);
void	devm_kfree(struct device *d, const void *p);
void	kpi_devres_release_all(struct device *d);	/* the driver let go of d */
/* groups: what was added since opening one, released together (id NULL: the latest) */
void	*devres_open_group(struct device *d, void *id, gfp_t gfp);
void	devres_close_group(struct device *d, void *id);
int	devres_release_group(struct device *d, void *id);
int	devm_add_action(struct device *d, void (*action)(void *), void *data);
#define devm_device_add_group(d, g)	0	/* no sysfs: as sysfs_create_group */
#define devm_device_add_groups(d, g)	0
int	devm_add_action_or_reset(struct device *d, void (*action)(void *), void *data);
void	devm_remove_action(struct device *d, void (*action)(void *), void *data);
void	devm_release_action(struct device *d, void (*action)(void *), void *data);
char	*devm_kstrdup(struct device *d, const char *s, gfp_t gfp);
char	*devm_kasprintf(struct device *d, gfp_t gfp, const char *fmt, ...) __printf(3, 4);
#define devm_kcalloc(d, n, s, g)	devm_kzalloc(d, size_mul(n, s), g)
#define devm_kmalloc_array(d, n, s, g)	devm_kmalloc(d, size_mul(n, s), g)
/* printing */
void	kpi_dev_printk(const char *level, const struct device *d, const char *fmt, ...) __printf(3, 4);
#define dev_printk(l, d, fmt, ...)	kpi_dev_printk(l, d, fmt, ##__VA_ARGS__)
#define dev_emerg(d, fmt, ...)		dev_printk(KERN_EMERG, d, fmt, ##__VA_ARGS__)
#define dev_alert(d, fmt, ...)		dev_printk(KERN_ALERT, d, fmt, ##__VA_ARGS__)
#define dev_crit(d, fmt, ...)		dev_printk(KERN_CRIT, d, fmt, ##__VA_ARGS__)
#define dev_err(d, fmt, ...)		dev_printk(KERN_ERR, d, fmt, ##__VA_ARGS__)
#define dev_warn(d, fmt, ...)		dev_printk(KERN_WARNING, d, fmt, ##__VA_ARGS__)
#define dev_notice(d, fmt, ...)		dev_printk(KERN_NOTICE, d, fmt, ##__VA_ARGS__)
#define dev_info(d, fmt, ...)		dev_printk(KERN_INFO, d, fmt, ##__VA_ARGS__)
#define dev_dbg(d, fmt, ...)		do { if (0) dev_printk(KERN_DEBUG, d, fmt, ##__VA_ARGS__); } while (0)
#define dev_vdbg(d, fmt, ...)		dev_dbg(d, fmt, ##__VA_ARGS__)
/* 9front: once per call site, as Linux's (a mode check printed one each frame) */
#define __kpi_dev_once(call) ({				\
	static bool __kpi_printed;			\
	if (!__kpi_printed) {				\
		__kpi_printed = true;			\
		call;					\
	}						\
})
#define dev_err_once(d, fmt, ...)	__kpi_dev_once(dev_err(d, fmt, ##__VA_ARGS__))
#define dev_warn_once(d, fmt, ...)	__kpi_dev_once(dev_warn(d, fmt, ##__VA_ARGS__))
#define dev_info_once(d, fmt, ...)	__kpi_dev_once(dev_info(d, fmt, ##__VA_ARGS__))
#define dev_notice_once(d, fmt, ...)	__kpi_dev_once(dev_notice(d, fmt, ##__VA_ARGS__))
#define dev_dbg_once(d, fmt, ...)	__kpi_dev_once(dev_dbg(d, fmt, ##__VA_ARGS__))
#define dev_err_ratelimited(d, fmt, ...)	dev_err(d, fmt, ##__VA_ARGS__)
#define dev_warn_ratelimited(d, fmt, ...)	dev_warn(d, fmt, ##__VA_ARGS__)
#define dev_notice_ratelimited(d, fmt, ...)	dev_notice(d, fmt, ##__VA_ARGS__)
#define dev_info_ratelimited(d, fmt, ...)	dev_info(d, fmt, ##__VA_ARGS__)
#define dev_dbg_ratelimited(d, fmt, ...)	dev_dbg(d, fmt, ##__VA_ARGS__)
#define dev_err_probe(d, e, fmt, ...)	({ dev_err(d, fmt, ##__VA_ARGS__); (e); })
#define dev_WARN(d, fmt, ...)		WARN(1, fmt, ##__VA_ARGS__)
#define dev_WARN_ONCE(d, c, fmt, ...)	WARN_ONCE(c, fmt, ##__VA_ARGS__)

/* 9front: __free(put_device) (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_FREE(put_device, struct device *, if (_T) put_device(_T))

/* 9front: a GPU in a slot does not go away by itself (Linux 6.7) */
#define dev_is_removable(d)	false

#define devm_kmemdup(d, p, n, gfp)	kmemdup(p, n, gfp)	/* 9front: no device-managed memory */

#endif
