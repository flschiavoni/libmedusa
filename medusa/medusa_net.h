#ifndef MEDUSA_NET_H
#define MEDUSA_NET_H

/** @file medusa_net.h
 *
 * @brief
 *
 *
 * @ingroup net
 */

#include "medusa.h"

int medusa_net_create_socket(
      MEDUSA_PROTOCOL medusa_protocol
      );

int medusa_net_send_connectionless(
      int socket,
      struct sockaddr * addr,
      socklen_t addr_len,
      void * data,
      int data_size
      );

int medusa_net_send_connected(
      int socket,
      void * data,
      int data_size
      );

int medusa_net_set_nonblocking(
      int socket
      );

int medusa_net_set_sndtimeout(
      int socket
      );

int medusa_net_set_tcpnowait(
      int socket
      );

int medusa_net_set_sctpnowait(
      int socket
      );

#endif /* MEDUSA_NET_H */
