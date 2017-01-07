/*
 * OS Abstraction Layer for for Snake and on QDOS for the Sinclair QL
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#ifndef OSAL_CURSES_H
#define OSAL_CURSES_H

#include <ncurses.h>

/*! keycodes for the arrow keys */
#define KEY_Q             0x71
#define KEY_UP_ARROW      0x0103
#define KEY_DOWN_ARROW    0x0102
#define KEY_LEFT_ARROW    0x0104
#define KEY_RIGHT_ARROW   0x0105

/*! color definitions */
#define COLOR_BACKGROUND  COLOR_GREEN

/*! game window in pixels */
#define WINDOW_WIDTH      100
#define WINDOW_HEIGHT     50


/*! OSAL handel to abstract QL specific definitions */
typedef struct {
  unsigned short    key_code;
  int               idle_ticks;
  int               width;
  int               height;
  int               scale_x;
  int               scale_y;
  WINDOW*           win;
} t_osal_handle;


/*! event loop callback handler which is invoked every miliseconds
    when it retuns a non-zero value, the event loop is terminated */
typedef int (*t_evt_cb)( void* evt_context, unsigned short key_code );


/*!
 * initialize OS abstraction layer
 *
 * @returns pointer to OSAL handle or NULL in case of error
 */
t_osal_handle* init_osal( void );


/*!
 * release of OS abstraction layer
 *
 * @param pointer to OSAL handle
 */
void release_osal( t_osal_handle* p );


/*!
 * call before quit application
 */
void osal_exit();


/*!
 * starts event loop
 */
void start_event_loop( t_osal_handle* p, t_evt_cb p_evt_cb, void* evt_context );


/*!
 * draws a point on the game's grid
 *
 * @param x x-coordinate
 * @param y y-coordinate
 * @color given color
 */
void draw_point( t_osal_handle* p, int x, int y, int color );


/*!
 * writes given text to console window
 */
void write_splash_text( t_osal_handle* p, const char* text );


/*!
 * clears the game screen
 */
void clear_screen( t_osal_handle* p );


#endif /* #ifndef OSAL_CURSES_H */
