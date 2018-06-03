/* Minimal init that shuts the system down gracefully after the real executable
 * has exited */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef EXECUTE
# error "EXECUTE must be defined"
#endif

int main(int argc, char **argv)
{
	struct sigaction sa;
	pid_t p;

	(void)argc;

	/* Restart waitpid() syscall rather than failing if SIGCHLD is received
	 * in middle of call. */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGCHLD, &sa, NULL);

	p = fork();
	if (p == 0) {
		/* child */
		argv[0] = EXECUTE;
		execv(argv[0], argv);
		perror("execv failed");
		exit(1);
	} else if (p > 0) {
		/* parent */
		waitpid(p, NULL, 0);
    uint64_t cycle;
    asm volatile("rdcycle %0": "=r"(cycle));
    printf("cycle = %llx\n", cycle);
		reboot(RB_POWER_OFF);
		for (;;)
			sleep(1);
	}
	/* fail */
	perror("fork failed");
	exit(2);
}
