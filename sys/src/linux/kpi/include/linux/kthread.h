/* Public domain. */

#ifndef _LINUX_KTHREAD_H
#define _LINUX_KTHREAD_H

/* both for printf */
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/task.h>

/* 9front: Linux's name is a format (OpenBSD changes the Linux code to a name) */
struct task_struct *__kthread_run(int (*)(void *), void *, const char *);
#define kthread_run(fn, data, fmt, ...) ({				\
	char __kr_name[MAXCOMLEN + 1];					\
	snprintf(__kr_name, sizeof(__kr_name), fmt, ##__VA_ARGS__);	\
	__kthread_run(fn, data, __kr_name);				\
})
void 	kthread_park(struct task_struct *);
void 	kthread_unpark(struct task_struct *);
int	kthread_should_park(void);
void 	kthread_parkme(void);
int	kthread_stop(struct task_struct *);
int	kthread_should_stop(void);

struct kthread_work {
	struct task	 task;
	struct taskq	*tq;
};

struct kthread_worker {
	struct taskq	*tq;
};

struct kthread_worker *
	kthread_run_worker(unsigned int, const char *, ...);
void	kthread_destroy_worker(struct kthread_worker *);
void	kthread_init_work(struct kthread_work *, void (*)(struct kthread_work *));
bool	kthread_queue_work(struct kthread_worker *, struct kthread_work *);
bool	kthread_cancel_work_sync(struct kthread_work *);
void	kthread_flush_work(struct kthread_work *);
void	kthread_flush_worker(struct kthread_worker *);

#define kthread_stop_put(k)	({ int __r = kthread_stop(k); put_task_struct(k); __r; })	/* 9front: Linux 6.7 */

#define kthread_use_mm(mm)	do { } while (0)	/* 9front: no process memory */
#define kthread_unuse_mm(mm)	do { } while (0)

#endif
