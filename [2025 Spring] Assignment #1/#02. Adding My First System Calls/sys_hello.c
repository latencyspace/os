#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(hello) {
	printk(KERN_INFO "Hello, Jihwan Park!\n");
	printk(KERN_INFO "184128\n");
	return 0;
}
