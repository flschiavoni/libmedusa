#include "medusa.h"

/** @file medusa_messages.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE CHAT
   ---------------------------------------------------------------------------*/
t_medusa_message_chat * medusa_message_create_chat(
            uint64_t node_uid,
            char * data){

   uint16_t size = 24 + strlen(data);
   size += ((size % 4) > 0) ? 4 - (size % 4) : 0;

   t_medusa_message_chat * message;
   message = malloc(size);
   message->type = MEDUSA_CHAT;
   message->version = MEDUSA_VERSION;
   message->size = size;
   message->node_uid = node_uid;
   message->data_size = strlen(data);
   strcpy(message->data, data);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK CHAT
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_chat(
      t_medusa_message_chat * message,
      void * msg
      ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_uint64_t(msg, message->node_uid, shift);
   shift += medusa_pack_uint8_t(msg, message->data_size, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_blob(msg, message->data, message->data_size,  shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK CHAT
   ---------------------------------------------------------------------------*/
t_medusa_message_chat * medusa_message_unpack_chat(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   uint16_t data_size = 0;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_chat * message;
   message = malloc(data_size);
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += 4; //padding
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint8_t(&message->data_size, msg, shift);
   shift += 3; //padding
   shift += medusa_unpack_blob(message->data, msg, message->data_size, shift);
   return message;
}


