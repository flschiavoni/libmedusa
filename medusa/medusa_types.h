#ifndef MEDUSA_TYPES_H
#define MEDUSA_TYPES_H

/** \file medusa_types.h
 *
 * \brief
 * (other Doxygen tags)
 *
 * \ingroup main
 */

#define MEDUSA_LOCAL_RESOURCES 128

#define MEDUSA_MAX_CLIENTS 128
#define MEDUSA_PI 3.141592653

#define MEDUSA_MULTICAST_GROUP "225.0.0.37" //from 224.0.0.0 to 239.255.255.255

#define MEDUSA_CONTROL_PORT 30001
#define MEDUSA_DEFAULT_PORT 40001
#define MEDUSA_RING_BUFFER_SIZE 131072

#define MEDUSA_RECEIVE_DATA_SIZE 65536

#define MEDUSA_SERVER_LOOP 30000 //# of try to send

#define MEDUSA_VERSION 1

/**
 * @brief A generic linked list.
 * @ingroup util
 */
typedef struct medusa_list t_medusa_list;
/**
 * @brief A generic timestamp type.
 * @ingroup util
 */
typedef struct medusa_timestamp t_medusa_timestamp;

/**
 * @brief An opaque type to describe the network config.
 * @ingroup net
 */
typedef struct medusa_network_config t_medusa_network_config;
/**
 * @brief An opaque type to describe the network server.
 * @ingroup net
 */
typedef struct medusa_server t_medusa_server;
/**
 * @brief An opaque type to describe the network client.
 * @ingroup net
 */
typedef struct medusa_client t_medusa_client;

/**
 * @brief An opaque type to describe the sender.
 * @ingroup control
 */
typedef struct medusa_sender t_medusa_sender;
/**
 * @brief An opaque type to describe the receiver.
 * @ingroup control
 */
typedef struct medusa_receiver t_medusa_receiver;
/**
 * @brief An opaque type to describe the control.
 * @ingroup control
 */
typedef struct medusa_control t_medusa_control;

/**
 * @brief A type to describe the midi resource in sender and receiver.
 * @ingroup control
 */
typedef struct medusa_midi_resource t_medusa_midi_resource;
/**
 * @brief A type to describe the audio resource in sender and receiver.
 * @ingroup control
 */
typedef struct medusa_audio_resource t_medusa_audio_resource;

/**
 * @brief A type to describe a node to the control.
 * @ingroup control
 */
typedef struct medusa_node t_medusa_node;
/**
 * @brief A type to describe a node audio resource to the control.
 * @ingroup control
 */
typedef struct medusa_node_audio_resource t_medusa_node_audio_resource;
/**
 * @brief A type to describe a node midi resource to the control.
 * @ingroup control
 */
typedef struct medusa_node_midi_resource t_medusa_node_midi_resource;
/**
 * @brief A type to describe a node connection to the control.
 * @ingroup control
 */
typedef struct medusa_node_connection t_medusa_node_connection;

typedef struct medusa_data_received_callback t_medusa_data_received_callback;

typedef struct medusa_nw_data_received_callback t_medusa_nw_data_received_callback;

typedef struct medusa_waiting_server_callback t_medusa_waiting_server_callback;

typedef struct medusa_audio_config_callback t_medusa_audio_config_callback;

typedef struct medusa_data_lost_callback t_medusa_data_lost_callback;

typedef struct medusa_data_sent_callback t_medusa_data_sent_callback;

typedef struct medusa_connection_callback t_medusa_connection_callback;

typedef struct medusa_meter_callback t_medusa_meter_callback;

typedef struct medusa_free_callback t_medusa_free_callback;

typedef struct medusa_stop_callback t_medusa_stop_callback;

typedef struct medusa_prepare_data_callback t_medusa_prepare_data_callback;

typedef struct medusa_chat_callback t_medusa_chat_callback;

typedef struct medusa_remove_node_midi_resource_callback
      t_medusa_remove_node_midi_resource_callback;

typedef struct medusa_remove_node_audio_resource_callback
      t_medusa_remove_node_audio_resource_callback;

typedef struct medusa_add_node_midi_resource_callback
      t_medusa_add_node_midi_resource_callback;

typedef struct medusa_add_node_audio_resource_callback
      t_medusa_add_node_audio_resource_callback;

typedef struct medusa_remove_node_callback t_medusa_remove_node_callback;

typedef struct medusa_add_node_callback t_medusa_add_node_callback;

typedef struct medusa_add_sender_callback t_medusa_add_sender_callback;

typedef struct medusa_add_receiver_callback t_medusa_add_receiver_callback;

typedef struct medusa_remove_sender_callback t_medusa_remove_sender_callback;

typedef struct medusa_remove_receiver_callback
      t_medusa_remove_receiver_callback;

/**
 * \enum MEDUSA_PROTOCOL
 * \brief Specifies the available network communications protocols and
 * addressing methods like Broadcast and Multicast.
 */
typedef enum {
   MEDUSA_NONE   = 0,   ///< Undefined Protocol
   MEDUSA_UDP,          ///< UDP Protocol
   MEDUSA_TCP,          ///< TCP Protocol
   MEDUSA_DCCP,         ///< DCCP Protocol
   MEDUSA_SCTP1,        ///< SCTP Protocol using SOCK_STREAM
   MEDUSA_SCTP2,        ///< SCTP Protocol using SOCK_SEQPACKET
   MEDUSA_BROAD,        ///< Broadcast communication with UDP protocol
   MEDUSA_MULTI,        ///< Multicast communication with UDP protocol
}MEDUSA_PROTOCOL;

/**
 * \enum MEDUSA_CODEC
 * \brief Specifies the available CODECs do audio compression.
 */
typedef enum {
   MEDUSA_NOCODEC = 0,  ///< No CODEC is defined
   MEDUSA_OPUS,         ///< OPUS / CELT codec
}MEDUSA_CODEC;

/**
 * \enum MEDUSA_ENDIANNESS
 * \brief Specifies the endianness types.
 */
typedef enum {
   MEDUSA_NO_ENDIAN     = 0,  ///< Undefined endianness
   MEDUSA_LITTLE_ENDIAN = 1,  ///< Little endian
   MEDUSA_BIG_ENDIAN    = 2   ///< Big endian
}MEDUSA_ENDIANNESS;

/**
 * \enum MEDUSA_BIT_DEPTH
 * \brief Specifies the bit depth audio resolution.
 */
typedef enum {
   MEDUSA_NO_BITS    = 0,  ///< Undefined bit depth
   MEDUSA_8_BITS     = 1,  ///< 8 bits
   MEDUSA_16_BITS    = 2,  ///< 16 bits
   MEDUSA_24_BITS    = 3,  ///< 24 bits
   MEDUSA_32_BITS    = 4   ///< 32 bits
} MEDUSA_BIT_DEPTH;

/**
 * \enum MEDUSA_BIT_STATUS
 * \brief Specifies the server status. Used also to loopback status.
 */
typedef enum {
   MEDUSA_CONNECTED     = 0,  ///< Connected
   MEDUSA_DISCONNECTED  = 1,  ///< Disconnected
}MEDUSA_STATUS;

/**
 * \enum MEDUSA_MESSAGE
 * \brief Specifies the system messages.
 */
typedef enum {
   MEDUSA_HELLO = 0, ///< Hello message. Used by server / client.
   MEDUSA_BYE,       ///< Bye message. Used by server / client.

   MEDUSA_DATA,            ///< Data message. Used by sender / receiver.
   MEDUSA_LOOPBACK_DATA,   ///< Loopback data message. Used by sender / receiver
   MEDUSA_AUDIO_CONFIG,    ///< Audio configuration. Used by sender / receiver

   MEDUSA_ADD_NODE,              ///< Add node message. Used by control.
   MEDUSA_REMOVE_NODE,           ///< Remove node message. Used by control.
   MEDUSA_NETWORK_CONFIG,        ///< Set node network message. Used by control.

   MEDUSA_ADD_AUDIO_RESOURCE,    ///< Add node audio message. Used by control.
   MEDUSA_REM_AUDIO_RESOURCE,       ///< Remove resource. Used by control.

   MEDUSA_ADD_MIDI_RESOURCE,     ///< Add node midi message. Used by control.
   MEDUSA_REM_MIDI_RESOURCE,     ///< Add node midi message. Used by control.

   MEDUSA_ADD_CHAT_RESOURCE,     ///< Add node chat message. Used by control.
   MEDUSA_REM_CHAT_RESOURCE,     ///< Add node chat message. Used by control.

}MEDUSA_MESSAGE;

/**
 * \enum MEDUSA_DATA_TYPE
 * \brief Specifies the message type in case of it being a MEDUSA_DATA
 */
typedef enum {
   MEDUSA_AUDIO = 0,    ///< Audio data type.
   MEDUSA_MIDI,         ///< Midi data type.
   MEDUSA_CHAT,         ///< Chat data type.
} MEDUSA_DATA_TYPE;

/**
 * \enum MEDUSA_RESOURCE
 * \brief Specifies the resource type
 */
typedef enum {
   MEDUSA_SENDER     = 0,///< Sender
   MEDUSA_RECEIVER   = 1,///< Receiver
} MEDUSA_RESOURCE;

typedef enum{
   MEDUSA_UNMUTED = 0,
   MEDUSA_MUTED   = 1,
} MEDUSA_MUTE;

typedef enum {
   MEDUSA_ERROR_COULD_NOT_CREATE_SOCKET    = -1,  ///< 
   MEDUSA_ERROR_COULD_NOT_BIND_SOCKET      = -2,
   MEDUSA_ERROR_COULD_NOT_SET_IP_MEMBERSHIP= -3,
   MEDUSA_ERROR_COULD_NOT_SET_IP_MULTICAST_LOOP = -4,
   MEDUSA_ERROR_COULD_NOT_SET_SO_REUSEADDR = -5,
   MEDUSA_ERROR_COULD_NOT_SET_SO_BROADCAST = -6,
   MEDUSA_ERROR_COULD_NOT_SET_SO_PRIORITY  = -7,
   MEDUSA_ERROR_COULD_NOT_SET_SO_SNDTIMEO  = -8,
   MEDUSA_ERROR_COULD_NOT_SET_TCP_NODELAY  = -9,
   MEDUSA_ERROR_COULD_NOT_SET_SCTP_NODELAY = -10,
   MEDUSA_ERROR_COULD_NOT_SET_NONBLOCKING  = -11,
} MEDUSA_NET_ERROR;

#endif /* MEDUSA_TYPES_H */
