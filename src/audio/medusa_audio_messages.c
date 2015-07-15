#include "medusa.h"

/** @file medusa_messages.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE AUDIO CONFIG
   ---------------------------------------------------------------------------*/
t_medusa_message_audio_config * medusa_message_create_audio_config(
         t_medusa_audio_resource * resource
         ){

   t_medusa_message_audio_config * message;
   message = malloc(sizeof(t_medusa_message_audio_config));
   message->type = MEDUSA_AUDIO_CONFIG;
   message->version = MEDUSA_VERSION;
   message->size = 20;
   message->bit_depth = resource->bit_depth;
   message->endianness = resource->endianness;
   message->CODEC = resource->CODEC;
   message->channels = resource->channels;
   message->block_size = resource->block_size;
   message->sample_rate = resource->sample_rate;
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK AUDIO CONFIG
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_audio_config(
      t_medusa_message_audio_config * message,
      void * msg
      ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_uint8_t(msg, message->bit_depth, shift);
   shift += medusa_pack_uint8_t(msg, message->endianness, shift);
   shift += medusa_pack_uint8_t(msg, message->CODEC, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_uint16_t(msg, message->channels, shift);
   shift += medusa_pack_uint16_t(msg, message->block_size, shift);
   shift += medusa_pack_uint32_t(msg, message->sample_rate, shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK AUDIO CONFIG
   ---------------------------------------------------------------------------*/
t_medusa_message_audio_config * medusa_message_unpack_audio_config(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   t_medusa_message_audio_config * message;
   message = malloc(sizeof(t_medusa_message_audio_config));
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += medusa_unpack_uint8_t(&message->bit_depth, msg, shift);
   shift += medusa_unpack_uint8_t(&message->endianness, msg, shift);
   shift += medusa_unpack_uint8_t(&message->CODEC, msg, shift);
   shift += 1; //padding
   shift += medusa_unpack_uint16_t(&message->channels, msg, shift);
   shift += medusa_unpack_uint16_t(&message->block_size, msg, shift);
   shift += medusa_unpack_uint32_t(&message->sample_rate, msg, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE ADD AUDIO
   ---------------------------------------------------------------------------*/
t_medusa_message_add_audio_resource * medusa_message_create_add_audio(
            uint64_t node_uid,
            MEDUSA_RESOURCE type,
            char * name,
            uint64_t resource_uid,
            const t_medusa_audio_resource * audio_resource){

   uint16_t size = 44 + strlen(name);
   size += ((size % 4) > 0) ? 4 - (size % 4) : 0;

   t_medusa_message_add_audio_resource * message;
   message = malloc(size);
   message->type = MEDUSA_ADD_AUDIO_RESOURCE;
   message->version = MEDUSA_VERSION;
   message->size = size;
   message->node_uid = node_uid;

   message->resource_uid = resource_uid;
   medusa_util_format_name(name);
   message->resource_type = type;

   message->channels = audio_resource->channels;
   message->sample_rate = audio_resource->sample_rate;
   message->bit_depth = audio_resource->bit_depth;
   message->endianness = audio_resource->endianness;
   message->block_size = audio_resource->block_size;
   message->CODEC = audio_resource->CODEC;
   message->name_size = strlen(name);
   strcpy(message->name, name);
   return message;
}


/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK ADD AUDIO
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_add_audio(
      t_medusa_message_add_audio_resource * message,
      void * msg
      ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_uint8_t(msg, message->resource_type, shift);
   shift += medusa_pack_uint16_t(msg, message->channels, shift);
   shift += medusa_pack_uint32_t(msg, message->sample_rate, shift);
   shift += medusa_pack_uint8_t(msg, message->bit_depth, shift);
   shift += medusa_pack_uint8_t(msg, message->endianness, shift);
   shift += medusa_pack_uint16_t(msg, message->block_size, shift);
   shift += medusa_pack_uint8_t(msg, message->CODEC, shift);
   shift += medusa_pack_uint64_t(msg, message->resource_uid, shift);
   shift += medusa_pack_uint64_t(msg, message->node_uid, shift);
   shift += medusa_pack_uint8_t(msg, message->name_size, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_blob(msg, message->name, message->name_size,  shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK ADD AUDIO
   ---------------------------------------------------------------------------*/
t_medusa_message_add_audio_resource * medusa_message_unpack_add_audio(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   uint16_t data_size;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_add_audio_resource * message;
   message = malloc(data_size);
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += medusa_unpack_uint8_t(&message->resource_type, msg, shift);
   shift += medusa_unpack_uint16_t(&message->channels, msg, shift);
   shift += medusa_unpack_uint32_t(&message->sample_rate, msg, shift);
   shift += medusa_unpack_uint8_t(&message->bit_depth, msg, shift);
   shift += medusa_unpack_uint8_t(&message->endianness, msg, shift);
   shift += medusa_unpack_uint16_t(&message->block_size, msg, shift);
   shift += medusa_unpack_uint8_t(&message->CODEC, msg, shift);
   shift += medusa_unpack_uint64_t(&message->resource_uid, msg, shift);
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint8_t(&message->name_size, msg, shift);
   shift += 7; // padding
   shift += medusa_unpack_blob(message->name, msg, message->name_size, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE REMOVE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_message_remove_audio_resource * medusa_message_create_remove_audio_resource(
            uint64_t node_uid,
            uint64_t resource_uid){

   t_medusa_message_remove_audio_resource * message;
   message = malloc(sizeof (t_medusa_message_remove_audio_resource));
   message->type = MEDUSA_REM_AUDIO_RESOURCE;
   message->version = MEDUSA_VERSION;
   message->size = 28;
   message->node_uid = node_uid;
   message->resource_uid = resource_uid;
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK REMOVE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_remove_audio_resource(
      t_medusa_message_remove_audio_resource * message,
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
   MEDUSA MESSAGE UNPACK REMOVE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_message_remove_audio_resource * medusa_message_unpack_remove_audio_resource(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   t_medusa_message_remove_audio_resource * message;
   message = malloc(sizeof (t_medusa_message_remove_audio_resource));
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += 4; // padding
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint64_t(&message->resource_uid, msg, shift);
   return message;
}

