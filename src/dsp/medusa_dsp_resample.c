#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA DSP LOW PASSFILTER
   ---------------------------------------------------------------------------*/
void medusa_dsp_lpf(float * input, size_t signal_size, float sr, float fc){

   float K = (float) tan(MEDUSA_PI * fc / sr);
   float b0 = K / (K +1);
   float b1 = K / (K +1);
   float a1 = (K - 1) / (K + 1);

   float last_input = 0;
   float last_output = 0;
   float temp = 0;

   int i = 0;
   for(i = 0 ; i < (int) signal_size ; i++){
      temp = input[i];
      input[i] = b0 * input[i] + b1 * last_input - a1 * last_output;
      last_input = temp;
      last_output = input[i];
      }
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP RESAMPLE
   ---------------------------------------------------------------------------*/
void medusa_dsp_resample(
         float * input,
         float * output,
         size_t input_size,
         size_t output_size,
         float input_sr,
         float output_sr){

   (void) input_size;
   float j = 0, interpolated_step = input_sr / output_sr;

   //Convert to desired SR
   int i = 0;
   for(i = 0, j = 0 ; i < (int) output_size; i++, j += interpolated_step){
      int my_floor = floor(j);
      float y = j - my_floor;
      output[i] = input[my_floor] * (1.0f - y) + input[my_floor + 1] * y;
      }

   // Low pass filter to remove aliasing
   medusa_dsp_lpf(output, output_size, output_sr , output_sr / 2.1f);
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CHANGE SAMPLE RATE
   ---------------------------------------------------------------------------*/
int medusa_dsp_change_sample_rate(
      float from,
      float to,
      float * src,
      float * dest,
      int data_size){

   if(from == to){
      memcpy(dest,src, data_size);
      return data_size;
   }
   data_size = data_size / sizeof(float);
   int new_size = ceil(to / from * data_size);
   to = new_size * from / data_size;
   medusa_dsp_resample(src, dest, data_size, new_size, from, to);
   return new_size * sizeof(float);
}
/* --------------------------------------------------------------------------*/
