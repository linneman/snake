#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

void o_print( char* p_o )
{
  printf( "->%s\n", p_o );
}


void test_queue()
{
  queue_t* my_queue = queue_init( 100 );
  int i;
  char* o;

  for( i=0; i<3; ++i ) {
    o = malloc( 10 );
    sprintf( o, "%d", i );
    queue_push( my_queue, o );
  }

  queue_apply( my_queue, o_print );
  printf("\n\n");

  for( i=3; i<200; ++i ) {
    o = queue_pop( my_queue );
    sprintf( o, "%d", i );
    queue_push( my_queue, o );
  }

  queue_apply( my_queue, o_print );
}
