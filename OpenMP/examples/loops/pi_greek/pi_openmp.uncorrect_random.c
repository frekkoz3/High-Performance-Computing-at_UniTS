#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define DEFAULT 1000000
#define SEED    9918273

int main ( int argc, char **argv)
{

  long long unsigned int M = 0;
  int                    nthreads;
  
 #pragma omp parallel  
 #pragma omp master
  nthreads = omp_get_num_threads();    

  long long int N = (argc > 1 ? atoll(argv[1]) : DEFAULT ) ;
  printf("omp calculation with %d threads\nN=%Ld\n", nthreads ,N);

  double timing = omp_get_wtime();
 #pragma omp parallel
  {
    int myid = omp_get_thread_num();

    // -------------------------------------------
    //
    // set-up the random generator
    
   #if defined(USE_CORRECT_RND)
    // one of the possible thread-safe choices
    //
    int unsigned short myseeds[3] = {SEED+(myid),SEED^(myid*3+1), SEED&(myid*4+2)};
    seed48( myseeds );
   #define RND_VAL erand48(myseeds)
    
   #else
    // a thread-unsafe choice that results in a
    // corrupted series of values
    //
    srand48(SEED*(myid+1));
   #define RND_VAL drand48()

   #endif
    // ------------------------------------------

    for( long long unsigned int i = 0; i < N; i++)
      {
	double x = RND_VAL; 
	double y = RND_VAL;

       #pragma omp critical
	M += ( (x*x + y*y) < 1.0 );
      }

  }    
    
  timing = omp_get_wtime() - timing;
  
  printf("Estimation of pi: %1.9f\n Walltime:%g\n",
	 (4.0*(double)M)/(N*nthreads), timing );
  return 0;
}
