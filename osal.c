/*
 * OS Abstraction Layer
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#ifdef QDOS
#include "osal_qdos.c"
#else
#include "osal_curses.c"
#endif
