#include "medusa.h"

/** @file medusa_midi_sender.c
 *
 * @brief
 * 
 *
 * @ingroup control
 */

extern int medusa_sender_send_packet(
      t_medusa_sender * sender,
      t_medusa_message_data * packet,
      size_t data_size
      );

extern size_t medusa_sender_pack(
      t_medusa_sender * sender,
      uint16_t channel,
      void * data,
      uint32_t data_size,
      t_medusa_message_data * packet,
      MEDUSA_DATA_TYPE data_type
      );

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CREATE MIDI CHANNELS
   ---------------------------------------------------------------------------*/
void medusa_sender_create_midi_channels(t_medusa_sender * sender){
   if(sender->local_midi_resource == NULL)
      return;

   if(sender->network_midi_resource == NULL)
      sender->network_midi_resource = medusa_midi_resource_create();

   medusa_midi_resource_check_config(
            sender->local_midi_resource,
            sender->network_midi_resource);

   int channels = medusa_sender_get_network_channels(sender);
   sender->seq_number = malloc(channels * sizeof (uint32_t));
   sender->data_rb = malloc(channels * sizeof (t_medusa_ringbuffer));
   sender->muted_channels = malloc(channels * sizeof(MEDUSA_MUTE));
   sender->channels_volume = malloc(channels * sizeof(float));

   int i = 0;
   for (i = 0; i < channels; i++){
      sender->data_rb[i] = medusa_ringbuffer_create(MEDUSA_RING_BUFFER_SIZE);
      medusa_ringbuffer_reset(sender->data_rb[i]);
      sender->seq_number[i] = 0;
      sender->muted_channels[i] = MEDUSA_UNMUTED;
      sender->channels_volume[i] = 1;
   }

   if(sender->control)
      medusa_control_notify_add_midi_resource(
               sender->control,
               NULL,
               MEDUSA_SENDER,
               sender->name,
               sender->uid,
               sender->network_midi_resource);
}


/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET LOCAL MIDI RESOURCE
   ---------------------------------------------------------------------------*/
const t_medusa_midi_resource * medusa_sender_get_local_midi_resource(
      t_medusa_sender * sender){
   return sender->local_midi_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NETWORK MIDI RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_midi_resource * medusa_sender_get_network_midi_resource(
      t_medusa_sender * sender){
   return sender->network_midi_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET LOCAL MIDI CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_sender_get_local_midi_channels(t_medusa_sender * sender){
   if(sender->local_midi_resource == NULL)
      return 0;
   else
      return sender->local_midi_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NETWORK MIDI CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_sender_get_network_midi_channels(t_medusa_sender * sender){
   if(sender->network_midi_resource == NULL)
      return 0;
   else
      return sender->network_midi_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PREPARE MIDI DATA
   ---------------------------------------------------------------------------*/

void medusa_sender_prepare_midi_data(
      t_medusa_sender * sender,
      int channel,
      void * data,
      uint16_t data_size){
   if(channel < 0 
         || medusa_sender_get_network_midi_resource(sender) == NULL
         || channel >= medusa_sender_get_network_midi_channels(sender))
      return;
   //store data in RB only if connected
   if(medusa_server_get_client_count(sender->server) < 1)
      return;

   //First add the data_size
   medusa_ringbuffer_write(sender->data_rb[channel],
         (const char *) &data_size, sizeof(data_size));
   // Then add the data
   medusa_ringbuffer_write(sender->data_rb[channel], data, data_size);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SEND MIDI
   ---------------------------------------------------------------------------*/
void * medusa_sender_send_midi(void * arg){
   t_medusa_sender * sender = (t_medusa_sender *) arg;
   size_t data_size = 0;
   int i = 0;
   char temp_data[4096 * 4];
   int size = 0;
   sender->packet = malloc(MEDUSA_RECEIVE_DATA_SIZE);
   while(sender->status == MEDUSA_CONNECTED){
      // Thread will wait for data
      pthread_mutex_lock(&sender->dataMutex);
      while (sender->data_ready == 0)
         pthread_cond_wait(&sender->dataPresentCondition, &sender->dataMutex);
      sender->data_ready--;
      pthread_mutex_unlock(&sender->dataMutex);

      for(i = 0 ; i < medusa_sender_get_network_channels(sender); i++){
         data_size = medusa_ringbuffer_read_space(sender->data_rb[i]);
         if(data_size < 1) continue;
         data_size = medusa_ringbuffer_read(sender->data_rb[i],
               ((void *)temp_data), data_size);
         // if muted, play zeros and advance the ringbuffer
         if(sender->muted_channels[i] == MEDUSA_MUTED){
            memset(temp_data, '\0', data_size);
         }
         data_size = medusa_sender_pack(sender,
               i,
               temp_data,
               data_size,
               sender->packet,
               MEDUSA_MIDI);
         medusa_sender_send_packet(sender, sender->packet, data_size);
      }
   }
   pthread_exit(0);
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CREATE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_sender_create_midi_resource(
      t_medusa_sender * sender,
      uint16_t channels){

   if(sender->local_midi_resource == NULL)
      sender->local_midi_resource = medusa_midi_resource_create();
   medusa_midi_resource_set(sender->local_midi_resource,
                  channels);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET NETWORK MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_sender_set_network_midi_resource(
      t_medusa_sender * sender,
      uint16_t channels
      ){

   if(sender->network_midi_resource == NULL)
      sender->network_midi_resource = medusa_midi_resource_create();

   medusa_midi_resource_set(sender->network_midi_resource, channels);
}

/* ---------------------------------------------------------------------------*/
