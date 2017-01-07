/*
 * syslog, used only when available
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#ifndef QDOS
#include <sys/syslog.h>
#endif

#define LOG_SETTING  ( LOG_NOWAIT | LOG_PID )
#define LOG_TAG "snake"


/*! initialize logging (syslog) */
int snake_log_init(void)
{
#ifndef QDOS
  openlog( LOG_TAG, LOG_SETTING, LOG_SYSLOG );
#endif

  return 0;
}


/*! release logging (syslog) */
void snake_log_release(void)
{
#ifndef QDOS
  closelog();
#endif
}


/*!
 * for message log output, currently to stdout
 */
void snake_message( const char* fmt, ... )
{
#ifndef QDOS
  va_list args;

  va_start( args, fmt );
  vsyslog( LOG_NOTICE, fmt, args );
  va_end( args );
#endif
}

/*!
 * for error log output, currently to stderr
 */
void snake_error( const char* fmt, ... )
{
#ifndef QDOS
  va_list args;

  va_start( args, fmt );
  vsyslog( LOG_ERR, fmt, args );
  va_end( args );
#endif
}
