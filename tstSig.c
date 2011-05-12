#include <signal.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char * argv[])
{
  pid_t pid;

  fprintf(stderr,"Child %d started and is sending SIGCHLD to parent\n", getpid());

  kill(getppid(), SIGCHLD);

  fprintf(stderr, "Child %d is not yet dead, just gone to sleep\n", getpid());
  sleep(100);

  fprintf(stderr,"Child %d is finally exiting\n", getpid());
}
