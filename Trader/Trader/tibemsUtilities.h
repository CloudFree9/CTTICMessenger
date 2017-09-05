/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tibemsUtilities.h 33866 2007-10-02 17:30:51Z $
 * 
 */

#ifndef _INCLUDED_tibemsUtilities_h
#define _INCLUDED_tibemsUtilities_h

#ifdef __MVS__  /* TIBEMSOS_ZOS  */
#include <tibems.h>
#include <emsadmin.h>
#else
#include <tibems/tibems.h>
#include <tibems/emsadmin.h>
#endif

#if defined(_WIN32)

#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0400
#  endif
#    include <windows.h>
#    include <process.h>
// #  endif

#  define THREAD_RETVAL         unsigned __stdcall
#  define THREAD_OBJ            HANDLE
#  define THREAD_CREATE(t,f,a)  t=(HANDLE)_beginthreadex(NULL,0,(f),(a),0,NULL)

#else

#  include <pthread.h>
#  include <errno.h>
#  define THREAD_RETVAL         void*
#  define THREAD_OBJ            pthread_t
#  define THREAD_CREATE(t,f,a)  pthread_create(&(t),NULL,(f),(a))

#endif

extern void ThreadJoin(int threads, THREAD_OBJ* threadArray);

extern tibems_long currentMillis(void);

extern char* stringdup(const char* s);

extern void sslUsage();

extern void setSSLParams(tibemsSSLParams ssl_params, int argc, char* argv[], char* *pk_password);

extern void ldapUsage();

extern tibems_status setLookupParams(tibemsLookupParams lookup_params, int argc, char* argv[]);

#endif /* _INCLUDED_tibemsUtilities_h */

