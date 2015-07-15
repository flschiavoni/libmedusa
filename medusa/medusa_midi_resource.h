#ifndef MEDUSA_MIDI_RESOURCE_H
#define MEDUSA_MIDI_RESOURCE_H

#include "medusa.h"

/** @file medusa_midi_resource.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup control
 */


struct medusa_midi_resource {
      uint16_t channels;
      };

t_medusa_midi_resource * medusa_midi_resource_create(void);

void medusa_midi_resource_set(
      t_medusa_midi_resource * midi_resource,
      uint16_t channels
      );

void medusa_midi_resource_check_config(
      t_medusa_midi_resource * local_midi_resource,
      t_medusa_midi_resource * network_midi_resource
      );


#endif /* MEDUSA_MIDI_RESOURCE_H */
