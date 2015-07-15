#ifndef MEDUSA_DSP_H
#define MEDUSA_DSP_H

/** @file medusa_dsp.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup dsp
 */
#include <opus/opus.h>
#include "medusa.h"

union medusa_integer{
   char a[4];        ///< 4 bytes as a char array
   int32_t i;        ///< 32 bits integer
} t_medusa_integer;

MEDUSA_ENDIANNESS medusa_dsp_get_endianness(void);

uint64_t medusa_dsp_ntoh64(
      uint64_t input
      );

uint64_t medusa_dsp_hton64(
      uint64_t input
      );

uint32_t medusa_dsp_ntoh32(
      uint32_t input
      );

uint32_t medusa_dsp_hton32(
      uint32_t input
      );

uint16_t medusa_dsp_ntoh16(
      uint16_t input
      );

uint16_t medusa_dsp_hton16(
      uint16_t input
      );


int medusa_dsp_change_quantization(
      MEDUSA_BIT_DEPTH from,
      MEDUSA_BIT_DEPTH to,
      char * src,
      char * dest,
      int data_size
      );

int medusa_dsp_change_sample_rate(
      float from,
      float to,
      float * src,
      float * dest,
      int data_size
      );

uint16_t medusa_dsp_adjust_block_size(
         uint16_t block_size
         );

MEDUSA_BIT_DEPTH medusa_dsp_adjust_bit_depth(
         int bit_depth
         );

int medusa_dsp_interleaved_separe(
         void * origin,
         void * dest_l,
         void * dest_r,
         int data_size,
         int sample_size
         );

int medusa_dsp_interleaved_merge(
         void * origin_l,
         void * origin_r,
         void * dest,
         int data_size,
         int sample_size
         );

void medusa_dsp_fade_in(
         float * data,
         size_t data_size
         );

void medusa_dsp_fade_out(
         float * data,
         size_t data_size
         );

float medusa_dsp_rms(
         float * data,
         size_t data_size
         );

float medusa_dsp_gain(
         float * data,
         size_t data_size,
         float gain
         );

void medusa_dsp_mix_channels(
      float ** data_src,
      float * data_dest,
      size_t data_size,
      size_t channels
      );

void medusa_dsp_white_noise(
         float * data,
         size_t data_size
         );

int medusa_dsp_encode_opus32(
         int sr,
         const float * audio_frame,
         int frame_size,
         unsigned char * packet
         );

int medusa_dsp_decode_opus32(
         int sr,
         unsigned char * packet,
         int len,
         float * decoded,
         int max_size
         );

#endif /* MEDUSA_DSP_H */
