#ifndef MEDUSA_LIST_H
#define MEDUSA_LIST_H

/** @file medusa_list.h
 *
 * @brief
 * (other Doxygen tags)
 *
 * @ingroup util
 */

#include "medusa.h"

// Generic list
struct medusa_list{
   void *data;          ///< The data stored in the node
   t_medusa_list *next; ///< Next node in list
   };

typedef int (*medusa_list_compare)(
   const void * data1,
   const void * data2
   );

t_medusa_list * medusa_list_create(void);

void medusa_list_add_element(
      t_medusa_list **list,
      void * data
      );

int medusa_list_contains(
      t_medusa_list **list,
      void * data,
      medusa_list_compare compare_function
      );

void medusa_list_add_unique_element(
      t_medusa_list **list,
      void * data,
      medusa_list_compare compare_function
      );

void * medusa_list_remove_element(t_medusa_list **list,
      const void * data,
      medusa_list_compare compare_function
      );

void medusa_list_free(
      t_medusa_list * list
      );

#endif /* MEDUSA_LIST_H */
