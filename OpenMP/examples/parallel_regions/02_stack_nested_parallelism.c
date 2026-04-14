



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>


int main( int argc, char **argv )
{

  int k = 0;

  #pragma omp parallel
  {
    int me = omp_get_thread_num();

    printf("Level 0  thread %d sees k at addr %p, me at addr %p\n",
	   me, &k, &me );

   #pragma omp masked
    fflush(stdout);
   #pragma omp barrier
      
   #pragma omp parallel num_threads(2)
    {
      int j = omp_get_thread_num();

      printf("\tLevel 1  thread %d child of %d sees "
	     "k at addr %p, me at addr %p "
	     "and j at addr %p\n",
	     j, me, &k, &me, &j );            
    }

  }

  return 0;
}
