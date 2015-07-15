#ifndef MEDUSA_CALLBACK_H
#define MEDUSA_CALLBACK_H

#include "medusa.h"

/** @file medusa_callback.h
 *
 * @brief
 * 
 *
 * @ingroup main
 */

typedef void (*t_medusa_data_received_callback_function)(
      int data_size,
      void * args
      );

typedef void (*t_medusa_nw_data_received_callback_function)(
      const t_medusa_network_config * network_config,
      void * data,
      int data_size,
      void * args
      );

typedef void (*t_medusa_waiting_server_callback_function)(
      void * args
      );

typedef void (*t_medusa_data_lost_callback_function)(
      void * args
      );

typedef void (*t_medusa_data_sent_callback_function)(
      int data_size,
      void * args
      );

typedef void (*t_medusa_connection_callback_function)(
      const t_medusa_network_config * client,
      MEDUSA_STATUS status,
      void * args
      );

typedef void (*t_medusa_meter_callback_function)(
         const t_medusa_network_config * client,
         t_medusa_timestamp pkt_time,
         t_medusa_timestamp received,
         t_medusa_timestamp loopback_time,
         uint16_t pkt_channel,
         uint32_t pkt_seq_number,
         uint32_t pkt_size,
         void * meter_callback_args
         );

typedef void (*t_medusa_free_callback_function)(
      void * args
      );

typedef void (*t_medusa_stop_callback_function)(
      void * args
      );

typedef void (*t_medusa_prepare_data_callback_function)(
      int channel,
      void * data,
      int data_size,
      void * args
      );

typedef void (*t_medusa_add_node_callback_function)(
      t_medusa_node * node,
      void * args
      );

typedef void (*t_medusa_remove_node_callback_function)(
      t_medusa_node * node,
      void * args
      );

typedef void (*t_medusa_add_sender_callback_function)(
      t_medusa_control * control,
      t_medusa_sender * sender,
      void * args
      );

typedef void (*t_medusa_add_receiver_callback_function)(
      t_medusa_control * control,
      t_medusa_receiver * receiver,
      void * args
      );

typedef void (*t_medusa_remove_sender_callback_function)(
      t_medusa_control * control,
      t_medusa_sender * sender,
      void * args
      );

typedef void (*t_medusa_remove_receiver_callback_function)(
      t_medusa_control * control,
      t_medusa_receiver * receiver,
      void * args
      );

struct medusa_waiting_server_callback{
   t_medusa_waiting_server_callback_function function;
   void * args;
   };

struct medusa_data_lost_callback{
   t_medusa_data_lost_callback_function function;
   void * args;
   };

struct medusa_data_sent_callback{
   t_medusa_data_sent_callback_function function;
   void * args;
   };

struct medusa_connection_callback{
   t_medusa_connection_callback_function function;
   void * args;
   };

struct medusa_meter_callback{
   t_medusa_meter_callback_function function;
   void * args;
   };

struct medusa_free_callback{
   t_medusa_free_callback_function function;
   void * args;
   };

struct medusa_stop_callback{
   t_medusa_stop_callback_function function;
   void * args;
   };

struct medusa_prepare_data_callback{
   t_medusa_prepare_data_callback_function function;
   void * args;
   };

struct medusa_add_node_callback{
   t_medusa_add_node_callback_function function;
   void * args;
   };

struct medusa_remove_node_callback{
   t_medusa_remove_node_callback_function function;
   void * args;
   };

struct medusa_add_sender_callback{
   t_medusa_add_sender_callback_function function;
   void * args;
   };

struct medusa_add_receiver_callback{
   t_medusa_add_receiver_callback_function function;
   void * args;
   };

struct medusa_remove_sender_callback{
   t_medusa_remove_sender_callback_function function;
   void * args;
   };

struct medusa_remove_receiver_callback{
   t_medusa_remove_receiver_callback_function function;
   void * args;
   };

struct medusa_data_received_callback{
   t_medusa_data_received_callback_function function;
   void * args;
};

struct medusa_nw_data_received_callback{
   t_medusa_nw_data_received_callback_function function;
   void * args;
};

#endif /* MEDUSA_CALLBACK_H */
