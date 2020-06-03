#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
void sig_handler(int signo);
int main(int argc, char **argv){ 

	signal(SIGINT, sig_handler);
	signal(SIGTSTP, sig_handler);
	signal(SIGCONT, sig_handler);
	while(1) {
		sleep(2);
	}

	return 0;
}


void sig_handler(int signo)
{
    printf("Loop handle's with signal %s\n" , strsignal(signo));
	if(signo == SIGTSTP)
	{
		signal(SIGCONT , sig_handler);
	}
	if(signo == SIGCONT){
		signal(SIGTSTP , sig_handler);
	}
	signal(signo, SIG_DFL);
	raise(signo);
}