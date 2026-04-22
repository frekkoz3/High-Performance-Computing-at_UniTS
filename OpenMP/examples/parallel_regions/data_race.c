
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(void)
{
  int sum = 0;
  int nthreads;

 #pragma omp parallel
 #pragma omp masked
  nthreads = omp_get_num_threads();
  
 #pragma omp parallel
  {
    int myid = omp_get_thread_num();
    
    // #pragma omp critical
    //or
    // #pragma omp atomic update
    sum += myid;
  }

  int expected_value = nthreads*(nthreads+1) / 2;

  if ( sum == expected_value )    
    printf ( "The resulting sum is %d as expected\n", sum );
  else
    printf ( "!! The resulting sum is %d while the expected value is\n",
	     sum, expected_value );
  
  return 0;
}
