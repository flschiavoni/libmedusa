#include "medusa.h"

/** @file medusa_audio_receiver.c
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
   MEDUSA RECEIVER GET LOCAL AUDIO CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_local_audio_channels(t_medusa_receiver * receiver){
   if(receiver->local_audio_resource == NULL)
      return 0;
   else
      return receiver->local_audio_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NETWORK AUDIO CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_network_audio_channels(t_medusa_receiver * receiver){
   if(receiver->network_audio_resource == NULL)
      return 0;
   else
      return receiver->network_audio_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET LOCAL AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
const t_medusa_audio_resource * medusa_receiver_get_local_audio_resource(
      t_medusa_receiver * receiver){
   return receiver->local_audio_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NETWORK AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_audio_resource * medusa_receiver_get_network_audio_resource(
      t_medusa_receiver * receiver){
   return receiver->network_audio_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER PREPARE AUDIO DATA
   ---------------------------------------------------------------------------*/
void medusa_receiver_prepare_audio_data(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet){

   if(receiver->network_audio_resource == NULL
            || packet->channel >= receiver->network_audio_resource->channels)
      return;

   int data_size = packet->data_size;

   // Detect packet loss
   if(packet->seq_number != receiver->seq_number[packet->channel] + 1
         && receiver->seq_number[packet->channel] != 0){
      if(receiver->data_lost_callback != NULL)
         receiver->data_lost_callback->function(
               receiver->data_lost_callback->args);
      // What else can I do?
   }

   // Adjust audio representation

   char clone_data[MEDUSA_RECEIVE_DATA_SIZE];
   memcpy(clone_data, packet->data, packet->data_size);
   char temp_data[MEDUSA_RECEIVE_DATA_SIZE];

   if(receiver->network_audio_resource->bit_depth !=
         receiver->local_audio_resource->bit_depth){
      data_size = medusa_dsp_change_quantization(
               receiver->network_audio_resource->bit_depth,
               receiver->local_audio_resource->bit_depth,
               (void *)clone_data,
               (void *)temp_data,
               data_size);
      memcpy(clone_data, temp_data, data_size);
   }

   if(receiver->network_audio_resource->sample_rate !=
         receiver->local_audio_resource->sample_rate){
      data_size = medusa_dsp_change_sample_rate(
            receiver->network_audio_resource->sample_rate,
            receiver->local_audio_resource->sample_rate,
            (void *)clone_data,
            (void *)temp_data,
            data_size);
      memcpy(clone_data, temp_data, data_size);
   }
   // Write on channel ring buffer

   data_size = medusa_ringbuffer_write(receiver->data_rb[packet->channel],
               clone_data, data_size);

   receiver->seq_number[packet->channel] = packet->seq_number;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER READ AUDIO DATA
   ---------------------------------------------------------------------------*/
int medusa_receiver_read_audio_data(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      uint16_t data_size){

   if(channel > medusa_receiver_get_network_audio_channels(receiver) - 1)
      return 0;

   size_t rb_space = medusa_ringbuffer_read_space(receiver->data_rb[channel]);

   // if muted, play zeros and advance the ringbuffer
   if(receiver->muted_channels[channel] == MEDUSA_MUTED){
      memset(data, '\0', data_size);
      medusa_ringbuffer_read_advance(receiver->data_rb[channel], data_size);
      return data_size;
   }

   // If no data to play, play zeros
   if((int) rb_space < data_size){
      memset(data, '\0', data_size);
      return 0;
   }

   //To be faster, if we have exceeding data in this rb, waste it
   if((int)rb_space > data_size * 10){
      medusa_ringbuffer_read_advance(receiver->data_rb[channel],
               rb_space - data_size);
//      printf("Audio eliminado\n");
   }

   size_t size = medusa_ringbuffer_read(
               receiver->data_rb[channel],
               (char *)data,
               data_size);
   //trigger loopback, if on
   medusa_receiver_trigger_loopback(receiver);

   //apply volume
   if(receiver->channels_volume[channel] != 1){
      medusa_dsp_gain(data, data_size / 4, receiver->channels_volume[channel]);
   }

   return size;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET AUDIO CONFIG CHANGE CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_audio_config_callback(
      t_medusa_receiver * receiver,
      t_medusa_audio_config_callback_function config_change_callback,
      void * args){
   if(receiver->audio_config_callback == NULL)
      receiver->audio_config_callback
            = malloc(sizeof(t_medusa_audio_config_callback));
   receiver->audio_config_callback->function = config_change_callback;
   receiver->audio_config_callback->args = args;
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CONNECT AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
int medusa_receiver_connect_audio_resource(
            t_medusa_receiver * receiver, char * name){
   if(receiver->control == NULL)
      return 0;
   usleep(200);
   t_medusa_node_audio_resource * resource;
   resource = medusa_control_get_audio_resource_by_name(receiver->control,name);
   if(resource == NULL || resource->resource_type != MEDUSA_SENDER)
      return 0;
   medusa_receiver_set_network_audio_resource(
         receiver,
         resource->channels,
         resource->sample_rate,
         resource->bit_depth,
         resource->endianness,
         resource->block_size,
         resource->CODEC);
   medusa_receiver_start_network(
         receiver,
         resource->protocol,
         resource->ip,
         resource->port);
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CREATE AUDIO CHANNELS
   ---------------------------------------------------------------------------*/
void medusa_receiver_create_audio_channels(t_medusa_receiver * receiver){
   if(receiver->local_audio_resource == NULL)
      return;

   if(receiver->network_audio_resource == NULL)
      receiver->network_audio_resource = medusa_audio_resource_create();

   medusa_audio_resource_check_config(
         receiver->local_audio_resource,
         receiver->network_audio_resource);

   int channels = medusa_receiver_get_network_audio_channels(receiver);
   receiver->seq_number = malloc(channels * sizeof (uint32_t));
   receiver->data_rb = malloc(channels * sizeof (t_medusa_ringbuffer));
   receiver->muted_channels = malloc(channels * sizeof (MEDUSA_MUTE));
   receiver->channels_volume = malloc(channels * sizeof(float));

   short i = 0;
   for (i = 0; i < channels; i++){
      receiver->data_rb[i]
               = medusa_ringbuffer_create(
                     (int)(receiver->local_audio_resource->bit_depth 
                     * receiver->local_audio_resource->sample_rate));
      medusa_ringbuffer_reset(receiver->data_rb[i]);
      receiver->seq_number[i] = 0;
      receiver->muted_channels[i] = MEDUSA_UNMUTED;
      receiver->channels_volume[i] = 1;
   }
   if(receiver->control)
      medusa_control_notify_add_audio_resource(
            receiver->control,
            NULL,
            MEDUSA_RECEIVER,
            receiver->name,
            receiver->uid,
            receiver->network_audio_resource);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER RECEIVE AUDIO
   ---------------------------------------------------------------------------*/
void * medusa_receiver_receive_audio(void * arg){
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

      //Write the received data to network buffer
      size = medusa_ringbuffer_write(receiver->net_rb, data, size);

      if(data[0] == MEDUSA_AUDIO_CONFIG){
         size = medusa_ringbuffer_read(
                  receiver->net_rb,
                  (char *)config,
                  sizeof(t_medusa_message_audio_config) + 1);
         if(receiver->network_audio_resource == NULL)
            continue;
/*         receiver->audio_resource->channels = config->channels;*/
         receiver->network_audio_resource->sample_rate = config->sample_rate;
         receiver->network_audio_resource->bit_depth = config->bit_depth;
         receiver->network_audio_resource->endianness = config->endianness;
         receiver->network_audio_resource->block_size = config->block_size;
         receiver->network_audio_resource->CODEC = config->CODEC;
         if(receiver->audio_config_callback != NULL)
            receiver->audio_config_callback->function(
                     config,
                     receiver->audio_config_callback->args);
      }

      if(data[0] == MEDUSA_BYE){
         medusa_receiver_server_connected(
            medusa_client_get_network_config(receiver->client),
            MEDUSA_DISCONNECTED,
            receiver);
         continue;
      }

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

         if(packet->data_type == MEDUSA_AUDIO)
            medusa_receiver_prepare_data(receiver, packet);
         if(receiver->data_received_callback != NULL)
            receiver->data_received_callback->function(
                     size,
                     receiver->data_received_callback->args);
         medusa_receiver_prepare_loopback(receiver, packet);
      }
   }
   free(data);
   free(packet);
   pthread_exit(0);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CREATE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_receiver_create_audio_resource(
      t_medusa_receiver * receiver,
      uint16_t channels,
      float sample_rate,
      MEDUSA_BIT_DEPTH bit_depth,
      uint16_t block_size){

   if(channels < 1)
      return;
   if(receiver->local_audio_resource == NULL)
      receiver->local_audio_resource = medusa_audio_resource_create();
   medusa_audio_resource_set(
               receiver->local_audio_resource,
               channels,
               sample_rate,
               bit_depth,
               medusa_dsp_get_endianness(),
               block_size,
               MEDUSA_NOCODEC);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET NETWORK AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_receiver_set_network_audio_resource(
   t_medusa_receiver * receiver,
   uint16_t channels,
   float sample_rate,
   MEDUSA_BIT_DEPTH bit_depth,
   MEDUSA_ENDIANNESS endianness,
   uint16_t block_size,
   MEDUSA_CODEC CODEC
   ){

   if(receiver->network_audio_resource == NULL)
      receiver->network_audio_resource = medusa_audio_resource_create();

   medusa_audio_resource_set(
         receiver->network_audio_resource,
         channels,
         sample_rate,
         bit_depth,
         endianness,
         block_size,
         CODEC);
}

/*----------------------------------------------------------------------------*/
