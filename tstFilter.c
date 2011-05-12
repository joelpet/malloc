/*

This is small filter (i.e. a program which reads from stdin (fd = 0) and writes to stdout (fd = 1)) which can be used to test pipes. When used with more than one argument it will clone its input/output to stderr.

Robert 101111

*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  int tkn; char string[500];

  fprintf(stderr, "Filter with pid %d started\n", getpid());

  while((tkn = getchar())!= EOF) { 
    if(argc > 1) 
      fprintf(stderr, "pid %d:%c\n", getpid(), tkn);
    putchar(tkn);
  }

  if(errno) {
    sprintf(string, "ERROR: Filter with pid %d exiting with errno %d", getpid(), errno);
    perror(string);
  }
  else fprintf(stderr, "Filter with pid %d exiting\n", getpid());
}

    
