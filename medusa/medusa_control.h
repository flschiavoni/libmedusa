#ifndef MEDUSA_CONTROL_H
#define MEDUSA_CONTROL_H

#include "medusa.h"

/** @file medusa_control.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */
// Callback functions definition

t_medusa_control * medusa_control_create(
      char * name
      );

int medusa_control_free(
      t_medusa_control * control
      );

const char * medusa_control_get_name(
      const t_medusa_control * control
      );

uint64_t medusa_control_get_uid(
      const t_medusa_control * control
      );

t_medusa_node * medusa_control_get_node(
      const t_medusa_control * control,
      uint64_t uid
      );

t_medusa_node_audio_resource * medusa_control_get_audio_resource_by_name(
      t_medusa_control * control,
      char * name
      );

t_medusa_node_midi_resource * medusa_control_get_midi_resource_by_name(
      t_medusa_control * control,
      char * name
      );

void medusa_control_receive_data(
      const t_medusa_network_config * client,
      void * data,
      int data_size,
      void * args
      );

t_medusa_list * medusa_control_get_nodes(
      const t_medusa_control * control
      );

t_medusa_list * medusa_control_get_senders(
      const t_medusa_control * control
      );

t_medusa_list * medusa_control_get_receivers(
      const t_medusa_control * control
      );

int medusa_control_add_receiver(
      t_medusa_control * control,
      t_medusa_receiver * receiver
      );

int medusa_control_remove_receiver(
      t_medusa_control * control,
      t_medusa_receiver * receiver
      );

t_medusa_receiver * medusa_control_get_receiver_by_uid(
      t_medusa_control * control,
      uint64_t uid
      );

int medusa_control_add_sender(
      t_medusa_control * control,
      t_medusa_sender * sender
      );

int medusa_control_remove_sender(
      t_medusa_control * control,
      t_medusa_sender * sender
      );

t_medusa_sender * medusa_control_get_sender_by_uid(
      t_medusa_control * control,
      uint64_t uid
      );


void medusa_control_notify_add_node(
      t_medusa_control * control,
      t_medusa_node * node
      );

void medusa_control_notify_remove_node(
      t_medusa_control * control,
      t_medusa_node * node
      );

void medusa_control_notify_add_audio_resource(
      t_medusa_control * control,
      t_medusa_node * node,
      MEDUSA_RESOURCE type,
      char * name,
      uint64_t uid,
      const t_medusa_audio_resource * audio_resource
      );

void medusa_control_notify_add_midi_resource(
      t_medusa_control * control,
      t_medusa_node * node,
      MEDUSA_RESOURCE type,
      char * name,
      uint64_t uid,
      const t_medusa_midi_resource * midi_resource
      );

void medusa_control_notify_remove_audio_resource(
      t_medusa_control * control,
      t_medusa_node * node,
      uint64_t uid,
      t_medusa_audio_resource * audio_resource
      );

void medusa_control_notify_remove_midi_resource(
      t_medusa_control * control,
      t_medusa_node * node,
      uint64_t uid,
      t_medusa_midi_resource * midi_resource
      );

void medusa_control_notify_sender_network_config(
      t_medusa_control * control,
      t_medusa_node * node,
      t_medusa_sender * sender
      );

void medusa_control_notify_receiver_network_config(
      t_medusa_control * control,
      t_medusa_node * node,
      t_medusa_receiver * receiver
      );


// callback settings

void medusa_control_set_add_node_callback(
      t_medusa_control * control,
      t_medusa_add_node_callback_function function,
      void * args
      );

void medusa_control_set_remove_node_callback(
      t_medusa_control * control,
      t_medusa_remove_node_callback_function function,
      void * args
      );

void medusa_control_set_add_node_audio_resource_callback(
      t_medusa_control * control,
      t_medusa_add_node_audio_resource_callback_function function,
      void * args
      );

void medusa_control_set_add_node_midi_resource_callback(
      t_medusa_control * control,
      t_medusa_add_node_midi_resource_callback_function function,
      void * args
      );

void medusa_control_set_remove_node_audio_resource_callback(
      t_medusa_control * control,
      t_medusa_remove_node_audio_resource_callback_function function,
      void * args
      );

void medusa_control_set_remove_node_midi_resource_callback(
      t_medusa_control * control,
      t_medusa_remove_node_midi_resource_callback_function function,
      void * args
      );

void medusa_control_set_add_sender_callback(
      t_medusa_control * control,
      t_medusa_add_sender_callback_function function,
      void * args
      );

void medusa_control_set_add_receiver_callback(
      t_medusa_control * control,
      t_medusa_add_receiver_callback_function function,
      void * args
      );

void medusa_control_set_remove_sender_callback(
      t_medusa_control * control,
      t_medusa_remove_sender_callback_function function,
      void * args
      );

void medusa_control_set_remove_receiver_callback(
      t_medusa_control * control,
      t_medusa_remove_receiver_callback_function function,
      void * args
      );

#endif /* MEDUSA_CONTROL_H */
