#include "medusa.h"

/** @file medusa_midi_resource.c
 *
 * @brief The implementation of a midi resource.
 * 
 *
 * @ingroup control
 */

/* -----------------------------------------------------------------------------
   MEDUSA MIDI RESOURCE CREATE
   ---------------------------------------------------------------------------*/
t_medusa_midi_resource * medusa_midi_resource_create(){
   t_medusa_midi_resource * midi_resource
            = malloc(sizeof(t_medusa_midi_resource));
   midi_resource->channels = 0;
   return midi_resource;
}

/* -----------------------------------------------------------------------------
   MEDUSA MIDI RESOURCE SET LOCAL
   ---------------------------------------------------------------------------*/
void medusa_midi_resource_set(
      t_medusa_midi_resource * midi_resource,
      uint16_t channels){
   midi_resource->channels = channels;
}

/* -----------------------------------------------------------------------------
   MEDUSA MIDI RESOURCE CHECK CONFIG
   ---------------------------------------------------------------------------*/
void medusa_midi_resource_check_config(
         t_medusa_midi_resource * local_midi_resource,
         t_medusa_midi_resource * network_midi_resource){

   if(local_midi_resource == NULL)
      return;

   if(network_midi_resource->channels < 1)
      network_midi_resource->channels = local_midi_resource->channels;

}
/*----------------------------------------------------------------------------*/
