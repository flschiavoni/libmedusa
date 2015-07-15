#ifndef MEDUSA_CHAT_MESSAGES_H
#define MEDUSA_CHAT_MESSAGES_H

#include "medusa.h"

/** @file medusa_messages.h
 *
 * @brief
 * This module defines the Medusa network messages.
 *
 * @ingroup control
 */

typedef struct medusa_message_chat
         t_medusa_message_chat;

struct medusa_message_chat{
   uint8_t type;   ///< Message type
   uint8_t version;  ///< Medusa version
   uint16_t size;  ///< Message size
   unsigned char pad1[4];   ///< Padding to align bytes

   uint64_t node_uid;

   uint8_t data_size;
   unsigned char pad2[3];   ///< Padding to align bytes
   unsigned char pad3[4];   ///< Padding to align bytes
   char data[];
};
_Static_assert(sizeof(t_medusa_message_chat) == 24,
      "ERROR: medusa_message_chat size");

t_medusa_message_chat * medusa_message_create_chat(
            uint64_t node_uid,
            char * data
            );

size_t medusa_message_pack_chat(
            t_medusa_message_chat * message,
            void * msg
            );

t_medusa_message_chat * medusa_message_unpack_chat(
            void* msg,
            size_t size
            );

typedef void (*t_medusa_chat_callback_function)(
      t_medusa_node * node,
      const t_medusa_message_chat * message,
      void * args
      );


struct medusa_chat_callback{
   t_medusa_chat_callback_function function;
   void * args;
   };

void medusa_control_notify_chat(
      t_medusa_control * control,
      t_medusa_node * node,
      char * data
      );

void medusa_control_set_chat_callback(
      t_medusa_control * control,
      t_medusa_chat_callback_function function,
      void * args
      );


#endif /* MEDUSA_CHAT_MESSAGES_H */
