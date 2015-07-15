#include "medusa.h"

// -----------------------------------------------------------------------------
// ------------------------ MEDUSA CLIENT --------------------------------------
// -----------------------------------------------------------------------------

typedef int (*t_medusa_client_receive_function)(
      t_medusa_client * client,
      void * data
      );

typedef int (*t_medusa_client_send_function)(
      t_medusa_client * client,
      void * data,
      int data_size
      );

typedef void * (*t_medusa_client_connect)(
      void * args
      );

struct medusa_client{
   t_medusa_network_config * network_config;
   MEDUSA_STATUS status;
   MEDUSA_STATUS loopback;

   t_medusa_connection_callback     * connection_callback;
   t_medusa_waiting_server_callback * waiting_server_callback;
   t_medusa_data_received_callback  * data_received_callback;

   t_medusa_client_receive_function receive;
   t_medusa_client_send_function send;
   t_medusa_client_connect connect;
   };

void * medusa_client_connect(
      void * args
      );

void * medusa_client_connect_broadcast(
      void * args
      );

void * medusa_client_connect_multicast(
      void * args
      );

static int medusa_client_receive_connectionless(
      t_medusa_client * client,
      void * data
      );

static int medusa_client_receive_connected(
      t_medusa_client * client,
      void * data
      );

static int medusa_client_send_connectionless(
      t_medusa_client * client,
      void * data,
      int data_size
      );

static int medusa_client_send_connected(
      t_medusa_client * client,
      void * data,
      int data_size
      );

static void medusa_client_notify_server(
   t_medusa_client * client
   );

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT CREATE
   ---------------------------------------------------------------------------*/
t_medusa_client * medusa_client_create(){
   t_medusa_client * client = malloc(sizeof(t_medusa_client));
   client->network_config = malloc(sizeof(t_medusa_network_config));

   client->connection_callback = NULL;
   client->waiting_server_callback = NULL;
   client->network_config->socket = -1;
   client->status = MEDUSA_DISCONNECTED;
   client->loopback = MEDUSA_DISCONNECTED;
   client->receive = NULL;
   client->send = NULL;

   return client;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT FREE
   ---------------------------------------------------------------------------*/
int medusa_client_free(t_medusa_client * client){
   t_medusa_message_bye * message = medusa_message_create_bye();
   char * msg = malloc(8);
   size_t size = 0;
   size = medusa_message_pack_bye(message, msg);
   medusa_client_send(client, msg, size);

   client->status = MEDUSA_DISCONNECTED;
   shutdown(client->network_config->socket, 2);
   close(client->network_config->socket);
   client->network_config->socket = -1;
   free(client->network_config);
   free(client);
   free(msg);
   free(message);
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT SET CONFIG
   ---------------------------------------------------------------------------*/
int medusa_client_set_config(
      t_medusa_client * client,
      MEDUSA_PROTOCOL protocol,
      const char * ip,
      int port){

   client->network_config->protocol = protocol;

   struct sockaddr_in si;
   socklen_t len = sizeof (si);
   memset(&si, '\0', len);
   si.sin_family = AF_INET;
   si.sin_port = htons(port);
   si.sin_addr.s_addr = inet_addr(ip);

   client->network_config->addr = si;

   switch(client->network_config->protocol){
      case MEDUSA_MULTI:{
         client->connect = medusa_client_connect_multicast;
         client->receive = medusa_client_receive_connectionless;
         client->send = medusa_client_send_connectionless;
         break;
      }
      case MEDUSA_BROAD:{
         client->connect = medusa_client_connect_broadcast;
         client->receive = medusa_client_receive_connectionless;
         client->send = medusa_client_send_connectionless;
         break;
      }
      case MEDUSA_UDP:{
         client->connect = medusa_client_connect;
         client->receive = medusa_client_receive_connected;
         client->send = medusa_client_send_connected;
         break;
      }
      case MEDUSA_TCP:{
         client->connect = medusa_client_connect;
         client->receive = medusa_client_receive_connected;
         client->send = medusa_client_send_connected;
         break;
      }
      case MEDUSA_DCCP:{
         client->connect = medusa_client_connect;
         client->receive = medusa_client_receive_connected;
         client->send = medusa_client_send_connected;
         break;
      }
      case MEDUSA_SCTP1:{
         client->connect = medusa_client_connect;
         client->receive = medusa_client_receive_connected;
         client->send = medusa_client_send_connected;
         break;
      }
      case MEDUSA_SCTP2:{
         client->connect = medusa_client_connect;
         client->receive = medusa_client_receive_connected;
         client->send = medusa_client_send_connectionless;
         break;
      }
   }

   //Creates the socket
   client->network_config->socket
               = medusa_net_create_socket(client->network_config->protocol);

   if(client->network_config->socket < 0){
      return MEDUSA_ERROR_COULD_NOT_CREATE_SOCKET;
   }

   // Starts a thread to connect to a server when protocol IP port is defined
   pthread_t connection_thread;
   pthread_create(&connection_thread, NULL, client->connect, client);
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT NOTIFY SERVER
   ---------------------------------------------------------------------------*/
static void medusa_client_notify_server(t_medusa_client * client){

   t_medusa_message_hello * message = medusa_message_create_hello();
   char * msg = malloc(8);
   size_t size = 0;
   size = medusa_message_pack_hello(message, msg);
   medusa_client_send(client, msg, size);
   medusa_client_server_connected(client, MEDUSA_CONNECTED);
   free(message);
   free(msg);
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT CONNECT
   ---------------------------------------------------------------------------*/
void * medusa_client_connect(void * args){
   t_medusa_client * client = (t_medusa_client *) args;

   while(connect(client->network_config->socket,
            (struct sockaddr *) &client->network_config->addr,
            sizeof(client->network_config->addr)) == -1) {
      if(client->waiting_server_callback)
         client->waiting_server_callback->function(
                  client->waiting_server_callback->args);
      sleep(1);
   }
   medusa_client_notify_server(client);
   pthread_exit(0);
   return client;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT CONNECT MULTICAST
   ---------------------------------------------------------------------------*/
void * medusa_client_connect_multicast(void * args){
   t_medusa_client * client = (t_medusa_client *) args;

   struct sockaddr_in addr = client->network_config->addr;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   if (bind(client->network_config->socket,
            (struct sockaddr *) &addr,
            sizeof(addr)) == -1) {
      return MEDUSA_ERROR_COULD_NOT_BIND_SOCKET;
   }

   medusa_client_notify_server(client);
   pthread_exit(0);
   return client;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT CONNECT BROADCAST
   ---------------------------------------------------------------------------*/
void * medusa_client_connect_broadcast(void * args){
   t_medusa_client * client = (t_medusa_client *) args;

   struct sockaddr_in addr = client->network_config->addr;
   addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
   if (bind(client->network_config->socket,
            (struct sockaddr *) &addr,
            sizeof(addr)) == -1){
      return MEDUSA_ERROR_COULD_NOT_BIND_SOCKET;
   }

   medusa_client_notify_server(client);
   pthread_exit(0);
   return client;
}


/* ----------------------------------------------------------------------------
   MEDUSA CLIENT RECEIVE FUNCTION
   --------------------------------------------------------------------------*/
int medusa_client_receive(t_medusa_client * client, void * data){
   int data_size = client->receive(client, data);
   if(client && client->loopback == MEDUSA_CONNECTED)
      medusa_client_send(client, data, data_size);
   return data_size;
}

/* ----------------------------------------------------------------------------
   MEDUSA CLIENT CONNECTED RECEIVE FUNCTION (TCP / UDP / DCCP / SCTP1 / SCTP2)
   --------------------------------------------------------------------------*/
int medusa_client_receive_connected(t_medusa_client * client, void * data) {
   if(client->network_config->socket < 0
      || client->status == MEDUSA_DISCONNECTED)
      return -1;
   int received_size = recv(client->network_config->socket,
            data, MEDUSA_RECEIVE_DATA_SIZE, MSG_NOSIGNAL);
   if(received_size == 0 || (received_size == -1 && errno == ENOTCONN)){
      medusa_client_server_connected(client, MEDUSA_DISCONNECTED);
      return 0;
   }
   return received_size;
}

/* ----------------------------------------------------------------------------
   MEDUSA CLIENT RECEIVE CONNECTIONLESS FUNCTION (MULTICAST / BROADCAST)
   --------------------------------------------------------------------------*/
int medusa_client_receive_connectionless(t_medusa_client * client, void * data){
   if(client->network_config->socket < 0
      || client->status == MEDUSA_DISCONNECTED)
      return -1;
   int received_size =
      recvfrom(client->network_config->socket,
               data, MEDUSA_RECEIVE_DATA_SIZE , 0, NULL, NULL);
   if(((char *)data)[0] == MEDUSA_BYE)
      medusa_client_server_connected(client, MEDUSA_DISCONNECTED);
   return received_size;
}

/* ----------------------------------------------------------------------------
   MEDUSA CLIENT SEND TO
   --------------------------------------------------------------------------*/
int medusa_client_send_connectionless(
      t_medusa_client * client,
      void * data,
      int data_size
      ){
   int size = 0;
   size = medusa_net_send_connectionless(
               client->network_config->socket,
               (struct sockaddr *) &client->network_config->addr,
               sizeof(client->network_config->addr),
               data,
               data_size);
   return size;
}

/* ----------------------------------------------------------------------------
   MEDUSA CLIENT SEND CONNECTED
   --------------------------------------------------------------------------*/
int medusa_client_send_connected(
      t_medusa_client * client,
      void * data,
      int data_size
      ){
   int size = 0;
   size = medusa_net_send_connected(
            client->network_config->socket,
            data,
            data_size);
   return size;
}

/* ----------------------------------------------------------------------------
   MEDUSA CLIENT SEND LOOPBACK FUNCTION
   --------------------------------------------------------------------------*/

int medusa_client_send(t_medusa_client * client, void * data, int data_size){
   if(client->send){
      return client->send(client, data, data_size);
   }else{
      return 0;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT GET NETWORK CONFIG
   ---------------------------------------------------------------------------*/
const t_medusa_network_config * medusa_client_get_network_config(
      const t_medusa_client * client){
   if(client == NULL)
      return NULL;
   return client->network_config;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT GET PROTOCOL
   ---------------------------------------------------------------------------*/
MEDUSA_PROTOCOL medusa_client_get_protocol(const t_medusa_client * client){
   return client->network_config->protocol;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT SET CONNECTION CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_client_set_connection_callback(
      t_medusa_client * client,
      t_medusa_connection_callback_function connection_callback,
      void * args){
   if(client->connection_callback == NULL)
      client->connection_callback= malloc(sizeof(t_medusa_connection_callback));
   client->connection_callback->function = connection_callback;
   client->connection_callback->args = args;
   // callback the last status
   client->connection_callback->function(
         client->network_config,
         client->status,
         client->connection_callback->args);
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT SET WAITING SERVER CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_client_set_waiting_server_callback(
      t_medusa_client * client,
      t_medusa_waiting_server_callback_function waiting_server_callback,
      void * args){
   if(client->waiting_server_callback == NULL)
      client->waiting_server_callback = malloc(sizeof(t_medusa_waiting_server_callback));
   client->waiting_server_callback->function = waiting_server_callback;
   client->waiting_server_callback->args = args;
   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT SET DATA RECEIVED CALLBACK
   ---------------------------------------------------------------------------*/
int medusa_client_set_data_received_callback(
      t_medusa_client * client,
      t_medusa_data_received_callback_function data_received_callback,
      void * args){

   client->data_received_callback->function = data_received_callback;
   client->data_received_callback->args = args;

   return 1;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT GET STATUS
   ---------------------------------------------------------------------------*/
MEDUSA_STATUS medusa_client_get_status(const t_medusa_client * client){
   if (client == NULL)
      return MEDUSA_DISCONNECTED;
   if (client->status == MEDUSA_DISCONNECTED)
      return MEDUSA_DISCONNECTED;
   if (client->receive == NULL )
      return MEDUSA_DISCONNECTED;
   return MEDUSA_CONNECTED;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT SET LOOPBACK STATUS
   ---------------------------------------------------------------------------*/
void medusa_client_set_looopback_status(
      t_medusa_client * client,
      MEDUSA_STATUS status){
   client->loopback = status;
}

/* -----------------------------------------------------------------------------
   MEDUSA CLIENT SERVER CONNECTED / DISCONNECTED
   ---------------------------------------------------------------------------*/
int medusa_client_server_connected(t_medusa_client * client, int status){
   //Change local status
   client->status = status;
   //Notify receiver
   if(client->connection_callback)
      client->connection_callback->function(
         client->network_config,
         status,
         client->connection_callback->args);
   return 1;
}
/*----------------------------------------------------------------------------*/
