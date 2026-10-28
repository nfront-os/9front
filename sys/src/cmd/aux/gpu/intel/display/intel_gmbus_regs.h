/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_gmbus_regs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_DISPLAY_INTEL_GMBUS_REGS_H_
#define _INTELGPU_DISPLAY_INTEL_GMBUS_REGS_H_

#define GMBUS0(__i915)		_MMIO(GMBUS_MMIO_BASE(__i915) + 0x5100)
#define GMBUS1(__i915)		_MMIO(GMBUS_MMIO_BASE(__i915) + 0x5104)
#define   GMBUS_SW_CLR_INT		(1 << 31)
#define   GMBUS_SW_RDY			(1 << 30)
#define   GMBUS_CYCLE_WAIT		(1 << 25)
#define   GMBUS_CYCLE_INDEX		(2 << 25)
#define   GMBUS_CYCLE_STOP		(4 << 25)
#define   GMBUS_BYTE_COUNT_SHIFT	16
#define   GMBUS_SLAVE_INDEX_SHIFT	8
#define   GMBUS_SLAVE_ADDR_SHIFT	1
#define   GMBUS_SLAVE_READ		(1 << 0)
#define GMBUS2(__i915)		_MMIO(GMBUS_MMIO_BASE(__i915) + 0x5108)
#define   GMBUS_HW_RDY			(1 << 11)
#define   GMBUS_SATOER			(1 << 10)
#define   GMBUS_ACTIVE			(1 << 9)
#define GMBUS3(__i915)		_MMIO(GMBUS_MMIO_BASE(__i915) + 0x510c)
#define GMBUS4(__i915)		_MMIO(GMBUS_MMIO_BASE(__i915) + 0x5110)

#endif
