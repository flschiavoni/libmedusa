#include "medusa.h"

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA SERVER --------------------------------------
// -----------------------------------------------------------------------------

typedef int (* t_medusa_server_send_function)(
      t_medusa_server * server,
      void * data,
      int data_size
      );

typedef int (* t_medusa_server_receive_function)(
      t_medusa_server * server,
      int socket,
      struct sockaddr_in cliaddr,
      void * data
      );

typedef void (* t_medusa_server_poll)(
      t_medusa_server * server,
      char * data
      );

typedef void (* t_medusa_server_build_select_list)(
      t_medusa_server * server
      );

struct medusa_server{
   t_medusa_network_config * network_config;

   t_medusa_list * clients;

   MEDUSA_STATUS status;
   pthread_t server_receive_thread;

   fd_set read_set;
   int max_socket;
   uint32_t client_count;

   char * data;

   t_medusa_connection_callback * connection_callback;
   t_medusa_nw_data_received_callback * data_received_callback;

   t_medusa_server_send_function send;
   t_medusa_server_receive_function receive;
   t_medusa_server_poll poll;
   t_medusa_server_build_select_list select_list;
   };

static void * medusa_server_receive_thread(void * arg);

static int medusa_server_send_multicast(
      t_medusa_server * server,
      void * data,
      int data_size);

static int medusa_server_send_broadcast(
      t_medusa_server * server,
      void * data,
      int data_size);

static int medusa_server_send_connected(
      t_medusa_server * server,
      void * data,
      int data_size);

static int medusa_server_send_connectionless(
      t_medusa_server * server,
      void * data,
      int data_size);

static int medusa_server_receive_connected(
      t_medusa_server * server,
      int socket,
      struct sockaddr_in cliaddr,
      void * data
      );

static int medusa_server_receive_connectionless(
      t_medusa_server * server,
      int socket,
      struct sockaddr_in cliaddr,
      void * data
      );

static int medusa_server_accept(
      t_medusa_server * server
      );

static int medusa_server_receive(
      t_medusa_server * server,
      int socket,
      struct sockaddr_in cliaddr,
      void * data
      );

static void medusa_server_build_select_list_connected(
      t_medusa_server * server
      );

static void medusa_server_build_select_list_connectionless(
      t_medusa_server * server
      );

static void medusa_server_poll_connected(
      t_medusa_server * server,
      char * data
      );

static void medusa_server_poll_connectionless(
      t_medusa_server * server,
      char * data
      );


/* -----------------------------------------------------------------------------
   MEDUSA SERVER CREATE
   ---------------------------------------------------------------------------*/
t_medusa_server * medusa_server_create(){
   t_medusa_server * server = malloc(sizeof(t_medusa_server));
   server->network_config = NULL;
   server->status = MEDUSA_DISCONNECTED;
   server->clients = medusa_list_create();
   server->connection_callback = NULL;
   server->data_received_callback = NULL;
   server->max_socket = 0;
   server->client_count = 0;

   server->send = NULL;
   server->receive = NULL;
   server->poll = NULL;
   server->select_list = NULL;
   server->data = malloc(MEDUSA_RECEIVE_DATA_SIZE);

   return server;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER FREE
   ---------------------------------------------------------------------------*/
int medusa_server_free(t_medusa_server * server){

   t_medusa_message_bye * message = medusa_message_create_bye();
   char * msg = malloc(8);
   size_t size = 0;
   size = medusa_message_pack_bye(message, msg);
   medusa_server_send(server, msg, size);
   free(message);
   free(msg);

   server->status = MEDUSA_DISCONNECTED;
   while (pthread_cancel(server->server_receive_thread) != 0);
   shutdown(server->network_config->socket, 2);
   close(server->network_config->socket);
   server->network_config->socket = -1;
   t_medusa_list * list = server->clients;
   while(list){
      t_medusa_network_config * client = (t_medusa_network_config *) list->data;
      close(client->socket);
      free(client);
      list = list->next;
   }
   medusa_list_free(list);
   free(server->data);
   free(server->network_config);
   free(server);
   server = NULL;
   return 1;
}


/* -----------------------------------------------------------------------------
   MEDUSA SERVER SET CONFIG
   ---------------------------------------------------------------------------*/
void medusa_server_set_config(
      t_medusa_server * server,
      MEDUSA_PROTOCOL protocol,
      int port){

   if(server->network_config == NULL)
      server->network_config = malloc(sizeof(t_medusa_network_config));

   server->network_config->protocol = protocol;
   port = (port > 0) ? port : MEDUSA_DEFAULT_PORT;
   struct sockaddr_in si;
   memset((void *) &si, '\0', sizeof (struct sockaddr_in));
   si.sin_family = AF_INET; // IPV4 or IPV6 AF_UNSPEC Problems to SCTP
   si.sin_port = htons(port);
   si.sin_addr.s_addr = htonl(INADDR_ANY);
   server->network_config->addr = si;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER GET NETWORK CONFIG
   ---------------------------------------------------------------------------*/
const t_medusa_network_config * medusa_server_get_network_config(
         const t_medusa_server * server){
   if(server == NULL)
      return NULL;
   if(server->network_config == NULL)
      return NULL;
   return server->network_config;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER GET PROTOCOL
   ---------------------------------------------------------------------------*/
MEDUSA_PROTOCOL medusa_server_get_protocol(const t_medusa_server * server){
   return server->network_config->protocol;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SEND
   ---------------------------------------------------------------------------*/
int medusa_server_send(t_medusa_server * server, void * data, int data_size){
   if(server->send){
      return server->send(server, data, data_size);
   }else{
      return 0;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SEND TO IP
   ---------------------------------------------------------------------------*/
int medusa_server_send_to_ip(
         const t_medusa_server * server,
         const char * ip,
         int port,
         void * data,
         int data_size){

   struct sockaddr_in addr;
   bzero(&addr,sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr=inet_addr(ip);
   addr.sin_port=htons(port);

   int size = medusa_net_send_connectionless(
         server->network_config->socket,
         (struct sockaddr *) &addr,
         sizeof(addr),
         data,
         data_size);

   return size;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER GET CLIENT COUNT
   ---------------------------------------------------------------------------*/
int medusa_server_get_client_count(const t_medusa_server * server){
   if(server == NULL)
      return 0;
   return server->client_count;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SEND CONNECTIONLESS (UDP / SCTP2)
   ---------------------------------------------------------------------------*/
static int medusa_server_send_connectionless(
      t_medusa_server * server,
      void * data,
      int data_size){
   if(server->network_config->socket < 0)
      return -1;
   int i;
   int sent = 0;
   t_medusa_list *list = server->clients;
   while(list){
      t_medusa_network_config *client = (t_medusa_network_config *)list->data;
      sent += medusa_net_send_connectionless(
         server->network_config->socket,
         (struct sockaddr *) &client->addr,
         sizeof(client->addr),
         data,
         data_size);
      list = list->next;
   }
   return sent;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SEND (TCP / DCCP / SCTP1)
   ---------------------------------------------------------------------------*/
static int medusa_server_send_connected(
      t_medusa_server * server,
      void * data,
      int data_size){
   if(server->network_config->socket < 0)
      return -1;
   int sent = 0, node_sent = 0;
   t_medusa_list *list = server->clients;
   while(list){
      t_medusa_network_config *client = (t_medusa_network_config *)list->data;
      node_sent = medusa_net_send_connected(client->socket, data, data_size);
      if(errno == EPIPE && node_sent == -1){// client closed the connection
         FD_CLR(client->socket,  &server->read_set);
         medusa_server_client_disconnected(server, client->addr);
      }
      sent += node_sent;
      list = list->next;
   }
   return sent;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SEND BROADCAST
   ---------------------------------------------------------------------------*/
static int medusa_server_send_broadcast(
      t_medusa_server * server,
      void * data,
      int data_size) {
   if(server->network_config->socket < 0)
      return -1;
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port
      = htons(medusa_network_config_get_port(server->network_config));
   addr.sin_addr.s_addr = INADDR_BROADCAST;
   memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
   int sent = medusa_net_send_connectionless(
         server->network_config->socket,
         (struct sockaddr *) &addr,
         sizeof(addr),
         data,
         data_size);
   return sent;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SEND MULTICAST
   ---------------------------------------------------------------------------*/
static int medusa_server_send_multicast(
      t_medusa_server * server,
      void * data,
      int data_size) {
   if(server->network_config->socket < 0)
      return -1;
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port
      = htons(medusa_network_config_get_port(server->network_config));
   addr.sin_addr.s_addr = inet_addr(MEDUSA_MULTICAST_GROUP);
   int sent = medusa_net_send_connectionless(server->network_config->socket,
         (struct sockaddr *) &addr,
         sizeof(addr),
         data,
         data_size);
   return sent;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SET CLIENT CONNECTED CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_server_set_client_connected_callback(
      t_medusa_server * server,
      t_medusa_connection_callback_function client_connected_callback,
      void * args){
   if(server->connection_callback == NULL)
      server->connection_callback =malloc(sizeof(t_medusa_connection_callback));
   server->connection_callback->function = client_connected_callback;
   server->connection_callback->args = args;

   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER SET DATA RECEIVED CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_server_set_data_received_callback(
      t_medusa_server * server,
      t_medusa_nw_data_received_callback_function data_received_callback,
      void * args){
   if(server->data_received_callback == NULL)
      server->data_received_callback
               = malloc(sizeof(t_medusa_data_received_callback));
   server->data_received_callback->function = data_received_callback;
   server->data_received_callback->args = args;

   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER CONNECT
   ---------------------------------------------------------------------------*/
int medusa_server_connect(t_medusa_server * server){

   //Create Socket
   if(server->network_config->socket > 0){
      shutdown(server->network_config->socket, 2);
      close(server->network_config->socket);
   }
   server->network_config->socket
            = medusa_net_create_socket(server->network_config->protocol);
   

   if(server->network_config->socket < 0){
      server->send = NULL;
      return MEDUSA_ERROR_COULD_NOT_CREATE_SOCKET;
   }

   medusa_net_set_nonblocking(server->network_config->socket);

   // Look for a port to bind to
   while (bind(server->network_config->socket,
            (struct sockaddr *) &server->network_config->addr,
            sizeof(server->network_config->addr)) == -1) {
      int port = ntohs(server->network_config->addr.sin_port);
      port++;
      server->network_config->addr.sin_port = htons(port);
   }

   // Listen
   listen(server->network_config->socket, MEDUSA_MAX_CLIENTS);

   switch(server->network_config->protocol){
      case MEDUSA_MULTI:{
         server->poll = medusa_server_poll_connectionless;
         server->send = medusa_server_send_multicast;
         server->receive = medusa_server_receive_connectionless;
         server->select_list = medusa_server_build_select_list_connectionless;
         break;
      }
      case MEDUSA_BROAD:{
         server->poll = medusa_server_poll_connectionless;
         server->send = medusa_server_send_broadcast; 
         server->receive = medusa_server_receive_connectionless;
         server->select_list = medusa_server_build_select_list_connectionless;
         break;
      }
      case MEDUSA_UDP:{
         server->poll = medusa_server_poll_connectionless;
         server->send = medusa_server_send_connectionless;
         server->receive = medusa_server_receive_connectionless;
         server->select_list = medusa_server_build_select_list_connectionless;
         break;
      }
      case MEDUSA_TCP:{
         server->poll = medusa_server_poll_connected;
         server->send = medusa_server_send_connected;
         server->receive = medusa_server_receive_connected;
         server->select_list = medusa_server_build_select_list_connected;
         break;
      }
      case MEDUSA_DCCP:{
         server->poll = medusa_server_poll_connected;
         server->send = medusa_server_send_connected;
         server->receive = medusa_server_receive_connected;
         server->select_list = medusa_server_build_select_list_connected;
         break;
      }
      case MEDUSA_SCTP1:{
         server->poll = medusa_server_poll_connected;
         server->send = medusa_server_send_connected;
         server->receive = medusa_server_receive_connected;
         server->select_list = medusa_server_build_select_list_connected;
         break;
      }
      case MEDUSA_SCTP2:{
         server->poll = medusa_server_poll_connectionless;
         server->send = medusa_server_send_connectionless;
         server->receive = medusa_server_receive_connectionless;
         server->select_list = medusa_server_build_select_list_connectionless;
         break;
      }
   }

   server->status = MEDUSA_CONNECTED;

   //Start receive thread
   pthread_create(&server->server_receive_thread, NULL,
            medusa_server_receive_thread, server);

   struct sched_param param;
   int policy = SCHED_OTHER;
   param.sched_priority = sched_get_priority_max(policy);
   pthread_setschedparam(server->server_receive_thread, policy, &param);

   return medusa_network_config_get_port(server->network_config);
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER ACCEPT
   ---------------------------------------------------------------------------*/
int medusa_server_accept(t_medusa_server * server){
   // Connection protocols runs accept
   struct sockaddr_in cliaddr;
   socklen_t len = sizeof (cliaddr);

   int newsockfd = accept(
            server->network_config->socket,
            (struct sockaddr *) &cliaddr,
            &len);
   // Set flags to the new socket
   medusa_net_set_nonblocking(newsockfd);
   medusa_net_set_sndtimeout(newsockfd);
   if(server->network_config->protocol == MEDUSA_TCP){
      medusa_net_set_tcpnowait(newsockfd);
   }
   if(server->network_config->protocol == MEDUSA_SCTP1){
      medusa_net_set_sctpnowait(newsockfd);
   }
   // Add to list
   (void) medusa_server_client_connected(server, newsockfd, cliaddr);
   // Add to select
   if(newsockfd > server->max_socket)
      server->max_socket = newsockfd;
   FD_SET(newsockfd, &server->read_set);
   return 1;
}



/* -----------------------------------------------------------------------------
   MEDUSA SERVER RECEIVE CONNECTIONLESS (UDP SCTP2)
   ---------------------------------------------------------------------------*/
int medusa_server_receive_connectionless(t_medusa_server * server,
      int socket_fd,
      struct sockaddr_in cliaddr,
      void * data){

   socklen_t len = sizeof (cliaddr);
   int size = recvfrom(socket_fd,
                  data,
                  MEDUSA_RECEIVE_DATA_SIZE,
                  0,
                  (struct sockaddr *) &cliaddr,
                  &len);

   if(((char *)data)[0] == MEDUSA_HELLO){//Client disconnected
      (void) medusa_server_client_connected(
               server,
               socket_fd,
               cliaddr);
      return 0;
   }

   if(((char *)data)[0] == MEDUSA_BYE){//Client disconnected
      (void) medusa_server_client_disconnected(
               server,
               cliaddr);
      return 0;
   }

   if(server->data_received_callback != NULL){
      t_medusa_network_config * client= malloc(sizeof(t_medusa_network_config));
      client->addr = cliaddr;
      client->socket = socket_fd;
      server->data_received_callback->function(
                  client,
                  data,
                  size,
                  server->data_received_callback->args
                  );
      free(client);
      return size;
   }
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER RECEIVE CONNECTED (TCP SCTP1 DCCP)
   ---------------------------------------------------------------------------*/
int medusa_server_receive_connected(
      t_medusa_server * server,
      int socket_fd,
      struct sockaddr_in cliaddr,
      void * data){
   int size;

   size = recv(socket_fd, data, MEDUSA_RECEIVE_DATA_SIZE, 0);

   // Should not receive this messages in this socket
   if(((char *)data)[0] == MEDUSA_HELLO || ((char *)data)[0] == MEDUSA_BYE){
      return 0;
   }

   if(size < 1)
      return 0;

   t_medusa_network_config * client = malloc(sizeof(t_medusa_network_config));
   client->addr = cliaddr;
   client->socket = socket_fd;

   if(server->data_received_callback != NULL)
         server->data_received_callback->function(
         client,
         data,
         size,
         server->data_received_callback->args
         );
   return size;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER RECEIVE
   ---------------------------------------------------------------------------*/
int medusa_server_receive(
      t_medusa_server * server,
      int socket_fd,
      struct sockaddr_in cliaddr,
      void * data
      ){
   if(server->receive){
      return server->receive(server, socket_fd, cliaddr, data);
   }
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER BUILD SELECT LIST
   ---------------------------------------------------------------------------*/
void medusa_server_build_select_list_connected(t_medusa_server * server){

   FD_ZERO(&server->read_set);
   FD_SET(server->network_config->socket, &server->read_set);
   server->max_socket = server->network_config->socket;

   t_medusa_list * list = server->clients;
   while(list){
      t_medusa_network_config * client = (t_medusa_network_config *) list->data;
      if(client->socket > server->max_socket)
      server->max_socket = client->socket;
      FD_SET(client->socket, &server->read_set);
      list = list->next;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER BUILD SELECT LIST
   ---------------------------------------------------------------------------*/
void medusa_server_build_select_list_connectionless(t_medusa_server * server){

   FD_ZERO(&server->read_set);
   FD_SET(server->network_config->socket, &server->read_set);
   server->max_socket = server->network_config->socket;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER RECEIVE THREAD
   ---------------------------------------------------------------------------*/
static void * medusa_server_receive_thread(void * arg){
   t_medusa_server *server = (t_medusa_server *) arg;

   struct timeval receive_timeout;

   medusa_server_build_select_list_connectionless(server);

   int read_socket = 0;
   int size = 0;

   while(server != NULL && server->status == MEDUSA_CONNECTED){

      //reset the select variables
      receive_timeout.tv_sec = 60; //5 minutes timeout
      receive_timeout.tv_usec = 0;
      server->select_list(server);

      // Blocking method
      read_socket = select(server->max_socket + 1,
               &server->read_set,
               NULL,
               NULL,
               &receive_timeout);

      if(read_socket == 0 || server == NULL) //timeout expired
         continue;
      server->poll(server, server->data); // Will depend on the socket type
   }
   pthread_exit(0);
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER POLL CONNECTED
   ---------------------------------------------------------------------------*/
static void medusa_server_poll_connected(t_medusa_server * server, char * data){
   // Server socket
   if (FD_ISSET(server->network_config->socket, &server->read_set)){
         medusa_server_accept(server);
   }
   t_medusa_list * list = server->clients;
   while(list){ // Not a server socket
      t_medusa_network_config * client
               = (t_medusa_network_config *) list->data;
      if(FD_ISSET(client->socket, &server->read_set)){
         // Loopback message
         medusa_server_receive(
               server,
               client->socket,
               client->addr,
               data);
      }
      list = list->next;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER POLL CONNECTIONLESS
   ---------------------------------------------------------------------------*/
static void medusa_server_poll_connectionless(t_medusa_server * server,
            char * data){
   // Server socket
   medusa_server_receive(server,
               server->network_config->socket,
               server->network_config->addr,
               data);
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER CLIENT DISCONNECTED
   ---------------------------------------------------------------------------*/
int medusa_server_client_disconnected(
      t_medusa_server * server,
      struct sockaddr_in cliaddr){
   //Notify Sender

   t_medusa_network_config * client = malloc(sizeof(t_medusa_network_config));
   client->addr = cliaddr;

   if(server->connection_callback)
      server->connection_callback->function(
               client,
               MEDUSA_DISCONNECTED,
               server->connection_callback->args);

   medusa_list_remove_element(&server->clients, client,
         medusa_network_config_compare);
   free(client);
   server->client_count--;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA SERVER CLIENT CONNECTED
   ---------------------------------------------------------------------------*/
int medusa_server_client_connected(
      t_medusa_server * server,
      int socket_fd,
      struct sockaddr_in cliaddr){
   // Notify Sender

   t_medusa_network_config * client = malloc(sizeof(t_medusa_network_config));
   client->socket = socket_fd;
   client->addr = cliaddr;
   medusa_list_add_element(&server->clients, client);

   if(server->connection_callback){
      server->connection_callback->function(
            client,
            MEDUSA_CONNECTED,
            server->connection_callback->args);
   }
   server->client_count++;
   return 1;
}
/*----------------------------------------------------------------------------*/
