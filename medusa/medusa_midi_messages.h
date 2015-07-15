#ifndef MEDUSA_MIDI_MESSAGES_H
#define MEDUSA_MIDI_MESSAGES_H

#include "medusa.h"

typedef struct medusa_message_add_midi_resource
         t_medusa_message_add_midi_resource;

typedef struct medusa_message_remove_midi_resource
         t_medusa_message_remove_midi_resource;


struct medusa_message_add_midi_resource{
   uint8_t type;   ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   uint8_t resource_type;
   unsigned char pad1[1];   ///< Padding to align bytes
   uint16_t channels;
   uint64_t node_uid;
   uint64_t resource_uid;
   uint8_t name_size;
   unsigned char pad2[3];   ///< Padding to align bytes
   unsigned char pad3[4];   ///< Padding to align bytes
   char name[];
};
_Static_assert(sizeof(t_medusa_message_add_midi_resource) == 32,
      "ERROR: medusa_message_add_midi_resource size");

struct medusa_message_remove_midi_resource{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   unsigned char pad1[4];   ///< Padding to align bytes
   uint64_t node_uid;
   uint64_t resource_uid;
};
_Static_assert(sizeof(t_medusa_message_remove_midi_resource) == 24,
      "ERROR: medusa_message_remove_midi_resource size");

t_medusa_message_add_midi_resource * medusa_message_create_add_midi(
            uint64_t node_uid,
            MEDUSA_RESOURCE type,
            char * name,
            uint64_t resource_uid,
            const t_medusa_midi_resource * midi_resource
            );

size_t medusa_message_pack_add_midi(
            t_medusa_message_add_midi_resource * message,
            void * msg
            );

t_medusa_message_add_midi_resource * medusa_message_unpack_add_midi(
            void* msg,
            size_t size
            );

t_medusa_message_remove_midi_resource * medusa_message_create_remove_midi_resource(
            uint64_t node_uid,
            uint64_t resource_uid);

size_t medusa_message_pack_remove_midi_resource(
            t_medusa_message_remove_midi_resource * message,
            void * msg
            );

t_medusa_message_remove_midi_resource * medusa_message_unpack_remove_midi_resource(
            void* msg,
            size_t size
            );


typedef void (*t_medusa_add_node_midi_resource_callback_function)(
      t_medusa_node * node,
      t_medusa_node_midi_resource * midi_resource,
      void * args
      );

typedef void (*t_medusa_remove_node_midi_resource_callback_function)(
      t_medusa_node * node,
      t_medusa_node_midi_resource * midi_resource,
      void * args
      );

struct medusa_add_node_midi_resource_callback{
   t_medusa_add_node_midi_resource_callback_function function;
   void * args;
   };

struct medusa_remove_node_midi_resource_callback{
   t_medusa_remove_node_midi_resource_callback_function function;
   void * args;
   };


#endif /* MEDUSA_MIDI_MESSAGES_H */
