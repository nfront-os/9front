/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pci.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PCI_H
#define _KPI_LINUX_PCI_H

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/mod_devicetable.h>
#include <linux/io.h>
#include <linux/pm.h>
#include <linux/log2.h>

/*
 * the PCI device the file server drives (kpi/pci.c: configuration space
 * through #$/pci, BARs and interrupts through #G)
 */
#define PCI_VENDOR_ID		0x00
#define PCI_DEVICE_ID		0x02
#define PCI_COMMAND		0x04
#define  PCI_COMMAND_IO		0x1
#define  PCI_COMMAND_MEMORY	0x2
#define  PCI_COMMAND_MASTER	0x4
#define PCI_STATUS		0x06
#define PCI_REVISION_ID		0x08
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e
#define PCI_CAPABILITY_LIST	0x34
#define PCI_INTERRUPT_LINE	0x3c
#define PCI_CAP_ID_PM		0x01
#define PCI_CAP_ID_EXP		0x10
#define PCI_CAP_ID_MSIX		0x11
#define PCI_MSIX_FLAGS		2	/* MSI-X: message control */
#define  PCI_MSIX_FLAGS_QSIZE	0x07ff	/* table size - 1 */
#define  PCI_MSIX_FLAGS_ENABLE	0x8000
#define PCI_EXP_DEVCTL		8	/* PCI Express: device control */
#define  PCI_EXP_DEVCTL_PAYLOAD	0x00e0	/* max payload size: 128 << n */
#define PCI_EXP_DEVSTA		10	/* device status */
#define  PCI_EXP_DEVSTA_TRPND	0x0020	/* transactions pending */
#define PCI_EXP_LNKCAP		12	/* link capabilities */
#define  PCI_EXP_LNKCAP_SLS	0x0000000f	/* max link speed */
#define  PCI_EXP_LNKCAP_MLW	0x000003f0	/* max link width */
#define PCI_EXP_LNKSTA		18	/* link status */
#define  PCI_EXP_LNKSTA_CLS	0x000f	/* current link speed */
#define  PCI_EXP_LNKSTA_NLW	0x03f0	/* negotiated link width */
#define PCI_EXP_DEVCAP2		36	/* device capabilities 2 */
#define  PCI_EXP_DEVCAP2_ATOMIC_COMP32	0x00000080
#define  PCI_EXP_DEVCAP2_ATOMIC_COMP64	0x00000100
#define PCI_EXP_DEVCTL2		40	/* device control 2 */
#define PCI_EXP_LNKCAP2		44	/* link capabilities 2 */
#define  PCI_EXP_LNKCAP2_SLS	0x000000fe	/* supported speeds: bit n+1 for speed n+1 */
#define PCI_PM_CTRL		4
#define PCI_VENDOR_ID_INTEL	0x8086
#define PCI_VENDOR_ID_ATI	0x1002
#define PCI_VENDOR_ID_AMD	0x1022
#define PCI_VENDOR_ID_APPLE	0x106b
#define PCI_VENDOR_ID_ASUSTEK	0x1043
#define PCI_VENDOR_ID_DELL	0x1028
#define PCI_VENDOR_ID_HP	0x103c
#define PCI_VENDOR_ID_SONY	0x104d
#define PCI_VENDOR_ID_ACER	0x1025
#define PCI_CLASS_DISPLAY_VGA	0x0300
#define PCI_CLASS_DISPLAY_OTHER	0x0380
#define PCI_CLASS_BRIDGE_ISA	0x0601
#define PCI_CLASS_ACCELERATOR_PROCESSING	0x1200
#define PCI_BASE_CLASS_DISPLAY	0x03
#define PCI_DEVID(bus, devfn)	((u16)(((bus) << 8) | (devfn)))
#define PCI_DEVFN(s, f)		((((s) & 0x1f) << 3) | ((f) & 0x07))
#define PCI_SLOT(d)		(((d) >> 3) & 0x1f)
#define PCI_FUNC(d)		((d) & 0x07)
#define PCI_STD_NUM_BARS	6
#define DEVICE_COUNT_RESOURCE	17
#define PCI_ROM_RESOURCE	6
#define PCI_IOBASE		0
typedef int pci_power_t;
#define PCI_D0			((pci_power_t)0)
#define PCI_D1			((pci_power_t)1)
#define PCI_D2			((pci_power_t)2)
#define PCI_D3hot		((pci_power_t)3)
#define PCI_D3cold		((pci_power_t)4)
#define PCI_UNKNOWN		((pci_power_t)5)
struct pci_bus { struct list_head node; unsigned char number; struct pci_bus *parent; struct pci_dev *self; struct resource *resource[4]; void *sysdata; };
struct pci_driver;
/* PCI error recovery (Linux's names; nothing here reports such errors) */
typedef unsigned int pci_channel_state_t;
enum pci_channel_state {
	pci_channel_io_normal = 1,
	pci_channel_io_frozen = 2,
	pci_channel_io_perm_failure = 3,
};
typedef unsigned int pci_ers_result_t;
enum pci_ers_result {
	PCI_ERS_RESULT_NONE = 1,
	PCI_ERS_RESULT_CAN_RECOVER = 2,
	PCI_ERS_RESULT_NEED_RESET = 3,
	PCI_ERS_RESULT_DISCONNECT = 4,
	PCI_ERS_RESULT_RECOVERED = 5,
	PCI_ERS_RESULT_NO_AER_DRIVER = 6,
};
struct pci_dev;
struct pci_error_handlers {
	pci_ers_result_t (*error_detected)(struct pci_dev *, pci_channel_state_t);
	pci_ers_result_t (*mmio_enabled)(struct pci_dev *);
	pci_ers_result_t (*slot_reset)(struct pci_dev *);
	void (*reset_prepare)(struct pci_dev *);
	void (*reset_done)(struct pci_dev *);
	void (*resume)(struct pci_dev *);
};

struct pci_dev {
	u32	saved_config_space[16];	/* 9front: the configuration header, as saved before a reset */
	struct pci_bus	*bus;
	unsigned int	devfn;
	unsigned short	vendor, device, subsystem_vendor, subsystem_device;
	unsigned int	class;
	u8	revision;
	u8	pm_cap;
	unsigned int	irq;
	struct device	dev;
	struct pci_driver	*driver;
	pci_power_t	current_state;
	struct resource	resource[DEVICE_COUNT_RESOURCE];
	u8	msix_cap;	/* offset of the MSI-X capability, 0 without */
	phys_addr_t	rom;	/* a ROM image the platform has (none) */
	size_t	romlen;
	unsigned int	msi_enabled:1;
	unsigned int	msix_enabled:1;
	unsigned int	is_virtfn:1;
	unsigned int	no_d1d2:1;
	unsigned int	d3cold_allowed:1;
	unsigned int	is_thunderbolt:1;
	unsigned int	untrusted:1;
	void	*kpi;	/* kpi/pci.c */
};
struct pci_driver {
	const char	*name;
	const struct pci_device_id	*id_table;
	int	(*probe)(struct pci_dev *d, const struct pci_device_id *id);
	void	(*remove)(struct pci_dev *d);
	void	(*shutdown)(struct pci_dev *d);
	const struct attribute_group	**dev_groups;
	const struct pci_error_handlers	*err_handler;
	struct device_driver	driver;
};
#define to_pci_dev(d)			container_of(d, struct pci_dev, dev)
#define dev_is_pci(d)			1
#define PCI_DEVICE(v, d)		.vendor = (v), .device = (d), .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID
#define PCI_VDEVICE(v, d)		.vendor = PCI_VENDOR_ID_##v, .device = (d), .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID
#define pci_resource_start(d, b)	((d)->resource[b].start)
#define pci_resource_end(d, b)		((d)->resource[b].end)
#define pci_resource_flags(d, b)	((d)->resource[b].flags)
#define pci_resource_len(d, b)		(pci_resource_end(d, b) ? resource_size(&(d)->resource[b]) : 0)
#define pci_resource_n(d, b)		(&(d)->resource[b])
#define pci_domain_nr(b)		0
#define pci_dev_put(d)			do { } while (0)
#define pci_dev_get(d)			(d)
#define pci_name(d)			dev_name(&(d)->dev)
static inline void *pci_get_drvdata(struct pci_dev *d) { return dev_get_drvdata(&d->dev); }
static inline void pci_set_drvdata(struct pci_dev *d, void *p) { dev_set_drvdata(&d->dev, p); }
int	pci_read_config_byte(const struct pci_dev *d, int where, u8 *v);
int	pci_read_config_word(const struct pci_dev *d, int where, u16 *v);
int	pci_read_config_dword(const struct pci_dev *d, int where, u32 *v);
int	pci_write_config_byte(const struct pci_dev *d, int where, u8 v);
int	pci_write_config_word(const struct pci_dev *d, int where, u16 v);
int	pci_write_config_dword(const struct pci_dev *d, int where, u32 v);
int	pci_bus_read_config_byte(struct pci_bus *b, unsigned int devfn, int where, u8 *v);
int	pci_bus_read_config_word(struct pci_bus *b, unsigned int devfn, int where, u16 *v);
int	pci_bus_read_config_dword(struct pci_bus *b, unsigned int devfn, int where, u32 *v);
int	pci_bus_write_config_byte(struct pci_bus *b, unsigned int devfn, int where, u8 v);
int	pci_bus_write_config_word(struct pci_bus *b, unsigned int devfn, int where, u16 v);
int	pci_bus_write_config_dword(struct pci_bus *b, unsigned int devfn, int where, u32 v);
struct pci_dev	*pci_get_domain_bus_and_slot(int domain, unsigned int bus, unsigned int devfn);
struct pci_dev	*pci_get_class(unsigned int class, struct pci_dev *from);
struct pci_dev	*pci_get_device(unsigned int vendor, unsigned int device, struct pci_dev *from);
struct pci_dev	*pci_get_subsys(unsigned int vendor, unsigned int device, unsigned int ss_vendor, unsigned int ss_device, struct pci_dev *from);
int	pci_dev_present(const struct pci_device_id *ids);
const struct pci_device_id	*pci_match_id(const struct pci_device_id *ids, struct pci_dev *d);
int	pci_find_capability(struct pci_dev *d, int cap);
int	pci_find_ext_capability(struct pci_dev *d, int cap);
int	pci_enable_device(struct pci_dev *d);
void	pci_disable_device(struct pci_dev *d);
void	pci_set_master(struct pci_dev *d);
void	pci_clear_master(struct pci_dev *d);
int	pci_enable_msi(struct pci_dev *d);
void	pci_disable_msi(struct pci_dev *d);
int	pci_set_power_state(struct pci_dev *d, pci_power_t s);
int	pci_save_state(struct pci_dev *d);
void	pci_restore_state(struct pci_dev *d);
void __iomem	*pci_iomap(struct pci_dev *d, int bar, unsigned long max);
void __iomem	*pci_iomap_range(struct pci_dev *d, int bar, unsigned long off, unsigned long max);
void	pci_iounmap(struct pci_dev *d, void __iomem *a);
void __iomem	*pci_map_rom(struct pci_dev *d, size_t *size);
/*
 * interrupt vectors: one, the interrupt #G delivers (the kernel chooses
 * MSI where the device has it)
 */
#define PCI_IRQ_LEGACY			(1 << 0)
#define PCI_IRQ_INTX			PCI_IRQ_LEGACY
#define PCI_IRQ_MSI			(1 << 1)
#define PCI_IRQ_MSIX			(1 << 2)
#define PCI_IRQ_ALL_TYPES		(PCI_IRQ_LEGACY | PCI_IRQ_MSI | PCI_IRQ_MSIX)
int	pci_alloc_irq_vectors(struct pci_dev *d, unsigned int min, unsigned int max, unsigned int flags);
void	pci_free_irq_vectors(struct pci_dev *d);
int	pci_irq_vector(struct pci_dev *d, unsigned int nr);
int	pci_msix_vec_count(struct pci_dev *d);
static inline u16 pci_dev_id(struct pci_dev *d) { return PCI_DEVID(d->bus->number, d->devfn); }
int	pcie_get_mps(struct pci_dev *d);
/* the link: speeds as PCIe numbers them (LNKCAP's encoding), widths in lanes */
enum pci_bus_speed {
	PCIE_SPEED_2_5GT = 0x14,
	PCIE_SPEED_5_0GT = 0x15,
	PCIE_SPEED_8_0GT = 0x16,
	PCIE_SPEED_16_0GT = 0x17,
	PCIE_SPEED_32_0GT = 0x18,
	PCIE_SPEED_64_0GT = 0x19,
	PCI_SPEED_UNKNOWN = 0xff,
};
enum pcie_link_width {
	PCIE_LNK_WIDTH_RESRV = 0,
	PCIE_LNK_X1 = 1,
	PCIE_LNK_X2 = 2,
	PCIE_LNK_X4 = 4,
	PCIE_LNK_X8 = 8,
	PCIE_LNK_X12 = 12,
	PCIE_LNK_X16 = 16,
	PCIE_LNK_X32 = 32,
	PCIE_LNK_WIDTH_UNKNOWN = 0xff,
};
enum pci_bus_speed	pcie_get_speed_cap(struct pci_dev *d);
enum pcie_link_width	pcie_get_width_cap(struct pci_dev *d);
u32	pcie_bandwidth_available(struct pci_dev *d, struct pci_dev **limiting,
	    enum pci_bus_speed *speed, enum pcie_link_width *width);
int	pci_enable_atomic_ops_to_root(struct pci_dev *d, u32 cap_mask);
/* the kernel resets devices (no function reset here) */
static inline int pci_reset_function(struct pci_dev *d) { return -ENOTTY; }
static inline bool pci_pr3_present(struct pci_dev *d) { return false; }	/* ACPI _PR3: no namespace here */
/* a copy of the saved configuration (pci_save_state), to load back later */
struct pci_saved_state;
struct pci_saved_state	*pci_store_saved_state(struct pci_dev *d);
int	pci_load_saved_state(struct pci_dev *d, struct pci_saved_state *s);
int	pci_load_and_free_saved_state(struct pci_dev *d, struct pci_saved_state **s);
/* the kernel owns link power management and the device's presence */
static inline bool pcie_aspm_enabled(struct pci_dev *d) { return false; }
bool	pci_device_is_present(struct pci_dev *d);
int	pci_wait_for_pending_transaction(struct pci_dev *d);
/* peer to peer DMA between devices: not here */
static inline int pci_p2pdma_distance(struct pci_dev *provider, struct device *client, bool verbose) { return -1; }
void	pci_unmap_rom(struct pci_dev *d, void __iomem *rom);
int	pci_resize_resource(struct pci_dev *d, int resno, int size, int exclude_bars);	/* 9front: Linux 6.18's form */
void	pci_release_resource(struct pci_dev *d, int resno);
u32	pci_rebar_get_possible_sizes(struct pci_dev *d, int bar);
int	pci_register_driver(struct pci_driver *drv);
void	pci_unregister_driver(struct pci_driver *drv);
void	pci_assign_unassigned_bus_resources(struct pci_bus *b);
int	pci_bus_alloc_resource(struct pci_bus *b, struct resource *r, resource_size_t size, resource_size_t align, resource_size_t min, unsigned long type, resource_size_t (*alignf)(void *, const struct resource *, resource_size_t, resource_size_t), void *data);
const char	*pci_power_name(pci_power_t s);
#define pci_rebar_bytes_to_size(b)	(ilog2(b) - 20)
resource_size_t	pcibios_align_resource(void *data, const struct resource *r, resource_size_t size, resource_size_t align);
#define PCIBIOS_MIN_MEM			0x10000000
#define pcie_find_root_port(d)		((struct pci_dev *)NULL)
#define PCI_STD_RESOURCES		0
#define PCI_STD_RESOURCE_END		5
#define PCI_SUBDEVICE_ID_QEMU		0x1100
#define PCI_SUBVENDOR_ID_REDHAT_QUMRANET	0x1af4
#define PCI_VENDOR_ID_REDHAT_QUMRANET	0x1af4
#define pci_is_root_bus(b)		(!(b)->parent)
#define pci_is_thunderbolt_attached(d)	0
#define pci_d3cold_disable(d)		do { } while (0)
#define pci_d3cold_enable(d)		do { } while (0)
#define pci_ignore_hotplug(d)		do { } while (0)
#define pci_wake_from_d3(d, e)		0
#define pci_is_enabled(d)		1
#define pci_dev_run_wake(d)		0
#define pci_bus_for_each_resource(b, r, ...)	for (unsigned int __i = 0; __i < 4 && ((r) = (b)->resource[__i], 1); __i++)
#define pci_dev_is_disconnected(d)	0
#define dev_is_untrusted(d)		0
#define pci_root_buses			kpi_pci_root_buses
extern struct list_head kpi_pci_root_buses;
#define module_pci_driver(d)		static int __unused_##d

#include <linux/dma-mapping.h>	/* Linux's pci.h brings it */

/* 9front: a read from a device that is gone or failed: all ones (Linux 5.17) */
#define PCI_ERROR_RESPONSE		(~0ULL)
#define PCI_SET_ERROR_RESPONSE(val)	(*(val) = ((typeof(*(val)))PCI_ERROR_RESPONSE))
#define PCI_POSSIBLE_ERROR(val)		((val) == ((typeof(val))PCI_ERROR_RESPONSE))

/* 9front: more of the PCI specification's numbers (Linux 7.3's drivers use them) */
#ifndef PCI_BRIDGE_CONTROL
#define PCI_BRIDGE_CONTROL		0x3e
#define PCI_BRIDGE_CTL_VGA		0x08
#endif
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_SERIAL_USB_XHCI	0x0c0330
#define PCI_EXT_CAP_ID_VNDR		0x0b

/* 9front: the first device after from whose base class (the class's top byte) is class */
struct pci_dev	*pci_get_base_class(unsigned int class, struct pci_dev *from);
/* the bridge above the device: 9front's PCI gives no tree */
#define pci_upstream_bridge(d)	((struct pci_dev *)NULL)

/* 9front: the device's PCIe port type (Linux's from its PCIe capability): none known here */
#define pci_pcie_type(d)	0
/* no resizable bars here (pci_rebar_get_possible_sizes is 0) */
#define pci_rebar_get_max_size(d, bar)	(-ENOTSUPP)

#define pci_restore_msi_state(d)	do { } while (0)	/* 9front: #G keeps the interrupt */
#define pci_rebar_size_supported(d, bar, size)	false
#ifndef PCI_EXP_TYPE_DOWNSTREAM
#define PCI_EXP_TYPE_UPSTREAM	0x5
#define PCI_EXP_TYPE_DOWNSTREAM	0x6
#endif

/* 9front: PCIe capability registers, not read here (aqua_vanjaram: MI300) */
static inline int pcie_capability_read_word(struct pci_dev *d, int pos, u16 *v) { *v = 0; return -EINVAL; }

#define PCI_EXT_CAP_ID_ERR	0x01

#define PCI_ERR_COR_STATUS	0x10	/* AER's correctable error status */

/* AER's registers (PCIe spec) */
#define PCI_ERR_UNCOR_STATUS	0x04

#ifndef PCI_PRIMARY_BUS
#define PCI_PRIMARY_BUS		0x18
#endif

#endif
