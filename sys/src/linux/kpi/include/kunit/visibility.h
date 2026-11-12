/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: kunit/visibility.h (ours): no KUnit, so what it would see is static and not exported */
#ifndef _KPI_KUNIT_VISIBILITY_H
#define _KPI_KUNIT_VISIBILITY_H
#define VISIBLE_IF_KUNIT	static
#define EXPORT_SYMBOL_IF_KUNIT(s)
#endif
