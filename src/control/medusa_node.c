#include "medusa.h"

/** @file medusa_node.c
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA NODE CREATE
   ---------------------------------------------------------------------------*/
t_medusa_node * medusa_node_create(
         const t_medusa_message_add_node * message,
         const t_medusa_network_config * client){
   t_medusa_node * node = malloc(sizeof(t_medusa_node));
   node->uid = message->node_uid;
   strcpy(node->name, message->name);
   medusa_util_format_name(node->name);
   strcpy(node->ip, medusa_network_config_get_ip(client));
   node->port = medusa_network_config_get_port(client);

   node->audio_resources = medusa_list_create();
   node->midi_resources = medusa_list_create();
   node->connections = medusa_list_create();

   return node;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE FREE
   ---------------------------------------------------------------------------*/
void medusa_node_free(t_medusa_node * node){
   t_medusa_list * list;
   
   list = node->audio_resources;
   while(list){
      free(list->data);
      list = list->next;
   }
   medusa_list_free(node->audio_resources);

   list = node->midi_resources;
   while(list){
      free(list->data);
      list = list->next;
   }
   medusa_list_free(node->midi_resources);

   list = node->connections;
   while(list){
      free(list->data);
      list = list->next;
   }
   medusa_list_free(node->connections);

   free(node);
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET AUDIO RESOURCES
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_node_get_audio_resources(
      t_medusa_node * node
      ){
   if(node!= NULL)
      return node->audio_resources;
   else
      return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET MIDI RESOURCES
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_node_get_midi_resources(
      t_medusa_node * node
      ){
   if(node!= NULL)
      return node->midi_resources;
   else
      return NULL;

}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET CONNECTIONS
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_node_get_connections(
      t_medusa_node * node
      ){
   if(node!= NULL)
      return node->connections;
   else
      return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE ADD AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_node_add_audio_resource(
      t_medusa_node * node,
      t_medusa_node_audio_resource * resource
      ){
   t_medusa_list * list = medusa_node_get_audio_resources(node);
   //look for the element
   while(list){
      if(((t_medusa_node_audio_resource *)list->data)->uid == resource->uid){
         // if exist, update
         list->data = resource;
         return;
      }
      list = list->next;
   }

   // Checking name unicity
   list = medusa_node_get_audio_resources(node);
   char old_name[strlen(resource->name) + 3];
   strcpy(old_name, resource->name);
   int count = 1;
   while(list){
      t_medusa_node_audio_resource * temp
            = (t_medusa_node_audio_resource *) list->data;
      if(strcmp(resource->name, temp->name) == 0){
         // We have someone with this name
         // Change resource name
         sprintf(resource->name, "%s_%d", old_name, count);
         // Restart searching
         list = medusa_node_get_audio_resources(node);
         count++;
      }
      list = list->next;
   }
   strcpy(resource->ip, node->ip);
   medusa_list_add_element(&node->audio_resources, resource);
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE ADD MIDI RESOURCE
   ---------------------------------------------------------------------------*/
void medusa_node_add_midi_resource(
      t_medusa_node * node,
      t_medusa_node_midi_resource * resource
      ){

   //look for the element
   t_medusa_list * list = medusa_node_get_midi_resources(node);
   while(list){
      if(((t_medusa_node_midi_resource *)list->data)->uid == resource->uid){
         // if exist, update
         list->data = resource;
         return;
      }
      list = list->next;
   }

   // Checking name unicity
   list = medusa_node_get_midi_resources(node);
   char old_name[strlen(resource->name) + 3];
   strcpy(old_name, resource->name);
   int count = 1;
   while(list){
      t_medusa_node_midi_resource * temp
            = (t_medusa_node_midi_resource *) list->data;
      if(strcmp(resource->name, temp->name) == 0){
         // We have someone with this name
         // Change resource name
         sprintf(resource->name, "%s(%d)", old_name, count);
         // Restart searching
         list = medusa_node_get_midi_resources(node);
         count++;
      }
      list = list->next;
   }
   strcpy(resource->ip, node->ip);
   medusa_list_add_element(&node->midi_resources, resource);
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE ADD CONNECTION
   ---------------------------------------------------------------------------*/
void medusa_node_add_connection(
      t_medusa_node * node,
      t_medusa_node_connection * connection
      ){
   medusa_list_add_element(&node->connections, connection);
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE REMOVE AUDIO RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_node_audio_resource * medusa_node_remove_audio_resource(
      t_medusa_node * node,
      uint64_t resource_uid
      ){
   t_medusa_node_audio_resource resource;
   resource.uid = resource_uid;
   return (t_medusa_node_audio_resource *) medusa_list_remove_element(
               &node->audio_resources,
               &resource,
               medusa_node_audio_resource_compare);
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE REMOVE MIDI RESOURCE
   ---------------------------------------------------------------------------*/
t_medusa_node_midi_resource *  medusa_node_remove_midi_resource(
      t_medusa_node * node,
      uint64_t resource_uid
      ){
   t_medusa_node_midi_resource resource;
   resource.uid = resource_uid;
   return (t_medusa_node_midi_resource *) medusa_list_remove_element(
               &node->midi_resources,
               &resource,
               medusa_node_midi_resource_compare);
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE REMOVE CONNECTION
   ---------------------------------------------------------------------------*/
t_medusa_node_connection * medusa_node_remove_connection(
      t_medusa_node * node,
      uint64_t src_uid,
      uint64_t dest_uid
      ){
   t_medusa_node_connection * connection
               = malloc(sizeof(t_medusa_node_connection));
   connection->src_uid = src_uid;
   connection->dest_uid = dest_uid;
   connection = medusa_list_remove_element(
               &node->connections,
               connection,
               medusa_node_connection_compare);
   return connection;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET ID
   ---------------------------------------------------------------------------*/
uint64_t medusa_node_get_id(
      t_medusa_node * node
      ){
   if(node != NULL)
      return node->uid;
   else
      return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET IP
   ---------------------------------------------------------------------------*/
const char * medusa_node_get_ip(
      t_medusa_node * node
      ){
   return node->ip;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET NAME
   ---------------------------------------------------------------------------*/
const char * medusa_node_get_name(
      t_medusa_node * node
      ){
   return node->name;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE GET PORT
   ---------------------------------------------------------------------------*/
int medusa_node_get_port(
      t_medusa_node * node
      ){
   return node->port;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE AUDIO RESOURCE CREATE
   ---------------------------------------------------------------------------*/
t_medusa_node_audio_resource * medusa_node_audio_resource_create(
      const t_medusa_message_add_audio_resource * message){

   t_medusa_node_audio_resource * audio_resource
      = malloc(sizeof(t_medusa_node_audio_resource));
   strcpy(audio_resource->name, message->name);
   medusa_util_format_name(audio_resource->name);
   audio_resource->uid = message->resource_uid;
   audio_resource->resource_type = message->resource_type;
   audio_resource->channels = message->channels;
   audio_resource->sample_rate = message->sample_rate;
   audio_resource->bit_depth = message->bit_depth;
   audio_resource->endianness = message->endianness;
   audio_resource->block_size = message->block_size;
   audio_resource->CODEC = message->CODEC;
   return audio_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE MIDI RESOURCE CREATE
   ---------------------------------------------------------------------------*/
t_medusa_node_midi_resource * medusa_node_midi_resource_create(
         const t_medusa_message_add_midi_resource * message){
   t_medusa_node_midi_resource * midi_resource
      = malloc(sizeof(t_medusa_node_midi_resource));
   strcpy(midi_resource->name, message->name);
   medusa_util_format_name(midi_resource->name);
   midi_resource->uid = message->resource_uid;
   midi_resource->resource_type = message->resource_type;
   midi_resource->channels = message->channels;
   return midi_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE AUDIO RESOURCE SET NETWORK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_node_audio_set_network_config(
      t_medusa_node_audio_resource * audio_resource,
      const t_medusa_message_network_config * message){
   audio_resource->protocol = message->protocol;
   audio_resource->port = message->port;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE MIDI RESOURCE SET NETWORK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_node_midi_set_network_config(
      t_medusa_node_midi_resource * midi_resource,
      const t_medusa_message_network_config * message){
   midi_resource->protocol = message->protocol;
   midi_resource->port = message->port;
}
/* -----------------------------------------------------------------------------
   MEDUSA NODE COMPARE
   ---------------------------------------------------------------------------*/
int medusa_node_compare(const void * data1, const void * data2){
   t_medusa_node * n1 = (t_medusa_node *) data1;
   t_medusa_node * n2 = (t_medusa_node *) data2;
   if(n1 == NULL || n2 == NULL)
      return 0;
   if(n1->uid == n2->uid)
      return 1;
   else
      return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE AUDIO RESOURCE COMPARE
   ---------------------------------------------------------------------------*/
int medusa_node_audio_resource_compare(const void * data1, const void * data2){
   t_medusa_node_audio_resource * n1 = (t_medusa_node_audio_resource *) data1;
   t_medusa_node_audio_resource * n2 = (t_medusa_node_audio_resource *) data2;
   if(n1->uid == n2->uid)
      return 1;
   else
      return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE MIDI RESOURCE COMPARE
   ---------------------------------------------------------------------------*/
int medusa_node_midi_resource_compare(const void * data1, const void * data2){
   t_medusa_node_midi_resource * n1 = (t_medusa_node_midi_resource *) data1;
   t_medusa_node_midi_resource * n2 = (t_medusa_node_midi_resource *) data2;
   if(n1->uid == n2->uid)
      return 1;
   else
      return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA NODE CONNECTION COMPARE
   ---------------------------------------------------------------------------*/
int medusa_node_connection_compare(const void * data1, const void * data2){
   t_medusa_node_connection * n1 = (t_medusa_node_connection *) data1;
   t_medusa_node_connection * n2 = (t_medusa_node_connection *) data2;
   if(n1 == NULL || n2 == NULL)
      return 0;
   if(n1->src_uid == n2->src_uid && n1->dest_uid == n2->dest_uid)
      return 1;
   else
      return 0;
}

/*----------------------------------------------------------------------------*/
