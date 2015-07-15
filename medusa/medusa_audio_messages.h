#ifndef MEDUSA_AUDIO_MESSAGES_H
#define MEDUSA_AUDIO_MESSAGES_H

#include "medusa.h"

/**
 * @brief An audio configure message. Used by sender / receiver to adjust the
 audio parameters.
 * @ingroup control
 */
typedef struct medusa_message_audio_config
         t_medusa_message_audio_config;

typedef struct medusa_message_add_audio_resource
         t_medusa_message_add_audio_resource;

typedef struct medusa_message_remove_audio_resource
         t_medusa_message_remove_audio_resource;


struct medusa_message_audio_config{
   uint8_t type;     ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;    ///< Message size
   uint8_t bit_depth;
   uint8_t endianness;
   uint8_t CODEC;
   unsigned char pad[1];   ///< Padding to align bytes
   uint16_t channels;
   uint16_t block_size;
   uint32_t sample_rate;
};
_Static_assert(sizeof(t_medusa_message_audio_config) == 16,
      "ERROR: medusa_message_audio_config size");

struct medusa_message_add_audio_resource{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   uint8_t CODEC;
   uint8_t resource_type;
   uint8_t bit_depth;
   uint8_t endianness;
   uint16_t channels;
   uint16_t block_size;
   uint32_t sample_rate;
   uint64_t node_uid;
   uint64_t resource_uid;
   uint8_t name_size;
   unsigned char pad2[3];   ///< Padding to align bytes
   unsigned char pad3[4];   ///< Padding to align bytes
   char name[];
};

struct medusa_message_remove_audio_resource{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   unsigned char pad1[4];   ///< Padding to align bytes
   uint64_t node_uid;
   uint64_t resource_uid;
};
_Static_assert(sizeof(t_medusa_message_remove_audio_resource) == 24,
      "ERROR: medusa_message_remove_midi_resource size");


_Static_assert(sizeof(t_medusa_message_add_audio_resource) == 40,
      "ERROR: medusa_message_add_audio_resource size");

t_medusa_message_audio_config * medusa_message_create_audio_config(
            t_medusa_audio_resource * resource
            );

size_t medusa_message_pack_audio_config(
            t_medusa_message_audio_config * message,
            void * msg
            );

t_medusa_message_audio_config * medusa_message_unpack_audio_config(
            void* msg,
            size_t size
            );

t_medusa_message_add_audio_resource * medusa_message_create_add_audio(
            uint64_t node_uid,
            MEDUSA_RESOURCE type,
            char * name,
            uint64_t resource_uid,
            const t_medusa_audio_resource * audio_resource
            );

size_t medusa_message_pack_add_audio(
            t_medusa_message_add_audio_resource * message,
            void * msg
            );

t_medusa_message_add_audio_resource * medusa_message_unpack_add_audio(
            void* msg,
            size_t size
            );

t_medusa_message_remove_audio_resource * medusa_message_create_remove_audio_resource(
            uint64_t node_uid,
            uint64_t resource_uid);

size_t medusa_message_pack_remove_audio_resource(
            t_medusa_message_remove_audio_resource * message,
            void * msg
            );

t_medusa_message_remove_audio_resource * medusa_message_unpack_remove_audio_resource(
            void* msg,
            size_t size
            );

typedef void (*t_medusa_audio_config_callback_function)(
      t_medusa_message_audio_config * config,
      void * args
      );

typedef void (*t_medusa_add_node_audio_resource_callback_function)(
      t_medusa_node * node,
      t_medusa_node_audio_resource * audio_resource,
      void * args
      );

typedef void (*t_medusa_remove_node_audio_resource_callback_function)(
      t_medusa_node * node,
      t_medusa_node_audio_resource * audio_resource,
      void * args
      );

struct medusa_audio_config_callback{
   t_medusa_audio_config_callback_function function;
   void * args;
   };

struct medusa_add_node_audio_resource_callback{
   t_medusa_add_node_audio_resource_callback_function function;
   void * args;
   };

struct medusa_remove_node_audio_resource_callback{
   t_medusa_remove_node_audio_resource_callback_function function;
   void * args;
   };

#endif /* MEDUSA_AUDIO_MESSAGES_H */
