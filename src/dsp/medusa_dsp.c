#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA DSP ADJUST BLOCK SIZE
   ---------------------------------------------------------------------------*/
uint16_t medusa_dsp_adjust_block_size(uint16_t block_size){
   block_size = (block_size < 16)?16:block_size;
   block_size = (block_size > 16 && block_size < 32)    ?16  :block_size;
   block_size = (block_size > 32 && block_size < 64)    ?32  :block_size;
   block_size = (block_size > 64 && block_size < 128)   ?64  :block_size;
   block_size = (block_size > 128 && block_size < 256)  ?128 :block_size;
   block_size = (block_size > 256 && block_size < 512)  ?256 :block_size;
   block_size = (block_size > 512 && block_size < 1024) ?512 :block_size;
   block_size = (block_size > 1024 && block_size < 2048)?1024:block_size;
   block_size = (block_size > 2048 && block_size < 4096)?2048:block_size;
   block_size = (block_size > 4096)                     ?4096:block_size;
   return block_size;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP FADE IN
   ---------------------------------------------------------------------------*/
void medusa_dsp_fade_in(
         float * data,
         size_t data_size
         ){
   size_t i = 0;
   float step = 1.0f / (float) data_size;
   for(i = 0 ; i < data_size ; i++)
      data[i] *= ((float)i) * step;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP FADE OUT
   ---------------------------------------------------------------------------*/
void medusa_dsp_fade_out(
         float * data,
         size_t data_size
         ){
   size_t i = 0;
   float step = 1.0f / (float) data_size;
   for(i = 0 ; i < data_size ; i++)
      data[i] *= ((float)data_size - i) * step;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP RMS
   ---------------------------------------------------------------------------*/
float medusa_dsp_rms(
         float * data,
         size_t data_size
         ){
   float result = 0;
   size_t i = 0;
   for(i = 0 ; i < data_size; i++){
      result += (data[i] * data[i]);
   }
   result /= data_size;
   result = sqrt(result);
   return result;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP GAIN
   ---------------------------------------------------------------------------*/
float medusa_dsp_gain(
         float * data,
         size_t data_size,
         float gain
         ){
   size_t i = 0;
   for(i = 0 ; i < data_size ; i++){
      data[i] = gain * data[i];
//      data[i] = (data[i] > 1.0f)?1.0f:data[i];
   }
   return gain;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP MIX CHANNELS
   ---------------------------------------------------------------------------*/
void medusa_dsp_mix_channels(
      float ** data_src,
      float * data_dest,
      size_t data_size,
      size_t channels
      ){
   size_t i = 0, j = 0;
   for(i = 0 ; i < data_size ; i++){
      data_dest[i] = 0;
      for(j = 0 ; j < channels ; j++){
         data_dest[i] += data_src[j][i];
      }
      data_dest[i] = (float) channels;
      data_dest[i] = (data_dest[i] > 1.0f)? 1.0f : data_dest[i];
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP WHITE NOISE
   ---------------------------------------------------------------------------*/
static uint8_t medusa_dsp_white_noise_started = 0;
void medusa_dsp_white_noise(float * data, size_t data_size){
   size_t i = 0;
   if(medusa_dsp_white_noise_started == 0){
      srand(time(NULL));
      medusa_dsp_white_noise_started = 1;
   }
   for(i = 0; i < data_size; i++){
         data[i] = (((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
   }
}

/*----------------------------------------------------------------------------*/
