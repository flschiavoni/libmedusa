#include "medusa.h"

/** @file medusa_pack.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

size_t medusa_pack_uint8_t(
      char * msg,
      uint8_t field,
      size_t shift
      ){
   memcpy(msg + shift, &field, 1);
   return 1;
}

size_t medusa_unpack_uint8_t(
      uint8_t * field,
      char * msg,
      size_t shift
      ){
   memcpy(field, msg + shift, 1);
   return 1;
}

size_t medusa_pack_uint16_t(
      char * msg,
      uint16_t field,
      size_t shift
      ){
   field = medusa_dsp_hton16(field);
   memcpy(msg + shift, &field, 2);
   return 2;
}

size_t medusa_unpack_uint16_t(
      uint16_t * field,
      char * msg,
      size_t shift
      ){
   memcpy(field, msg + shift, 2);
   *field = medusa_dsp_ntoh16(*field);
   return 2;
}

size_t medusa_pack_uint32_t(
      char * msg,
      uint32_t field,
      size_t shift
      ){
   field = medusa_dsp_hton32(field);
   memcpy(msg + shift, &field, 4);
   return 4;
}

size_t medusa_unpack_uint32_t(
      uint32_t * field,
      char * msg,
      size_t shift
      ){
   memcpy(field, msg + shift, 4);
   *field = medusa_dsp_ntoh32(*field);
   return 4;
}

size_t medusa_pack_uint64_t(
      char * msg,
      uint64_t field,
      size_t shift
      ){
   field = medusa_dsp_hton64(field);
   memcpy(msg + shift, &field, 8);
   return 8;
}

size_t medusa_unpack_uint64_t(
      uint64_t * field,
      char * msg,
      size_t shift
      ){
   memcpy(field, msg + shift, 8);
   *field = medusa_dsp_ntoh64(*field);
   return 8;
}

size_t medusa_pack_timestamp(
      char * msg,
      t_medusa_timestamp field,
      size_t shift
      ){
   field.tv_sec = medusa_dsp_hton32(field.tv_sec);
   field.tv_nsec = medusa_dsp_hton32(field.tv_nsec);
   memcpy(msg + shift, &field, 8);
   return 8;
}

size_t medusa_unpack_timestamp(
      t_medusa_timestamp * field,
      char * msg,
      size_t shift
      ){
   memcpy(field, msg + shift, 8);
   (*field).tv_sec = medusa_dsp_ntoh32((*field).tv_sec);
   (*field).tv_nsec = medusa_dsp_ntoh32((*field).tv_nsec);
   return 8;
}

size_t medusa_pack_blob(
      char * msg,
      char * field,
      size_t size,
      size_t shift
      ){
   memcpy(msg + shift, field, size);
   field[size] = '\0';
   return size;
}

size_t medusa_unpack_blob(
      char * field,
      char * msg,
      size_t size,
      size_t shift
      ){
   memcpy(field, msg + shift, size);
   field[size] = '\0';
   return size;
}

size_t medusa_pack_end(
      char * msg,
      size_t shift
      ){
   char end = '\0';
   int left = 5 - ((shift + 1) % 4);
   int i = 0;
   for(i = 0 ; i < left ; i++)
      memcpy(msg + shift + i, &end, 1);
   return left;
}

size_t medusa_pack_padding(
      char * msg,
      size_t shift
      ){
   char padding = '\0';
   memcpy(msg + shift, &padding, 1);
   return 1;
}

/*----------------------------------------------------------------------------*/
