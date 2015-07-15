#include "medusa.h"

/** @file medusa_receiver.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA RECEIVER ------------------------------------
// -----------------------------------------------------------------------------

static void * medusa_receiver_send(
      void * arg
      );

void medusa_receiver_prepare_loopback(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet
      );

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA AUDIO RECEIVER ------------------------------
// -----------------------------------------------------------------------------
extern int medusa_receiver_get_local_audio_channels(
      t_medusa_receiver * receiver
      );

extern int medusa_receiver_get_network_audio_channels(
      t_medusa_receiver * receiver
      );

extern void medusa_receiver_prepare_audio_data(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet
      );

extern int medusa_receiver_read_audio_data(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      uint16_t data_size
      );

extern int medusa_receiver_connect_audio_resource(
      t_medusa_receiver * receiver,
      char * name
      );

extern void medusa_receiver_create_audio_channels(
      t_medusa_receiver * receiver
      );

extern void * medusa_receiver_receive_audio(
      void * arg
      );

extern void * medusa_receiver_receive_midi(
      void * arg
      );

// -----------------------------------------------------------------------------
// ------------------------- MEDUSA MIDI RECEIVER ------------------------------
// -----------------------------------------------------------------------------
extern int medusa_receiver_get_local_midi_channels(
      t_medusa_receiver * receiver
      );

extern int medusa_receiver_get_network_midi_channels(
      t_medusa_receiver * receiver
      );

extern void medusa_receiver_prepare_midi_data(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet
      );

extern int medusa_receiver_read_midi_data(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      uint16_t data_size
      );

extern int medusa_receiver_connect_midi_resource(
      t_medusa_receiver * receiver,
      char * name
      );

extern void medusa_receiver_create_midi_channels(
      t_medusa_receiver * receiver
      );

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA AUDIO RECEIVER ------------------------------
// -----------------------------------------------------------------------------

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CREATE
   ---------------------------------------------------------------------------*/
t_medusa_receiver * medusa_receiver_create(
      t_medusa_control * control,
      MEDUSA_DATA_TYPE data_type,
      char * name){

   t_medusa_receiver * receiver = malloc(sizeof (t_medusa_receiver));

   if(strlen(name) < MEDUSA_LOCAL_RESOURCES)
      strcpy(receiver->name, name);

   receiver->loopback = MEDUSA_DISCONNECTED;
   receiver->status = MEDUSA_DISCONNECTED;
   receiver->uid = medusa_timestamp_int(medusa_timestamp_now());

   receiver->net_rb = NULL;
   receiver->loopback_rb = NULL;
   receiver->lb_packet = NULL;

   receiver->control = control;
   receiver->client = NULL;

   receiver->network_audio_resource = NULL;
   receiver->local_audio_resource = NULL;
   receiver->network_midi_resource = NULL;
   receiver->local_midi_resource = NULL;

   receiver->connection_callback = NULL;
   receiver->waiting_server_callback = NULL;
   receiver->data_received_callback = NULL;
   receiver->data_lost_callback = NULL;
   receiver->free_callback = NULL;
   receiver->stop_callback = NULL;
   receiver->prepare_data_callback_list = medusa_list_create();
   receiver->audio_config_callback = NULL;

   receiver->data_type = data_type;
   if(data_type == MEDUSA_AUDIO){
      receiver->create_channels = medusa_receiver_create_audio_channels;
      receiver->prepare_data = medusa_receiver_prepare_audio_data;
      receiver->read_data = medusa_receiver_read_audio_data;
      receiver->receive = medusa_receiver_receive_audio;
      receiver->connect_resource = medusa_receiver_connect_audio_resource;
      receiver->get_local_channels = medusa_receiver_get_local_audio_channels;
      receiver->get_network_channels = medusa_receiver_get_network_audio_channels;
   }
   if(data_type == MEDUSA_MIDI){
      receiver->create_channels = medusa_receiver_create_midi_channels;
      receiver->prepare_data = medusa_receiver_prepare_midi_data;
      receiver->read_data = medusa_receiver_read_midi_data;
      receiver->receive = medusa_receiver_receive_midi;
      receiver->connect_resource = medusa_receiver_connect_midi_resource;
      receiver->get_local_channels = medusa_receiver_get_local_midi_channels;
      receiver->get_network_channels = medusa_receiver_get_network_midi_channels;
   }

   receiver->data_rb = NULL;
   receiver->seq_number = NULL;
   receiver->muted_channels = NULL;
   receiver->channels_volume = NULL;

   return receiver;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER FREE
   ---------------------------------------------------------------------------*/
void medusa_receiver_free(t_medusa_receiver *receiver) {
   receiver->status = MEDUSA_DISCONNECTED;

   if(receiver->control != NULL){
      medusa_control_notify_remove_audio_resource(
               receiver->control,
               NULL,
               receiver->uid,
               receiver->network_audio_resource);
      medusa_control_notify_remove_midi_resource(
               receiver->control,
               NULL,
               receiver->uid,
               receiver->network_midi_resource);
      medusa_control_remove_receiver(receiver->control, receiver);
   }

   if(receiver->stop_callback != NULL){
      receiver->stop_callback->function(receiver->stop_callback->args);
      receiver->stop_callback = NULL;
   }
   if(receiver->client != NULL){
      medusa_client_free(receiver->client);
      receiver->client = NULL;
   }
   if(receiver->net_rb != NULL)
      medusa_ringbuffer_free(receiver->net_rb);
   if(receiver->loopback_rb != NULL)
      medusa_ringbuffer_free(receiver->loopback_rb);
   if(receiver->lb_packet != NULL)
      free(receiver->lb_packet);
   int i = 0;

   if(receiver->local_midi_resource != NULL)
      free(receiver->local_midi_resource);

   if(receiver->network_midi_resource != NULL){
      for (i = 0; i < receiver->network_midi_resource->channels
                     && receiver->data_rb; i++)
         medusa_ringbuffer_free(receiver->data_rb[i]);
      free(receiver->seq_number);
      free(receiver->network_midi_resource);
   }

   if(receiver->local_audio_resource != NULL)
      free(receiver->local_audio_resource);

   if(receiver->network_audio_resource != NULL){
      for (i = 0; i < receiver->network_audio_resource->channels
                     && receiver->data_rb; i++)
         medusa_ringbuffer_free(receiver->data_rb[i]);
      free(receiver->seq_number);
      free(receiver->network_audio_resource);
   }

   if(receiver->free_callback != NULL){
      receiver->free_callback->function(receiver->free_callback->args);
      receiver->free_callback = NULL;
   }else{
      free(receiver);
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET LOCAL CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_local_channels(
      t_medusa_receiver * receiver
      ){
   return receiver->get_local_channels(receiver);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NETWORK CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_receiver_get_network_channels(
      t_medusa_receiver * receiver
      ){
   return receiver->get_network_channels(receiver);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER CONNECT
   ---------------------------------------------------------------------------*/
int medusa_receiver_connect_resource(t_medusa_receiver * receiver, char * name){
   return receiver->connect_resource(receiver, name);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER START NETWORK
   ---------------------------------------------------------------------------*/
void medusa_receiver_start_network(
      t_medusa_receiver * receiver,
      int protocol,
      const char * ip,
      int port){

   // Malloc infrastructure to send / receive
   receiver->create_channels(receiver);

   receiver->client = medusa_client_create();
   // Set network conf
   medusa_client_set_config(receiver->client, protocol, ip, port);
   // set callback functions
   medusa_client_set_connection_callback(
         receiver->client,
         medusa_receiver_server_connected,
         receiver);

   receiver->net_rb = medusa_ringbuffer_create(MEDUSA_RING_BUFFER_SIZE);
   medusa_ringbuffer_reset(receiver->net_rb);

   struct sched_param param;
   int policy = SCHED_OTHER;
   param.sched_priority = sched_get_priority_max(policy);

   receiver->status = MEDUSA_CONNECTED;

   pthread_create(
         &receiver->receive_thread,
         NULL,
         receiver->receive,
         receiver);
   pthread_setschedparam(receiver->receive_thread, policy, &param);

   medusa_control_add_receiver(receiver->control, receiver);

   if(receiver->control)
      medusa_control_notify_receiver_network_config(
            receiver->control,
            NULL,
            receiver);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SEND LOOPBACK
   ---------------------------------------------------------------------------*/
static void * medusa_receiver_send(void * arg){
   t_medusa_receiver *receiver = (t_medusa_receiver *) arg;
   size_t data_size = 0;
   t_medusa_message_loopback * lo_packet
            = malloc(sizeof(t_medusa_message_loopback));
   int sent = 0;
   while(receiver->loopback == MEDUSA_CONNECTED){
      // Thread will wait for data
      pthread_mutex_lock(&receiver->loopback_dataMutex);
      while (!receiver->loopback_data_ready){
         pthread_cond_wait(&receiver->loopback_dataPresentCondition,
               &receiver->loopback_dataMutex);
         }
      receiver->loopback_data_ready--;
      pthread_mutex_unlock(&receiver->loopback_dataMutex);
      // Data present!
      // Empty the loopback RB
      while((data_size =medusa_ringbuffer_read_space(receiver->loopback_rb))
                  >= sizeof(t_medusa_message_loopback)
            && receiver->loopback_data_ready > -1 ){
         data_size = medusa_ringbuffer_read(receiver->loopback_rb,
               ((void *)lo_packet), sizeof(t_medusa_message_loopback));
         lo_packet->type = MEDUSA_LOOPBACK_DATA;
         sent = medusa_client_send(receiver->client,
                  ((void *)lo_packet), data_size);
      }
   }
   pthread_exit(0);
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER PREPARE LOOPBACK
   ---------------------------------------------------------------------------*/
void medusa_receiver_prepare_loopback(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet){
   if(receiver->loopback == MEDUSA_DISCONNECTED)
      return;

   receiver->lb_packet->data_type   = MEDUSA_LOOPBACK_DATA;
   receiver->lb_packet->data_size   = packet->data_size;
   receiver->lb_packet->channel     = packet->channel;
   receiver->lb_packet->seq_number  = packet->seq_number;
   receiver->lb_packet->timestamp   = packet->timestamp;
   receiver->lb_packet->received    = medusa_timestamp_now();
   medusa_ringbuffer_write(receiver->loopback_rb,
         ((void *)receiver->lb_packet), sizeof(t_medusa_message_loopback));
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SERVER CONNECTED
   ---------------------------------------------------------------------------*/
void medusa_receiver_server_connected(
      const t_medusa_network_config * server,
      MEDUSA_STATUS status,
      void * arg){
   t_medusa_receiver * receiver = (t_medusa_receiver *) arg;
   if(receiver->connection_callback)
      receiver->connection_callback->function(server, status,
            receiver->connection_callback->args);
   // Comunicate control
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET DATA TYPE
   ---------------------------------------------------------------------------*/
MEDUSA_DATA_TYPE medusa_receiver_get_data_type(t_medusa_receiver * receiver){
   return receiver->data_type;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET SERVER CONNECTED CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_connection_callback(
      t_medusa_receiver * receiver,
      t_medusa_connection_callback_function server_connected_callback,
      void * args){
   if(receiver->connection_callback == NULL)
      receiver->connection_callback
                  = malloc(sizeof(t_medusa_connection_callback));
   receiver->connection_callback->function = server_connected_callback;
   receiver->connection_callback->args = args;
   // inform the last status for late callback settings
   if(medusa_client_get_status(receiver->client) == MEDUSA_CONNECTED){
   receiver->connection_callback->function(
         medusa_client_get_network_config(receiver->client),
         medusa_client_get_status(receiver->client),
         receiver->connection_callback->args);
   }
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET WAITING SERVER CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_waiting_server_callback(
      t_medusa_receiver * receiver,
      t_medusa_waiting_server_callback_function waiting_server_callback,
      void * args){
   if(receiver->client != NULL)
      medusa_client_set_waiting_server_callback(receiver->client,
            waiting_server_callback, args);
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET DATA RECEIVED CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_data_received_callback(
      t_medusa_receiver * receiver,
      t_medusa_data_received_callback_function data_received_callback,
      void * args){
   if(receiver->data_received_callback == NULL)
      receiver->data_received_callback = malloc(sizeof(t_medusa_data_received_callback));
   receiver->data_received_callback->function = data_received_callback;
   receiver->data_received_callback->args = args;
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET DATA LOST CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_data_lost_callback(
      t_medusa_receiver * receiver,
      t_medusa_data_lost_callback_function data_lost_callback,
      void * args){
   if(receiver->data_lost_callback == NULL)
      receiver->data_lost_callback= malloc(sizeof(t_medusa_data_lost_callback));
   receiver->data_lost_callback->function = data_lost_callback;
   receiver->data_lost_callback->args = args;
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET FREE CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_free_callback(
      t_medusa_receiver * receiver,
      t_medusa_free_callback_function free_callback,
      void * args){
   if(receiver->free_callback == NULL)
      receiver->free_callback = malloc(sizeof(t_medusa_free_callback));
   receiver->free_callback->function = free_callback;
   receiver->free_callback->args = args;
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET STOP CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_stop_callback(
      t_medusa_receiver * receiver,
      t_medusa_stop_callback_function stop_callback,
      void * args){
   if(receiver->stop_callback == NULL)
      receiver->stop_callback = malloc(sizeof(t_medusa_stop_callback));
   receiver->stop_callback->function = stop_callback;
   receiver->stop_callback->args = args;
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET PREPARE DATA CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_receiver_set_prepare_data_callback(
      t_medusa_receiver * receiver,
      t_medusa_prepare_data_callback_function function,
      void * args){
   t_medusa_prepare_data_callback * callback
         = malloc(sizeof(t_medusa_prepare_data_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&receiver->prepare_data_callback_list, callback);
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PREPARE DATA
   ---------------------------------------------------------------------------*/
void medusa_receiver_prepare_data(
      t_medusa_receiver * receiver,
      t_medusa_message_data * packet){
   if(receiver->prepare_data)
      receiver->prepare_data(receiver, packet);
   t_medusa_list * callback_list = receiver->prepare_data_callback_list;
   while(callback_list){
      t_medusa_prepare_data_callback * callback
            = (t_medusa_prepare_data_callback *)callback_list->data;
      callback->function(
            packet->channel,
            packet->data,
            packet->data_size,
            callback->args);
      callback_list = callback_list->next;
   }

}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER DATA READ
   ---------------------------------------------------------------------------*/
void medusa_receiver_trigger_loopback(
      t_medusa_receiver * receiver){
   if(receiver->loopback == MEDUSA_CONNECTED){
      pthread_mutex_lock(&receiver->loopback_dataMutex);
      receiver->loopback_data_ready++;
      pthread_cond_signal(&receiver->loopback_dataPresentCondition);
      pthread_mutex_unlock(&receiver->loopback_dataMutex);
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NAME
   ---------------------------------------------------------------------------*/
char * medusa_receiver_get_name(t_medusa_receiver * receiver){
   return receiver->name;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET UID
   ---------------------------------------------------------------------------*/
uint64_t medusa_receiver_get_uid(t_medusa_receiver * receiver){
   return receiver->uid;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET LOOPBACK
   ---------------------------------------------------------------------------*/
void medusa_receiver_set_loopback(
      t_medusa_receiver * receiver,
      MEDUSA_STATUS status){
   if(status == MEDUSA_CONNECTED){
      receiver->lb_packet = malloc(sizeof(t_medusa_message_loopback));
      receiver->loopback = status;
      struct sched_param param;
      int policy = SCHED_OTHER;
      param.sched_priority = sched_get_priority_max(policy);

      if(receiver->loopback_rb == NULL)
         receiver->loopback_rb = medusa_ringbuffer_create(MEDUSA_RING_BUFFER_SIZE
            * (sizeof(t_medusa_message_loopback)));

      pthread_create(&receiver->send_thread, NULL,
            medusa_receiver_send, receiver);
      pthread_setschedparam(receiver->send_thread, policy, &param);
      receiver->loopback_data_ready = 0;
      pthread_mutex_init(&receiver->loopback_dataMutex, NULL);
      pthread_cond_init(&receiver->loopback_dataPresentCondition, NULL);
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET STATUS
   ---------------------------------------------------------------------------*/
MEDUSA_STATUS medusa_receiver_get_status(t_medusa_receiver * receiver){
   return receiver->status;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET LOOPBACK STATUS
   ---------------------------------------------------------------------------*/
MEDUSA_STATUS medusa_receiver_get_loopback_status(t_medusa_receiver * receiver){
   return receiver->loopback;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER GET NETWORK CONFIG
   ---------------------------------------------------------------------------*/
const t_medusa_network_config * medusa_receiver_get_network_config(
      t_medusa_receiver * receiver){
   if(receiver->client == NULL)
      return NULL;
   else
      return medusa_client_get_network_config(receiver->client);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET CONTROL
   ---------------------------------------------------------------------------*/
void medusa_receiver_set_control(
      t_medusa_receiver * receiver,
      t_medusa_control * control){
   receiver->control = control;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER READ DATA
   ---------------------------------------------------------------------------*/
int medusa_receiver_read_data(
      t_medusa_receiver * receiver,
      int channel,
      void * data,
      int data_size
      ){
   return receiver->read_data(receiver, channel, data, data_size);
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER COMPARE
   ---------------------------------------------------------------------------*/
int medusa_receiver_compare(const void * data1, const void * data2){
   t_medusa_receiver * s1 = (t_medusa_receiver *) data1;
   t_medusa_receiver * s2 = (t_medusa_receiver *) data2;
   if(s1->uid == s2->uid)
      return 1;
   else
      return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET MUTE CHANNEL
   ---------------------------------------------------------------------------*/
void medusa_receiver_mute_channel(
      t_medusa_receiver * receiver,
      MEDUSA_MUTE status,
      int channel
      ){
   if(channel < medusa_receiver_get_network_channels(receiver))
      receiver->muted_channels[channel] = status;
}

/* -----------------------------------------------------------------------------
   MEDUSA RECEIVER SET CHANNEL VOLUME
   ---------------------------------------------------------------------------*/
void medusa_receiver_set_channel_volume(
      t_medusa_receiver * receiver,
      int channel,
      float gain
      ){
   if(channel < medusa_receiver_get_network_channels(receiver))
      receiver->channels_volume[channel] = gain;
}

/*----------------------------------------------------------------------------*/
