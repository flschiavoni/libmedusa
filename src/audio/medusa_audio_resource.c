#include "medusa.h"

/** @file medusa_audio_resource.c
 *
 * @brief The implementation of audio resource.
 * 
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA AUDIO RESOURCE CREATE
   ---------------------------------------------------------------------------*/
t_medusa_audio_resource * medusa_audio_resource_create(){
   t_medusa_audio_resource * audio_resource;
   audio_resource = malloc(sizeof(t_medusa_audio_resource));
   audio_resource->channels = 0;
   audio_resource->sample_rate = 0;
   audio_resource->bit_depth = MEDUSA_NO_BITS;
   audio_resource->endianness = MEDUSA_NO_ENDIAN;
   audio_resource->block_size = 0;
   audio_resource->CODEC = MEDUSA_NOCODEC;
   return audio_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA AUDIO RESOURCE SET LOCAL
   ---------------------------------------------------------------------------*/
void medusa_audio_resource_set(
      t_medusa_audio_resource * audio_resource,
      uint16_t channels,
      uint32_t sample_rate,
      MEDUSA_BIT_DEPTH bit_depth,
      MEDUSA_ENDIANNESS endianness,
      uint16_t block_size,
      MEDUSA_CODEC CODEC){

   if(audio_resource == NULL)
      return;
   audio_resource->channels    = channels;
   audio_resource->sample_rate = sample_rate;
   audio_resource->bit_depth   = bit_depth;
   audio_resource->endianness  = endianness;
   audio_resource->block_size  = block_size;
   audio_resource->CODEC       = CODEC;
}


/* -----------------------------------------------------------------------------
   MEDUSA AUDIO RESOURCE CHECK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_audio_resource_check_config(
      t_medusa_audio_resource * local_audio_resource,
      t_medusa_audio_resource * network_audio_resource){
   if(local_audio_resource == NULL)
      return;
   if(network_audio_resource->channels < 1)
      network_audio_resource->channels = local_audio_resource->channels;
   if(network_audio_resource->sample_rate < 1)
      network_audio_resource->sample_rate = local_audio_resource->sample_rate;
   if(network_audio_resource->bit_depth == MEDUSA_NO_BITS)
      network_audio_resource->bit_depth = local_audio_resource->bit_depth;
   if(network_audio_resource->endianness == MEDUSA_NO_ENDIAN)
      network_audio_resource->endianness = local_audio_resource->endianness;
   if(network_audio_resource->block_size < 1)
      network_audio_resource->block_size = local_audio_resource->block_size;
   if(network_audio_resource->CODEC < 1)
      network_audio_resource->CODEC = MEDUSA_NOCODEC;

}
/*----------------------------------------------------------------------------*/
