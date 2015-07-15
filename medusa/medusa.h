#ifndef MEDUSA_H
#define MEDUSA_H

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <pwd.h> // to user name

#include <netinet/in.h>
#include <netinet/sctp.h>

#include <linux/tcp.h>
#include <linux/dccp.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/unistd.h> // for host name

#include <arpa/inet.h>

/** @file medusa.h
 *
 * @brief This is the libmedusa main include file.
 * @details This is the libmedusa main include file. This file includes all medusa
 * library headers.
 * @ingroup main
 */

#include "medusa_types.h"
#include "medusa_util.h"
#include "medusa_list.h"
#include "medusa_timestamp.h"
#include "medusa_control_messages.h"

#include "medusa_audio_messages.h"
#include "medusa_audio_resource.h"

#include "medusa_midi_messages.h"
#include "medusa_midi_resource.h"

#include "medusa_chat_messages.h"

#include "medusa_callback.h"
#include "medusa_net.h"
#include "medusa_network_config.h"
#include "medusa_server.h"
#include "medusa_client.h"
#include "medusa_dsp.h"
#include "medusa_ringbuffer.h"
#include "medusa_pack.h"
#include "medusa_node.h"
#include "medusa_control.h"
#include "medusa_sender.h"
#include "medusa_receiver.h"


#endif /* MEDUSA_H */
