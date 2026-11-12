/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pwm.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PWM_H
#define _KPI_LINUX_PWM_H

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/string.h>

/* no PWM controllers here (only on SoCs i915 does not run on with 9front) */
struct pwm_device;
struct pwm_state { u64 period; u64 duty_cycle; int polarity; bool enabled; bool usage_power; };
#define pwm_get(d, n)			((struct pwm_device *)ERR_PTR(-ENODEV))
#define pwm_put(p)			do { } while (0)
#define pwm_apply_state(p, s)		(-ENODEV)
#define pwm_apply_might_sleep(p, s)	(-ENODEV)
#define pwm_init_state(p, s)		do { memset(s, 0, sizeof(*(s))); } while (0)
static inline void pwm_get_state(const struct pwm_device *p, struct pwm_state *s) { memset(s, 0, sizeof(*s)); }
static inline bool pwm_is_enabled(const struct pwm_device *p) { return false; }
static inline unsigned int pwm_get_relative_duty_cycle(const struct pwm_state *s, unsigned int scale) { return s->period ? (unsigned int)(s->duty_cycle * scale / s->period) : 0; }
static inline int pwm_set_relative_duty_cycle(struct pwm_state *s, unsigned int dc, unsigned int scale) { s->duty_cycle = s->period * dc / scale; return 0; }
#define PWM_POLARITY_NORMAL		0

#endif
