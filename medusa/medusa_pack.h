#ifndef MEDUSA_PACK_H
#define MEDUSA_PACK_H

#include "medusa.h"

/** @file medusa_pack.h
 *
 * @brief
 * This module defines functions to pack / unpack network messages.
 *
 * @ingroup control
 */

size_t medusa_pack_uint8_t(
      char * msg,
      uint8_t field,
      size_t shift
      );

size_t medusa_unpack_uint8_t(
      uint8_t * field,
      char * msg,
      size_t shift
      );

size_t medusa_pack_uint16_t(
      char * msg,
      uint16_t field,
      size_t shift
      );

size_t medusa_unpack_uint16_t(
      uint16_t * field,
      char * msg,
      size_t shift
      );

size_t medusa_pack_uint32_t(
      char * msg,
      uint32_t field,
      size_t shift
      );

size_t medusa_unpack_uint32_t(
      uint32_t * field,
      char * msg,
      size_t shift
      );

size_t medusa_pack_uint64_t(
      char * msg,
      uint64_t field,
      size_t shift
      );

size_t medusa_unpack_uint64_t(
      uint64_t * field,
      char * msg,
      size_t shift
      );

size_t medusa_pack_timestamp(
      char * msg,
      t_medusa_timestamp field,
      size_t shift
      );

size_t medusa_unpack_timestamp(
      t_medusa_timestamp * field,
      char * msg,
      size_t shift
      );

size_t medusa_pack_blob(
      char * msg,
      char * field,
      size_t size,
      size_t shift
      );

size_t medusa_unpack_blob(
      char * field,
      char * msg,
      size_t size,
      size_t shift
      );

size_t medusa_pack_end(
      char * msg,
      size_t shift
      );

size_t medusa_pack_padding(
      char * msg,
      size_t shift
      );
#endif /* MEDUSA_PACK_H */
