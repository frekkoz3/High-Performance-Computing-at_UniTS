
/* ────────────────────────────────────────────────────────────────────────── *
 │                                                                            │
 │ This file is part of the exercises for the Lectures on                     │
 │   "Foundations of High Performance Computing"                              │
 │ given at                                                                   │
 │   Master in HPC and                                                        │
 │   Master in Data Science and Scientific Computing                          │
 │ @ SISSA, ICTP and University of Trieste                                    │
 │                                                                            │
 │ contact: luca.tornatore@inaf.it                                            │
 │                                                                            │
 │     This is free software; you can redistribute it and/or modify           │
 │     it under the terms of the GNU General Public License as published by   │
 │     the Free Software Foundation; either version 3 of the License, or      │
 │     (at your option) any later version.                                    │
 │     This code is distributed in the hope that it will be useful,           │
 │     but WITHOUT ANY WARRANTY; without even the implied warranty of         │
 │     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          │
 │     GNU General Public License for more details.                           │
 │                                                                            │
 │     You should have received a copy of the GNU General Public License      │
 │     along with this program.  If not, see <http://www.gnu.org/licenses/>   │
 │                                                                            │
 * ────────────────────────────────────────────────────────────────────────── */

/*
 * This code is part of a set of codes that uses a trivial algorithm
 * for the estimate of pi greek to illustrate some basic facts
 * about performance in OpenMP.
 *
 * -- version 2 --
 * discriminate points and accumulate valid ones using a local accumulator; 
 * we expect the best performance respect to the usage of critical,
 * both in terms of run-time and of scalability.
 * the expected performance is the same than using the reduce construct
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>


#define CPU_TIME_W ({ struct timespec ts; (clock_gettime( CLOCK_REALTIME, &ts ), \
(double)ts.tv_sec + (double)ts.tv_nsec * 1e-9); })

#define CPU_TIME_T ({ struct timespec myts; (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), \
(double)myts.tv_sec + (double)myts.tv_nsec * 1e-9); })

#define CPU_TIME_P ({ struct timespec myts; (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &myts ), \
(double)myts.tv_sec + (double)myts.tv_nsec * 1e-9); })

#define myprintf(l, ...) { if( (l) > verbose_level ) printf(__VA_ARGS__); }

#define DEFAULT 1000000


int main ( int argc, char **argv)
{

  long long unsigned int N = (argc > 1 ? atoll(argv[1]) : DEFAULT ) ;
  long long unsigned int Nvalid = 0;
  int                    nthreads;
  int                    verbose_level = -1;
  
  
 #pragma omp parallel  
 #pragma omp master
  nthreads = omp_get_num_threads();    

 #if defined(SCALABILITY)
  verbose_level = 3;
  double *thread_times = (double*)malloc( nthreads*sizeof(double) );
 #endif
  
  myprintf(0, "omp calculation with %d threads\nN=%Ld\n", nthreads ,N);

  double timing = omp_get_wtime();
 #pragma omp parallel
  {
    int myid = omp_get_thread_num();

    long int SEED = time(NULL);
    int unsigned short myseeds[3] = {(short)(SEED+(myid)),
				     (short)(SEED^(myid*3+1)),
				     (short)(SEED&(myid*4+2))};
    seed48( myseeds );
    // ------------------------------------------

    double mytime = CPU_TIME_T;
    long long unsigned int local_valid_points = 0;
    for( long long unsigned int i = 0; i < N; i++)
      {
	double x = erand48(myseeds); 
	double y = erand48(myseeds);

	local_valid_points += ( (x*x + y*y) < 1.0 );
      }
    mytime = CPU_TIME_T - mytime;

   #pragma omp atomic update
    Nvalid += local_valid_points;

   #if !defined(SCALABILITY)
    printf ( "\tthread %2d timing: %g\n", myid, mytime );

   #else
    /*
     * =============================================================  
     *
     *  output for scalability study
     * .............................................................
     */

    // 1. Save this thread's execution time into the shared array
    thread_times[myid] = mytime;

    // 2. Ensure all threads have recorded their times 
    #pragma omp barrier

    // 3. Have a single thread compute and print the
    //    aggregate statistics
    #pragma omp single
    {
        double min_time = thread_times[0];
        double max_time = thread_times[0];
        double sum_time = 0.0;

        // Calculate Min, Max, and Sum
        for (int i = 0; i < nthreads; i++) {
	  if (thread_times[i] < min_time) min_time = thread_times[i];
	  if (thread_times[i] > max_time) max_time = thread_times[i];
	  sum_time += thread_times[i];
        }

        double avg_time = sum_time / nthreads;

        // Calculate Variance for Standard Deviation
        double sum_sq_diff = 0.0;
        for (int i = 0; i < nthreads; i++)
	  sum_sq_diff += (thread_times[i] - avg_time) *
	    (thread_times[i] - avg_time);
        
        // Compute Standard Deviation (Population)
        double std_dev = sqrt(sum_sq_diff / nthreads);

        // Output the metrics on a single line for easier recollection
	printf ( "%d %g %g %g %g %g %g\n", nthreads,
		 min_time, max_time, avg_time, std_dev,
		 avg_time - std_dev, avg_time + std_dev );

	free ( thread_times );
	/*
        printf("\n--- Loop Timing Statistics ---\n");
        printf("Min timing:           %g\n", min_time);
        printf("Max timing:           %g\n", max_time);
        printf("Average (Avg):        %g\n", avg_time);
        printf("Std Deviation:        %g\n", std_dev);
        printf("Avg - StdDev:         %g\n", avg_time - std_dev);
        printf("Avg + StdDev:         %g\n", avg_time + std_dev);
        printf("------------------------------\n\n");
	*/
    }
   #endif

    /*
     * =============================================================
     */

  }    
    
  timing = omp_get_wtime() - timing;
  
  myprintf(0, "Estimation of pi: %1.9f\n Walltime:%g\n",
	   (4.0*(double)Nvalid)/(N*nthreads), timing );
  return 0;
}
