/*
 * OS Abstraction Layer for for Snake and on QDOS for the Sinclair QL
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#ifndef OSAL_H
#define OSAL_H

#ifdef QDOS
#include "osal_qdos.h"
#else
#include "osal_curses.h"
#endif

#endif /* #define OSAL_H */
