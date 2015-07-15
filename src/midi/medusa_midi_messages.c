#include "medusa.h"

/** @file medusa_messages.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE ADD MIDI
   ---------------------------------------------------------------------------*/
t_medusa_message_add_midi_resource * medusa_message_create_add_midi(
            uint64_t node_uid,
            MEDUSA_RESOURCE type,
            char * name,
            uint64_t resource_uid,
            const t_medusa_midi_resource * midi_resource){

   uint16_t size = 32 + strlen(name);
   size += ((size % 4) > 0) ? 4 - (size % 4) : 0;

   t_medusa_message_add_midi_resource * message;
   message = malloc(size);
   message->type = MEDUSA_ADD_MIDI_RESOURCE;
   message->version = MEDUSA_VERSION;
   message->size = size;
   message->node_uid = node_uid;

   message->resource_uid = resource_uid;
   message->resource_type = type;
   message->name_size = strlen(name);
   strcpy(message->name, name);
   message->channels = midi_resource->channels;
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK ADD MIDI
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_add_midi(
      t_medusa_message_add_midi_resource * message,
      void * msg
      ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_uint8_t(msg, message->resource_type, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_uint16_t(msg, message->channels, shift);
   shift += medusa_pack_uint64_t(msg, message->node_uid, shift);
   shift += medusa_pack_uint64_t(msg, message->resource_uid, shift);
   shift += medusa_pack_uint8_t(msg, message->name_size, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_blob(msg, message->name, message->name_size,  shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK ADD MIDI
   ---------------------------------------------------------------------------*/
t_medusa_message_add_midi_resource * medusa_message_unpack_add_midi(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   uint16_t data_size = 0;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_add_midi_resource * message;
   message = malloc(data_size); //PADDING
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += medusa_unpack_uint8_t(&message->resource_type, msg, shift);
   shift += 1; // padding
   shift += medusa_unpack_uint16_t(&message->channels, msg, shift);
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint64_t(&message->resource_uid, msg, shift);
   shift += medusa_unpack_uint8_t(&message->name_size, msg, shift);
   shift += 3; // padding
   shift += medusa_unpack_blob(message->name, msg, message->name_size, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_message_remove_midi_resource * medusa_message_create_remove_midi_resource(
            uint64_t node_uid,
            uint64_t resource_uid){

   t_medusa_message_remove_midi_resource * message;
   message = malloc(sizeof (t_medusa_message_remove_midi_resource));
   message->type = MEDUSA_REM_MIDI_RESOURCE;
   message->version = MEDUSA_VERSION;
   message->size = 28;
   message->node_uid = node_uid;
   message->resource_uid = resource_uid;
   return message;
}


/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_remove_midi_resource(
      t_medusa_message_remove_midi_resource * message,
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
   shift += medusa_pack_uint64_t(msg, message->resource_uid, shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_message_remove_midi_resource * medusa_message_unpack_remove_midi_resource(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   t_medusa_message_remove_midi_resource * message;
   message = malloc(sizeof (t_medusa_message_remove_midi_resource));
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += 4; // padding
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint64_t(&message->resource_uid, msg, shift);
   return message;
}

