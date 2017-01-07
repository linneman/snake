/*
 * OS Abstraction Layer for for Snake and on QDOS for the Sinclair QL
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "osal.h"
#include "queue.h"
#include "log.h"

const char start_screen[] =
  "\n\n"
  "         Snake for the Sinclair QL and Unix Machines\n \n "
  "                 (C) 2016 by Otto Linnemann\n \n "
  "              Licensed under GNU GPL v2 or later\n \n \n \n "
  "       Use arrow keys <,>,^,v to naviate the white snake!\n \n "
  "       Press any key  to start game or Q to terminate ...\n ";

#define MAX_SNAKE_LEN         1000
#define MAX_FOOD_LOCATIONS    10


typedef struct { int x; int y; } coord_t;

coord_t* init_coord( const int x, const int y )
{
  coord_t* p = malloc( sizeof(coord_t) );
  if( p ) {
    p->x = x;
    p->y = y;
  }

  return p;
}

void release_coord( coord_t* p )
{
  if( p ) {
    free( p );
  }
}


typedef struct {
  queue_t* elements;
  short    color;
  int      move_x;
  int      move_y;
  int      growing;
} snake_t;


snake_t* init_snake(
  t_osal_handle* p_osal,
  const int max_length,
  const short color,
  const int start_x,
  const int start_y,
  const int imove_x,
  const int imove_y )
{
  snake_t* p = malloc( sizeof(snake_t) );
  if( p ) {
    memset( p, 0, sizeof(snake_t) );
    p->elements = queue_init( max_length );
    if( p->elements ) {
      p->color = color;
      p->move_x = imove_x;
      p->move_y = imove_y;

      queue_push( p->elements, init_coord( start_x, start_y ) );
      /* draw_point( p_osal, start_x, start_y, color ); */
    } else {
      free( p );
      p = NULL;
    }
  }

  return p;
}

void release_snake( snake_t* p )
{
  if( p ) {
    queue_apply( p->elements, (queue_apply_fn_t)release_coord );
    queue_release( p->elements );
    free( p );
  }
}

void update_snake( t_osal_handle* p_osal, snake_t* p )
{
  coord_t* nc;
  coord_t* last = queue_last( p->elements );
  int nx, ny;

  nx = last->x + p->move_x;
  ny = last->y + p->move_y;

  if( p->growing ) {
    nc = init_coord( nx, ny );
    p->growing = 0;
  } else {
    nc = queue_pop( p->elements );
    draw_point( p_osal, nc->x, nc->y, COLOR_BACKGROUND );
    nc->x = nx; nc->y = ny;
  }
  queue_push( p->elements, nc  );
  draw_point( p_osal, nx, ny, p->color );
}

typedef enum {
  state_start_screen,
  state_game,
  state_game_over_screen,
  state_new_game,
  state_terminate
} gamestate_t;

typedef struct {
  t_osal_handle*  p_osal;
  snake_t*        human_snake;
  snake_t*        computer_snake;
  queue_t*        food;
  long            iteration;
  int             terminate;
  gamestate_t     state;
} game_t;


game_t* init_game()
{
  game_t* p = malloc( sizeof(game_t) );
  if( p ) {
    memset( p, 0, sizeof(game_t) );

    p->p_osal  = init_osal();
    if( ! p->p_osal ) {
      fprintf( stderr, "could not initialize operating system abstraction layer error!\n" );
      return NULL;
    }

    p->human_snake = init_snake( p->p_osal, MAX_SNAKE_LEN, COLOR_WHITE, 30, 30, 1, 0 );
    if( ! p->human_snake ) {
      release_osal( p->p_osal );
      free( p );
      return NULL;
    }

    p->computer_snake = init_snake( p->p_osal, MAX_SNAKE_LEN, COLOR_BLACK, 70, 30, -1, 0 );
    if( ! p->computer_snake ) {
      release_snake( p->human_snake );
      release_osal( p->p_osal );
      free( p );
      return NULL;
    }

    p->food = queue_init( MAX_FOOD_LOCATIONS );
    if( ! p->food ) {
      release_snake( p->computer_snake );
      release_snake( p->human_snake );
      release_osal( p->p_osal );
      free( p );
      return NULL;
    }
  }

  return p;
}

void release_game( game_t* p )
{
  if( p ) {
    release_snake( p->computer_snake );
    release_snake( p->human_snake );
    queue_apply( p->food, (queue_apply_fn_t)release_coord );
    queue_release( p->food );
    release_osal( p->p_osal );
    free( p );
  }
}

static int is_same_coordinate( const coord_t* c1, const coord_t* c2 )
{
  return ( c1->x == c2->x && c1->y == c2->y );
}

static void grow_snake_when_eaten( game_t* p, snake_t* p_snake )
{
  coord_t* p_snake_head = queue_last( p_snake->elements );
  coord_t* p_eaten;
  int idx;

  if( queue_has_element( p->food, p_snake_head, (queue_check_fn_t)is_same_coordinate, &idx ) )
  {
    p_eaten = queue_get_element_at( p->food, idx );
    release_coord( p_eaten );
    queue_remove_element( p->food, idx );
    p_snake->growing = 1;
  }
}

static void update_food( game_t* p )
{
  coord_t* p_nf = init_coord( rand() % p->p_osal->width,
                              rand() % p->p_osal->height );
  int tmp;

  if( ! p_nf ) {
    fprintf( stderr, "out of memory error!\n" );
    return;
  }

  if( !queue_has_element( p->human_snake->elements, p_nf, (queue_check_fn_t)is_same_coordinate, &tmp ) &&
      !queue_has_element( p->food, p_nf, (queue_check_fn_t)is_same_coordinate, &tmp ) &&
      p->food->count < MAX_FOOD_LOCATIONS )
  {
    queue_push( p->food, p_nf );
    draw_point( p->p_osal, p_nf->x, p_nf->y, COLOR_RED );
  } else {
    release_coord( p_nf );
  }
}


typedef enum {
  no_game_over,
  computer_win,
  human_win,
  undecided
} game_over_t;


static game_over_t game_over( game_t* p )
{
  coord_t* p_human_snake_head = queue_last( p->human_snake->elements );
  coord_t* p_computer_snake_head = queue_last( p->computer_snake->elements );
  int fail_human = 0;
  int fail_computer = 0;

  if( p_human_snake_head->x < 0 || p_human_snake_head->x > p->p_osal->width-1 ||
      p_human_snake_head->y < 0 || p_human_snake_head->y > p->p_osal->height-1 )
    fail_human = 1;

  if( p_computer_snake_head->x < 0 || p_computer_snake_head->x > p->p_osal->width-1 ||
      p_computer_snake_head->y < 0 || p_computer_snake_head->y > p->p_osal->height-1 )
    fail_computer = 1;

  if( queue_has_elements( p->human_snake->elements, p_human_snake_head, (queue_check_fn_t)is_same_coordinate ) > 1 ||
      queue_has_elements( p->computer_snake->elements, p_human_snake_head, (queue_check_fn_t)is_same_coordinate ) > 0 )
    /* check whether snake's contains more elements in than just its head */
    fail_human = 1;

  if( queue_has_elements( p->computer_snake->elements, p_computer_snake_head, (queue_check_fn_t)is_same_coordinate ) > 1 ||
      queue_has_elements( p->human_snake->elements, p_computer_snake_head, (queue_check_fn_t)is_same_coordinate ) > 0 )
    /* check whether snake's contains more elements in than just its head */
    fail_computer = 1;

  if( fail_human && fail_computer )
    return undecided;
  if( fail_human )
    return computer_win;
  if( fail_computer )
    return human_win;

  return no_game_over;
}

static coord_t distance_min_coordinate;
static long distance_min;
static int tmp_x, tmp_y;
static void distance( const coord_t* c ) {
  long distance = abs(c->x - tmp_x) + abs(c->y - tmp_y );

  if( distance < distance_min ) {
    distance_min = distance;
    distance_min_coordinate = *c;
  }
}

static int find_closest_food_to( game_t* p, const int x, const int y, int* food_x, int* food_y )
{
  int found;

  if( p->food->count > 0 ) {
    distance_min = 32767;
    tmp_x = x; tmp_y = y;
    queue_apply( p->food, (queue_apply_fn_t)distance );
    *food_x = distance_min_coordinate.x;
    *food_y = distance_min_coordinate.y;
    found = 1;
  } else {
    /* when there is no food, return center position */
    *food_x = p->p_osal->width>>1;
    *food_y = p->p_osal->height>>1;
    found = 0;
  }

  return found;
}


static int direction_towards_pos(
  game_t* p, const coord_t dirs[],
  const int x, const int y, const int pos_x, const int pos_y )
{
  const int dx = (x - pos_x);
  const int dy = (y - pos_y);
  int dir_x, dir_y, dir;

  if( dx < 0 )
    dir_x = 1;
  else
    dir_x = -1;

  if( dy < 0 )
    dir_y = 1;
  else
    dir_y = -1;

  /* constrain movement to rectangular directions */
  if( abs(dx) > abs(dy) )
    dir_y = 0;
  else
    dir_x = 0;

  for( dir=0; dir<4; ++dir )
    if( dirs[dir].x == dir_x && dirs[dir].y == dir_y )
      break;

  return dir;
}

static void computer_move( game_t* p )
{
  coord_t* head = queue_last( p->computer_snake->elements );
  const coord_t dirs[] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
  coord_t pos;
  int rate[] = { 0, 0, 0, 0 };
  int max_rate = -32768, max_dir = 0;
  int dir, dir_to_center, dir_to_food, depth, food_x, food_y, x, y, idx;
  const int max_depth = 10;
  const int worst_score = 1<<max_depth;
  const int food_score = worst_score>>1;

  /* score blockades and food and vincinity */
  for( dir=0; dir<4; ++dir ) {
    for( depth=1; depth<max_depth; ++depth ) {
      x = head->x + depth*dirs[dir].x;
      y = head->y + depth*dirs[dir].y;
      if( x<0 || y<0 || x>p->p_osal->width-1 || y>p->p_osal->height-1 ) {
        rate[dir] -= 1<<(max_depth - depth);
        continue;
      } else {
        pos.x = x; pos.y = y;

        if( queue_has_elements( p->computer_snake->elements, &pos,
                                (queue_check_fn_t)is_same_coordinate ) > 0 ||
            queue_has_elements( p->human_snake->elements, &pos,
                                (queue_check_fn_t)is_same_coordinate ) > 0 )
        {
          rate[dir] -= (depth<=1) ? worst_score : 1<<(max_depth - depth);
          continue;
        }

        if( queue_has_element( p->food, &pos, (queue_check_fn_t)is_same_coordinate, &idx ) )
          rate[dir] += 1<<(1 + max_depth - depth);
      }
    }
  }

  /* score movements to the center and to nearest food */
  x = head->x; y = head->y;
  if( find_closest_food_to( p, x, y, &food_x, &food_y ) ) {
    dir_to_food  = direction_towards_pos( p, dirs, x, y, food_x, food_y );
    rate[dir_to_food] += food_score;
    snake_message("next food: (%d, %d), direction to it: (%d, %d)\n",
                  food_x, food_y, dirs[dir_to_food].x, dirs[dir_to_food].x );
  }
  dir_to_center = direction_towards_pos( p, dirs, x, y, p->p_osal->width >> 1, p->p_osal->height >> 1 );
  rate[dir_to_center] += 2;

  for( dir=0; dir<4; ++dir )
    if( rate[dir] > max_rate ) {
      max_rate = rate[dir];
      max_dir = dir;
    }

  snake_message("best dir: (%d, %d)->%d, ", dirs[max_dir].x, dirs[max_dir].y, max_rate );
#if(0)
  for( dir=0; dir<4; ++dir )
    snake_message( "(%d, %d)->%d, ", dirs[dir].x, dirs[dir].y, rate[dir] );
#endif

  p->computer_snake->move_x = dirs[max_dir].x;
  p->computer_snake->move_y = dirs[max_dir].y;
}

static void update_game( game_t* p, unsigned short key_code )
{
  switch( key_code ) {
  case KEY_Q:
    p->state = state_terminate;
    return;
  case KEY_UP_ARROW:
    p->human_snake->move_x = 0;
    p->human_snake->move_y = -1;
    break;
  case KEY_DOWN_ARROW:
    p->human_snake->move_x = 0;
    p->human_snake->move_y = 1;
    break;
  case KEY_LEFT_ARROW:
    p->human_snake->move_x = -1;
    p->human_snake->move_y = 0;
    break;
  case KEY_RIGHT_ARROW:
    p->human_snake->move_x = 1;
    p->human_snake->move_y = 0;
    break;
  default:
    /* no key action */
    break;
  }

  computer_move( p );

  if( ! ( p->iteration % 1 ) )
    update_food( p );

  update_snake( p->p_osal, p->human_snake );
  grow_snake_when_eaten( p, p->human_snake );

  update_snake( p->p_osal, p->computer_snake );
  grow_snake_when_eaten( p, p->computer_snake );

  ++p->iteration;
}


int evt_handler( game_t* p_game, unsigned short key_code )
{
  int terminate_evt_loop = 0;
  static game_over_t over;
  static int start_screen_printed = 0;
  static int game_over_screen_printed = 0;

  if( 0 && key_code )
    printf( "key %x pressed\n", key_code );

  switch( p_game->state )
  {
  case state_start_screen:
    if( ! start_screen_printed ) {
      write_splash_text( p_game->p_osal, start_screen );
      start_screen_printed = 1;
      game_over_screen_printed = 0;
    }

    if( key_code == KEY_Q )
      p_game->state = state_terminate;
    else if( key_code != 0 ) {
      clear_screen( p_game->p_osal );
      p_game->state = state_game;
    }
    break;

  case state_game:
    update_game( p_game, key_code );
    over = game_over( p_game );
    if( over != no_game_over )
      p_game->state = state_game_over_screen;
    break;

  case state_game_over_screen:
    if( ! game_over_screen_printed ) {
      switch( over ) {
      case computer_win:
        write_splash_text( p_game->p_osal, "GAME OVER, YOU LOST!\n"); break;
      case human_win:
        write_splash_text( p_game->p_osal, "GAME OVER, YOU WON!\n"); break;
      case undecided:
        write_splash_text( p_game->p_osal, "GAME OVER, UNDECIDED!\n"); break;
      case no_game_over: /* shall not happen */
        break;
      }
      game_over_screen_printed = 1;
      start_screen_printed = 0;
    }

    if( key_code == KEY_Q )
      p_game->state = state_terminate;
    else if( key_code != 0 &&
             key_code != KEY_UP_ARROW &&
             key_code != KEY_DOWN_ARROW &&
             key_code != KEY_LEFT_ARROW &&
             key_code != KEY_RIGHT_ARROW )
      p_game->state = state_new_game;
    break;

  case state_new_game:
  case state_terminate:
    start_screen_printed = 0;
    game_over_screen_printed = 0;
    clear_screen( p_game->p_osal );
    terminate_evt_loop = 1;
    break;
  }

  return terminate_evt_loop;
}

int main()
{
  game_t* p_game;
  int terminate = 0;

  snake_log_init();

  while( !terminate ) {
    p_game = init_game();
    if( p_game ) {
      start_event_loop( p_game->p_osal, (t_evt_cb)evt_handler, p_game );
      terminate = ( p_game->state == state_terminate );
      release_game( p_game );
    } else {
      fprintf( stderr, "could not initialize new game error!\n" );
      break;
    }
  }

  snake_log_release();
  osal_exit();

  return 0;
}
