#include "medusa.h"

/* -----------------------------------------------------------------------------
   MEDUSA NET CREATE SOCKET
   ---------------------------------------------------------------------------*/
int medusa_net_create_socket(MEDUSA_PROTOCOL medusa_protocol){

   int sockfd;
   int domain = AF_INET, type, prot;

   switch(medusa_protocol){
      case MEDUSA_UDP:  { type = SOCK_DGRAM;     prot = IPPROTO_UDP;  break; }
      case MEDUSA_MULTI:{ type = SOCK_DGRAM;     prot = IPPROTO_UDP;  break; }
      case MEDUSA_BROAD:{ type = SOCK_DGRAM;     prot = IPPROTO_UDP;  break; }
      case MEDUSA_TCP:  { type = SOCK_STREAM;    prot = IPPROTO_TCP;  break; }
      case MEDUSA_DCCP: { type = SOCK_DCCP;      prot = IPPROTO_DCCP; break; }
      case MEDUSA_SCTP1:{ type = SOCK_STREAM;    prot = IPPROTO_SCTP; break; }
      case MEDUSA_SCTP2:{ type = SOCK_SEQPACKET; prot = IPPROTO_SCTP; break; }
      // No protocol assumes UDP as default
      default: { type =  SOCK_DGRAM;    prot = IPPROTO_UDP;  break; }
   }

   if ((sockfd = socket(domain, type, prot)) == -1) {
      return MEDUSA_ERROR_COULD_NOT_CREATE_SOCKET;
   }

   int flag = 1;
   switch(medusa_protocol){
      case MEDUSA_TCP:{ medusa_net_set_tcpnowait(sockfd); break;}
      case MEDUSA_MULTI:{
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = inet_addr(MEDUSA_MULTICAST_GROUP);
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                  &mreq, sizeof(mreq)) < 0)
               return MEDUSA_ERROR_COULD_NOT_SET_IP_MEMBERSHIP;
            // To enable localhost to receive multicast messages
            flag = 1;
            if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP,
                  &flag, sizeof(flag)) < 0)
               return MEDUSA_ERROR_COULD_NOT_SET_IP_MULTICAST_LOOP;
            if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                        (const char *) &flag, sizeof(flag)) < 0)
               return MEDUSA_ERROR_COULD_NOT_SET_SO_REUSEADDR;
            break;
            }
      case MEDUSA_BROAD:{
            if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                  &flag,sizeof flag) < 0)
               return MEDUSA_ERROR_COULD_NOT_SET_SO_BROADCAST;
            if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                        (const char *) &flag, sizeof(flag)) < 0)
               return MEDUSA_ERROR_COULD_NOT_SET_SO_REUSEADDR;
            break;
            }
      case MEDUSA_SCTP1:
      case MEDUSA_SCTP2: {
            medusa_net_set_sctpnowait(sockfd);
            break;
            }
   }

   medusa_net_set_sndtimeout(sockfd);
   flag = 6; // max value without root privilege
   if(setsockopt(sockfd, SOL_SOCKET, SO_PRIORITY, (char *)&flag,
               sizeof(flag)) < 0)
      return MEDUSA_ERROR_COULD_NOT_SET_SO_PRIORITY;

   return sockfd;
}

/* -----------------------------------------------------------------------------
   MEDUSA NET SET SEND TIMEOUT
   ---------------------------------------------------------------------------*/
int medusa_net_set_sndtimeout(int socket_fd){
   //Set socket options
   struct timeval timeout;
   timeout.tv_sec = 0;
   timeout.tv_usec = 40000;
   if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
               sizeof(timeout)) < 0)
      return MEDUSA_ERROR_COULD_NOT_SET_SO_SNDTIMEO;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA NET SET TCP NO WAIT
   ---------------------------------------------------------------------------*/
int medusa_net_set_tcpnowait(int socket_fd){
   int flag = 1;
   if(setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY,
         (char *) &flag, sizeof(int)) < 0)
      return MEDUSA_ERROR_COULD_NOT_SET_TCP_NODELAY;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA NET SET SCTP NO WAIT
   ---------------------------------------------------------------------------*/
int medusa_net_set_sctpnowait(int socket_fd){
   int flag = 1;
   if(setsockopt(socket_fd, IPPROTO_SCTP, SCTP_NODELAY,
            (char *) &flag, sizeof(int)) < 0)
      return MEDUSA_ERROR_COULD_NOT_SET_SCTP_NODELAY;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA NET SET NON BLOCKING
   ---------------------------------------------------------------------------*/
int medusa_net_set_nonblocking(int socket_fd){
   int opts;
   opts = fcntl(socket_fd, F_GETFL);
   if(opts < 0){
      return MEDUSA_ERROR_COULD_NOT_SET_NONBLOCKING;
   }
   opts = (opts | O_NONBLOCK);
   if(fcntl(socket_fd, F_SETFL,opts) < 0){
      return MEDUSA_ERROR_COULD_NOT_SET_NONBLOCKING;
   }
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA NET SEND CONNECTIONLESS (UDP AND SCTP2)
   ---------------------------------------------------------------------------*/
int medusa_net_send_connectionless(
      int socket_fd,
      struct sockaddr * addr,
      socklen_t addr_len,
      void * data,
      int data_size){
   int try = MEDUSA_SERVER_LOOP;
   int sent = 0;
   do{
      sent +=  sendto(
            socket_fd,
            data + sent,
            data_size - sent,
            0,
            addr,
            addr_len);
      try--;
      }while((sent != -1) && (sent != data_size)&&(errno == EAGAIN) && try > 0);
   return sent;
}

/* -----------------------------------------------------------------------------
   MEDUSA NET SEND (TCP / DCCP / SCTP1)
   ---------------------------------------------------------------------------*/
int medusa_net_send_connected(int socket_fd, void * data, int data_size){
      int try = MEDUSA_SERVER_LOOP;
      int sent = 0;
      do{ // dealing with partial send
         sent +=  send(socket_fd, data + sent, data_size - sent, MSG_NOSIGNAL);
         try--;
      }while((sent != data_size)
            &&(errno == EAGAIN)
            && try > 0
            && (errno !=EPIPE)
            && sent != -1);
   return sent;
}
/*----------------------------------------------------------------------------*/
