#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/kernel.h>

#define sys_hello 548

int main(int argc, char *argv[]) {
        int ret = syscall(sys_hello);

        return 0;
}
