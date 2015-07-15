#include "medusa.h"

/** @file medusa_messages.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE HELLO
   ---------------------------------------------------------------------------*/
t_medusa_message_hello * medusa_message_create_hello(){
   t_medusa_message_hello * message;
   message = malloc(sizeof(message));
   message->type = MEDUSA_HELLO;
   message->version = MEDUSA_VERSION;
   message->size = 8;
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK HELLO
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_hello(
      t_medusa_message_hello * message,
      void * msg){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK HELLO
   ---------------------------------------------------------------------------*/
t_medusa_message_hello * medusa_message_unpack_hello(void * msg, size_t size){

   (void) size;
   int shift = 0;
   t_medusa_message_hello * message;
   message = malloc(sizeof(message));
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE BYE
   ---------------------------------------------------------------------------*/
t_medusa_message_bye * medusa_message_create_bye(){
   t_medusa_message_bye * message;
   message = malloc(sizeof(t_medusa_message_bye));
   message->type = MEDUSA_BYE;
   message->version = MEDUSA_VERSION;
   message->size = 8;
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK BYE
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_bye(
      t_medusa_message_bye * message,
      void * msg
      ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK BYE
   ---------------------------------------------------------------------------*/
t_medusa_message_bye * medusa_message_unpack_bye(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   t_medusa_message_bye * message;
   message = malloc(sizeof(t_medusa_message_bye));
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE ADD NODE
   ---------------------------------------------------------------------------*/
t_medusa_message_add_node * medusa_message_create_add_node(
            uint64_t node_uid,
            char * name
         ){

   uint16_t size = 24 + strlen(name);
   size += ((size % 4) > 0) ? 4 - (size % 4) : 0;

   t_medusa_message_add_node * message;
   message = malloc(size);
   message->type = MEDUSA_ADD_NODE;
   message->version = MEDUSA_VERSION;
   message->size = size;
   message->node_uid = node_uid;
   message->name_size = strlen(name);
   strcpy(message->name, name);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK ADD NODE
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_add_node(
      t_medusa_message_add_node * message,
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
   shift += medusa_pack_uint8_t(msg, message->name_size, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_blob(msg, message->name, message->name_size,  shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK ADD NODE
   ---------------------------------------------------------------------------*/
t_medusa_message_add_node * medusa_message_unpack_add_node(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   uint16_t data_size = 0;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_add_node * message;
   message = malloc(data_size);
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += 4; //padding
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint8_t(&message->name_size, msg, shift);
   shift += 3; //padding
   shift += medusa_unpack_blob(message->name, msg, message->name_size, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE REMOVE NODE
   ---------------------------------------------------------------------------*/
t_medusa_message_remove_node * medusa_message_create_remove_node(
            uint64_t node_uid
            ){
   t_medusa_message_remove_node * message;
   message = malloc(sizeof(t_medusa_message_remove_node));
   message->type = MEDUSA_REMOVE_NODE;
   message->version = MEDUSA_VERSION;
   message->size = 20;
   message->node_uid = node_uid;
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK REMOVE NODE
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_remove_node(
      t_medusa_message_remove_node * message,
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
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK REMOVE NODE
   ---------------------------------------------------------------------------*/
t_medusa_message_remove_node * medusa_message_unpack_remove_node(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   t_medusa_message_remove_node * message;
   message = malloc(sizeof(t_medusa_message_remove_node));
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += 4; // padding
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE NETWORK CONFIG
   ---------------------------------------------------------------------------*/
t_medusa_message_network_config * medusa_message_create_network_config(
            uint64_t node_uid,
            uint64_t resource_uid,
            const t_medusa_network_config * network_config
            ){

   uint16_t size = 32 + strlen(medusa_network_config_get_ip(network_config));
   size += ((size % 4) > 0) ? 4 - (size % 4) : 0;

   t_medusa_message_network_config * message;
   message = malloc(size);
   message->type = MEDUSA_NETWORK_CONFIG;
   message->version = MEDUSA_VERSION;
   message->size = size;
   message->node_uid = node_uid;
   message->resource_uid = resource_uid;
   message->protocol = network_config->protocol;
   message->port = medusa_network_config_get_port(network_config);
   message->ip_size = strlen(medusa_network_config_get_ip(network_config));
   strcpy(message->ip, medusa_network_config_get_ip(network_config));
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK NETWORK CONFIG
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_network_config(
      t_medusa_message_network_config * message,
      void * msg
      ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_uint16_t(msg, message->port, shift);
   shift += medusa_pack_uint8_t(msg, message->protocol, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_uint64_t(msg, message->node_uid, shift);
   shift += medusa_pack_uint64_t(msg, message->resource_uid, shift);
   shift += medusa_pack_uint8_t(msg, message->ip_size, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_padding(msg, shift);
   shift += medusa_pack_blob(msg, message->ip, message->ip_size,  shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK NETWORK CONFIG
   ---------------------------------------------------------------------------*/
t_medusa_message_network_config * medusa_message_unpack_network_config(
      void* msg,
      size_t size
      ){
   (void) size;
   int shift = 0;
   uint16_t data_size = 0;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_network_config * message;
   message = malloc(data_size);//PADDING
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += medusa_unpack_uint16_t(&message->port, msg, shift);
   shift += medusa_unpack_uint8_t(&message->protocol, msg, shift);
   shift += 1; // padding
   shift += medusa_unpack_uint64_t(&message->node_uid, msg, shift);
   shift += medusa_unpack_uint64_t(&message->resource_uid, msg, shift);
   shift += medusa_unpack_uint8_t(&message->ip_size, msg, shift);
   shift += 7; // padding
   shift += medusa_unpack_blob(message->ip, msg, message->ip_size, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE DATA
   ---------------------------------------------------------------------------*/
t_medusa_message_data * medusa_message_create_data(
            uint32_t seq_number,
            uint16_t channel,
            void * data,
            t_medusa_timestamp timestamp,
            uint32_t data_size,
            MEDUSA_DATA_TYPE data_type
            ){
   uint16_t size = 24 + data_size;
   size += ((size % 4) > 0) ? 4 - (size % 4) : 0;

   t_medusa_message_data * message;
   message = malloc(size);
   message->type = MEDUSA_DATA;
   message->version = MEDUSA_VERSION;
   message->size = size;
   message->data_type = data_type;
   message->seq_number = seq_number;
   message->channel = channel;
   message->timestamp = timestamp;
   message->data_size = data_size;
   memcpy(message->data, data, data_size);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK DATA
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_data(
            t_medusa_message_data * message,
            void * msg
            ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_uint16_t(msg, message->data_type, shift);
   shift += medusa_pack_uint32_t(msg, message->seq_number, shift);
   shift += medusa_pack_timestamp(msg, message->timestamp, shift);
   shift += medusa_pack_uint16_t(msg, message->channel, shift);
   shift += medusa_pack_uint32_t(msg, message->data_size, shift);
   shift += medusa_pack_blob(msg, message->data, message->data_size,  shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK DATA
   ---------------------------------------------------------------------------*/
t_medusa_message_data * medusa_message_unpack_data(
            void* msg,
            size_t size
            ){
   (void) size;
   int shift = 0;
   uint16_t data_size = 0;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_data * message = NULL;
   message = malloc(data_size); //NO PADDING
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += medusa_unpack_uint16_t(&message->data_type, msg, shift);
   shift += medusa_unpack_uint32_t(&message->seq_number, msg, shift);
   shift += medusa_unpack_timestamp(&message->timestamp, msg, shift);
   shift += medusa_unpack_uint16_t(&message->channel, msg, shift);
   shift += medusa_unpack_uint32_t(&message->data_size, msg, shift);
   shift += medusa_unpack_blob(message->data, msg, message->data_size, shift);
   return message;
}

/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE CREATE LOOPBACK
   ---------------------------------------------------------------------------*/
t_medusa_message_loopback * medusa_message_create_loopback(
            uint32_t seq_number,
            uint16_t channel,
            uint32_t data_size,
            MEDUSA_DATA_TYPE data_type,
            t_medusa_timestamp timestamp,
            t_medusa_timestamp received
            ){
   t_medusa_message_loopback * message;
   message = malloc(sizeof(t_medusa_message_loopback));
   message->type = MEDUSA_LOOPBACK_DATA;
   message->version = MEDUSA_VERSION;
   message->size = 36;
   message->data_type = data_type;
   message->seq_number = seq_number;
   message->channel = channel;
   message->timestamp = timestamp;
   message->received = received;
   message->data_size = data_size;
   return message;

}


/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE PACK LOOPBACK
   ---------------------------------------------------------------------------*/
size_t medusa_message_pack_loopback(
            t_medusa_message_loopback * message,
            void * msg
            ){
   int shift = 0;
   shift += medusa_pack_uint8_t(msg, message->type, shift);
   shift += medusa_pack_uint8_t(msg, message->version, shift);
   shift += medusa_pack_uint16_t(msg, message->size, shift);
   shift += medusa_pack_uint16_t(msg, message->data_type, shift);
   shift += medusa_pack_uint32_t(msg, message->seq_number, shift);
   shift += medusa_pack_timestamp(msg, message->timestamp, shift);
   shift += medusa_pack_timestamp(msg, message->received, shift);
   shift += medusa_pack_uint16_t(msg, message->channel, shift);
   shift += medusa_pack_uint32_t(msg, message->data_size, shift);
   shift += medusa_pack_end(msg, shift);
   return shift;
}


/* -----------------------------------------------------------------------------
   MEDUSA MESSAGE UNPACK LOOPBACK
   ---------------------------------------------------------------------------*/
t_medusa_message_loopback * medusa_message_unpack_loopback(
            void* msg,
            size_t size
            ){
   (void) size;
   int shift = 0;
   uint16_t data_size = 0;
   medusa_unpack_uint16_t(&data_size, msg, 2);
   t_medusa_message_loopback * message;
   message = malloc(data_size);
   shift += medusa_unpack_uint8_t(&message->type, msg, shift);
   shift += medusa_unpack_uint8_t(&message->version, msg, shift);
   shift += medusa_unpack_uint16_t(&message->size, msg, shift);
   shift += medusa_unpack_uint16_t(&message->data_type, msg, shift);
   shift += medusa_unpack_uint32_t(&message->seq_number, msg, shift);
   shift += medusa_unpack_timestamp(&message->timestamp, msg, shift);
   shift += medusa_unpack_timestamp(&message->received, msg, shift);
   shift += medusa_unpack_uint16_t(&message->channel, msg, shift);
   shift += medusa_unpack_uint32_t(&message->data_size, msg, shift);
   return message;
}

/*----------------------------------------------------------------------------*/
