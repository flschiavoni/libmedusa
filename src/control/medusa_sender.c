#include "medusa.h"

/** @file medusa_sender.c
 *
 * @brief
 * 
 *
 * @ingroup control
 */

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA SENDER --------------------------------------
// -----------------------------------------------------------------------------

static void medusa_sender_create_channels(
      t_medusa_sender * sender
      );

int medusa_sender_send_packet(
      t_medusa_sender * sender,
      t_medusa_message_data * packet,
      size_t data_size
      );

size_t medusa_sender_pack(
      t_medusa_sender * sender,
      uint16_t channel,
      void * data,
      uint32_t data_size,
      t_medusa_message_data * packet,
      MEDUSA_DATA_TYPE data_type
      );

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA AUDIO SENDER --------------------------------
// -----------------------------------------------------------------------------
extern void medusa_sender_prepare_audio_data(
      t_medusa_sender * sender,
      int channel,
      void * data,
      uint16_t data_size
      );

extern void medusa_sender_create_audio_channels(t_medusa_sender * sender);

extern void * medusa_sender_send_audio(void * arg);

extern int medusa_sender_get_local_audio_channels(
      t_medusa_sender * sender
      );

extern int medusa_sender_get_network_audio_channels(
      t_medusa_sender * sender
      );

// -----------------------------------------------------------------------------
// ------------------------- MEDUSA MIDI SENDER --------------------------------
// -----------------------------------------------------------------------------
extern void medusa_sender_prepare_midi_data(
      t_medusa_sender * sender,
      int channel,
      void * data,
      uint16_t data_size
      );

extern void medusa_sender_create_midi_channels(t_medusa_sender * sender);

extern void * medusa_sender_send_midi(void * arg);

extern int medusa_sender_get_local_midi_channels(
      t_medusa_sender * sender
      );

extern int medusa_sender_get_network_midi_channels(
      t_medusa_sender * sender
      );

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CREATE
   ---------------------------------------------------------------------------*/

t_medusa_sender * medusa_sender_create(
      t_medusa_control * control,
      MEDUSA_DATA_TYPE data_type,
      char * name){

   t_medusa_sender * sender =  malloc(sizeof(t_medusa_sender));

   medusa_util_format_name(name);
   if(strlen(name) < MEDUSA_LOCAL_RESOURCES)
      strcpy(sender->name, name);
   else
      strcpy(sender->name, "sender");

   sender->server = NULL;
   sender->control = control;

   sender->local_midi_resource = NULL;
   sender->network_midi_resource = NULL;

   sender->local_audio_resource = NULL;
   sender->network_audio_resource = NULL;

   sender->status = MEDUSA_DISCONNECTED;
   sender->loopback = MEDUSA_DISCONNECTED;
   sender->lb_packet = NULL;
   sender->uid = medusa_timestamp_int(medusa_timestamp_now());
   sender->new_client = 0;

   sender->connection_callback = NULL;
   sender->data_lost_callback = NULL;
   sender->data_sent_callback = NULL;
   sender->meter_callback = NULL;
   sender->free_callback = NULL;
   sender->stop_callback = NULL;
   sender->prepare_data_callback_list = medusa_list_create();

   sender->prepare = NULL;

   sender->data_type = data_type;
   if(data_type == MEDUSA_MIDI){
      sender->get_local_channels = medusa_sender_get_local_midi_channels;
      sender->get_network_channels = medusa_sender_get_network_midi_channels;
      sender->prepare = medusa_sender_prepare_midi_data;
      sender->create_channels = medusa_sender_create_midi_channels;
      sender->send = medusa_sender_send_midi;
   }
   if(data_type == MEDUSA_AUDIO){
      sender->get_local_channels = medusa_sender_get_local_audio_channels;
      sender->get_network_channels = medusa_sender_get_network_audio_channels;
      sender->prepare = medusa_sender_prepare_audio_data;
      sender->create_channels = medusa_sender_create_audio_channels;
      sender->send = medusa_sender_send_audio;
   }

   sender->packet = NULL;
   sender->seq_number = NULL;
   sender->muted_channels = NULL;
   sender->channels_volume = NULL;
   sender->data_rb = NULL;

   return sender;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER FREE
   ---------------------------------------------------------------------------*/
void medusa_sender_free(t_medusa_sender * sender) {
   sender->status = MEDUSA_DISCONNECTED;
   
   pthread_cancel(sender->send_thread);

   if(sender->control != NULL){
      medusa_control_notify_remove_audio_resource(
                  sender->control,
                  NULL,
                  sender->uid,
                  sender->network_audio_resource);
      medusa_control_notify_remove_midi_resource(
                  sender->control,
                  NULL,
                  sender->uid,
                  sender->network_midi_resource);
      medusa_control_remove_sender(sender->control, sender);
   }

   if(sender->stop_callback != NULL){
      sender->stop_callback->function(sender->stop_callback->args);
      free(sender->stop_callback);
      sender->stop_callback = NULL;
   }

   if(sender->server != NULL){
      medusa_server_free(sender->server);
      sender->server = NULL;
   }

   if(sender->local_midi_resource != NULL)
      free(sender->local_midi_resource);

   int i = 0;
   if(sender->network_midi_resource != NULL){
      for (i = 0; i < sender->network_midi_resource->channels
                  && sender->data_rb; i++)
         medusa_ringbuffer_free(sender->data_rb[i]);
      free(sender->seq_number);
      free(sender->network_midi_resource);
      sender->network_midi_resource = NULL;
   }

   if(sender->local_audio_resource != NULL)
      free(sender->network_midi_resource);

   if(sender->network_audio_resource != NULL){
      for (i = 0; i < sender->network_audio_resource->channels
                  && sender->data_rb; i++)
         medusa_ringbuffer_free(sender->data_rb[i]);
      free(sender->seq_number);
      free(sender->network_audio_resource);
      sender->network_audio_resource = NULL;
   }

   if(sender->lb_packet != NULL){
      free(sender->lb_packet);
      sender->lb_packet = NULL;
   }

   if(sender->packet != NULL)
      free(sender->packet);

   if(sender->free_callback != NULL){
      sender->free_callback->function(sender->free_callback->args);
      free(sender->free_callback);
      sender->free_callback = NULL;
   }else{
      free(sender);
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CREATE CHANNELS
   ---------------------------------------------------------------------------*/
void medusa_sender_create_channels(t_medusa_sender * sender){
   if(sender->create_channels != NULL)
      sender->create_channels(sender);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NETWORK CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_sender_get_network_channels(
      t_medusa_sender * sender
      ){
   return sender->get_network_channels(sender);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET LOCAL CHANNELS
   ---------------------------------------------------------------------------*/
int medusa_sender_get_local_channels(
      t_medusa_sender * sender
      ){
   return sender->get_local_channels(sender);
}


/* -----------------------------------------------------------------------------
   MEDUSA SENDER START NETWORK
   ---------------------------------------------------------------------------*/
void medusa_sender_start_network(
         t_medusa_sender * sender,
         MEDUSA_PROTOCOL protocol,
         int port){

   if(sender->status == MEDUSA_CONNECTED)
      return;

   // Malloc infrastructure to send / receive
   medusa_sender_create_channels(sender);

   //set network
   if(sender->server == NULL)
      sender->server = medusa_server_create();
   medusa_server_set_config(sender->server, protocol, port);
   if(medusa_server_connect(sender->server) < 0){
      return;
   }
   // set callback functions
   medusa_server_set_client_connected_callback(sender->server,
         medusa_sender_client_connected, sender);

   //start send thread
   struct sched_param param;
   int policy = SCHED_OTHER;
   param.sched_priority = sched_get_priority_max(policy);

   pthread_create(&sender->send_thread, NULL, sender->send,
            sender);
   pthread_setschedparam(sender->send_thread, policy, &param);

   sender->data_ready = 0;
   pthread_mutex_init(&sender->dataMutex, NULL);
   pthread_cond_init(&sender->dataPresentCondition, NULL);

   sender->status = MEDUSA_CONNECTED;


   medusa_control_add_sender(sender->control, sender);
   if(sender->control)
      medusa_control_notify_sender_network_config(
            sender->control,
            NULL,
            sender);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SEND DATA
   ---------------------------------------------------------------------------*/
void medusa_sender_send_data(t_medusa_sender * sender){
   if(sender->status == MEDUSA_DISCONNECTED)
      return;
    pthread_mutex_lock(&sender->dataMutex);
    sender->data_ready++;
    pthread_cond_signal(&sender->dataPresentCondition);
    pthread_mutex_unlock(&sender->dataMutex);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SEND PACKET
   ---------------------------------------------------------------------------*/
int medusa_sender_send_packet(
      t_medusa_sender * sender,
      t_medusa_message_data * packet,
      size_t data_size){
   size_t sent = 0;
   // Check if there are clients connected
   if(medusa_server_get_client_count(sender->server) < 1)
      return 0;
      sent = medusa_server_send(sender->server, ((void *)packet), data_size);
   if(sent != data_size && sender->data_lost_callback != NULL)
      sender->data_lost_callback->function(sender->data_lost_callback->args);
   if(sender->data_sent_callback != NULL)
      sender->data_sent_callback->function(sent, sender->data_sent_callback->args);
   return (int) sent;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET LOOPBACK
   ---------------------------------------------------------------------------*/
void medusa_sender_set_loopback(t_medusa_sender * sender, MEDUSA_STATUS status){
   if(status == MEDUSA_CONNECTED){
      sender->loopback = status;
      medusa_server_set_data_received_callback(sender->server,
            medusa_sender_receive_data,
            sender);
      if(sender->lb_packet == NULL)
          sender->lb_packet = malloc(sizeof(t_medusa_message_loopback *));
   }else{
      sender->loopback = status;
      medusa_server_set_data_received_callback(sender->server, NULL, NULL);
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET LOOPBACK
   ---------------------------------------------------------------------------*/
MEDUSA_STATUS medusa_sender_get_loopback_status(t_medusa_sender * sender){
   return sender->loopback;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET UID
   ---------------------------------------------------------------------------*/
uint64_t medusa_sender_get_uid(t_medusa_sender * sender){
   return sender->uid;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NAME
   ---------------------------------------------------------------------------*/
char * medusa_sender_get_name(t_medusa_sender * sender){
   return sender->name;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PROCESS LOOPBACK
   ---------------------------------------------------------------------------*/
void medusa_sender_receive_data(
         const t_medusa_network_config * client,
         void * data,
         int data_size,
         void * args){
   t_medusa_sender *sender = (t_medusa_sender *) args;

   if(sender->meter_callback == NULL)
      return;

   if(data_size < (int) sizeof(t_medusa_message_loopback)){
      return;
   }

   if(((uint8_t *)data)[0] != MEDUSA_LOOPBACK_DATA)
      return;

   sender->meter_callback->function(
               client,
               sender->lb_packet->timestamp,
               sender->lb_packet->received,
               medusa_timestamp_now(),
               sender->lb_packet->channel,
               sender->lb_packet->seq_number,
               sender->lb_packet->data_size,
               sender->meter_callback->args);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CLIENT CONNECTED
   ---------------------------------------------------------------------------*/
void medusa_sender_client_connected(
      const t_medusa_network_config * client,
      MEDUSA_STATUS status,
      void * arg){
   t_medusa_sender * sender = (t_medusa_sender *) arg;
   if(sender->connection_callback)
      sender->connection_callback->function(
            client,
            status,
            sender->connection_callback->args);
   sender->new_client++;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER CLIENT GET DATA TYPE
   ---------------------------------------------------------------------------*/
MEDUSA_DATA_TYPE medusa_sender_get_data_type(t_medusa_sender * sender){
   return sender->data_type;
}


/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET CLIENT CONNECTED CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_sender_set_connection_callback(
      t_medusa_sender * sender,
      t_medusa_connection_callback_function connection_callback,
      void * args){
   if(sender->connection_callback == NULL)
      sender->connection_callback= malloc(sizeof(t_medusa_connection_callback));
   sender->connection_callback->function = connection_callback;
   sender->connection_callback->args = args;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET DATA LOST CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_sender_set_data_lost_callback(
      t_medusa_sender * sender,
      t_medusa_data_lost_callback_function data_lost_callback,
      void * args){
   if(sender->data_lost_callback == NULL)
      sender->data_lost_callback = malloc(sizeof(t_medusa_data_lost_callback));
   sender->data_lost_callback->function = data_lost_callback;
   sender->data_lost_callback->args = args;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET DATA SENT CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_sender_set_data_sent_callback(
      t_medusa_sender * sender,
      t_medusa_data_sent_callback_function data_sent_callback,
      void * args){
   if(sender->data_sent_callback == NULL)
      sender->data_sent_callback = malloc(sizeof(t_medusa_data_sent_callback));
   sender->data_sent_callback->function = data_sent_callback;
   sender->data_sent_callback->args = args;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET METER CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_sender_set_meter_callback(
      t_medusa_sender * sender,
      t_medusa_meter_callback_function meter_callback,
      void * args){
   if(sender->meter_callback == NULL)
      sender->meter_callback = malloc(sizeof(t_medusa_meter_callback));
   sender->meter_callback->function = meter_callback;
   sender->meter_callback->args = args;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET FREE CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_sender_set_free_callback(
      t_medusa_sender * sender,
      t_medusa_free_callback_function free_callback,
      void * args){
   if(sender->free_callback == NULL)
      sender->free_callback = malloc(sizeof(t_medusa_free_callback));
   sender->free_callback->function = free_callback;
   sender->free_callback->args = args;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET STOP CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_sender_set_stop_callback(
      t_medusa_sender * sender,
      t_medusa_stop_callback_function stop_callback,
      void * args){
   if(sender->stop_callback == NULL)
      sender->stop_callback = malloc(sizeof(t_medusa_stop_callback));
   sender->stop_callback->function = stop_callback;
   sender->stop_callback->args = args;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET PREPARE DATA CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_sender_set_prepare_data_callback(
      t_medusa_sender * sender,
      t_medusa_prepare_data_callback_function function,
      void * args){
   t_medusa_prepare_data_callback * callback
         = malloc(sizeof(t_medusa_prepare_data_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&sender->prepare_data_callback_list, callback);
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET NETWORK CONFIG
   ---------------------------------------------------------------------------*/
const t_medusa_network_config * medusa_sender_get_network_config(
      t_medusa_sender * sender){
   if(sender == NULL)
      return NULL;
   else
      return medusa_server_get_network_config(sender->server);
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER GET STATUS
   ---------------------------------------------------------------------------*/
MEDUSA_STATUS medusa_sender_get_status(t_medusa_sender * sender){
   return sender->status;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER SET CONTROL
   ---------------------------------------------------------------------------*/
void medusa_sender_set_control(
      t_medusa_sender * sender,
      t_medusa_control * control){
   sender->control = control;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PACK
   ---------------------------------------------------------------------------*/
size_t medusa_sender_pack(
      t_medusa_sender * sender,
      uint16_t channel,
      void * data,
      uint32_t data_size,
      t_medusa_message_data * packet,
      MEDUSA_DATA_TYPE data_type) {

   size_t packet_size = sizeof(t_medusa_message_data) + data_size;
   packet->type = MEDUSA_DATA;
   packet->data_size = (uint32_t) data_size;
   packet->data_type = data_type;
   packet->channel = channel;
   packet->seq_number = sender->seq_number[channel]++;

   packet->timestamp = medusa_timestamp_now();
   memcpy(packet->data, data, data_size);

   return packet_size;
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER PREPARE DATA
   ---------------------------------------------------------------------------*/
void medusa_sender_prepare_data(
      t_medusa_sender * sender,
      int channel,
      void * data,
      int data_size){
   if(sender->prepare != NULL)
      sender->prepare(sender, channel, data, data_size);
   t_medusa_list * callback_list = sender->prepare_data_callback_list;
   while(callback_list){
      t_medusa_prepare_data_callback * callback
            = (t_medusa_prepare_data_callback *)callback_list->data;
      callback->function(channel, data, data_size, callback->args);
      callback_list = callback_list->next;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA SENDER COMPARE
   ---------------------------------------------------------------------------*/
int medusa_sender_compare(const void * data1, const void * data2){
   t_medusa_sender * s1 = (t_medusa_sender *) data1;
   t_medusa_sender * s2 = (t_medusa_sender *) data2;
   if(s1->uid == s2->uid)
      return 1;
   else
      return 0;
}

/*----------------------------------------------------------------------------*/
