#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA DSP FLOAT TO INT32
   ---------------------------------------------------------------------------*/
int32_t medusa_dsp_f2i(float f){
   if (f >= 1.0f)
      return 2147483647;
   if (f <= -1.0f)
      return -2147483647;
   return floor(f * 2147483647.0f);
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP INT32 TO FLOAT
   ---------------------------------------------------------------------------*/
float medusa_dsp_i2f(int32_t i){
   return i / 2147483647.0f;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP INT TO FLOAT (BLOCK)
   ---------------------------------------------------------------------------*/
int medusa_dsp_int2float(void * data, int data_size){
   int32_t datum;
   float f;
   int i;
   for(i = 0 ; i < data_size ; i += sizeof(float)){
      memcpy(&datum, data + i , sizeof(uint32_t));
      f = medusa_dsp_i2f(datum);
      memcpy(data + i, &f, sizeof(float));
   }
   return data_size;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP FLOAT TO INT32 (BLOCK)
   ---------------------------------------------------------------------------*/
int medusa_dsp_float2int(void * data, int data_size){
   int32_t datum;
   float f;
   int i;
   for(i = 0 ; i < data_size ; i += sizeof(float)){
      memcpy(&f, data + i , sizeof(float));
      datum = medusa_dsp_f2i(f);
      memcpy(data + i, &(datum), sizeof(uint32_t));
   }
   return data_size;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP ADJUST BIT DEPTH
   ---------------------------------------------------------------------------*/
MEDUSA_BIT_DEPTH medusa_dsp_adjust_bit_depth(int bit_depth){
   if(bit_depth == 0)
      return MEDUSA_NO_BITS;
   if(bit_depth > 0 && bit_depth <= 8)
      return MEDUSA_8_BITS;
   else if(bit_depth > 8 && bit_depth <= 16)
      return MEDUSA_16_BITS;
   else if(bit_depth > 16 && bit_depth <= 24)
      return MEDUSA_24_BITS;
   else if(bit_depth > 24)
      return MEDUSA_32_BITS;
   return MEDUSA_32_BITS;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 32 TO 8 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_32to8_bits(void * src, void * dest, int data_size){
   int j = 0, i = 0;
   float f = 0;
   for(i = 0 ; i < data_size / 4 ; i++){
      memcpy(&f, src + i * 4, 4);
      t_medusa_integer.i = medusa_dsp_f2i(f);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j] = t_medusa_integer.a[3];
      }else{
         ((char *)dest)[j] = t_medusa_integer.a[0];
      }
      j += 1;
   }
   return data_size / 4;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 32 TO 16 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_32to16_bits(void * src, void * dest, int data_size){
   int j = 0, i = 0;
   float f = 0;
   for(i = 0 ; i < data_size / 4 ; i++){
      memcpy(&f, src + i * 4, 4);
      t_medusa_integer.i = medusa_dsp_f2i(f);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j] = t_medusa_integer.a[2];
         ((char *)dest)[j + 1] = t_medusa_integer.a[3];
      }else{
         ((char *)dest)[j] = t_medusa_integer.a[0];
         ((char *)dest)[j + 1] = t_medusa_integer.a[1];
      }
      j += 2;
   }
   return data_size / 2;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 32 TO 24 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_32to24_bits(void * src, void * dest, int data_size){
   int j = 0, i = 0;
   float f = 0;
   for(i = 0 ; i < data_size / 4 ; i++){
      memcpy(&f, src + i * 4, 4);
      t_medusa_integer.i = medusa_dsp_f2i(f);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j + 0] = t_medusa_integer.a[1];
         ((char *)dest)[j + 1] = t_medusa_integer.a[2];
         ((char *)dest)[j + 2] = t_medusa_integer.a[3];
      }else{
         ((char *)dest)[j + 0] = t_medusa_integer.a[0];
         ((char *)dest)[j + 1] = t_medusa_integer.a[1];
         ((char *)dest)[j + 2] = t_medusa_integer.a[2];
      }
      j += 3;
   }
   return data_size * 3 / 4 ;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 24 TO 8 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_24to8_bits(void * src, void * dest, int data_size){
   int i, j = 0;
   for(i = 0 ; i < data_size / 3 ; i++){
      memcpy(&(t_medusa_integer.a), src + i * 3, 3);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j] = t_medusa_integer.a[2];
      }else{
         ((char *)dest)[j] = t_medusa_integer.a[0];
      }
      j += 1;
   }
   return data_size / 3;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 24 TO 16 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_24to16_bits(void * src, void * dest, int data_size){
   int i, j = 0;
   for(i = 0 ; i < data_size / 3 ; i++){
      memcpy(&(t_medusa_integer.a), src + i * 3, 3);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j + 0] = t_medusa_integer.a[1];
         ((char *)dest)[j + 1] = t_medusa_integer.a[2];
      }else{
         ((char *)dest)[j + 0] = t_medusa_integer.a[0];
         ((char *)dest)[j + 1] = t_medusa_integer.a[1];
      }
      j += 2;
   }
   return data_size * 2 / 3;
}


/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 24 TO 32 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_24to32_bits(void * src, void * dest, int data_size){
   int i;
   for(i = 0 ; i < data_size / 3 ; i++){
      memcpy(&(t_medusa_integer.a), src + i * 3, 3);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         t_medusa_integer.a[3] = t_medusa_integer.a[2];
         t_medusa_integer.a[2] = t_medusa_integer.a[1];
         t_medusa_integer.a[1] = t_medusa_integer.a[0];
         t_medusa_integer.a[0] = 0;
      }else{
         t_medusa_integer.a[0] = t_medusa_integer.a[2];
         t_medusa_integer.a[1] = t_medusa_integer.a[1];
         t_medusa_integer.a[2] = t_medusa_integer.a[0];
         t_medusa_integer.a[3] = 0;
      }
      ((float *)dest)[i] = medusa_dsp_i2f(t_medusa_integer.i);
   }
   return data_size * 4 / 3;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 16 TO 8 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_16to8_bits(void * src, void * dest, int data_size){
   int i, j = 0;
   for(i = 0 ; i < data_size / 2 ; i++){
      memcpy(&(t_medusa_integer.a), src + i * 2, 2);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j] = t_medusa_integer.a[1];
      }else{
         ((char *)dest)[j] = t_medusa_integer.a[0];
      }
      j += 1;
   }
   return data_size / 2;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 16 TO 24 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_16to24_bits(void * src, void * dest, int data_size){
   int i, j = 0;
   for(i = 0 ; i < data_size / 2 ; i++){
      memcpy(&(t_medusa_integer.a), src + i * 2, 2);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j + 0] = 0;
         ((char *)dest)[j + 1] = t_medusa_integer.a[0];
         ((char *)dest)[j + 2] = t_medusa_integer.a[1];
      }else{
         ((char *)dest)[j + 0] = t_medusa_integer.a[0];
         ((char *)dest)[j + 1] = t_medusa_integer.a[1];
         ((char *)dest)[j + 2] = 0;
      }
      j += 3;
   }
   return data_size * 3 / 2;
}


/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 16 TO 32 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_16to32_bits(void * src, void * dest, int data_size){
   int i;
   for(i = 0 ; i < data_size / 2 ; i++){
      memcpy(&(t_medusa_integer.a), src + i * 2, 2);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         t_medusa_integer.a[3] = t_medusa_integer.a[1];
         t_medusa_integer.a[2] = t_medusa_integer.a[0];
         t_medusa_integer.a[1] = 0;
         t_medusa_integer.a[0] = 0;
      }else{
         t_medusa_integer.a[0] = t_medusa_integer.a[1];
         t_medusa_integer.a[1] = t_medusa_integer.a[0];
         t_medusa_integer.a[2] = 0;
         t_medusa_integer.a[3] = 0;
      }
      ((float *)dest)[i] = medusa_dsp_i2f(t_medusa_integer.i);
   }
   return data_size * 2;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 8 TO 16 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_8to16_bits(void * src, void * dest, int data_size){
   int i, j = 0;
   for(i = 0 ; i < data_size / 1; i++){
      memcpy(&(t_medusa_integer.a), src + i * 1, 1);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j + 0] = 0;
         ((char *)dest)[j + 1] = t_medusa_integer.a[0];
      }else{
         ((char *)dest)[j + 0] = t_medusa_integer.a[0];
         ((char *)dest)[j + 1] = 0;
      }
      j += 2;
   }
   return data_size * 2;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 8 TO 24 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_8to24_bits(void * src, void * dest, int data_size){
   int i, j = 0;
   for(i = 0 ; i < data_size / 1; i++){
      memcpy(&(t_medusa_integer.a), src + i * 1, 1);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         ((char *)dest)[j + 0] = 0;
         ((char *)dest)[j + 1] = 0;
         ((char *)dest)[j + 2] = t_medusa_integer.a[0];
      }else{
         ((char *)dest)[j + 0] = t_medusa_integer.a[0];
         ((char *)dest)[j + 1] = 0;
         ((char *)dest)[j + 0] = 0;
      }
      j += 3;
   }
   return data_size * 3;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CONVERT 8 TO 32 BITS
   ---------------------------------------------------------------------------*/
int medusa_dsp_8to32_bits(void * src, void * dest, int data_size){
   int i;
   for(i = 0 ; i < data_size / 1 ; i++){
      memcpy(&(t_medusa_integer.i), src + i, 1);
      if(medusa_dsp_get_endianness() == MEDUSA_LITTLE_ENDIAN){
         t_medusa_integer.a[3] = t_medusa_integer.a[0];
         t_medusa_integer.a[2] = 0;
         t_medusa_integer.a[1] = 0;
         t_medusa_integer.a[0] = 0;
      }else{
         t_medusa_integer.a[0] = t_medusa_integer.a[0];
         t_medusa_integer.a[1] = 0;
         t_medusa_integer.a[2] = 0;
         t_medusa_integer.a[3] = 0;
      }
      ((float *)dest)[i] = medusa_dsp_i2f(t_medusa_integer.i);
   }
   return data_size * 4;
}

/* -----------------------------------------------------------------------------
   MEDUSA DSP CHANGE QUANTIZATION
   ---------------------------------------------------------------------------*/
int medusa_dsp_change_quantization(
      MEDUSA_BIT_DEPTH from,
      MEDUSA_BIT_DEPTH to,
      char * src,
      char * dest,
      int data_size){

   if(from == to){
      memcpy(dest,src, data_size);
      return data_size;
   }
   if(from == MEDUSA_32_BITS && to == MEDUSA_24_BITS){
      return medusa_dsp_32to24_bits(src, dest, data_size);
   }
   if(from == MEDUSA_32_BITS && to == MEDUSA_16_BITS){
      return medusa_dsp_32to16_bits(src, dest, data_size);
   }
   if(from == MEDUSA_32_BITS && to == MEDUSA_8_BITS){
      return medusa_dsp_32to8_bits(src, dest, data_size);
   }
   if(from == MEDUSA_8_BITS && to == MEDUSA_16_BITS){
      return medusa_dsp_8to16_bits(src, dest, data_size);
   }
   if(from == MEDUSA_8_BITS && to == MEDUSA_24_BITS){
      return medusa_dsp_8to24_bits(src, dest, data_size);
   }
   if(from == MEDUSA_8_BITS && to == MEDUSA_32_BITS){
      return medusa_dsp_8to32_bits(src, dest, data_size);
   }
   if(from == MEDUSA_16_BITS && to == MEDUSA_8_BITS){
      return medusa_dsp_16to8_bits(src, dest, data_size);
   }
   if(from == MEDUSA_16_BITS && to == MEDUSA_24_BITS){
      return medusa_dsp_16to24_bits(src, dest, data_size);
   }
   if(from == MEDUSA_16_BITS && to == MEDUSA_32_BITS){
      return medusa_dsp_16to32_bits(src, dest, data_size);
   }
   if(from == MEDUSA_24_BITS && to == MEDUSA_8_BITS){
      return medusa_dsp_24to8_bits(src, dest, data_size);
   }
   if(from == MEDUSA_24_BITS && to == MEDUSA_16_BITS){
      return medusa_dsp_24to16_bits(src, dest, data_size);
   }
   if(from == MEDUSA_24_BITS && to == MEDUSA_32_BITS){
      return medusa_dsp_24to32_bits(src, dest, data_size);
   }
   return 0;
}

/* ---------------------------------------------------------------------------*/
