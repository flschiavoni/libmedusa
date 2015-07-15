#ifndef MEDUSA_NODE_H
#define MEDUSA_NODE_H

#include "medusa.h"

/** @file medusa_node.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

struct medusa_node{
   uint32_t port;
   uint64_t uid;
   char name[MEDUSA_LOCAL_RESOURCES];
   char ip[INET6_ADDRSTRLEN];

   t_medusa_list * audio_resources;
   t_medusa_list * midi_resources;
   t_medusa_list * connections;
};


struct medusa_node_audio_resource{
   uint8_t resource_type;
   uint8_t bit_depth;
   uint8_t endianness;
   uint8_t CODEC;
   MEDUSA_PROTOCOL protocol;
   uint16_t channels;
   uint16_t block_size;
   uint16_t port;
   uint32_t sample_rate;
   uint64_t uid;
   char name[MEDUSA_LOCAL_RESOURCES];
   char ip[INET6_ADDRSTRLEN];
};

struct medusa_node_midi_resource{
   uint8_t resource_type;
   MEDUSA_PROTOCOL protocol;
   uint16_t channels;
   uint16_t port;
   uint64_t uid;
   char name[MEDUSA_LOCAL_RESOURCES];
   char ip[INET6_ADDRSTRLEN];
};

struct medusa_node_connection{
   uint64_t src_uid;
   uint64_t dest_uid;
};

t_medusa_node * medusa_node_create(
      const t_medusa_message_add_node * message,
      const t_medusa_network_config * client
      );

t_medusa_node_audio_resource * medusa_node_audio_resource_create(
      const t_medusa_message_add_audio_resource * message
      );

t_medusa_node_midi_resource * medusa_node_midi_resource_create(
      const t_medusa_message_add_midi_resource * message
      );


uint64_t medusa_node_get_id(
      t_medusa_node * node
      );

t_medusa_list * medusa_node_get_audio_resources(
      t_medusa_node * node
      );

t_medusa_list * medusa_node_get_midi_resources(
      t_medusa_node * node
      );

t_medusa_list * medusa_node_get_connections(
      t_medusa_node * node
      );

void medusa_node_free(
      t_medusa_node * node
      );

void medusa_node_add_audio_resource(
      t_medusa_node * node,
      t_medusa_node_audio_resource * resource
      );

void medusa_node_add_midi_resource(
      t_medusa_node * node,
      t_medusa_node_midi_resource * resource
      );

void medusa_node_add_connection(
      t_medusa_node * node,
      t_medusa_node_connection * connection
      );

t_medusa_node_audio_resource * medusa_node_remove_audio_resource(
      t_medusa_node * node,
      uint64_t resource_uid
      );

t_medusa_node_midi_resource *  medusa_node_remove_midi_resource(
      t_medusa_node * node,
      uint64_t resource_uid
      );

t_medusa_node_connection * medusa_node_remove_connection(
      t_medusa_node * node,
      uint64_t src_uid,
      uint64_t dest_uid
      );

const char * medusa_node_get_ip(
      t_medusa_node * node
      );

const char * medusa_node_get_name(
      t_medusa_node * node
      );

int medusa_node_get_port(
      t_medusa_node * node
      );

void medusa_node_audio_set_network_config(
      t_medusa_node_audio_resource * audio_resource,
      const t_medusa_message_network_config * message
      );

void medusa_node_midi_set_network_config(
      t_medusa_node_midi_resource * midi_resource,
      const t_medusa_message_network_config * message
      );

int medusa_node_compare(
      const void * data1,
      const void * data2
      );

int medusa_node_audio_resource_compare(
      const void * data1,
      const void * data2
      );

int medusa_node_midi_resource_compare(
      const void * data1,
      const void * data2
      );

int medusa_node_connection_compare(
      const void * data1,
      const void * data2
      );

#endif /* MEDUSA_NODE_H */
