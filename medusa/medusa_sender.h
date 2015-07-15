#ifndef MEDUSA_SENDER_H
#define MEDUSA_SENDER_H

#include "medusa.h"

/** @file medusa_sender.h
 *
 * @brief
 * 
 *
 * @ingroup control
 */

// -----------------------------------------------------------------------------
// ------------------------------ SENDER ---------------------------------------
// -----------------------------------------------------------------------------

typedef void (*t_medusa_sender_prepare_data)(
      t_medusa_sender * sender,
      int channel,
      void * data,
      uint16_t data_size
      );

typedef void (*t_medusa_sender_create_channels)(
      t_medusa_sender * sender
      );

typedef void * (*t_medusa_sender_send)(
      void * arg
      );

typedef int (*t_medusa_sender_get_channels)(
      t_medusa_sender * sender
      );

struct medusa_sender{
   char name[MEDUSA_LOCAL_RESOURCES];
   t_medusa_server * server;
   t_medusa_control * control;
   t_medusa_message_loopback * lb_packet;
   MEDUSA_STATUS status;
   MEDUSA_STATUS loopback;
   MEDUSA_DATA_TYPE data_type;

   uint64_t uid;

   t_medusa_audio_resource * local_audio_resource;
   t_medusa_audio_resource * network_audio_resource;

   t_medusa_midi_resource * local_midi_resource;
   t_medusa_midi_resource * network_midi_resource;

   uint32_t * seq_number;
   MEDUSA_MUTE * muted_channels;
   float * channels_volume;
   t_medusa_ringbuffer ** data_rb;
   t_medusa_message_data * packet;

   int new_client;

   pthread_t send_thread;
   pthread_mutex_t dataMutex;
   pthread_cond_t dataPresentCondition;
   int data_ready;

   t_medusa_connection_callback * connection_callback;
   t_medusa_data_lost_callback * data_lost_callback;
   t_medusa_data_sent_callback * data_sent_callback;
   t_medusa_meter_callback * meter_callback;
   t_medusa_free_callback * free_callback;
   t_medusa_stop_callback * stop_callback;
   t_medusa_list * prepare_data_callback_list;

   t_medusa_sender_prepare_data prepare;
   t_medusa_sender_create_channels create_channels;
   t_medusa_sender_send send;
   t_medusa_sender_get_channels get_local_channels;
   t_medusa_sender_get_channels get_network_channels;
   };

t_medusa_sender * medusa_sender_create(
      t_medusa_control * control,
      MEDUSA_DATA_TYPE data_type,
      char * name
      );

void medusa_sender_free(
      t_medusa_sender * sender
      );

void medusa_sender_start_network(
      t_medusa_sender * sender,
      MEDUSA_PROTOCOL protocol,
      int port
      );

void medusa_sender_prepare_data(
      t_medusa_sender * sender,
      int channel,
      void * data,
      int data_size
      );

void medusa_sender_send_data(
      t_medusa_sender * sender
      );

void medusa_sender_client_connected(
      const t_medusa_network_config * client,
      MEDUSA_STATUS status,
      void * arg
      );

MEDUSA_DATA_TYPE medusa_sender_get_data_type(
      t_medusa_sender * sender
      );

void medusa_sender_create_audio_resource(
      t_medusa_sender * sender,
      uint16_t local_channels,
      uint32_t local_sample_rate,
      MEDUSA_BIT_DEPTH local_bit_depth,
      uint16_t local_block_size
      );

void medusa_sender_set_network_audio_resource(
      t_medusa_sender * sender,
      uint16_t network_channels,
      uint32_t network_sample_rate,
      MEDUSA_BIT_DEPTH network_bit_depth,
      MEDUSA_ENDIANNESS network_endianness,
      uint16_t network_block_size,
      MEDUSA_CODEC network_CODEC
      );

void medusa_sender_create_midi_resource(
      t_medusa_sender * sender,
      uint16_t local_channels
      );

void medusa_sender_set_network_midi_resource(
      t_medusa_sender * sender,
      uint16_t network_channels
      );

int medusa_sender_get_network_channels(
      t_medusa_sender * sender
      );

int medusa_sender_get_local_channels(
      t_medusa_sender * sender
      );

void medusa_sender_set_loopback(
      t_medusa_sender * sender,
      MEDUSA_STATUS status
      );

void medusa_sender_mute_channel(
      t_medusa_sender * sender,
      MEDUSA_MUTE state,
      int channel
      );

void medusa_sender_set_channel_volume(
      t_medusa_sender * sender,
      int channel,
      float gain
      );

char * medusa_sender_get_name(
      t_medusa_sender * sender
      );

uint64_t medusa_sender_get_uid(
      t_medusa_sender * sender
      );

MEDUSA_STATUS medusa_sender_get_status(
      t_medusa_sender * sender
      );

MEDUSA_STATUS medusa_sender_get_loopback_status(
      t_medusa_sender * sender
      );

const t_medusa_midi_resource * medusa_sender_get_local_midi_resource(
      t_medusa_sender * sender
      );

t_medusa_midi_resource * medusa_sender_get_network_midi_resource(
      t_medusa_sender * sender
      );

const t_medusa_audio_resource * medusa_sender_get_local_audio_resource(
      t_medusa_sender * sender
      );

t_medusa_audio_resource * medusa_sender_get_network_audio_resource(
      t_medusa_sender * sender
      );

const t_medusa_network_config * medusa_sender_get_network_config(
      t_medusa_sender * sender
      );

void medusa_sender_set_control(
      t_medusa_sender * sender,
      t_medusa_control * control
      );

void medusa_sender_receive_data(
      const t_medusa_network_config * client,
      void * data,
      int data_size,
      void * args
      );

int medusa_sender_compare(
      const void * data1,
      const void * data2
      );

// CALLBACK SETTINGS
void medusa_sender_set_connection_callback(
      t_medusa_sender * sender,
      t_medusa_connection_callback_function connection_callback,
      void * args
      );

void medusa_sender_set_data_lost_callback(
      t_medusa_sender * sender,
      t_medusa_data_lost_callback_function data_lost_callback,
      void * args
      );

void medusa_sender_set_data_sent_callback(
      t_medusa_sender * sender,
      t_medusa_data_sent_callback_function data_sent_callback,
      void * args
      );

int medusa_sender_set_meter_callback(
      t_medusa_sender * sender,
      t_medusa_meter_callback_function meter_callback,
      void * args
      );

int medusa_sender_set_free_callback(
      t_medusa_sender * sender,
      t_medusa_free_callback_function free_callback,
      void * args
      );

int medusa_sender_set_stop_callback(
      t_medusa_sender * sender,
      t_medusa_stop_callback_function stop_callback,
      void * args
      );

int medusa_sender_set_prepare_data_callback(
      t_medusa_sender * sender,
      t_medusa_prepare_data_callback_function function,
      void * args
      );

#endif /* MEDUSA_SENDER_H */
