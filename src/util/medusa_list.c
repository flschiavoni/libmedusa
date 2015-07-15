#include "medusa.h"

// Generic list

/* -----------------------------------------------------------------------------
   MEDUSA LIST CREATE
   ---------------------------------------------------------------------------*/
t_medusa_list * medusa_list_create(void){
   return NULL;
}

/* -----------------------------------------------------------------------------
   MEDUSA LIST FREE
   ---------------------------------------------------------------------------*/
void medusa_list_free(t_medusa_list * list){
   t_medusa_list * temp = list;
   while(temp){
      t_medusa_list * temp2 = temp->next;
      free(temp);
      temp = temp2;
   }
}

/* -----------------------------------------------------------------------------
   MEDUSA LIST ADD ELEMENT
   ---------------------------------------------------------------------------*/
void medusa_list_add_element(t_medusa_list **list, void * data){

   if(*list == NULL){
      *list = malloc(sizeof(t_medusa_list));
      (*list)->data = data;
      (*list)->next = NULL;
      return;
   }

   t_medusa_list * last = *list;
   while(last && last->next){ //Find last element
      last = last->next;
   }
   t_medusa_list * current = malloc(sizeof(t_medusa_list));
   current->data = data;
   current->next = NULL;
   last->next = current;
}

/* -----------------------------------------------------------------------------
   MEDUSA LIST ADD UNIQUE ELEMENT
   ---------------------------------------------------------------------------*/
void medusa_list_add_unique_element(
      t_medusa_list **list,
      void * data,
      medusa_list_compare compare_function
      ){
   if(medusa_list_contains(list, data, compare_function) == 0)
      medusa_list_add_element(list, data);
}

/* -----------------------------------------------------------------------------
   MEDUSA LIST CONTAINS ELEMENT
   ---------------------------------------------------------------------------*/
int medusa_list_contains(
      t_medusa_list **list,
      void * data,
      medusa_list_compare compare_function
      ){

   if(data == NULL || *list == NULL)
      return 0;

   int result = 0;
   t_medusa_list * last = *list;
   while(last){ //Find last element
      if(compare_function(data, last->data) == 1)
         return 1;
      last = last->next;
   }
   return 0;
}

/* -----------------------------------------------------------------------------
   MEDUSA LIST REMOVE ELEMENT
   ---------------------------------------------------------------------------*/
void * medusa_list_remove_element(
      t_medusa_list **list,
      const void * data,
      medusa_list_compare compare_function){

   t_medusa_list *current = NULL, *previous = NULL;

   if(list == NULL)
      return NULL;

   for (current = *list; current != NULL;
               previous = current, current = current->next) {
      // check value
      if(compare_function(current->data, data)){
         if (previous == NULL) { //remove first
           *list = current->next;
         }else{
            previous->next = current->next;
         }
         return current->data;
      }
   }

  return NULL;
}
/*----------------------------------------------------------------------------*/
