/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: kunit/test.h (ours: Linux's KUnit is GPL).  As
 * much of KUnit as Linux's dma-buf tests use (drivers/dma-buf/st-*.c,
 * KUnit since Linux 6.19): suites of cases, parameterised cases,
 * expectations (the case fails and goes on), assertions and skips (the
 * case ends).  replace/drivers/dma-buf/selftest.c runs the suites
 * (aux/i915 -T).  A case ends early by a jump back to the runner, so an
 * assertion only ends it in the proc that runs the case; in another
 * (a test's own kthread) it only fails it.
 */
#ifndef _KPI_KUNIT_TEST_H
#define _KPI_KUNIT_TEST_H

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/sched.h>

enum kunit_status {
	KUNIT_SUCCESS,
	KUNIT_FAILURE,
	KUNIT_SKIPPED,
};

#define KUNIT_PARAM_DESC_SIZE	128

struct kunit {
	const char	*name;		/* the case's */
	const void	*param_value;	/* a parameterised case's parameter */
	enum kunit_status	status;
	struct task_struct	*task;	/* the proc that runs the case */
	void	*jump[5];		/* __builtin_setjmp's, in the runner */
	void	*priv;
};

struct kunit_case {
	void	(*run_case)(struct kunit *test);
	const char	*name;
	const void *	(*generate_params)(struct kunit *test, const void *prev, char *desc);
};

struct kunit_suite {
	const char	*name;
	int	(*suite_init)(struct kunit_suite *suite);
	void	(*suite_exit)(struct kunit_suite *suite);
	int	(*init)(struct kunit *test);
	void	(*exit)(struct kunit *test);
	struct kunit_case	*test_cases;
};

#define KUNIT_CASE(f)	{ .run_case = f, .name = #f }
#define KUNIT_CASE_PARAM(f, gen)	{ .run_case = f, .name = #f, .generate_params = gen }

/* name##_gen_params: the elements of array one after the other, desc from their member */
#define KUNIT_ARRAY_PARAM_DESC(name, array, member)					\
	static const void *								\
	name##_gen_params(struct kunit *test, const void *prev, char *desc)		\
	{										\
		typeof((array)[0]) *p = prev ? (typeof((array)[0]) *)prev + 1 : (array);	\
										\
		if (p >= (array) + ARRAY_SIZE(array))					\
			return NULL;							\
		strscpy(desc, p->member, KUNIT_PARAM_DESC_SIZE);			\
		return p;								\
	}

/*
 * a suite is found by the runner under kpi_kunit_<suite>; there is no
 * section of suites to walk, and a static library brings in only what
 * is named
 */
#define kunit_test_suite(s)	struct kunit_suite *kpi_kunit_##s = &(s)

void	kpi_kunit_fail(struct kunit *test, const char *file, int line, const char *fmt, ...)
	    __attribute__((format(printf, 4, 5)));
void	kpi_kunit_end(struct kunit *test);
int	kpi_kunit_run(struct kunit_suite *suite);

#define KUNIT_FAIL(test, fmt, ...)	\
	kpi_kunit_fail(test, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define kunit_mark_skipped(test, fmt, ...) do {				\
		(test)->status = KUNIT_SKIPPED;				\
		pr_info("    # %s: SKIP " fmt "\n", (test)->name, ##__VA_ARGS__);	\
	} while (0)

#define kunit_skip(test, fmt, ...) do {				\
		kunit_mark_skipped(test, fmt, ##__VA_ARGS__);		\
		kpi_kunit_end(test);					\
	} while (0)

#define KUNIT_EXPECT_EQ_MSG(test, a, b, fmt, ...) do {			\
		typeof(a) _a = (a);					\
		typeof(b) _b = (b);					\
									\
		if (!(_a == _b))					\
			KUNIT_FAIL(test, "%s == %s: %lld != %lld " fmt,	\
			    #a, #b, (long long)_a, (long long)_b, ##__VA_ARGS__);	\
	} while (0)
#define KUNIT_EXPECT_EQ(test, a, b)	KUNIT_EXPECT_EQ_MSG(test, a, b, "")

#define KUNIT_ASSERT_EQ_MSG(test, a, b, fmt, ...) do {			\
		typeof(a) _a = (a);					\
		typeof(b) _b = (b);					\
									\
		if (!(_a == _b)) {					\
			KUNIT_FAIL(test, "%s == %s: %lld != %lld " fmt,	\
			    #a, #b, (long long)_a, (long long)_b, ##__VA_ARGS__);	\
			kpi_kunit_end(test);				\
		}							\
	} while (0)
#define KUNIT_ASSERT_EQ(test, a, b)	KUNIT_ASSERT_EQ_MSG(test, a, b, "")

#define KUNIT_ASSERT_NOT_NULL(test, p) do {				\
		if ((p) == NULL) {					\
			KUNIT_FAIL(test, "%s is NULL", #p);		\
			kpi_kunit_end(test);				\
		}							\
	} while (0)

#endif
