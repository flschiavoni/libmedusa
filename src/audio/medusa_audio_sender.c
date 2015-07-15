#include "medusa.h"

/** @file medusa_audio_sender.c
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
   MEDUSA SENDER CREATE AUDIO CHANNELS
   ---------------------------------------------------------------------------*/
void medusa_sender_create_audio_channels(t_medusa_sender * sender){
   if(sender->local_audio_resource == NULL)
      return;

   if(sender->network_audio_resource == NULL)
      sender->network_audio_resource = medusa_audio_resource_create();

   medusa_audio_resource_check_config(
            sender->local_audio_resource,
            sender->network_audio_resource);

   int channels = medusa_sender_get_network_channels(sender);
   sender->seq_number = malloc(channels * sizeof(uint32_t));
   sender->data_rb = malloc(channels * sizeof(t_medusa_ringbuffer *));
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
      medusa_control_notify_add_audio_resource(
               sender->control,
               NULL,
               MEDUSA_SENDER,
               sender->name,
               sender->uid,
               sender->network_audio_resource);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET LOCAL AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
const t_medusa_audio_resource * medusa_sender_get_local_audio_resource(
      t_medusa_sender * sender){
   return sender->local_audio_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NETWORK AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_audio_resource * medusa_sender_get_network_audio_resource(
      t_medusa_sender * sender){
   if(sender->network_audio_resource != NULL)
      return sender->network_audio_resource;
   else return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET LOCAL AUDIO CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_sender_get_local_audio_channels(t_medusa_sender * sender){
   if(sender->local_audio_resource == NULL)
      return 0;
   else
      return sender->local_audio_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NETWORK AUDIO CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_sender_get_network_audio_channels(t_medusa_sender * sender){
   if(sender->network_audio_resource == NULL)
      return 0;
   else
      return sender->network_audio_resource->channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PREPARE AUDIO DATA
   ---------------------------------------------------------------------------*/
void medusa_sender_prepare_audio_data(
      t_medusa_sender * sender,
      int channel,
      void * data,
      uint16_t data_size){

   if(channel < 0 
         || medusa_sender_get_network_audio_resource(sender) == NULL
         || channel >= medusa_sender_get_network_channels(sender))
      return;

   //store data in RB only if connected
   if(medusa_server_get_client_count(sender->server) < 1)
      return;

   //copy audio data to avoid changing the original
   char clone_data[MEDUSA_RECEIVE_DATA_SIZE];
   memcpy(clone_data, data, data_size);

   if(sender->channels_volume[channel] != 1){
      medusa_dsp_gain((float *)clone_data, data_size / 4, sender->channels_volume[channel]);
   }

   char temp_data[MEDUSA_RECEIVE_DATA_SIZE];

/*   printf("Data size: %d ", data_size);*/
/*   data_size = medusa_dsp_encode_opus32(*/
/*                  48000,*/
/*                  (float *)clone_data,*/
/*                  480,*/
/*                  (unsigned char *)temp_data);*/

/*   printf("  %d\n", data_size);*/

/*   data_size = medusa_dsp_decode_opus32(*/
/*                  48000,*/
/*                  (unsigned char *)temp_data,*/
/*                  data_size,*/
/*                  (float *) clone_data,*/
/*                  MEDUSA_RECEIVE_DATA_SIZE*/
/*                  );*/

/*   printf("DECODED  %d\n", data_size);*/

/*   return;*/
   if(medusa_sender_get_local_audio_resource(sender)->sample_rate
               !=medusa_sender_get_network_audio_resource(sender)->sample_rate){
      data_size = medusa_dsp_change_sample_rate(
            medusa_sender_get_local_audio_resource(sender)->sample_rate,
            medusa_sender_get_network_audio_resource(sender)->sample_rate,
            (void *)clone_data,
            (void *)temp_data,
            data_size);
      memcpy(clone_data, temp_data, data_size);
   }
   if(medusa_sender_get_local_audio_resource(sender)->bit_depth
               != medusa_sender_get_network_audio_resource(sender)->bit_depth){
      data_size = medusa_dsp_change_quantization(
               medusa_sender_get_local_audio_resource(sender)->bit_depth,
               medusa_sender_get_network_audio_resource(sender)->bit_depth,
               (void *)clone_data,
               (void *)temp_data,
               data_size);
      memcpy(clone_data, temp_data, data_size);
   }
   data_size = 
         medusa_ringbuffer_write(sender->data_rb[channel], clone_data, data_size);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SEND AUDIO
   ---------------------------------------------------------------------------*/
void * medusa_sender_send_audio(void * arg){
   t_medusa_sender * sender = (t_medusa_sender *) arg;
   size_t data_size = 0;
   int i = 0;
   char temp_data[4096 * 4];
   int size = 0;
   sender->packet = malloc(MEDUSA_RECEIVE_DATA_SIZE);
   while(sender->status == MEDUSA_CONNECTED){
      while(sender->network_audio_resource == NULL)
         usleep(50000);
      // Thread will wait for data
      pthread_mutex_lock(&sender->dataMutex);
      while (sender->data_ready == 0)
         pthread_cond_wait(&sender->dataPresentCondition, &sender->dataMutex);
      sender->data_ready--;
      pthread_mutex_unlock(&sender->dataMutex);

      // New client connected. Send the audio config
      if(sender->new_client > 0 && sender->network_audio_resource != NULL){
         t_medusa_message_audio_config * config = 
               medusa_message_create_audio_config(
                        sender->network_audio_resource);
         medusa_server_send(
                  sender->server,
                  (void *)config,
                  sizeof(t_medusa_message_audio_config) + 1);
         sender->new_client--;
      }

      for(i = 0 ; i < medusa_sender_get_network_channels(sender); i++){
         data_size = (size_t) (sender->network_audio_resource->block_size
                           * sender->network_audio_resource->bit_depth);
         //check if we have enought data to send
         while(medusa_ringbuffer_read_space(sender->data_rb[i]) > data_size){
            size = medusa_ringbuffer_read(
                     sender->data_rb[i],
                     ((void *)temp_data),
                     data_size);
            // if muted, play zeros and advance the ringbuffer
            if(sender->muted_channels[i] == MEDUSA_MUTED){
                  memset(temp_data, '\0', data_size);
            }
            size = medusa_sender_pack(sender,
                     i,
                     temp_data,
                     size,
                     sender->packet,
                     MEDUSA_AUDIO);
            size = medusa_sender_send_packet(sender, sender->packet, size);
         }
      }
   }
   pthread_exit(0);
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CREATE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_sender_create_audio_resource(
      t_medusa_sender * sender,
      uint16_t channels,
      uint32_t sample_rate,
      MEDUSA_BIT_DEPTH bit_depth,
      uint16_t block_size){

   if(sender->local_audio_resource == NULL)
      sender->local_audio_resource = medusa_audio_resource_create();
   medusa_audio_resource_set(
               sender->local_audio_resource,
               channels,
               sample_rate,
               bit_depth,
               medusa_dsp_get_endianness(),
               block_size,
               MEDUSA_NOCODEC);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET NETWORK AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_sender_set_network_audio_resource(
         t_medusa_sender * sender,
         uint16_t channels,
         uint32_t sample_rate,
         MEDUSA_BIT_DEPTH bit_depth,
         MEDUSA_ENDIANNESS endianness,
         uint16_t block_size,
         MEDUSA_CODEC CODEC
         ){
   if(sender->network_audio_resource == NULL)
      sender->network_audio_resource = medusa_audio_resource_create();

   medusa_audio_resource_set(
         sender->network_audio_resource,
         channels,
         sample_rate,
         bit_depth,
         endianness,
         block_size,
         CODEC);

   sender->new_client++;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET CHANNEL VOLUME
   ---------------------------------------------------------------------------*/
void medusa_sender_set_channel_volume(
      t_medusa_sender * sender,
      int channel,
      float gain
      ){
   if(channel < medusa_sender_get_network_channels(sender))
      sender->channels_volume[channel] = gain;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET MUTE CHANNEL
   ---------------------------------------------------------------------------*/
void medusa_sender_mute_channel(
      t_medusa_sender * sender,
      MEDUSA_MUTE status,
      int channel
      ){
   if(channel < medusa_sender_get_network_channels(sender))
      sender->muted_channels[channel] = status;
}


/* ---------------------------------------------------------------------------*/
