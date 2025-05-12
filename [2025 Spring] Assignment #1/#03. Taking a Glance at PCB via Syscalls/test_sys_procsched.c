#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>

#define sys_procsched 549

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <pid>\n", argv[0]);
		return 1;
	}

	pid_t pid = atoi(argv[1]);
	int result = syscall(sys_procsched, pid);
	if (result < 0) {
		perror("syscall failed");
		return 1;
	}

	printf("pcount of process %d = %d\n", pid, result);
	return 0;
}

