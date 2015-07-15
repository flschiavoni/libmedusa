#include "medusa.h"

/** @file medusa_midi_receiver.c
 *
 * @brief
 * 
 *
 * @ingroup control
 */
extern void medusa_receiver_trigger_loopback(
      t_medusa_receiver * receiver
      );

extern void medusa_receiver_prepare_loopback(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet
      );

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET LOCAL MIDI CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_local_midi_channels(t_medusa_receiver * receiver){
   if(receiver->local_midi_resource == NULL)
      return 0;
   else
      return receiver->local_midi_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NETWORK MIDI CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_network_midi_channels(t_medusa_receiver * receiver){
   if(receiver->network_midi_resource == NULL)
      return 0;
   else
      return receiver->network_midi_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET LOCAL MIDI RESOURCE
   ---------------------------------------------------------------------------*/
const t_medusa_midi_resource * medusa_receiver_get_local_midi_resource(
      t_medusa_receiver * receiver){
   return receiver->local_midi_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NETWORK MIDI RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_midi_resource * medusa_receiver_get_network_midi_resource(
      t_medusa_receiver * receiver){
   if(receiver->network_midi_resource != NULL)
      return receiver->network_midi_resource;
   else 
      return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PREPARE MIDI DATA
   ---------------------------------------------------------------------------*/
void medusa_receiver_prepare_midi_data(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet){

   //Bad data or incompatible # of channels between sender and receiver
   if(receiver->network_midi_resource == NULL
      || packet->channel >= receiver->network_midi_resource->channels){
         return;
   }
   // Detect packet loss
   if(packet->seq_number != receiver->seq_number[packet->channel] + 1
         && receiver->seq_number[packet->channel] != 0)
      if(receiver->data_lost_callback != NULL)
         receiver->data_lost_callback->function(
                  receiver->data_lost_callback->args);
   //Write data on midi channel
   if(medusa_ringbuffer_write_space(receiver->data_rb[packet->channel])
         >= (size_t) packet->data_size + sizeof(packet->data_size)){
      medusa_ringbuffer_write(receiver->data_rb[packet->channel],
            packet->data, packet->data_size);
   }
   receiver->seq_number[packet->channel] = packet->seq_number;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET MIDI DATA SIZE
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_midi_data_size(
      t_medusa_receiver * receiver,
      int channel){
   if(channel <= medusa_receiver_get_network_midi_channels(receiver)
         && receiver->data_rb != NULL){
      return medusa_ringbuffer_read_space(receiver->data_rb[channel]);
   }
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER READ MIDI DATA
   ---------------------------------------------------------------------------*/
int medusa_receiver_read_midi_data(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      uint16_t data_size){

   if(channel >= medusa_receiver_get_network_midi_channels(receiver))
      return 0;

   // if muted, advance the ringbuffer
   if(receiver->muted_channels[channel] == MEDUSA_MUTED){
      medusa_ringbuffer_read_advance(receiver->data_rb[channel], data_size);
      return 0;
   }

   if(medusa_ringbuffer_read_space(receiver->data_rb[channel]) < data_size)
      return 0;

   // Read the first position. Should be the size of the event.
   medusa_ringbuffer_read(
            receiver->data_rb[channel],
            (char *) &data_size,
            sizeof(data_size));
   //loopback trigger
   medusa_receiver_trigger_loopback(receiver);
   return medusa_ringbuffer_read(receiver->data_rb[channel], data, data_size);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CONNECT
   ---------------------------------------------------------------------------*/
int medusa_receiver_connect_midi_resource(
         t_medusa_receiver * receiver, char * name){
   if(receiver->control == NULL)
      return 0;
   usleep(200);
   t_medusa_node_midi_resource * resource;
   resource = medusa_control_get_midi_resource_by_name(receiver->control, name);
   if(resource == NULL || resource->resource_type != MEDUSA_SENDER)
      return 0;
   medusa_receiver_set_network_midi_resource(receiver, resource->channels);
   medusa_receiver_start_network(
         receiver,
         resource->protocol,
         resource->ip,
         resource->port);
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CREATE MIDI CHANNELS
   ---------------------------------------------------------------------------*/
void medusa_receiver_create_midi_channels(t_medusa_receiver * receiver){
   if(receiver->local_midi_resource == NULL)
      return;

   if(receiver->network_midi_resource == NULL)
      receiver->network_midi_resource = medusa_midi_resource_create();

   medusa_midi_resource_check_config(
            receiver->local_midi_resource,
            receiver->network_midi_resource);

   int channels = medusa_receiver_get_network_midi_channels(receiver);
   receiver->seq_number = malloc(channels * sizeof (uint32_t));
   receiver->data_rb = malloc(channels * sizeof (t_medusa_ringbuffer *));
   receiver->muted_channels = malloc(channels * sizeof (MEDUSA_MUTE));
   receiver->channels_volume = malloc(channels * sizeof(float));

   short i = 0;
   for (i = 0; i < channels; i++){
      receiver->data_rb[i] = medusa_ringbuffer_create(MEDUSA_RING_BUFFER_SIZE);
      medusa_ringbuffer_reset(receiver->data_rb[i]);
      receiver->seq_number[i] = 0;
      receiver->muted_channels[i] = MEDUSA_UNMUTED;
      receiver->channels_volume[i] = 1;
   }
   if(receiver->control)
      medusa_control_notify_add_midi_resource(
            receiver->control,
            NULL,
            MEDUSA_RECEIVER,
            receiver->name,
            receiver->uid,
            receiver->network_midi_resource);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER RECEIVE MIDI
   ---------------------------------------------------------------------------*/
void * medusa_receiver_receive_midi(void * arg){
   t_medusa_receiver *receiver = (t_medusa_receiver *) arg;
   int size = 0;
   char * data = malloc(MEDUSA_RECEIVE_DATA_SIZE);
   t_medusa_message_data * packet = malloc(MEDUSA_RECEIVE_DATA_SIZE);
   t_medusa_message_audio_config * config
            = malloc(sizeof(t_medusa_message_audio_config));
   while(receiver->status == MEDUSA_CONNECTED){
      //Client not connected, sleep
      if(medusa_client_get_status(receiver->client) == MEDUSA_DISCONNECTED){
         sleep(1);
         continue;
      }
      // Receive Data from network. This is a blocking method
      size = medusa_client_receive(receiver->client, ((void *) data));
      if((int)size < 1){
         continue;
      }

      if(receiver->data_received_callback != NULL){
         receiver->data_received_callback->function(
                  size,
                  receiver->data_received_callback->args);
      }

      if(data[0] == MEDUSA_BYE){
         medusa_receiver_server_connected(
            medusa_client_get_network_config(receiver->client),
            MEDUSA_DISCONNECTED,
            receiver);
         continue;
      }

      //Write the received data to network buffer
      size = medusa_ringbuffer_write(receiver->net_rb, data, size);

      // Empty the network buffer
      while(medusa_ringbuffer_read_space(receiver->net_rb)
            >= sizeof(t_medusa_message_data)){

      //Read the rb without advance reader pointer
         size = medusa_ringbuffer_peek(receiver->net_rb,
               (void *)(packet),
               sizeof(t_medusa_message_data));

         //Data is still missing
         if(medusa_ringbuffer_read_space(receiver->net_rb) <
               sizeof(t_medusa_message_data)
               + (size_t)packet->data_size){
            break;
         }

         size = medusa_ringbuffer_read(receiver->net_rb,
               (void *)packet,
               sizeof(t_medusa_message_data) + packet->data_size);

         if(packet->data_type == MEDUSA_MIDI)
            medusa_receiver_prepare_data(receiver, packet);
         medusa_receiver_prepare_loopback(receiver, packet);
      }
   }
   free(data);
   free(packet);
   pthread_exit(0);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CREATE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_receiver_create_midi_resource(
      t_medusa_receiver * receiver,
      uint16_t local_channels){
   if(local_channels < 1)
      return;

   if(receiver->local_midi_resource == NULL)
      receiver->local_midi_resource = medusa_midi_resource_create();
   medusa_midi_resource_set(
                  receiver->local_midi_resource,
                  local_channels);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET NETWORK MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_receiver_set_network_midi_resource(
      t_medusa_receiver * receiver,
      uint16_t channels){

   if(receiver->network_midi_resource == NULL)
      receiver->network_midi_resource = medusa_midi_resource_create();

   medusa_midi_resource_set(receiver->network_midi_resource, channels);
}

/* ---------------------------------------------------------------------------*/
