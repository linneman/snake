/*
 * OS Abstraction Layer for for Snake and on QDOS for the Sinclair QL
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "osal_curses.h"


/*!
 * initialize OS abstraction layer
 *
 * @returns pointer to OSAL handle or NULL in case of error
 */
t_osal_handle* init_osal( void )
{
  t_osal_handle* p;

  p = malloc( sizeof( t_osal_handle ) );
  if(! p )
    return NULL;

  memset( p, 0, sizeof( t_osal_handle ) );

  initscr();			/* Start curses mode 		*/
  start_color();
  raw();				  /* Line buffering disabled	*/
  keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
  noecho();			/* Don't echo() while we do getch */
  curs_set(0);
  init_pair(1, COLOR_BLACK, COLOR_BACKGROUND );
  init_pair(2, COLOR_WHITE, COLOR_BACKGROUND );
  init_pair(3, COLOR_RED, COLOR_BACKGROUND );
  init_pair(4, COLOR_BACKGROUND, COLOR_BLACK );

  p->idle_ticks = 200; /* sleep in milliseconds */
  p->width = WINDOW_WIDTH;
  p->height = WINDOW_HEIGHT;
  p->scale_x = 1;
  p->scale_y = 1;

  p->win = newwin( WINDOW_HEIGHT + 2, WINDOW_WIDTH + 2, 0, 0 );
  if( ! p->win )
  {
    fprintf( stderr, "could not create main window error!\n" );
    release_osal( p );
    p = NULL;
  }
  else if( getmaxx(stdscr) < 2 + p->width || getmaxy(stdscr) < 2 + p->height )
  {
    printw( "Your screen is not big enough! You need at least %d rows and %d colums,\n",
            p->height, p->width );
    printw( "but your currently used terminatl has only %d rows and %d columns!\n\n",
            getmaxy(stdscr), getmaxx(stdscr) );
    printw( "Press any key to continue, resize your terminal window and restart the application!\n" );
    refresh();
    getch();

    release_osal(p);
    p = NULL;
  }
  else {
    timeout( p->idle_ticks );

    box( p->win, '*', '*' );
    touchwin( p->win );
    wbkgd( p->win, COLOR_PAIR(1) );
    wrefresh( p->win );
  }

  return p;
}

void release_osal( t_osal_handle* p )
{
  if( p ) {
    if( p->win )
      delwin( p->win );
    free( p );
  }
}

void osal_exit()
{
  endwin();			/* End curses mode */
}


/*!
 * starts event loop
 */
void start_event_loop( t_osal_handle* p, t_evt_cb p_evt_cb, void* evt_context )
{
  short key;

  while( 1 )
  {
    key = (short)getch();
    if( key == -1 )
      p->key_code = 0;
    else
      p->key_code = key;

    if( p_evt_cb( evt_context, p->key_code ) != 0 )
      break;

    wrefresh( p->win );
  }
}


/*!
 * draws a point on the game's grid
 *
 * @param x x-coordinate
 * @param y y-coordinate
 * @color given color
 */
void draw_point( t_osal_handle* p, const int x, const int y, const int color )
{
  switch( color ) {
  case COLOR_BLACK:
    wattron( p->win, COLOR_PAIR(1) | A_REVERSE );
    mvwaddch( p->win, 1+y ,1+x, ' ' );
    wattroff( p->win, COLOR_PAIR(1) );
    break;
  case COLOR_WHITE:
    wattron( p->win, COLOR_PAIR(2) | A_REVERSE );
    mvwaddch( p->win, 1+y, 1+x, ' ' );
    wattroff( p->win, COLOR_PAIR(2) );
    break;
  case COLOR_RED:
    wattron( p->win, COLOR_PAIR(3) | A_REVERSE );
    mvwaddch( p->win, 1+y, 1+x, ' ' );
    wattroff( p->win, COLOR_PAIR(3) );
    break;
  case COLOR_BACKGROUND:
    wattron( p->win, COLOR_PAIR(4) | A_REVERSE );
    mvwaddch( p->win, 1+y, 1+x, ' ' );
    wattroff( p->win, COLOR_PAIR(4) );
    break;
  }
}

void write_splash_text( t_osal_handle* p, const char* text )
{
  char buf[2048];
  char delimiter[] = "\n";
  char *p_buf;
  int line = 1;

  strncpy( buf, text, sizeof(buf) );
  p_buf = strtok( buf, delimiter );

  while( p_buf != NULL ) {
    wbkgd( p->win, COLOR_PAIR(1) );
    wattron( p->win, COLOR_PAIR(1) );
    mvwprintw( p->win, line++, 1, p_buf );
    p_buf = strtok( NULL, delimiter );
  }

  wrefresh( p->win );
}

void clear_screen( t_osal_handle* p )
{
  wclear( p->win );
  box( p->win, '*', '*' );
  touchwin( p->win );
  wbkgd( p->win, COLOR_PAIR(1) );
  wrefresh( p->win );
}
