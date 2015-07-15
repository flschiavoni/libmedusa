#include "medusa.h"

/** @file medusa_control.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA CONTROL -------------------------------------
// -----------------------------------------------------------------------------
struct medusa_control{
      char name[MEDUSA_LOCAL_RESOURCES];
      uint64_t uid;
      MEDUSA_STATUS status;

      t_medusa_list * senders;
      t_medusa_list * receivers;
      t_medusa_list * nodes;
//TODO List of servers
//TODO List of clients
      t_medusa_server * server;

      t_medusa_list * add_node_callback_list;
      t_medusa_list * remove_node_callback_list;
      t_medusa_list * add_node_audio_resource_callback_list;
      t_medusa_list * add_node_midi_resource_callback_list;
      t_medusa_list * remove_node_audio_resource_callback_list;
      t_medusa_list * remove_node_midi_resource_callback_list;
      t_medusa_list * chat_callback_list;
      t_medusa_list * add_sender_callback_list;
      t_medusa_list * add_receiver_callback_list;
      t_medusa_list * remove_sender_callback_list;
      t_medusa_list * remove_receiver_callback_list;
      };

void medusa_control_send_msg(
      t_medusa_control * control,
      t_medusa_node * node,
      void * data,
      int data_size
      );

void medusa_control_publish_my_resources(
      t_medusa_control * control,
      t_medusa_node * node
      );

// Received messages
void medusa_control_rcv_add_node_message(
      t_medusa_control * control,
      t_medusa_message_add_node * message,
      const t_medusa_network_config * client
      );

void medusa_control_rcv_remove_node_message(
      t_medusa_control * control,
      t_medusa_message_remove_node * message
      );

void medusa_control_rcv_chat_message(
      t_medusa_control * control,
      t_medusa_message_chat * message
      );

void medusa_control_rcv_add_node_audio_resource_message(
      t_medusa_control * control,
      t_medusa_message_add_audio_resource * message
      );

void medusa_control_rcv_add_node_midi_resource_message(
      t_medusa_control * control,
      t_medusa_message_add_midi_resource * message
      );

void medusa_control_rcv_remove_node_audio_resource_message(
      t_medusa_control * control,
      t_medusa_message_remove_audio_resource * message
      );

void medusa_control_rcv_remove_node_midi_resource_message(
      t_medusa_control * control,
      t_medusa_message_remove_midi_resource * message
      );

void medusa_control_rcv_network_config(
      t_medusa_control * control,
      t_medusa_message_network_config * message
      );

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL CREATE
   ---------------------------------------------------------------------------*/
t_medusa_control * medusa_control_create(char * name){
   t_medusa_control * control = malloc(sizeof(t_medusa_control));
   if(strcmp(name, "") != 0)
      strcpy(control->name, name);
   else
      strcpy(control->name, medusa_util_get_user_name());

   control->uid = medusa_timestamp_int(medusa_timestamp_now());

   control->senders = medusa_list_create();
   control->receivers = medusa_list_create();
   control->nodes = medusa_list_create();

   control->add_node_callback_list = medusa_list_create();
   control->remove_node_callback_list = medusa_list_create();
   control->add_node_midi_resource_callback_list = medusa_list_create();
   control->remove_node_midi_resource_callback_list = medusa_list_create();
   control->add_node_audio_resource_callback_list = medusa_list_create();
   control->remove_node_audio_resource_callback_list = medusa_list_create();
   control->chat_callback_list = medusa_list_create();
   control->add_sender_callback_list = medusa_list_create();
   control->add_receiver_callback_list = medusa_list_create();
   control->remove_sender_callback_list = medusa_list_create();
   control->remove_receiver_callback_list = medusa_list_create();

   control->server = medusa_server_create();
   medusa_server_set_config(control->server, MEDUSA_MULTI, MEDUSA_CONTROL_PORT);
   medusa_server_set_data_received_callback(control->server,
         medusa_control_receive_data,
         control);
   medusa_server_connect(control->server);
   control->status = MEDUSA_CONNECTED;
   medusa_control_notify_add_node(control, NULL);
   return control;
}

/**
   @callergraph
*/
/* -----------------------------------------------------------------------------
   MEDUSA CONTROL FREE
   ---------------------------------------------------------------------------*/
int medusa_control_free(t_medusa_control * control){

   t_medusa_list * list = control->senders;
   while(list){
      t_medusa_sender * sender = (t_medusa_sender *) list->data;
      medusa_control_remove_sender(control, sender);
      medusa_sender_free(sender);
      list = list->next;
   }
   medusa_list_free(control->senders);

   list = control->receivers;
   while(list){
      t_medusa_receiver * receiver = (t_medusa_receiver *) list->data;
      medusa_control_remove_receiver(control, receiver);
      medusa_receiver_free(receiver);
      list = list->next;
   }
   medusa_list_free(control->receivers);

   medusa_control_notify_remove_node(control, NULL);

   medusa_server_free(control->server);

   list = control->nodes;
   while(list){
      t_medusa_node * node = (t_medusa_node *)list->data;
      medusa_node_free(node);
      list = list->next;
   }
   medusa_list_free(control->nodes);

   free(control);

   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET NAME
   ---------------------------------------------------------------------------*/
const char * medusa_control_get_name(const t_medusa_control * control){
   return control->name;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET UID
   ---------------------------------------------------------------------------*/
uint64_t medusa_control_get_uid(const t_medusa_control * control){
   return control->uid;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET NODES
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_control_get_nodes(const t_medusa_control * control){
   return control->nodes;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET SENDERS
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_control_get_senders(const t_medusa_control * control){
   return control->senders;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET RECEIVERS
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_control_get_receivers(const t_medusa_control * control){
   return control->receivers;
}


/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET AUDIO RESOURCE BY NAME
   ---------------------------------------------------------------------------*/
t_medusa_node_audio_resource * medusa_control_get_audio_resource_by_name(
      t_medusa_control * control,
      char * name
      ){
   char temp[strlen(name)];
   strcpy(temp, name);
   char * node_name;
   char * resource_name;
   resource_name = strtok(temp, "@");
   node_name = strtok(NULL, "@");

   if(strlen(node_name) == 0 || strlen(resource_name) == 0)
      return NULL;

   t_medusa_list * list = control->nodes;
   while(list){
      t_medusa_node * node = (t_medusa_node *)list->data;
      if(strcmp(node->name, node_name) == 0){
         //found the node
         t_medusa_list * audio_list = medusa_node_get_audio_resources(node);
         while(audio_list){
            t_medusa_node_audio_resource * audio
                     = (t_medusa_node_audio_resource *) audio_list->data;
            if(strcmp(audio->name, resource_name) == 0){
               return audio;
            }
            audio_list = audio_list->next;
         }
      }
      list = list->next;
   }
   return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET MIDI RESOURCE BY NAME
   ---------------------------------------------------------------------------*/
t_medusa_node_midi_resource * medusa_control_get_midi_resource_by_name(
      t_medusa_control * control,
      char * name
      ){
   char temp[strlen(name)];
   strcpy(temp, name);
   char * node_name;
   char * resource_name;
   resource_name = strtok(temp, "@");
   node_name = strtok(NULL, "@");

   if(strlen(node_name) == 0 || strlen(resource_name) == 0)
      return NULL;

   t_medusa_list * list = control->nodes;
   while(list){
      t_medusa_node * node = (t_medusa_node *)list->data;
      if(strcmp(node->name, node_name) == 0){
         //found the node
         t_medusa_list * midi_list = medusa_node_get_midi_resources(node);
         while(midi_list){
            t_medusa_node_midi_resource * midi
                     = (t_medusa_node_midi_resource *) midi_list->data;
            if(strcmp(midi->name, resource_name) == 0){
               return midi;
            }
            midi_list = midi_list->next;
         }
      }
      list = list->next;
   }
   return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET NODE
   ---------------------------------------------------------------------------*/
t_medusa_node * medusa_control_get_node(
         const t_medusa_control * control,
         uint64_t uid){
   t_medusa_list * list = control->nodes;
   while(list){
      t_medusa_node * node = (t_medusa_node *)list->data;
      if(medusa_node_get_id(node) == uid){
         return (t_medusa_node *)list->data;
      }
      list = list->next;
   }
   return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL ADD SENDER
   ---------------------------------------------------------------------------*/
int medusa_control_add_sender(
         t_medusa_control * control,
         t_medusa_sender * sender){

   medusa_list_add_unique_element(
         &control->senders,
         sender,
         medusa_sender_compare);
   t_medusa_list * callback_list = control->add_sender_callback_list;
   while(callback_list){
      t_medusa_add_sender_callback * callback
            = (t_medusa_add_sender_callback *)callback_list->data;
      callback->function(control, sender, callback->args);
      callback_list = callback_list->next;
   }
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL REMOVE SENDER
   ---------------------------------------------------------------------------*/
int medusa_control_remove_sender(
         t_medusa_control * control,
         t_medusa_sender * sender){

   sender = medusa_list_remove_element(
         &control->senders,
         sender,
         medusa_sender_compare);

   if(sender == NULL)
      return 0;

   if(medusa_sender_get_network_audio_resource(sender) != NULL){
      medusa_control_notify_remove_audio_resource(control,
            NULL,
            medusa_sender_get_uid(sender),
            medusa_sender_get_network_audio_resource(sender));
   }

   if(medusa_sender_get_network_midi_resource(sender) != NULL){
      medusa_control_notify_remove_midi_resource(control,
            NULL,
            medusa_sender_get_uid(sender),
            medusa_sender_get_network_midi_resource(sender));
   }

   t_medusa_list * callback_list = control->remove_sender_callback_list;
   while(callback_list){
      t_medusa_remove_sender_callback * callback
            = (t_medusa_remove_sender_callback *)callback_list->data;
      callback->function(control, sender, callback->args);
      callback_list = callback_list->next;
   }
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET SENDER BY UID
   ---------------------------------------------------------------------------*/
t_medusa_sender * medusa_control_get_sender_by_uid(
      t_medusa_control * control,
      uint64_t uid
      ){
   t_medusa_list * senders = control->senders;
   while(senders){
      t_medusa_sender * sender = (t_medusa_sender *) senders->data;
      if(medusa_sender_get_uid(sender) == uid)
         return sender;
      senders = senders->next;
   }
   return NULL;
}

/* -----------------------------------------------------------------------------
    CONTROL ADD RECEIVER
   ---------------------------------------------------------------------------*/
int medusa_control_add_receiver(
         t_medusa_control * control,
         t_medusa_receiver * receiver){
   medusa_list_add_unique_element(
         &control->receivers,
         receiver,
         medusa_receiver_compare);
   t_medusa_list * callback_list = control->add_receiver_callback_list;
   while(callback_list){
      t_medusa_add_receiver_callback * callback
            = (t_medusa_add_receiver_callback *)callback_list->data;
      callback->function(control, receiver, callback->args);
      callback_list = callback_list->next;
   }
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL REMOVE RECEIVER
   ---------------------------------------------------------------------------*/
int medusa_control_remove_receiver(
         t_medusa_control * control,
         t_medusa_receiver * receiver){

   receiver = medusa_list_remove_element(
            &control->receivers,
            receiver,
            medusa_receiver_compare);
   if(receiver == NULL)
      return 0;

   if(medusa_receiver_get_network_audio_resource(receiver) != NULL)
      medusa_control_notify_remove_audio_resource(control,
            NULL,
            medusa_receiver_get_uid(receiver),
            medusa_receiver_get_network_audio_resource(receiver));

   if(medusa_receiver_get_network_midi_resource(receiver) != NULL)
      medusa_control_notify_remove_midi_resource(control,
            NULL,
            medusa_receiver_get_uid(receiver),
            medusa_receiver_get_network_midi_resource(receiver));

   t_medusa_list * callback_list = control->remove_receiver_callback_list;
   while(callback_list){
      t_medusa_remove_receiver_callback * callback
            = (t_medusa_remove_receiver_callback *)callback_list->data;
      callback->function(control, receiver, callback->args);
      callback_list = callback_list->next;
   }
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL GET RECEIVER BY UID
   ---------------------------------------------------------------------------*/
t_medusa_receiver * medusa_control_get_receiver_by_uid(
      t_medusa_control * control,
      uint64_t uid
      ){
   t_medusa_list * receivers = control->receivers;
   while(receivers){
      t_medusa_receiver * receiver = (t_medusa_receiver *) receivers->data;
      if(medusa_receiver_get_uid(receiver) == uid)
         return receiver;
      receivers = receivers->next;
   }
   return NULL;
}


/* -----------------------------------------------------------------------------
   MESSAGES RECEIVED BY THE NETWORK MUST NOTIFY THE UI BY CALLBACK FUNCTIONS
   ---------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE DATA
   ---------------------------------------------------------------------------*/
void medusa_control_receive_data(
         const t_medusa_network_config * client,
         void * data,
         int data_size,
         void * args){
   t_medusa_control * control = (t_medusa_control *) args;
   MEDUSA_MESSAGE message_type = ((uint8_t * ) data)[0];

   switch(message_type){

      case MEDUSA_ADD_NODE:{
         medusa_control_rcv_add_node_message(
               control,
               medusa_message_unpack_add_node(data, data_size),
               client
               );
         break;
      }

      case MEDUSA_REMOVE_NODE:{
         medusa_control_rcv_remove_node_message(
               control,
               medusa_message_unpack_remove_node(data, data_size)
               );
         break;
      }

      case MEDUSA_CHAT:{
         medusa_control_rcv_chat_message(
               control,
               medusa_message_unpack_chat(data, data_size)
               );
         break;
      }

      case MEDUSA_ADD_AUDIO_RESOURCE:{
         medusa_control_rcv_add_node_audio_resource_message(
               control,
               medusa_message_unpack_add_audio(data, data_size)
               );
         break;
      }

      case MEDUSA_ADD_MIDI_RESOURCE:{
         medusa_control_rcv_add_node_midi_resource_message(
               control,
               medusa_message_unpack_add_midi(data, data_size)
               );
         break;
      }

      case MEDUSA_REM_AUDIO_RESOURCE:{
         medusa_control_rcv_remove_node_audio_resource_message(
               control,
               medusa_message_unpack_remove_audio_resource(data, data_size)
               );
         break;
      }

      case MEDUSA_REM_MIDI_RESOURCE:{
         medusa_control_rcv_remove_node_midi_resource_message(
               control,
               medusa_message_unpack_remove_midi_resource(data, data_size)
               );
         break;
      }

      case MEDUSA_NETWORK_CONFIG:{
         medusa_control_rcv_network_config(
               control,
               medusa_message_unpack_network_config(data, data_size)
               );
         break;
      }
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL PUBLISH MY RESOURCES
   ---------------------------------------------------------------------------*/
void medusa_control_publish_my_resources(
            t_medusa_control * control,
            t_medusa_node * node){
   // publish me
   medusa_control_notify_add_node(control, node);
   t_medusa_list * senders = control->senders;
   while(senders){
      t_medusa_sender * sender = senders->data;
      medusa_control_notify_add_audio_resource(
            control,
            node,
            MEDUSA_SENDER,
            medusa_sender_get_name(sender),
            medusa_sender_get_uid(sender),
            medusa_sender_get_network_audio_resource(sender));
      medusa_control_notify_add_midi_resource(
            control,
            node,
            MEDUSA_SENDER,
            medusa_sender_get_name(sender),
            medusa_sender_get_uid(sender),
            medusa_sender_get_network_midi_resource(sender));
      if(medusa_sender_get_network_config(sender) != NULL)
         medusa_control_notify_sender_network_config(control, node, sender);
      senders = senders->next;
   }
   t_medusa_list * receivers = control->receivers;
   while(receivers){
      t_medusa_receiver * receiver = receivers->data;
      medusa_control_notify_add_audio_resource(
            control,
            node,
            MEDUSA_RECEIVER,
            medusa_receiver_get_name(receiver),
            medusa_receiver_get_uid(receiver),
            medusa_receiver_get_network_audio_resource(receiver));
      medusa_control_notify_add_midi_resource(
            control,
            node,
            MEDUSA_RECEIVER,
            medusa_receiver_get_name(receiver),
            medusa_receiver_get_uid(receiver),
            medusa_receiver_get_network_midi_resource(receiver));
      if(medusa_receiver_get_network_config(receiver) != NULL)
         medusa_control_notify_receiver_network_config(control, node, receiver);
      receivers = receivers->next;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE ADD NODE MESSAGE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_add_node_message(
               t_medusa_control * control,
               t_medusa_message_add_node * message,
               const t_medusa_network_config * client){

   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   // if node is not in the list, check for name duplicity and add it

   if(node != NULL){
      free(message);
      return;
   }
   node = medusa_node_create(message, client);
   // Checking name unicity
   t_medusa_list * list = medusa_control_get_nodes(control);
   char * old_name = malloc(strlen(node->name) + 10);
   strcpy(old_name, node->name);
   int count = 1;
   while(list){
      t_medusa_node * temp = (t_medusa_node *) list->data;
      if(strcmp(node->name, temp->name) == 0){
         // We have someone with this name
         // Change resource name
         sprintf(node->name, "%s_%d", old_name, count);
         // Restart searching
         list = medusa_control_get_nodes(control);
         count++;
      }
      list = list->next;
   }

   medusa_list_add_element(&control->nodes, node);
   // Added a noded, publish me
   medusa_control_publish_my_resources(control, node);
   // Callback
   t_medusa_list * callback_list = control->add_node_callback_list;
   while(callback_list){
      t_medusa_add_node_callback * callback
            = (t_medusa_add_node_callback *)callback_list->data;
      callback->function(node, callback->args);
      callback_list = callback_list->next;
   }
   free(message);
   free(old_name);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE REMOVE NODE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_remove_node_message(
               t_medusa_control * control,
               t_medusa_message_remove_node * message){

   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   node = medusa_list_remove_element(
               &control->nodes,
               node,
               medusa_node_compare);
   if(node == NULL)
      return;
   // Callback
   t_medusa_list * callback_list = control->remove_node_callback_list;
   while(callback_list){
      t_medusa_remove_node_callback * callback
            = (t_medusa_remove_node_callback *)callback_list->data;
      callback->function(node, callback->args);
      callback_list = callback_list->next;
   }

   free(node);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE ADD NODE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_add_node_audio_resource_message(
         t_medusa_control * control,
         t_medusa_message_add_audio_resource * message){

   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   // if node is not in the list, add it
   if(node == NULL){
      // there is someone unknow in the network
      medusa_control_notify_add_node(control, NULL);
      return;
   }
   t_medusa_node_audio_resource * resource 
         = medusa_node_audio_resource_create(message);
   medusa_node_add_audio_resource(node, resource);

   t_medusa_list * callback_list = control->add_node_audio_resource_callback_list;
   while(callback_list){
      t_medusa_add_node_audio_resource_callback * callback
            = (t_medusa_add_node_audio_resource_callback *)callback_list->data;
      callback->function(node, resource, callback->args);
      callback_list = callback_list->next;
   }
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE ADD NODE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_add_node_midi_resource_message(
         t_medusa_control * control,
         t_medusa_message_add_midi_resource * message){
   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   if(node == NULL){
      // there is someone unknow in the network
      medusa_control_notify_add_node(control, NULL);
      return;
   }

   t_medusa_node_midi_resource * resource 
         = medusa_node_midi_resource_create(message);
   // Callback
   medusa_node_add_midi_resource(node, resource);
   t_medusa_list * callback_list = control->add_node_midi_resource_callback_list;
   while(callback_list){
      t_medusa_add_node_midi_resource_callback * callback
            = (t_medusa_add_node_midi_resource_callback *)callback_list->data;
      callback->function(node, resource, callback->args);
      callback_list = callback_list->next;
   }
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE ADD REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_remove_node_midi_resource_message(
         t_medusa_control * control,
         t_medusa_message_remove_midi_resource * message){
   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   // if node is not in the list, ignore it
   if(node == NULL){
      free(message);
      return;
   }
   t_medusa_node_midi_resource * midi_resource
               = medusa_node_remove_midi_resource(node, message->resource_uid);
   if(midi_resource != NULL){
      t_medusa_list * callback_list = control->remove_node_midi_resource_callback_list;
      while(callback_list){
         t_medusa_remove_node_midi_resource_callback * callback
               = (t_medusa_remove_node_midi_resource_callback *)callback_list->data;
         callback->function(node, midi_resource, callback->args);
         callback_list = callback_list->next;
      }
   }
   if(midi_resource != NULL)
      free(midi_resource);

   t_medusa_node_audio_resource * audio_resource
         = medusa_node_remove_audio_resource(node, message->resource_uid);
   if(audio_resource != NULL){
      t_medusa_list * callback_list = control->remove_node_audio_resource_callback_list;
      while(callback_list){
         t_medusa_remove_node_audio_resource_callback * callback
               = (t_medusa_remove_node_audio_resource_callback *)callback_list->data;
         callback->function(node, audio_resource, callback->args);
         callback_list = callback_list->next;
      }
   }
   if(audio_resource != NULL)
      free(audio_resource);

   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE ADD REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_remove_node_audio_resource_message(
         t_medusa_control * control,
         t_medusa_message_remove_audio_resource * message){
   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   // if node is not in the list, ignore it
   if(node == NULL){
      free(message);
      return;
   }
   t_medusa_node_midi_resource * midi_resource
               = medusa_node_remove_midi_resource(node, message->resource_uid);
   if(midi_resource != NULL){
      t_medusa_list * callback_list = control->remove_node_midi_resource_callback_list;
      while(callback_list){
         t_medusa_remove_node_midi_resource_callback * callback
               = (t_medusa_remove_node_midi_resource_callback *)callback_list->data;
         callback->function(node, midi_resource, callback->args);
         callback_list = callback_list->next;
      }
   }
   if(midi_resource != NULL)
      free(midi_resource);

   t_medusa_node_audio_resource * audio_resource
         = medusa_node_remove_audio_resource(node, message->resource_uid);
   if(audio_resource != NULL){
      t_medusa_list * callback_list = control->remove_node_audio_resource_callback_list;
      while(callback_list){
         t_medusa_remove_node_audio_resource_callback * callback
               = (t_medusa_remove_node_audio_resource_callback *)callback_list->data;
         callback->function(node, audio_resource, callback->args);
         callback_list = callback_list->next;
      }
   }
   if(audio_resource != NULL)
      free(audio_resource);

   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE CHAT MESSAGE
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_chat_message(
               t_medusa_control * control,
               t_medusa_message_chat * message){
   // Callback
   t_medusa_node * node = medusa_control_get_node(control, message->node_uid);
   message->data[message->data_size] = '\0';
   t_medusa_list * callback_list = control->chat_callback_list;
   while(callback_list){
         t_medusa_chat_callback * callback
               = (t_medusa_chat_callback *)callback_list->data;
         callback->function(
               node,
               message,
               callback->args);
         callback_list = callback_list->next;
   }
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL RECEIVE NETWORK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_control_rcv_network_config(
         t_medusa_control * control,
         t_medusa_message_network_config * message){

   t_medusa_list * node_list = control->nodes;
   while(node_list){
      t_medusa_node * node = (t_medusa_node *) node_list->data;
      if(medusa_node_get_id(node) == message->node_uid){
         t_medusa_list * audio_list = medusa_node_get_audio_resources(node);
         while(audio_list){
            t_medusa_node_audio_resource * audio
                  = (t_medusa_node_audio_resource *) audio_list->data;
            if(audio->uid == message->resource_uid){
               medusa_node_audio_set_network_config(audio, message);
               // Callback
               t_medusa_list * callback_list
                        = control->add_node_audio_resource_callback_list;
               while(callback_list){
                  t_medusa_add_node_audio_resource_callback * callback
                        = (t_medusa_add_node_audio_resource_callback *)
                        callback_list->data;
                  callback->function(
                           node,
                           audio,
                           callback->args);
                  callback_list = callback_list->next;
               }
            }
            audio_list = audio_list->next;
         }
         t_medusa_list * midi_list = medusa_node_get_midi_resources(node);
         while(midi_list){
            t_medusa_node_midi_resource * midi
                  = (t_medusa_node_midi_resource *) midi_list->data;
            if(midi->uid == message->resource_uid){
               medusa_node_midi_set_network_config(midi, message);
               t_medusa_list * callback_list
                        = control->add_node_midi_resource_callback_list;
               while(callback_list){
                  t_medusa_add_node_midi_resource_callback * callback
                        = (t_medusa_add_node_midi_resource_callback *)
                        callback_list->data;
                  callback->function(
                           node,
                           midi,
                           callback->args);
                  callback_list = callback_list->next;
               }
            }
            midi_list = midi_list->next;
         }
         return;
      }
      node_list = node_list->next;
   }
   free(message);
}

/* -----------------------------------------------------------------------------
   MESSAGES RECEIVED BY THE UI MUST NOTIFY THE NETWORK
   ---------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY ADD NODE
   ---------------------------------------------------------------------------*/
void medusa_control_notify_add_node(
            t_medusa_control * control,
            t_medusa_node * node){
   t_medusa_message_add_node * message
            = medusa_message_create_add_node(control->uid, control->name);
   char msg[65535];
   size_t size = medusa_message_pack_add_node(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY REMOVE NODE
   ---------------------------------------------------------------------------*/
void medusa_control_notify_remove_node(
            t_medusa_control * control,
            t_medusa_node * node){
   t_medusa_message_remove_node * message
         = medusa_message_create_remove_node(control->uid);
   char msg[65535];
   size_t size = medusa_message_pack_remove_node(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY ADD AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_notify_add_audio_resource(
            t_medusa_control * control,
            t_medusa_node * node,
            MEDUSA_RESOURCE type,
            char * name,
            uint64_t resource_uid,
            const t_medusa_audio_resource * audio_resource){
   if(audio_resource == NULL)
      return;
   t_medusa_message_add_audio_resource * message
         = medusa_message_create_add_audio(
                  control->uid,
                  type,
                  name,
                  resource_uid,
                  audio_resource);
   char msg[65535];
   size_t size = medusa_message_pack_add_audio(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY REMOVE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_notify_remove_audio_resource(
            t_medusa_control * control,
            t_medusa_node * node,
            uint64_t resource_uid,
            t_medusa_audio_resource * audio_resource){
   if(audio_resource == NULL)
      return;
   t_medusa_message_remove_audio_resource * message
         = medusa_message_create_remove_audio_resource(control->uid, resource_uid);
   char msg[65535];
   size_t size = medusa_message_pack_remove_audio_resource(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY ADD MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_notify_add_midi_resource(
            t_medusa_control * control,
            t_medusa_node * node,
            MEDUSA_RESOURCE type,
            char * name,
            uint64_t resource_uid,
            const t_medusa_midi_resource * midi_resource){
   if(midi_resource == NULL)
      return;
   t_medusa_message_add_midi_resource * message
         = medusa_message_create_add_midi(
                  control->uid,
                  type,
                  name,
                  resource_uid,
                  midi_resource);
   char msg[65535];
   size_t size = medusa_message_pack_add_midi(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_control_notify_remove_midi_resource(
            t_medusa_control * control,
            t_medusa_node * node,
            uint64_t resource_uid,
            t_medusa_midi_resource * midi_resource){

   if(midi_resource == NULL)
      return;
   t_medusa_message_remove_midi_resource * message
         = medusa_message_create_remove_midi_resource(control->uid, resource_uid);
   char msg[65535];
   size_t size = medusa_message_pack_remove_midi_resource(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY CHAT
   ---------------------------------------------------------------------------*/
void medusa_control_notify_chat(
            t_medusa_control * control,
            t_medusa_node * node,
            char * data){
   t_medusa_message_chat * message
         = medusa_message_create_chat(control->uid, data);
   char msg[65535];
   size_t size = medusa_message_pack_chat(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY SENDER NETWORK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_control_notify_sender_network_config(
            t_medusa_control * control,
            t_medusa_node * node,
            t_medusa_sender * sender){
   t_medusa_message_network_config * message
         = medusa_message_create_network_config(
               control->uid,
               medusa_sender_get_uid(sender),
               medusa_sender_get_network_config(sender));
   char msg[65535];
   size_t size = medusa_message_pack_network_config(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL NOTIFY RECEIVER NETWORK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_control_notify_receiver_network_config(
            t_medusa_control * control,
            t_medusa_node * node,
            t_medusa_receiver * receiver){
   t_medusa_message_network_config * message
         = medusa_message_create_network_config(
               control->uid,
               medusa_receiver_get_uid(receiver),
               medusa_receiver_get_network_config(receiver));
   char msg[65535];
   size_t size = medusa_message_pack_network_config(message, msg);
   medusa_control_send_msg(control, node, msg, size);
   free(message);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SEND MSG
   ---------------------------------------------------------------------------*/
void medusa_control_send_msg(
         t_medusa_control * control,
         t_medusa_node * node,
         void * data,
         int data_size){
   if(node == NULL){ // Multicast
      medusa_server_send(control->server, data, data_size);
   }else{ // Unicast
      medusa_server_send_to_ip(
            control->server,
            medusa_node_get_ip(node),
            medusa_node_get_port(node),
            data,
            data_size);
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET ADD NODE CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_add_node_callback(
            t_medusa_control * control,
            t_medusa_add_node_callback_function function,
            void * args
            ){
   t_medusa_add_node_callback * callback = malloc(sizeof(t_medusa_add_node_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->add_node_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET REMOVE NODE CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_remove_node_callback(
            t_medusa_control * control,
            t_medusa_remove_node_callback_function function,
            void * args
            ){
   t_medusa_remove_node_callback * callback
         = malloc(sizeof(t_medusa_remove_node_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->remove_node_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET ADD NODE AUDIO RESOURCE CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_add_node_audio_resource_callback(
            t_medusa_control * control,
            t_medusa_add_node_audio_resource_callback_function function,
            void * args
            ){
   t_medusa_add_node_audio_resource_callback * callback
               = malloc(sizeof(t_medusa_add_node_audio_resource_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->add_node_audio_resource_callback_list, callback);
}
/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET ADD NODE MIDI RESOURCE CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_add_node_midi_resource_callback(
            t_medusa_control * control,
            t_medusa_add_node_midi_resource_callback_function function,
            void * args
            ){
   t_medusa_add_node_midi_resource_callback * callback
                  = malloc(sizeof(t_medusa_add_node_midi_resource_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->add_node_midi_resource_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET REMOVE NODE AUDIO RESOURCE CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_remove_node_audio_resource_callback(
            t_medusa_control * control,
            t_medusa_remove_node_audio_resource_callback_function function,
            void * args
            ){
   t_medusa_remove_node_audio_resource_callback * callback
               = malloc(sizeof(t_medusa_remove_node_audio_resource_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->remove_node_audio_resource_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET REMOVE NODE MIDI RESOURCE CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_remove_node_midi_resource_callback(
            t_medusa_control * control,
            t_medusa_remove_node_midi_resource_callback_function function,
            void * args
            ){
   t_medusa_remove_node_midi_resource_callback * callback
               = malloc(sizeof(t_medusa_remove_node_midi_resource_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->remove_node_midi_resource_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET CHAT CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_chat_callback(
            t_medusa_control * control,
            t_medusa_chat_callback_function function,
            void * args
            ){
   t_medusa_chat_callback * callback = malloc(sizeof(t_medusa_chat_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->chat_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET ADD SENDER CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_add_sender_callback(
            t_medusa_control * control,
            t_medusa_add_sender_callback_function function,
            void * args
            ){
   t_medusa_add_sender_callback * callback
         = malloc(sizeof(t_medusa_add_sender_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->add_sender_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET ADD RECEIVER CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_add_receiver_callback(
            t_medusa_control * control,
            t_medusa_add_receiver_callback_function function,
            void * args
            ){
   t_medusa_add_receiver_callback * callback
         = malloc(sizeof(t_medusa_add_receiver_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->add_receiver_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET REMOVE SENDER CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_remove_sender_callback(
            t_medusa_control * control,
            t_medusa_remove_sender_callback_function function,
            void * args
            ){
   t_medusa_remove_sender_callback * callback
         = malloc(sizeof(t_medusa_remove_sender_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->remove_sender_callback_list, callback);
}

/* -----------------------------------------------------------------------------
   MEDUSA CONTROL SET REMOVE RECEIVER CALLBACK
   ---------------------------------------------------------------------------*/
void medusa_control_set_remove_receiver_callback(
            t_medusa_control * control,
            t_medusa_remove_receiver_callback_function function,
            void * args
            ){
   t_medusa_remove_receiver_callback * callback
         = malloc(sizeof(t_medusa_remove_receiver_callback));
   callback->function = function;
   callback->args = args;
   medusa_list_add_element(&control->remove_receiver_callback_list, callback);
}

/*----------------------------------------------------------------------------*/
