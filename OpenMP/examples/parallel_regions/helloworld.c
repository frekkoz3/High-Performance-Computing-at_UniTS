#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


int main ( void )
{
  #pragma omp parallel
  {
    int myid = omp_get_thread_num();
    printf("hello world from thread %d\n", myid);
  }
  
  return 0;
}
