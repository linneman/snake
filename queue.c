/*
 * Queue Data Structure
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "queue.h"

queue_t* queue_init( const int max_elements )
{
  queue_t* p = malloc( sizeof(queue_t) );
  if( p ) {
    p->elements = malloc( max_elements * sizeof(void*) );
    if( p->elements ) {
      p->start = p->end = p->count = 0;
      p->max_elements = max_elements;
    } else {
      free( p );
      p = NULL;
    }
  }

  return p;
}

void queue_release( queue_t* p )
{
  if( p ) {
    free( p->elements );
    free( p );
  }
}

void queue_push( queue_t* p, void* elem )
{
  if( p && p->count < p->max_elements ) {
    if( p->end >= p->max_elements ) {
      p->end=0;
    }

    p->elements[p->end++] = elem;
    ++p->count;
  } else {
    fprintf( stderr, "overflow error in queue push operation!\n" );
  }
}

void* queue_pop( queue_t* p )
{
  void* elem = NULL;

  if( p && p->count ) {
    elem = p->elements[p->start];
    if( ++p->start >= p->max_elements )
      p->start = 0;

    --p->count;
  } else {
    fprintf( stderr, "underflow error in queue push operation!\n" );
  }

  return elem;
}

void* queue_peek( queue_t* p )
{
  void* elem = NULL;

  if( p && p->count ) {
    elem = p->elements[p->start];
  }

  return elem;
}

void* queue_last( queue_t* p )
{
  void* elem = NULL;

  if( p && p->count ) {
    if( p->end )
      elem = p->elements[p->end-1];
    else
      elem = p->elements[p->max_elements - 1];
  }

  return elem;
}

int queue_has_element( queue_t* p, void* e, int (*check_fn)( void* e1, void* e2 ), int* p_idx )
{
  int result = 0; /* it has not */

  int elements_to_check = p->count;
  int idx = p->start;

  while( elements_to_check-- ) {
    if( check_fn( p->elements[idx], e ) ) {
      result = 1; /* found */
      *p_idx = idx;
      break;
    }

    if( ++idx >= p->max_elements )
      idx=0;
  }

  return result;
}

int queue_has_elements( queue_t* p, void* e, int (*check_fn)( void* e1, void* e2 ) )
{
  int found = 0; /* it has not */

  int elements_to_check = p->count;
  int idx = p->start;

  while( elements_to_check-- ) {
    if( check_fn( p->elements[idx], e ) ) {
      ++found; /* found */
    }

    if( ++idx >= p->max_elements )
      idx=0;
  }

  return found;
}

void* queue_get_element_at( queue_t* p, const int idx )
{
  return p->elements[idx];
}

void queue_remove_element( queue_t* p, const int idx )
{
  int count = p->count;
  int i = p->start;
  int n = 0;
  void** new_elements = malloc( p->max_elements * sizeof(void*) );

  if( ! new_elements ) {
    fprintf( stderr, "out of memory error!\n" );
    return;
  }

  while( count-- ) {
    if( i == idx ) {
      ++i;
      --p->count;
      continue;
    }

    new_elements[n++] = p->elements[i++];
    if( i >= p->max_elements )
      i=0;
  }

  free( p->elements );
  p->elements = new_elements;
  p->start = 0;
  p->end = n;
}

void queue_apply( queue_t* p, void (*fn)( void* e ) )
{
  int elements_to_check = p->count;
  int idx = p->start;

  while( elements_to_check-- ) {
    fn( p->elements[idx] );
    if( ++idx >= p->max_elements )
      idx=0;
  }
}
