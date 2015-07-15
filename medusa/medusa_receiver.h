#ifndef MEDUSA_RECEIVER_H
#define MEDUSA_RECEIVER_H

#include "medusa.h"

/** @file medusa_receiver.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

// -----------------------------------------------------------------------------
// -------------------------------- RECEIVER -----------------------------------
// -----------------------------------------------------------------------------

typedef void (* t_medusa_receiver_create_channels)(
      t_medusa_receiver * receiver
      );

typedef void (* t_medusa_receiver_prepare_data)(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet
      );

typedef int (* t_medusa_receiver_read_data)(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      uint16_t data_size
      );

typedef void * (* t_medusa_receiver_receive)(
      void * arg
      );

typedef int (* t_medusa_receiver_connect_resource)(
      t_medusa_receiver * receiver,
      char * name
      );

typedef int (* t_medusa_receiver_get_channels)(
      t_medusa_receiver * receiver
      );


struct medusa_receiver{
      char name[MEDUSA_LOCAL_RESOURCES];
      t_medusa_client * client;
      t_medusa_control * control;
      MEDUSA_STATUS status;
      MEDUSA_STATUS loopback;
      MEDUSA_DATA_TYPE data_type;

      uint64_t uid;

      t_medusa_message_loopback * lb_packet;

      t_medusa_audio_resource * local_audio_resource;
      t_medusa_audio_resource * network_audio_resource;

      t_medusa_midi_resource * local_midi_resource;
      t_medusa_midi_resource * network_midi_resource;

      uint32_t * seq_number;
      MEDUSA_MUTE * muted_channels;
      float * channels_volume;
      t_medusa_ringbuffer ** data_rb;

      pthread_t send_thread;
      t_medusa_ringbuffer * loopback_rb;
      pthread_mutex_t loopback_dataMutex;
      pthread_cond_t loopback_dataPresentCondition;
      int loopback_data_ready;

      t_medusa_ringbuffer * net_rb;
      pthread_t receive_thread;

      t_medusa_waiting_server_callback * waiting_server_callback;
      t_medusa_connection_callback * connection_callback;
      t_medusa_data_received_callback * data_received_callback;
      t_medusa_data_lost_callback * data_lost_callback;
      t_medusa_free_callback * free_callback;
      t_medusa_stop_callback * stop_callback;
      t_medusa_list * prepare_data_callback_list;
      t_medusa_audio_config_callback * audio_config_callback;

      t_medusa_receiver_prepare_data prepare_data;
      t_medusa_receiver_create_channels create_channels;
      t_medusa_receiver_read_data read_data;
      t_medusa_receiver_receive receive;
      t_medusa_receiver_connect_resource connect_resource;
      t_medusa_receiver_get_channels get_local_channels;
      t_medusa_receiver_get_channels get_network_channels;
      };

t_medusa_receiver * medusa_receiver_create(
      t_medusa_control * control,
      MEDUSA_DATA_TYPE data_type,
      char * name
      );

void medusa_receiver_prepare_data(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet
      );

void medusa_receiver_free(
      t_medusa_receiver * receiver
      );

int medusa_receiver_connect_resource(
      t_medusa_receiver * receiver,
      char * name
      );

void medusa_receiver_start_network(
      t_medusa_receiver * receiver,
      int protocol,
      const char * ip,
      int port
      );

int medusa_receiver_get_midi_data_size(
      t_medusa_receiver * receiver,
      int channel
      );

int medusa_receiver_read_data(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      int data_size
      );

void medusa_receiver_server_connected(
      const t_medusa_network_config * server,
      MEDUSA_STATUS status,
      void * arg
      );

MEDUSA_DATA_TYPE medusa_receiver_get_data_type(
      t_medusa_receiver * receiver
      );

void medusa_receiver_create_audio_resource(
      t_medusa_receiver * receiver,
      uint16_t local_channels,
      float local_sample_rate,
      MEDUSA_BIT_DEPTH local_bit_depth,
      uint16_t local_block_size
      );

void medusa_receiver_create_midi_resource(
      t_medusa_receiver * receiver,
      uint16_t local_channels
      );

void medusa_receiver_set_network_audio_resource(
      t_medusa_receiver * receiver,
      uint16_t network_channels,
      float network_sample_rate,
      MEDUSA_BIT_DEPTH network_bit_depth,
      MEDUSA_ENDIANNESS network_endianness,
      uint16_t network_block_size,
      MEDUSA_CODEC network_CODEC
      );

void medusa_receiver_set_network_midi_resource(
      t_medusa_receiver * receiver,
      uint16_t network_channels
      );

void medusa_receiver_set_loopback(
      t_medusa_receiver * receiver,
      MEDUSA_STATUS status);

MEDUSA_STATUS medusa_receiver_get_status(
      t_medusa_receiver * receiver
      );

MEDUSA_STATUS medusa_receiver_get_loopback_status(
      t_medusa_receiver * receiver
      );

int medusa_receiver_get_local_channels(
      t_medusa_receiver * receiver
      );

int medusa_receiver_get_network_channels(
      t_medusa_receiver * receiver
      );

void medusa_receiver_mute_channel(
      t_medusa_receiver * receiver,
      MEDUSA_MUTE state,
      int channel
      );

void medusa_receiver_set_channel_volume(
      t_medusa_receiver * receiver,
      int channel,
      float gain
      );

char * medusa_receiver_get_name(
      t_medusa_receiver * receiver
      );

uint64_t medusa_receiver_get_uid(
      t_medusa_receiver * receiver
      );

const t_medusa_midi_resource * medusa_receiver_get_local_midi_resource(
      t_medusa_receiver * receiver
      );

t_medusa_midi_resource * medusa_receiver_get_network_midi_resource(
      t_medusa_receiver * receiver
      );

const t_medusa_audio_resource * medusa_receiver_get_local_audio_resource(
      t_medusa_receiver * receiver
      );

t_medusa_audio_resource * medusa_receiver_get_network_audio_resource(
      t_medusa_receiver * receiver
      );

const t_medusa_network_config * medusa_receiver_get_network_config(
      t_medusa_receiver * receiver
      );

void medusa_receiver_set_control(
      t_medusa_receiver * receiver,
      t_medusa_control * control
      );

int medusa_receiver_compare(
      const void * data1,
      const void * data2
      );

// CALLBACK SETTINGS
int medusa_receiver_set_connection_callback(
      t_medusa_receiver * receiver,
      t_medusa_connection_callback_function server_connected_callback,
      void * args
      );

int medusa_receiver_set_waiting_server_callback(
      t_medusa_receiver * receiver,
      t_medusa_waiting_server_callback_function waiting_server_callback,
      void * args
      );

int medusa_receiver_set_data_received_callback(
      t_medusa_receiver * receiver,
      t_medusa_data_received_callback_function data_received_callback,
      void * args
      );

int medusa_receiver_set_data_lost_callback(
      t_medusa_receiver * receiver,
      t_medusa_data_lost_callback_function data_lost_callback,
      void * args
      );

int medusa_receiver_set_free_callback(
      t_medusa_receiver * receiver,
      t_medusa_free_callback_function free_callback,
      void * args
      );

int medusa_receiver_set_stop_callback(
      t_medusa_receiver * receiver,
      t_medusa_stop_callback_function stop_callback,
      void * args
      );

int medusa_receiver_set_prepare_data_callback(
      t_medusa_receiver * receiver,
      t_medusa_prepare_data_callback_function function,
      void * args
      );

int medusa_receiver_set_audio_config_callback(
      t_medusa_receiver * receiver,
      t_medusa_audio_config_callback_function audio_config_callback,
      void * args
      );

#endif /* MEDUSA_RECEIVER_H */
