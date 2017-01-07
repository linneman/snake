/*
 * syslog, used only when available
 *
 * GNU General Public License, Copyright by Otto Linnemann
 */

#ifndef LOG_H_
#define LOG_H_


/*! initialize logging (syslog) */
int snake_log_init(void);


/*! release logging (syslog) */
void snake_log_release(void);


/*!
 * for message log output, currently to stdout
 */
void snake_message( const char* fmt, ... );


/*!
 * for error log output, currently to stderr
 */
void snake_error( const char* fmt, ... );


#endif /* ifndef LOG_H_ */
