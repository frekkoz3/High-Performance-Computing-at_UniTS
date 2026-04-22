
#include <time.h>

#define CPU_TIME_W ({ struct timespec ts; (clock_gettime( CLOCK_REALTIME, &ts ), \
(double)ts.tv_sec + (double)ts.tv_nsec * 1e-9); })

#define CPU_TIME_T ({ struct timespec myts; (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), \
(double)myts.tv_sec + (double)myts.tv_nsec * 1e-9); })

#define CPU_TIME_P ({ struct timespec myts; (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &myts ), \
(double)myts.tv_sec + (double)myts.tv_nsec * 1e-9); })
