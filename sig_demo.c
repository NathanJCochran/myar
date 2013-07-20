#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigusr1_handler(int signal) {
	printf("Caught SIGUSR1\n");
}

void sigusr2_handler(int signal) {
	printf("Caught SIGUSR2\n");
}

void sigint_handler(int signal) {
	printf("Caught SIGINT, exiting\n");
	exit(0);
}

int main(int argc, char * argv[]) {
	pid_t pid = getpid();

	struct sigaction sigusr1, old_sigusr1;
	struct sigaction sigusr2, old_sigusr2;
	struct sigaction sigint, old_sigint;

	sigusr1.sa_handler = sigusr1_handler;
	sigemptyset(&sigusr1.sa_mask);
	sigusr1.sa_flags = 0;
	sigaction(SIGUSR1, &sigusr1, &old_sigusr1);

	sigusr2.sa_handler = sigusr2_handler;
	sigemptyset(&sigusr2.sa_mask);
	sigusr2.sa_flags = 0;
	sigaction(SIGUSR2, &sigusr2, &old_sigusr2);

	sigint.sa_handler = sigint_handler;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = 0;
	sigaction(SIGINT, &sigint, &old_sigint);

	kill(pid, SIGUSR1);
	kill(pid, SIGUSR2);
	kill(pid, SIGINT);

	return 0;
}
