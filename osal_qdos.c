/*
 * OS Abstraction Layer for for Snake and on QDOS for the Sinclair QL
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osal_qdos.h"


/*!
 * initialize OS abstraction layer
 *
 * @returns pointer to OSAL handle or NULL in case of error
 */
t_osal_handle* init_osal( void )
{
  t_osal_handle* p;
  WINDOWDEF_t my_windef = { WHITE_M4, 1, COLOR_BACKGROUND, RED_M4, WINDOW_WIDTH + 4, WINDOW_HEIGHT + 2, 2, 1 };

  p = malloc( sizeof( t_osal_handle ) );
  if(! p )
    return NULL;

  memset( p, 0, sizeof( t_osal_handle ) );
  memcpy( & p->windef, & my_windef, sizeof( WINDOWDEF_t ) );
  p->idle_ticks = 5;

  p->ch =  ut_con ( & p->windef );
  if( p->ch < 0 ) {
    fprintf( stderr, "could not open screen error!\n" );
    free( p );
    p = NULL;
  } else {
    p->width = 100;
    p->height = 50;
    p->scale_x = WINDOW_WIDTH / p->width;
    p->scale_y = WINDOW_HEIGHT / p->height;
  }

  return p;
}

void release_osal( t_osal_handle* p )
{
  if( p ) {
    free( p );
  }
}


void osal_exit()
{
  /* nothing to be done on QDOS */
}


/*!
 * starts event loop
 */
void start_event_loop( t_osal_handle* p, t_evt_cb p_evt_cb, void* evt_context )
{
  char byte;

  while( 1 )
  {
    if( io_fbyte( p->ch, 1, & byte ) >= 0 )
      p->key_code = (unsigned short)byte;
    else
      p->key_code = 0;

    if( p_evt_cb( evt_context, p->key_code ) != 0 )
      break;

    mt_susjb( -1, p->idle_ticks, NULL );
  }
}


/*!
 * draws a point on the game's grid
 *
 * @param x x-coordinate
 * @param y y-coordinate
 * @color given color
 */
void draw_point( t_osal_handle* p, int x, int y, int color )
{
  QLRECT_t my_rect;

  my_rect.q_width = p->scale_x;
  my_rect.q_height = p->scale_y;
  my_rect.q_x = x * p->scale_x;
  my_rect.q_y = y * p->scale_y;

  sd_fill( p->ch, 1, color, & my_rect );
}


void write_splash_text( t_osal_handle* p, const char* text )
{
  char* buf;
  QLSTR_t* p_qlstr;
  int len = strlen( text );

  buf = malloc( len + sizeof(short) );
  if( ! buf )
    return;

  p_qlstr = (QLSTR_t*)buf;
  sprintf( p_qlstr->qs_str, text);
  p_qlstr->qs_strlen = len;

  sd_setsz( p->ch, 1, 1, 0 );
  sd_setin( p->ch, 1, COLOR_BLACK );
  ut_mtext( p->ch, p_qlstr );

  free( buf );
}

void clear_screen( t_osal_handle* p )
{
  sd_clear( p->ch, 1 );
}
