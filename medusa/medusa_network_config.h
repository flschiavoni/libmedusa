#ifndef MEDUSA_NETWORK_CONFIG_H
#define MEDUSA_NETWORK_CONFIG_H

/** @file medusa_network_config.h
 *
 * @brief
 * 
 *
 * @ingroup net
 */

#include "medusa.h"

struct medusa_network_config {
   int socket;
   MEDUSA_PROTOCOL protocol;
   struct sockaddr_in addr;
};

t_medusa_network_config * medusa_network_config_create(
      MEDUSA_PROTOCOL protocol,
      const char * ip,
      uint16_t port
      );

const char * medusa_network_config_get_ip(
      const t_medusa_network_config * config
      );

int medusa_network_config_get_port(
      const t_medusa_network_config * config
      );

int medusa_network_config_compare(
      const void * data1,
      const void * data2
      );

#endif /* MEDUSA_NETWORK_CONFIG_H */
