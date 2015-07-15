#ifndef MEDUSA_AUDIO_RESOURCE_H
#define MEDUSA_AUDIO_RESOURCE_H

#include "medusa.h"

/** @file medusa_audio_resource.h
 *
 * @brief This module represents a Medusa audio resource.
 * 
 *
 * @ingroup control
 */

/**
 * @brief A medusa audio resource structure.
 * @details This resource is used by sender and receiver to store the
 * audio configuration.
 * It can be used to store local and network config.
 * The local part is set up by the Audio API.
 * The network part is set up by the user and it is published to the
   environment.
 */
struct medusa_audio_resource {
      MEDUSA_CODEC CODEC;  ///< CODEC
      MEDUSA_ENDIANNESS endianness; ///< endianness
      MEDUSA_BIT_DEPTH bit_depth; ///< bit depth
      uint16_t channels;  ///< number of channels
      uint16_t block_size; ///< block size
      uint32_t sample_rate; ///< sample rate
};

t_medusa_audio_resource * medusa_audio_resource_create(void);

void medusa_audio_resource_set(
      t_medusa_audio_resource * audio_resource,
      uint16_t channels,
      uint32_t sample_rate,
      MEDUSA_BIT_DEPTH bit_depth,
      MEDUSA_ENDIANNESS endianness,
      uint16_t block_size,
      MEDUSA_CODEC CODEC
      );

void medusa_audio_resource_check_config(
      t_medusa_audio_resource * local_audio_resource,
      t_medusa_audio_resource * network_audio_resource
      );

#endif /* MEDUSA_AUDIO_RESOURCE_H */
