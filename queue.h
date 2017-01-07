/*
 * Queue Data Structure
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
  void** elements;
  int max_elements;
  int start;
  int end;
  int count;
} queue_t;

typedef int (*queue_check_fn_t)( void* e1, void* e2 );
typedef void (*queue_apply_fn_t)( void* e1 );

queue_t* queue_init( const int max_elements );
void queue_release( queue_t* p );
void queue_push( queue_t* p, void* elem );
void* queue_pop( queue_t* p );
void* queue_peek( queue_t* p );
void* queue_last( queue_t* p );
int queue_has_element( queue_t* p, void* e, int (*check_fn)( void* e1, void* e2 ), int* p_idx );
int queue_has_elements( queue_t* p, void* e, int (*check_fn)( void* e1, void* e2 ) );
void* queue_get_element_at( queue_t* p, const int idx );
void queue_remove_element( queue_t* p, const int idx );
void queue_apply( queue_t* p, void (*fn)( void* e1 ) );

#endif /* #ifndef QUEUE_H */
