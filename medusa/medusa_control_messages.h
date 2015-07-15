#ifndef MEDUSA_MESSAGES_H
#define MEDUSA_MESSAGES_H

#include "medusa.h"

/** @file medusa_messages.h
 *
 * @brief
 * This module defines the Medusa network messages.
 *
 * @ingroup control
 */

/**
 * @brief A message to inform connection. Used by server / client.
 * @ingroup control
 */
typedef struct medusa_message_hello
         t_medusa_message_hello;
/**
 * @brief A message to inform desconnection. Used by server / client.
 * @ingroup control
 */
typedef struct medusa_message_bye
         t_medusa_message_bye;
/**
 * @brief A data message. Used by sender / receiver to pack audio and MIDI.
 * @ingroup control
 */
typedef struct medusa_message_data
         t_medusa_message_data;
/**
 * @brief A loopback data message. Used by sender / receiver to measure the
 connection.
 * @ingroup control
 */
typedef struct medusa_message_loopback
         t_medusa_message_loopback;
/**
 * @brief An Add node message.
 * @ingroup control
 */
typedef struct medusa_message_add_node
         t_medusa_message_add_node;
/**
 * @brief A remove node message.
 * @ingroup control
 */
typedef struct medusa_message_remove_node
         t_medusa_message_remove_node;
/**
 * @brief A Network configuration message.
 * @ingroup control
 */
typedef struct medusa_message_network_config
         t_medusa_message_network_config;


/**
 * @brief A message to inform connection. Used by server / client.
 * @ingroup control
 */
struct medusa_message_hello{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
};
_Static_assert(sizeof(t_medusa_message_hello) == 4,
      "ERROR: medusa_message_hello size");

/**
 * @brief A message to inform desconnection. Used by server / client.
 * @ingroup control
 */
struct medusa_message_bye{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
};
_Static_assert(sizeof(t_medusa_message_bye) == 4,
      "ERROR: medusa_message_bye size");


struct medusa_message_add_node{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   unsigned char pad1[4];   ///< Padding to align bytes
   uint64_t node_uid;
   uint8_t name_size;
   unsigned char pad2[3];   ///< Padding to align bytes
   unsigned char pad3[4];   ///< Padding to align bytes
   char name[];
};
_Static_assert(sizeof(t_medusa_message_add_node) == 24,
      "ERROR: medusa_message_add_node size");

struct medusa_message_remove_node{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   unsigned char pad1[4];   ///< Padding to align bytes
   uint64_t node_uid;
};
_Static_assert(sizeof(t_medusa_message_remove_node) == 16,
      "ERROR: medusa_message_remove_node size");


struct medusa_message_network_config{
   uint8_t type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   uint16_t port;
   uint8_t protocol;
   unsigned char pad1[1];   ///< Padding to align bytes
   uint64_t node_uid;
   uint64_t resource_uid;
   uint8_t ip_size;
   unsigned char pad2[3];   ///< Padding to align bytes
   unsigned char pad3[4];   ///< Padding to align bytes
   char ip[];
};
_Static_assert(sizeof(t_medusa_message_network_config) == 32,
      "ERROR: medusa_message_network_config");

struct medusa_message_data{
   uint8_t  type;  ///< Message type
   uint8_t  version;  ///< Medusa version
   uint16_t size;  ///< Message size
   uint16_t data_type;
   uint16_t channel;
   uint32_t data_size;
   uint32_t seq_number;
   t_medusa_timestamp timestamp;
   char data[];
};
_Static_assert(sizeof(t_medusa_message_data) == 24,
      "ERROR: medusa_message_data size");

struct medusa_message_loopback{
   uint8_t  type;  ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   uint16_t data_type;
   uint16_t channel;
   uint32_t data_size;
   uint32_t seq_number;
   t_medusa_timestamp timestamp;
   t_medusa_timestamp received;
};
_Static_assert(sizeof(t_medusa_message_loopback) == 32,
      "ERROR: medusa_message_loopback size");

/* --------------Functions to create, pack and unpack messages ---------------*/

t_medusa_message_hello * medusa_message_create_hello(void);

size_t medusa_message_pack_hello(
            t_medusa_message_hello * message,
            void * msg
            );

t_medusa_message_hello * medusa_message_unpack_hello(
            void* msg,
            size_t size
            );

t_medusa_message_bye * medusa_message_create_bye(void);

size_t medusa_message_pack_bye(
            t_medusa_message_bye * message,
            void * msg
            );

t_medusa_message_bye * medusa_message_unpack_bye(
            void* msg,
            size_t size
            );

t_medusa_message_add_node * medusa_message_create_add_node(
            uint64_t node_uid,
            char * name
            );

size_t medusa_message_pack_add_node(
            t_medusa_message_add_node * message,
            void * msg
            );

t_medusa_message_add_node * medusa_message_unpack_add_node(
            void* msg,
            size_t size
            );

t_medusa_message_remove_node * medusa_message_create_remove_node(
            uint64_t node_uid
            );

size_t medusa_message_pack_remove_node(
            t_medusa_message_remove_node * message,
            void * msg
            );

t_medusa_message_remove_node * medusa_message_unpack_remove_node(
            void* msg,
            size_t size
            );

t_medusa_message_network_config * medusa_message_create_network_config(
            uint64_t node_uid,
            uint64_t resource_uid,
            const t_medusa_network_config * network_config
            );

size_t medusa_message_pack_network_config(
            t_medusa_message_network_config * message,
            void * msg
            );

t_medusa_message_network_config * medusa_message_unpack_network_config(
            void* msg,
            size_t size
            );

t_medusa_message_data * medusa_message_create_data(
            uint32_t seq_number,
            uint16_t channel,
            void * data,
            t_medusa_timestamp timestamp,
            uint32_t data_size,
            MEDUSA_DATA_TYPE data_type
            );

size_t medusa_message_pack_data(
            t_medusa_message_data * message,
            void * msg
            );

t_medusa_message_data * medusa_message_unpack_data(
            void* msg,
            size_t size
            );

t_medusa_message_loopback * medusa_message_create_loopback(
            uint32_t seq_number,
            uint16_t channel,
            uint32_t data_size,
            MEDUSA_DATA_TYPE data_type,
            t_medusa_timestamp timestamp,
            t_medusa_timestamp received
            );

size_t medusa_message_pack_loopback(
            t_medusa_message_loopback * message,
            void * msg
            );

t_medusa_message_loopback * medusa_message_unpack_loopback(
            void* msg,
            size_t size
            );


#endif /* MEDUSA_MESSAGES_H */
