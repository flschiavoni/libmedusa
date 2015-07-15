#ifndef MEDUSA_CLIENT_H
#define MEDUSA_CLIENT_H

/** @file medusa_client.h
 *
 * @brief A client is a network node that connects on servers.
 *
 *
 * @ingroup net
 */

#include "medusa.h"


/* ------------------------------------------------------------------
   MEDUSA NETWORK CLIENT
   ------------------------------------------------------------------*/

typedef void (*t_medusa_client_server_connected_callback)(
      const t_medusa_network_config * server,
      MEDUSA_STATUS status,
      void * args
      );

typedef void (*t_medusa_client_waiting_server_callback)(
      void * args
      );

int medusa_client_set_config(
      t_medusa_client * client,
      MEDUSA_PROTOCOL protocol,
      const char * ip,
      int port
      );

const t_medusa_network_config * medusa_client_get_network_config(
      const t_medusa_client * client
      );

t_medusa_client * medusa_client_create(void);

int medusa_client_free(
      t_medusa_client * client
      );

int medusa_client_server_connected(
      t_medusa_client * client,
      int status
      );

MEDUSA_STATUS medusa_client_get_status(
      const t_medusa_client * client
      );

void medusa_client_set_looopback_status(
      t_medusa_client * client,
      MEDUSA_STATUS status
      );

int medusa_client_send(
      t_medusa_client * client,
      void * data,
      int data_size
      );

int medusa_client_receive(
      t_medusa_client * client,
      void * data
      );

// Set callback functions
int medusa_client_set_connection_callback(
      t_medusa_client * client,
      t_medusa_connection_callback_function connection_callback,
      void * args
      );

int medusa_client_set_waiting_server_callback(
      t_medusa_client * client,
      t_medusa_waiting_server_callback_function waiting_server_callback,
      void * args
      );

int medusa_client_set_data_received_callback(
      t_medusa_client * client,
      t_medusa_data_received_callback_function data_received_callback,
      void * args
      );

#endif /* MEDUSA_CLIENT_H */
