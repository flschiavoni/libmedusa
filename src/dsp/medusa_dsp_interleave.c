#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA DSP INTERLEAVED SEPARE
   ---------------------------------------------------------------------------*/
int medusa_dsp_interleaved_separe(
         void * origin,
         void * dest_l,
         void * dest_r,
         int data_size,
         int sample_size){

   int i = 0;
   for(i = 0 ; i < (int) (data_size / sample_size); i++){
      if(i % 2 == 0){
         memcpy(dest_l + ( (i >> 1) * sample_size), origin + (i * sample_size),
               sample_size);
      }else{
         memcpy(dest_r + ( (i >> 1) * sample_size), origin + (i * sample_size),
               sample_size);
      }
   }
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP INTERLEAVED MERGE
   ---------------------------------------------------------------------------*/
int medusa_dsp_interleaved_merge(
         void * origin_l,
         void * origin_r,
         void * dest,
         int data_size,
         int sample_size){

   int i = 0;
   for(i = 0 ; i < (int) (data_size / sample_size) ; i++){
      if(i % 2 == 0)
         memcpy(dest + (i * sample_size), origin_l + ( (i >> 1) * sample_size),
               sample_size);
      else
         memcpy(dest + (i * sample_size), origin_r + ( (i >> 1) * sample_size),
               sample_size);
   }
   return 1;
}

/*----------------------------------------------------------------------------*/
