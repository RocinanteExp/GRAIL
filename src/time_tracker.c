#include <sys/time.h>
#include <stddef.h>

long get_now() {
   struct timeval time;
   gettimeofday(&time, NULL);
   long time_ms = time.tv_sec*1000.0 + time.tv_usec*1.0/1000.0;
   return time_ms;
}
