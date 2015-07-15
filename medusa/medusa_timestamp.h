#ifndef MEDUSA_TIMESTAMP_H
#define MEDUSA_TIMESTAMP_H

/** @file medusa_timestamp.h
 *
 * @brief A timestamp to represent time in network packets.
 *
 * @ingroup util
 */


#include "medusa.h"

/**
 * @brief A timestamp type.
 * @ingroup util
 */
struct medusa_timestamp{
   uint32_t tv_sec;  ///< seconds
   uint32_t tv_nsec; ///< nanoseconds
};

t_medusa_timestamp medusa_timestamp_diff(
      t_medusa_timestamp start,
      t_medusa_timestamp end
      );

t_medusa_timestamp medusa_timestamp_now(void);

uint64_t medusa_timestamp_int(
      t_medusa_timestamp time
      );

float medusa_timestamp_millis(
      t_medusa_timestamp time
      );

int medusa_timestamp_compare(
      t_medusa_timestamp start,
      t_medusa_timestamp end
      );

#endif /* MEDUSA_TIMESTAMP_H */
