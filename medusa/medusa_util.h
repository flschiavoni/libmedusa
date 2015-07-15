#ifndef MEDUSA_UTIL_H
#define MEDUSA_UTIL_H

/** @file medusa_util.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup util
 */

#include "medusa.h"

//User and host name

const char * medusa_util_get_user_name(void);

// Names
int medusa_util_get_protocol(
      const char * protocol_name
      );
      
const char * medusa_util_get_protocol_name(
      int protocol
      );
      
const char * medusa_util_get_bit_depth_name(
      MEDUSA_BIT_DEPTH bit_depth
      );
      
const char * medusa_util_get_endianness_name(
      MEDUSA_ENDIANNESS endianness
      );
      
const char * medusa_util_get_resource_type_name(
      MEDUSA_RESOURCE resource
      );

void medusa_util_format_name(
      char * name
      );

#endif /* MEDUSA_UTIL_H */
