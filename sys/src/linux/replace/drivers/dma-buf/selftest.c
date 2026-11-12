// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the runner of Linux's dma-buf tests (ours).  Up
 * to Linux 6.18 drivers/dma-buf/selftest.c ran them as subtests; since
 * 6.19 they are KUnit suites (st-dma-fence*.c, st-dma-resv.c), which
 * KUnit runs in Linux.  Here st_init (aux/i915 -T: kpi_init_st_init)
 * runs each suite's cases, a parameterised case once per parameter,
 * and prints KUnit's result lines; 0 if none failed, else -EINVAL.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/stdarg.h>
#include <linux/errno.h>
#include <kunit/test.h>

extern struct kunit_suite *kpi_kunit_dma_fence_test_suite;
extern struct kunit_suite *kpi_kunit_dma_fence_chain_test_suite;
extern struct kunit_suite *kpi_kunit_dma_fence_unwrap_test_suite;
extern struct kunit_suite *kpi_kunit_dma_resv_test_suite;

void
kpi_kunit_fail(struct kunit *test, const char *file, int line, const char *fmt, ...)
{
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	test->status = KUNIT_FAILURE;
	pr_err("    # %s: EXPECTATION FAILED at %s:%d\n    %s\n", test->name, file, line, buf);
}

/* an assertion or a skip ends the case: back to the runner, if it is the runner's proc */
void
kpi_kunit_end(struct kunit *test)
{
	if (test->task == current)
		__builtin_longjmp(test->jump, 1);
}

/* one run of a case; its status */
static noinline enum kunit_status
run1(struct kunit_suite *suite, struct kunit_case *c, const void *param)
{
	struct kunit test;

	memset(&test, 0, sizeof test);
	test.name = c->name;
	test.param_value = param;
	test.status = KUNIT_SUCCESS;
	test.task = current;
	if (__builtin_setjmp(test.jump) == 0) {
		if (suite->init && suite->init(&test) != 0)
			test.status = KUNIT_FAILURE;
		else
			c->run_case(&test);
	}
	if (suite->exit)
		suite->exit(&test);
	return test.status;
}

static const char *
result(enum kunit_status s)
{
	return s == KUNIT_FAILURE ? "not ok" : "ok";
}

/* the suite's cases; the number of those that failed */
int
kpi_kunit_run(struct kunit_suite *suite)
{
	struct kunit_case *c;
	enum kunit_status s, cs;
	const void *param;
	char desc[KUNIT_PARAM_DESC_SIZE];
	int n, failed, i;

	pr_info("    # Subtest: %s\n", suite->name);
	if (suite->suite_init && suite->suite_init(suite) != 0) {
		pr_err("    # %s: suite_init failed\n", suite->name);
		return 1;
	}
	failed = 0;
	for (n = 1, c = suite->test_cases; c->run_case; c++, n++) {
		if (!c->generate_params) {
			s = run1(suite, c, NULL);
		} else {
			/* each parameter its own run; the case fails if one does, is skipped if all are */
			s = KUNIT_SKIPPED;
			i = 1;
			desc[0] = 0;
			for (param = c->generate_params(NULL, NULL, desc); param; param = c->generate_params(NULL, param, desc)) {
				cs = run1(suite, c, param);
				pr_info("        %s %d %s%s\n", result(cs), i++, desc, cs == KUNIT_SKIPPED ? " # SKIP" : "");
				if (cs == KUNIT_FAILURE)
					s = KUNIT_FAILURE;
				else if (cs == KUNIT_SUCCESS && s == KUNIT_SKIPPED)
					s = KUNIT_SUCCESS;
				desc[0] = 0;
			}
		}
		pr_info("    %s %d %s%s\n", result(s), n, c->name, s == KUNIT_SKIPPED ? " # SKIP" : "");
		if (s == KUNIT_FAILURE)
			failed++;
	}
	if (suite->suite_exit)
		suite->suite_exit(suite);
	pr_info("%s %s\n", failed ? "not ok" : "ok", suite->name);
	return failed;
}

static int __init
st_init(void)
{
	struct kunit_suite *suites[] = {
		kpi_kunit_dma_fence_test_suite,
		kpi_kunit_dma_fence_chain_test_suite,
		kpi_kunit_dma_fence_unwrap_test_suite,
		kpi_kunit_dma_resv_test_suite,
	};
	int i, failed;

	failed = 0;
	for (i = 0; i < ARRAY_SIZE(suites); i++)
		failed += kpi_kunit_run(suites[i]);
	return failed ? -EINVAL : 0;
}

module_init(st_init);
