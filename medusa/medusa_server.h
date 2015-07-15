#ifndef MEDUSA_SERVER_H
#define MEDUSA_SERVER_H

#include "medusa.h"

/** @file medusa_server.h
 *
 * @brief A server is a network node waiting for clients.
 *
 *
 * @ingroup net
 */

/* ------------------------------------------------------------------
   MEDUSA NETWORK SERVER
   ------------------------------------------------------------------*/
t_medusa_server * medusa_server_create(void);

int medusa_server_connect(
      t_medusa_server * server
      );

int medusa_server_free(
      t_medusa_server * server
      );

void medusa_server_set_config(
      t_medusa_server * server,
      MEDUSA_PROTOCOL protocol,
      int port
      );

const t_medusa_network_config * medusa_server_get_network_config(
      const t_medusa_server * server
      );

MEDUSA_PROTOCOL medusa_server_get_protocol(
      const t_medusa_server * server
      );

int medusa_server_get_client_count(
      const t_medusa_server * server
      );

int medusa_server_client_connected(
         t_medusa_server * server,
         int socket,
         struct sockaddr_in cliaddr
         );

int medusa_server_client_disconnected(
         t_medusa_server * server,
         struct sockaddr_in cliaddr
         );

int medusa_server_send(
      t_medusa_server * server,
      void * data,
      int data_size
      );

int medusa_server_send_to_ip(
         const t_medusa_server * server,
         const char * ip,
         int port,
         void * data,
         int data_size
         );

// Set callback functions
int medusa_server_set_client_connected_callback(
      t_medusa_server * server,
      t_medusa_connection_callback_function connection_callback,
      void * args
      );

int medusa_server_set_data_received_callback(
      t_medusa_server * server,
      t_medusa_nw_data_received_callback_function data_received_connected_callback,
      void * args
      );
#endif /* MEDUSA_SERVER_H */
