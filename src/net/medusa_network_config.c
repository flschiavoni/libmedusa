#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA NETWORK CONFIG CREATE
   ---------------------------------------------------------------------------*/
t_medusa_network_config * medusa_network_config_create(
      MEDUSA_PROTOCOL protocol,
      const char * ip,
      uint16_t port
      ){
   t_medusa_network_config * network_config;
   network_config = malloc(sizeof(t_medusa_network_config));
   network_config->socket = -1;
   network_config->protocol = protocol;

   struct sockaddr_in si;
   memset((void *) &si, '\0', sizeof (struct sockaddr_in));
   si.sin_family = AF_INET; // IPV4 or IPV6 AF_UNSPEC Problems to SCTP
   si.sin_port = htons(port);
   si.sin_addr.s_addr = inet_addr(ip);

   network_config->addr = si;
   return network_config;
}

/* -----------------------------------------------------------------------------
   MEDUSA NETWORK CONFIG GET IP
   ---------------------------------------------------------------------------*/
const char * medusa_network_config_get_ip(
         const t_medusa_network_config * config){
   return inet_ntoa(config->addr.sin_addr);
}

/* -----------------------------------------------------------------------------
   MEDUSA NETWORK CONFIG GET PORT
   ---------------------------------------------------------------------------*/
int medusa_network_config_get_port(const t_medusa_network_config * config){
   return ntohs(config->addr.sin_port);
}

/* -----------------------------------------------------------------------------
   MEDUSA NETWORK CONFIG COMPARE
   ---------------------------------------------------------------------------*/
int medusa_network_config_compare(const void * data1, const void * data2){
   t_medusa_network_config * n1 = (t_medusa_network_config *) data1;
   t_medusa_network_config * n2 = (t_medusa_network_config *) data2;

   if(n1->addr.sin_addr.s_addr == n2->addr.sin_addr.s_addr
      && n1->addr.sin_port == n1->addr.sin_port)
      return 1;
   else
      return 0;
}

/*----------------------------------------------------------------------------*/
