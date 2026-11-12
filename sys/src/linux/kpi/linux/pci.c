// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: PCI.  The device the file server drives comes
 * from #G (its BARs, its interrupt); its configuration space and that
 * of the other devices (host bridge, PCH, ...) from #$/pci.  A PCI
 * driver registered is probed with the device if its id table has it.
 */
#include <linux/pci.h>
#include <linux/acpi.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include "kpi.h"

LIST_HEAD(kpi_pci_root_buses);
static struct pci_bus root = { .number = 0 };
static struct pci_dev *gpu;		/* the device of #G */
static LIST_HEAD(others);		/* the others asked for */
static DEFINE_SPINLOCK(pcilock);

struct kpi_pci {
	struct list_head	node;
	struct pci_dev	*dev;
	u8	saved[64];
	bool	have_saved;
};

void	kpi_intel_graphics_quirks(struct pci_dev *d);

static inline struct kpi_pci *kp(const struct pci_dev *d) { return d->kpi; }

static int
cfg(const struct pci_dev *d, int write, int off, void *buf, int n)
{
	if (d == gpu)
		return p9_pcicfg(write, off, buf, n) < 0 ? -EIO : 0;
	return p9_pcicfgat(d->bus->number, PCI_SLOT(d->devfn), PCI_FUNC(d->devfn), write, off, buf, n) < 0 ? -EIO : 0;
}

int pci_read_config_byte(const struct pci_dev *d, int o, u8 *v) { return cfg(d, 0, o, v, 1); }
int pci_read_config_word(const struct pci_dev *d, int o, u16 *v) { return cfg(d, 0, o, v, 2); }
int pci_read_config_dword(const struct pci_dev *d, int o, u32 *v) { return cfg(d, 0, o, v, 4); }
int pci_write_config_byte(const struct pci_dev *d, int o, u8 v) { return cfg(d, 1, o, &v, 1); }
int pci_write_config_word(const struct pci_dev *d, int o, u16 v) { return cfg(d, 1, o, &v, 2); }
int pci_write_config_dword(const struct pci_dev *d, int o, u32 v) { return cfg(d, 1, o, &v, 4); }

static int
bcfg(struct pci_bus *b, unsigned int devfn, int write, int off, void *buf, int n)
{
	return p9_pcicfgat(b->number, PCI_SLOT(devfn), PCI_FUNC(devfn), write, off, buf, n) < 0 ? -EIO : 0;
}

int pci_bus_read_config_byte(struct pci_bus *b, unsigned int f, int o, u8 *v) { return bcfg(b, f, 0, o, v, 1); }
int pci_bus_read_config_word(struct pci_bus *b, unsigned int f, int o, u16 *v) { return bcfg(b, f, 0, o, v, 2); }
int pci_bus_read_config_dword(struct pci_bus *b, unsigned int f, int o, u32 *v) { return bcfg(b, f, 0, o, v, 4); }
int pci_bus_write_config_byte(struct pci_bus *b, unsigned int f, int o, u8 v) { return bcfg(b, f, 1, o, &v, 1); }
int pci_bus_write_config_word(struct pci_bus *b, unsigned int f, int o, u16 v) { return bcfg(b, f, 1, o, &v, 2); }
int pci_bus_write_config_dword(struct pci_bus *b, unsigned int f, int o, u32 v) { return bcfg(b, f, 1, o, &v, 4); }

/* the identity of d from its configuration space */
static int
readids(struct pci_dev *d)
{
	u32 class;
	u8 rev, irq;

	if (pci_read_config_word(d, PCI_VENDOR_ID, &d->vendor) || d->vendor == 0xffff)
		return -ENODEV;
	pci_read_config_word(d, PCI_DEVICE_ID, &d->device);
	pci_read_config_word(d, PCI_SUBSYSTEM_VENDOR_ID, &d->subsystem_vendor);
	pci_read_config_word(d, PCI_SUBSYSTEM_ID, &d->subsystem_device);
	pci_read_config_dword(d, PCI_REVISION_ID, &class);
	rev = class & 0xff;
	d->revision = rev;
	d->class = class >> 8;
	pci_read_config_byte(d, PCI_INTERRUPT_LINE, &irq);
	d->pm_cap = pci_find_capability(d, PCI_CAP_ID_PM);
	d->msix_cap = pci_find_capability(d, PCI_CAP_ID_MSIX);
	d->current_state = PCI_D0;
	dev_set_name(&d->dev, "0000:%02x:%02x.%d", d->bus->number, PCI_SLOT(d->devfn), PCI_FUNC(d->devfn));
	return 0;
}

static struct pci_dev *
newdev(struct pci_bus *bus, unsigned int devfn)
{
	struct pci_dev *d = kzalloc(sizeof(*d), GFP_KERNEL);
	struct kpi_pci *k = kzalloc(sizeof(*k), GFP_KERNEL);

	if (!d || !k) {
		kfree(d);
		kfree(k);
		return NULL;
	}
	d->bus = bus;
	d->devfn = devfn;
	d->kpi = k;
	d->dev.acpi = KPI_ACPI_PCI;	/* its ACPI node by _ADR, when asked */
	k->dev = d;
	return d;
}

/* the device of #G, made the PCI device of this file server */
struct pci_dev *
kpi_pci_gpu(const char *gdir)
{
	unsigned long long pa, size;
	int b, dv, fn, i, io;
	struct pci_dev *d;
	struct resource *r;

	if (gpu)
		return gpu;
	if (p9_devopen(gdir) < 0 || p9_pciaddr(&b, &dv, &fn) < 0)
		return NULL;
	root.number = b;	/* the GPU's bus; the others are found by number */
	if (list_empty(&kpi_pci_root_buses))
		list_add(&root.node, &kpi_pci_root_buses);
	d = newdev(&root, PCI_DEVFN(dv, fn));
	if (!d)
		return NULL;
	gpu = d;
	if (readids(d)) {
		gpu = NULL;
		return NULL;
	}
	for (i = 0; i < PCI_STD_NUM_BARS; i++) {
		if (p9_barinfo(i, &pa, &size, &io) < 0)
			continue;
		r = &d->resource[i];
		r->start = pa;
		r->end = pa + size - 1;
		r->flags = io ? IORESOURCE_IO : IORESOURCE_MEM;
		r->name = dev_name(&d->dev);
	}
	d->irq = 1;	/* request_irq(1): the irq of #G */
	kpi_intel_graphics_quirks(d);
	return d;
}

/* any other device: made when first asked for */
static struct pci_dev *
lookup(unsigned int busno, unsigned int devfn)
{
	struct kpi_pci *k;
	struct pci_bus *bus;
	struct pci_dev *d;

	if (gpu && gpu->bus->number == busno && gpu->devfn == devfn)
		return gpu;
	spin_lock(&pcilock);
	list_for_each_entry(k, &others, node)
		if (k->dev->bus->number == busno && k->dev->devfn == devfn) {
			spin_unlock(&pcilock);
			return k->dev;
		}
	spin_unlock(&pcilock);
	if (busno == root.number)
		bus = &root;
	else {
		bus = kzalloc(sizeof(*bus), GFP_KERNEL);
		if (!bus)
			return NULL;
		bus->number = busno;
		bus->parent = &root;
	}
	d = newdev(bus, devfn);
	if (!d || readids(d)) {
		if (d) {
			kfree(d->kpi);
			kfree(d);
		}
		if (bus != &root)
			kfree(bus);
		return NULL;
	}
	spin_lock(&pcilock);
	list_add_tail(&kp(d)->node, &others);
	spin_unlock(&pcilock);
	return d;
}

struct pci_dev *
pci_get_domain_bus_and_slot(int domain, unsigned int bus, unsigned int devfn)
{
	if (domain != 0)
		return NULL;
	return lookup(bus, devfn);
}

/* all devices in bus/dev/fn order, as #$/pci lists them */
struct each {
	struct pci_dev	*from;
	bool	past;
	bool	(*match)(struct pci_dev *d, void *arg);
	void	*arg;
	struct pci_dev	*found;
};

static int
eachfn(int b, int dv, int fn, void *arg)
{
	struct each *e = arg;
	struct pci_dev *d;

	d = lookup(b, PCI_DEVFN(dv, fn));
	if (!d)
		return 0;
	if (e->from && !e->past) {
		e->past = d == e->from;
		return 0;
	}
	if (e->match(d, e->arg)) {
		e->found = d;
		return 1;
	}
	return 0;
}

static struct pci_dev *
search(struct pci_dev *from, bool (*match)(struct pci_dev *, void *), void *arg)
{
	struct each e = { from, false, match, arg, NULL };

	p9_pcieach(eachfn, &e);
	return e.found;
}

static bool matchclass(struct pci_dev *d, void *arg) { return d->class == *(unsigned int *)arg; }

struct pci_dev *
pci_get_class(unsigned int class, struct pci_dev *from)
{
	return search(from, matchclass, &class);
}

static bool matchbaseclass(struct pci_dev *d, void *arg) { return (d->class >> 16) == *(unsigned int *)arg; }

struct pci_dev *
pci_get_base_class(unsigned int class, struct pci_dev *from)
{
	return search(from, matchbaseclass, &class);
}

struct subsys { unsigned int v, d, sv, sd; };

static bool
matchsubsys(struct pci_dev *d, void *arg)
{
	struct subsys *s = arg;

	return (s->v == PCI_ANY_ID || s->v == d->vendor) && (s->d == PCI_ANY_ID || s->d == d->device) &&
		(s->sv == PCI_ANY_ID || s->sv == d->subsystem_vendor) && (s->sd == PCI_ANY_ID || s->sd == d->subsystem_device);
}

struct pci_dev *
pci_get_subsys(unsigned int v, unsigned int dv, unsigned int sv, unsigned int sd, struct pci_dev *from)
{
	struct subsys s = { v, dv, sv, sd };

	return search(from, matchsubsys, &s);
}

struct pci_dev *
pci_get_device(unsigned int v, unsigned int d, struct pci_dev *from)
{
	return pci_get_subsys(v, d, PCI_ANY_ID, PCI_ANY_ID, from);
}

const struct pci_device_id *
pci_match_id(const struct pci_device_id *ids, struct pci_dev *d)
{
	if (!ids)
		return NULL;
	for (; ids->vendor || ids->subvendor || ids->class_mask; ids++)
		if ((ids->vendor == PCI_ANY_ID || ids->vendor == d->vendor) &&
		    (ids->device == PCI_ANY_ID || ids->device == d->device) &&
		    (ids->subvendor == PCI_ANY_ID || ids->subvendor == d->subsystem_vendor) &&
		    (ids->subdevice == PCI_ANY_ID || ids->subdevice == d->subsystem_device) &&
		    !((ids->class ^ d->class) & ids->class_mask))
			return ids;
	return NULL;
}

static bool matchids(struct pci_dev *d, void *arg) { return pci_match_id(arg, d) != NULL; }

int
pci_dev_present(const struct pci_device_id *ids)
{
	return search(NULL, matchids, (void *)ids) != NULL;
}

int
pci_find_capability(struct pci_dev *d, int cap)
{
	u8 pos, id;
	u16 status;
	int ttl = 48;

	if (pci_read_config_word(d, PCI_STATUS, &status) || !(status & 0x10))
		return 0;
	pci_read_config_byte(d, PCI_CAPABILITY_LIST, &pos);
	while (pos >= 0x40 && ttl--) {
		pos &= ~3;
		if (pci_read_config_byte(d, pos, &id))
			return 0;
		if (id == 0xff)
			return 0;
		if (id == cap)
			return pos;
		pci_read_config_byte(d, pos + 1, &pos);
	}
	return 0;
}

int
pci_find_ext_capability(struct pci_dev *d, int cap)
{
	int pos = 0x100, ttl = (4096 - 256) / 8;
	u32 h;

	while (ttl-- > 0) {
		if (pci_read_config_dword(d, pos, &h) || h == 0 || h == ~0U)
			return 0;
		if ((int)(h & 0xffff) == cap)
			return pos;
		pos = (h >> 20) & 0xffc;
		if (pos < 0x100)
			return 0;
	}
	return 0;
}

static void
command(struct pci_dev *d, u16 set, u16 clear)
{
	u16 c;

	pci_read_config_word(d, PCI_COMMAND, &c);
	pci_write_config_word(d, PCI_COMMAND, (c & ~clear) | set);
}

int
pci_enable_device(struct pci_dev *d)
{
	int i;
	u16 set = 0;

	for (i = 0; i < PCI_STD_NUM_BARS; i++) {
		if (d->resource[i].flags & IORESOURCE_IO)
			set |= PCI_COMMAND_IO;
		if (d->resource[i].flags & IORESOURCE_MEM)
			set |= PCI_COMMAND_MEMORY;
	}
	command(d, set, 0);
	return 0;
}

void pci_disable_device(struct pci_dev *d) { command(d, 0, PCI_COMMAND_MASTER); }
void pci_set_master(struct pci_dev *d) { command(d, PCI_COMMAND_MASTER, 0); }
void pci_clear_master(struct pci_dev *d) { command(d, 0, PCI_COMMAND_MASTER); }

/* #G turns MSI on itself when the device has it */
int pci_enable_msi(struct pci_dev *d) { d->msi_enabled = 1; return 0; }
void pci_disable_msi(struct pci_dev *d) { d->msi_enabled = 0; }

/* one vector, whatever the kernel chose for #G's interrupt */
int
pci_alloc_irq_vectors(struct pci_dev *d, unsigned int min, unsigned int max, unsigned int flags)
{
	if (min > 1 || max < 1)
		return -ENOSPC;
	if (flags & (PCI_IRQ_MSI | PCI_IRQ_MSIX))
		d->msi_enabled = 1;
	return 1;
}

void
pci_free_irq_vectors(struct pci_dev *d)
{
	d->msi_enabled = 0;
}

int
pci_irq_vector(struct pci_dev *d, unsigned int nr)
{
	return nr == 0 ? (int)d->irq : -EINVAL;
}

/* the size of the device's MSI-X table */
int
pci_msix_vec_count(struct pci_dev *d)
{
	u16 ctl;

	if (d->msix_cap == 0)
		return -EINVAL;
	pci_read_config_word(d, d->msix_cap + PCI_MSIX_FLAGS, &ctl);
	return (ctl & PCI_MSIX_FLAGS_QSIZE) + 1;
}

/* the max payload size it is set to, in bytes */
int
pcie_get_mps(struct pci_dev *d)
{
	int pos;
	u16 ctl;

	pos = pci_find_capability(d, PCI_CAP_ID_EXP);
	if (pos == 0)
		return 128;
	pci_read_config_word(d, pos + PCI_EXP_DEVCTL, &ctl);
	return 128 << ((ctl & PCI_EXP_DEVCTL_PAYLOAD) >> 5);
}

/* gone from the bus: its configuration space reads all ones */
bool
pci_device_is_present(struct pci_dev *d)
{
	u32 id;

	pci_read_config_dword(d, PCI_VENDOR_ID, &id);
	return id != 0xffffffff;
}

/* the speed of LNKCAP2's highest bit, else LNKCAP's max link speed */
enum pci_bus_speed
pcie_get_speed_cap(struct pci_dev *d)
{
	int pos, n;
	u32 cap2, cap;

	pos = pci_find_capability(d, PCI_CAP_ID_EXP);
	if (pos == 0)
		return PCI_SPEED_UNKNOWN;
	pci_read_config_dword(d, pos + PCI_EXP_LNKCAP2, &cap2);
	cap2 &= PCI_EXP_LNKCAP2_SLS;
	if (cap2 != 0) {
		n = fls(cap2) - 1;	/* bit n: speed n */
		return n >= 1 && n <= 6 ? (enum pci_bus_speed)(PCIE_SPEED_2_5GT + n - 1) : PCI_SPEED_UNKNOWN;
	}
	pci_read_config_dword(d, pos + PCI_EXP_LNKCAP, &cap);
	n = cap & PCI_EXP_LNKCAP_SLS;
	return n >= 1 && n <= 6 ? (enum pci_bus_speed)(PCIE_SPEED_2_5GT + n - 1) : PCI_SPEED_UNKNOWN;
}

enum pcie_link_width
pcie_get_width_cap(struct pci_dev *d)
{
	int pos;
	u32 cap;

	pos = pci_find_capability(d, PCI_CAP_ID_EXP);
	if (pos == 0)
		return PCIE_LNK_WIDTH_UNKNOWN;
	pci_read_config_dword(d, pos + PCI_EXP_LNKCAP, &cap);
	return (enum pcie_link_width)((cap & PCI_EXP_LNKCAP_MLW) >> 4);
}

/* Mb/s of a lane at a speed, after the line code (8b/10b, 128b/130b) */
static u32
lanembps(enum pci_bus_speed s)
{
	switch (s) {
	case PCIE_SPEED_2_5GT:	return 2000;
	case PCIE_SPEED_5_0GT:	return 4000;
	case PCIE_SPEED_8_0GT:	return 7877;
	case PCIE_SPEED_16_0GT:	return 15754;
	case PCIE_SPEED_32_0GT:	return 31508;
	case PCIE_SPEED_64_0GT:	return 63015;
	default:		return 0;
	}
}

/*
 * the bandwidth of the device's link as it was trained (LNKSTA): the
 * links further up are the kernel's, not seen here
 */
u32
pcie_bandwidth_available(struct pci_dev *d, struct pci_dev **limiting,
    enum pci_bus_speed *speed, enum pcie_link_width *width)
{
	enum pci_bus_speed s = PCI_SPEED_UNKNOWN;
	enum pcie_link_width w = PCIE_LNK_WIDTH_UNKNOWN;
	int pos, n;
	u16 sta;

	pos = pci_find_capability(d, PCI_CAP_ID_EXP);
	if (pos != 0) {
		pci_read_config_word(d, pos + PCI_EXP_LNKSTA, &sta);
		n = sta & PCI_EXP_LNKSTA_CLS;
		if (n >= 1 && n <= 6)
			s = (enum pci_bus_speed)(PCIE_SPEED_2_5GT + n - 1);
		w = (enum pcie_link_width)((sta & PCI_EXP_LNKSTA_NLW) >> 4);
	}
	if (limiting)
		*limiting = d;
	if (speed)
		*speed = s;
	if (width)
		*width = w;
	return w == PCIE_LNK_WIDTH_UNKNOWN ? 0 : w * lanembps(s);
}

/*
 * atomic operations need every switch and the root port on the way to
 * route and complete them: the bridges above are not seen here, so the
 * device does not get them (it works without, amdgpu's compute needs them)
 */
int
pci_enable_atomic_ops_to_root(struct pci_dev *d, u32 cap_mask)
{
	return -EINVAL;
}

/* 1 when the device has no transactions pending (within a second), else 0 */
int
pci_wait_for_pending_transaction(struct pci_dev *d)
{
	int pos, ms;
	u16 sta;

	pos = pci_find_capability(d, PCI_CAP_ID_EXP);
	if (pos == 0)
		return 1;
	for (ms = 0; ms < 1000; ms += 10) {
		pci_read_config_word(d, pos + PCI_EXP_DEVSTA, &sta);
		if ((sta & PCI_EXP_DEVSTA_TRPND) == 0)
			return 1;
		msleep(10);
	}
	return 0;
}

int
pci_set_power_state(struct pci_dev *d, pci_power_t s)
{
	u16 pmcsr;

	if (s > PCI_D3hot)
		s = PCI_D3hot;
	if (!d->pm_cap || d->current_state == s)
		return 0;
	pci_read_config_word(d, d->pm_cap + PCI_PM_CTRL, &pmcsr);
	pmcsr = (pmcsr & ~3) | s;
	pci_write_config_word(d, d->pm_cap + PCI_PM_CTRL, pmcsr);
	/* D3hot to D0 takes 10 ms, the others 200 us (PCI PM 1.2) */
	if (s == PCI_D3hot || d->current_state == PCI_D3hot)
		msleep(10);
	else
		usleep_range(200, 400);
	d->current_state = s;
	return 0;
}

int
pci_save_state(struct pci_dev *d)
{
	int i;

	for (i = 0; i < 64; i += 4)
		pci_read_config_dword(d, i, (u32 *)&kp(d)->saved[i]);
	kp(d)->have_saved = true;
	return 0;
}

void
pci_restore_state(struct pci_dev *d)
{
	int i;

	if (!kp(d)->have_saved)
		return;
	/* the header past the ids, the command register last */
	for (i = 60; i >= 8; i -= 4)
		pci_write_config_dword(d, i, *(u32 *)&kp(d)->saved[i]);
	pci_write_config_word(d, PCI_COMMAND, *(u16 *)&kp(d)->saved[PCI_COMMAND]);
}

void __iomem *
pci_iomap_range(struct pci_dev *d, int bar, unsigned long off, unsigned long max)
{
	resource_size_t len = pci_resource_len(d, bar);

	if (!len || off >= len)
		return NULL;
	len -= off;
	if (max && len > max)
		len = max;
	if (pci_resource_flags(d, bar) & IORESOURCE_IO)
		return NULL;
	return ioremap(pci_resource_start(d, bar) + off, len);
}

void __iomem *pci_iomap(struct pci_dev *d, int bar, unsigned long max) { return pci_iomap_range(d, bar, 0, max); }
void pci_iounmap(struct pci_dev *d, void __iomem *a) { iounmap(a); }

/* the option ROM is not reached from here: VBT comes from elsewhere */
struct pci_saved_state {
	u8	cfg[64];
};

struct pci_saved_state *
pci_store_saved_state(struct pci_dev *d)
{
	struct pci_saved_state *s;

	if (!kp(d)->have_saved)
		return NULL;
	s = kmalloc(sizeof(*s), GFP_KERNEL);
	if (s)
		memcpy(s->cfg, kp(d)->saved, sizeof(s->cfg));
	return s;
}

int
pci_load_saved_state(struct pci_dev *d, struct pci_saved_state *s)
{
	if (!s) {
		kp(d)->have_saved = false;
		return 0;
	}
	memcpy(kp(d)->saved, s->cfg, sizeof(s->cfg));
	kp(d)->have_saved = true;
	return 0;
}

int
pci_load_and_free_saved_state(struct pci_dev *d, struct pci_saved_state **s)
{
	int r = pci_load_saved_state(d, *s);

	kfree(*s);
	*s = NULL;
	return r;
}

void __iomem *pci_map_rom(struct pci_dev *d, size_t *size) { *size = 0; return NULL; }
void pci_unmap_rom(struct pci_dev *d, void __iomem *rom) { }

/* the kernel placed the BARs: no resizing or moving */
int pci_resize_resource(struct pci_dev *d, int resno, int size, int exclude_bars) { return -EOPNOTSUPP; }
void pci_release_resource(struct pci_dev *d, int resno) { }
u32 pci_rebar_get_possible_sizes(struct pci_dev *d, int bar) { return 0; }
void pci_assign_unassigned_bus_resources(struct pci_bus *b) { }

int
pci_bus_alloc_resource(struct pci_bus *b, struct resource *r, resource_size_t size, resource_size_t align,
	resource_size_t min, unsigned long type, resource_size_t (*alignf)(void *, const struct resource *, resource_size_t, resource_size_t), void *data)
{
	return -ENOMEM;
}

resource_size_t
pcibios_align_resource(void *data, const struct resource *r, resource_size_t size, resource_size_t align)
{
	return r->start;
}

const char *
pci_power_name(pci_power_t s)
{
	static const char *const names[] = { "D0", "D1", "D2", "D3hot", "D3cold", "unknown" };

	return s >= 0 && s <= PCI_UNKNOWN ? names[s] : "error";
}

/* drivers: the one whose ids have the device is probed with it */
static struct pci_driver *bound;

int
pci_register_driver(struct pci_driver *drv)
{
	const struct pci_device_id *id;
	int r;

	if (!gpu || bound)
		return 0;
	id = pci_match_id(drv->id_table, gpu);
	if (!id)
		return 0;
	gpu->driver = drv;
	gpu->dev.driver = &drv->driver;
	if (!drv->driver.name)
		drv->driver.name = drv->name;
	r = drv->probe(gpu, id);
	if (r) {
		kpi_devres_release_all(&gpu->dev);
		gpu->driver = NULL;
		gpu->dev.driver = NULL;
		return r;
	}
	bound = drv;
	return 0;
}

void
pci_unregister_driver(struct pci_driver *drv)
{
	if (bound != drv)
		return;
	if (drv->remove)
		drv->remove(gpu);
	kpi_devres_release_all(&gpu->dev);
	gpu->driver = NULL;
	gpu->dev.driver = NULL;
	bound = NULL;
}
