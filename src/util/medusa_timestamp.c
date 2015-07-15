#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA TIMESTAMP DIFF
   ---------------------------------------------------------------------------*/
t_medusa_timestamp medusa_timestamp_diff(
      t_medusa_timestamp start,
      t_medusa_timestamp end){
   t_medusa_timestamp temp;
   if ((end.tv_sec - start.tv_sec) > 0) {
      temp.tv_sec = end.tv_sec - start.tv_sec - 1;
      temp.tv_nsec = 1000000000 - start.tv_nsec + end.tv_nsec;
   } else {
      temp.tv_sec = end.tv_sec - start.tv_sec;
      temp.tv_nsec = end.tv_nsec - start.tv_nsec;
   }
   return temp;
}

/* -----------------------------------------------------------------------------
   MEDUSA TIMESTAMP NOW
   ---------------------------------------------------------------------------*/
t_medusa_timestamp medusa_timestamp_now(void){
   struct timespec timestamp;
   clock_gettime(CLOCK_REALTIME, &timestamp);
   t_medusa_timestamp ts;
   ts.tv_sec = timestamp.tv_sec;
   ts.tv_nsec = timestamp.tv_nsec;
   return ts;
}

/* -----------------------------------------------------------------------------
   MEDUSA TIMESTAMP MILLIS
   ---------------------------------------------------------------------------*/
float medusa_timestamp_millis(t_medusa_timestamp my_time){
   return ((float)my_time.tv_sec) * 1000.0f + ((float)my_time.tv_nsec) / 1000000.0f;
}

/* -----------------------------------------------------------------------------
   MEDUSA TIMESTAMP INT
   ---------------------------------------------------------------------------*/
uint64_t medusa_timestamp_int(t_medusa_timestamp my_time){
   uint64_t res = 0;
   res = my_time.tv_sec;
   res = res << 32;
   res = res | my_time.tv_nsec;
   return res;
}

/* -----------------------------------------------------------------------------
   MEDUSA TIMESTAMP COMPARE
   ---------------------------------------------------------------------------*/
int medusa_timestamp_compare(
      t_medusa_timestamp start,
      t_medusa_timestamp end
      ){
   if(start.tv_sec == end.tv_sec
      && start.tv_nsec == end.tv_nsec)
      return 1;
   return 0;
}

/*----------------------------------------------------------------------------*/
