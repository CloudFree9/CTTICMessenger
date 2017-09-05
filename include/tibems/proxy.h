/* 
 * Copyright 2001-2006 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: proxy.h 22230 2006-06-19 22:09:45Z $
 * 
 */

#ifndef _INCLUDED_proxy_h
#define _INCLUDED_proxy_h

#include "status.h"
#include "types.h"
#include "confact.h"

#if defined(__cplusplus)
extern "C" {
#endif


/*
 * DEPRECATION NOTICE
 *
 * The following type is deprecated.  In the future, these
 * functions will take tibemsConnectionFactory for the first
 * parameter.
 */
// typedef struct __tibemsConnectionFactory*       tibemsAnyConnectionFactory;
    
extern tibems_status
tibemsConnectionFactory_SetSSLProxy(
    tibemsConnectionFactory          factory,
    const char*                         proxy_host,
    tibems_int                          proxy_port);

extern tibems_status
tibemsConnectionFactory_SetSSLProxyAuth(
    tibemsConnectionFactory          factory,
    const char*                         proxy_user,
    const char*                         proxy_password);

extern tibems_status
tibemsConnectionFactory_GetSSLProxyHost(
    tibemsConnectionFactory          factory,
    const char**                        proxy_host);

extern tibems_status
tibemsConnectionFactory_GetSSLProxyPort(
    tibemsConnectionFactory          factory,
    tibems_int*                         proxy_port);

extern tibems_status
tibemsConnectionFactory_GetSSLProxyUser(
    tibemsConnectionFactory          factory,
    const char**                        proxy_user);

extern tibems_status
tibemsConnectionFactory_GetSSLProxyPassword(
    tibemsConnectionFactory          factory,
    const char**                        proxy_password);


#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_proxy_h */
