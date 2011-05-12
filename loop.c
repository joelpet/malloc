#include <stdio.h>
#define TIMES 10

main()
{
  int i;
  int svar;

  printf("Now we are going to prompt you %d times\n", TIMES);


  for(i=0; i < TIMES; i++) 
    {
      printf("Give a number: ");
      scanf("%d", &svar);
      printf("You gave the number %d\n", svar);
    }
}
