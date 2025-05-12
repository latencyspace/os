#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/pid.h>

SYSCALL_DEFINE1(procsched, pid_t, pid) {
	struct task_struct *task;
	task = find_task_by_vpid(pid);

	if (!task)
		return -ESRCH;

#ifdef CONFIG_SCHED_INFO
	return task->sched_info.pcount;
#else
	return -ENOSYS;
#endif
}

