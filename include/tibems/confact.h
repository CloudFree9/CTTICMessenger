/* 
 * Copyright 2001-2008 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: confact.h 37241 2008-09-03 20:59:43Z $
 * 
 */

#ifndef _INCLUDED_tibemsconnfactory_h
#define _INCLUDED_tibemsconnfactory_h

#include "types.h"
#include "status.h"
#include "cssl.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* All the connection factory object are the same with a type identifier */
typedef  void*              tibemsConnectionFactory;

/* deprecated as of 5.0. please use tibemsConnectionFactory */
typedef  void*              tibemsTopicConnectionFactory;

/* deprecated as of 5.0. please use tibemsConnectionFactory */
typedef  void*              tibemsQueueConnectionFactory;

/* deprecated as of 5.0. please use tibemsConnectionFactory */
typedef  void*              tibemsXAConnectionFactory;

/* deprecated as of 5.0. please use tibemsConnectionFactory */
typedef  void*              tibemsXATopicConnectionFactory;

/* deprecated as of 5.0. please use tibemsConnectionFactory */
typedef  void*              tibemsXAQueueConnectionFactory;

/*
 * connection factory types returned by tibemsConnectionFactory_GetType
 * all values except TIBEMS_CONNECTION_FACTORY are deprecated as of 5.0
 */
typedef enum  __tibemsConnectionFactoryType
{
    TIBEMS_CONNECTION_FACTORY           = 0,   /* should match TIBEMS_UNKNOWN etc*/

/* deprecated as of 5.0. please use TIBEMS_CONNECTION_FACTORY */
    TIBEMS_QUEUE_CONNECTION_FACTORY     = 1,

/* deprecated as of 5.0. please use TIBEMS_CONNECTION_FACTORY */
    TIBEMS_TOPIC_CONNECTION_FACTORY     = 2,

/* deprecated as of 5.0. please use TIBEMS_CONNECTION_FACTORY */
    TIBEMS_XA_CONNECTION_FACTORY        = 3,

/* deprecated as of 5.0. please use TIBEMS_CONNECTION_FACTORY */
    TIBEMS_XA_QUEUE_CONNECTION_FACTORY  = 4,

/* deprecated as of 5.0. please use TIBEMS_CONNECTION_FACTORY */
    TIBEMS_XA_TOPIC_CONNECTION_FACTORY  = 5

} tibemsConnectionFactoryType;

/* connection factory load balance metric types */
typedef enum __tibemsFactoryLoadBalanceMetric
{

    TIBEMS_FACTORY_LOAD_BALANCE_METRIC_NONE             = 0,
    TIBEMS_FACTORY_LOAD_BALANCE_METRIC_CONNECTIONS      = 1,
    TIBEMS_FACTORY_LOAD_BALANCE_METRIC_BYTE_RATE        = 2
    
} tibemsFactoryLoadBalanceMetric;

/* destroy the connection factory */
extern tibems_status
tibemsConnectionFactory_Destroy(
    tibemsConnectionFactory             factory);

/* deprecated as of 5.0. */
extern tibems_status
tibemsConnectionFactory_GetType(
    tibemsConnectionFactory             factory,
    tibemsConnectionFactoryType*        type);

extern tibems_status
tibemsConnectionFactory_CreateConnection(
    tibemsConnectionFactory             factory,
    tibemsConnection*                   connection,
    const char*                         username,
    const char*                         password);


extern tibems_status
tibemsConnectionFactory_CreateXAConnection(
    tibemsConnectionFactory             factory,
    tibemsConnection*                   connection,
    const char*                         username,
    const char*                         password);


/* deprecated as of 5.0. please use tibemsConnectionFactory_CreateConnection */
extern tibems_status
tibemsTopicConnectionFactory_CreateConnection(
    tibemsTopicConnectionFactory        factory,
    tibemsTopicConnection*              connection,
    const char*                         username,
    const char*                         password);


/* deprecated as of 5.0. please use tibemsConnectionFactory_CreateConnection */
extern tibems_status
tibemsQueueConnectionFactory_CreateConnection(
    tibemsQueueConnectionFactory        factory,
    tibemsQueueConnection*              connection,
    const char*                         username,
    const char*                         password);


/* deprecated as of 5.0. please use tibemsConnectionFactory_CreateXAConnection */
extern tibems_status
tibemsXAConnectionFactory_CreateXAConnection(
    tibemsXAConnectionFactory           factory,
    tibemsConnection*                   connection,
    const char*                         username,
    const char*                         password);


/* deprecated as of 5.0. please use tibemsConnectionFactory_CreateXAConnection */
extern tibems_status
tibemsXATopicConnectionFactory_CreateXAConnection(
    tibemsXATopicConnectionFactory      factory,
    tibemsTopicConnection*              connection,
    const char*                         username,
    const char*                         password);


/* deprecated as of 5.0. please use tibemsConnectionFactory_CreateXAConnection */
extern tibems_status
tibemsXAQueueConnectionFactory_CreateXAConnection(
    tibemsXAQueueConnectionFactory      factory,
    tibemsQueueConnection*              connection,
    const char*                         username,
    const char*                         password);


/*
 * deprecated as of 5.0. please use the following API call sequence:
 * 1) tibemsConnectionFactory_SetServerURL
 * 2) tibemsConnectionFactory_SetSSLParams
 * 3) tibemsConnectionFactory_SetPkPassword
 * 4) tibemsConnectionFactory_CreateConnection
 */
extern tibems_status
tibemsConnectionFactory_CreateConnectionSSL(
    tibemsConnectionFactory             factory,
    tibemsConnection*                   connection,
    const char*                         username,
    const char*                         password,
    tibemsSSLParams                     SSLparams,
    const char*                         pk_password);


/*
 * deprecated as of 5.0. please use the following API call sequence:
 * 1) tibemsConnectionFactory_SetServerURL
 * 2) tibemsConnectionFactory_SetSSLParams
 * 3) tibemsConnectionFactory_SetPkPassword
 * 4) tibemsConnectionFactory_CreateConnection
 */
extern tibems_status
tibemsTopicConnectionFactory_CreateConnectionSSL(
    tibemsTopicConnectionFactory        factory,
    tibemsTopicConnection*              connection,
    const char*                         username,
    const char*                         password,
    tibemsSSLParams                     SSLparams,
    const char*                         pk_password);


/*
 * deprecated as of 5.0. please use the following API call sequence:
 * 1) tibemsConnectionFactory_SetServerURL
 * 2) tibemsConnectionFactory_SetSSLParams
 * 3) tibemsConnectionFactory_SetPkPassword
 * 4) tibemsConnectionFactory_CreateConnection
 */
extern tibems_status
tibemsQueueConnectionFactory_CreateConnectionSSL(
    tibemsQueueConnectionFactory        factory,
    tibemsQueueConnection*              connection,
    const char*                         username,
    const char*                         password,
    tibemsSSLParams                     SSLparams,
    const char*                         pk_password);


/*
 * deprecated as of 5.0. please use the following API call sequence:
 * 1) tibemsConnectionFactory_SetServerURL
 * 2) tibemsConnectionFactory_SetSSLParams
 * 3) tibemsConnectionFactory_SetPkPassword
 * 4) tibemsConnectionFactory_CreateXAConnection
 */
extern tibems_status
tibemsXAConnectionFactory_CreateXAConnectionSSL(
    tibemsXAConnectionFactory           factory,
    tibemsConnection*                   connection,
    const char*                         username,
    const char*                         password,
    tibemsSSLParams                     SSLparams,
    const char*                         pk_password);


/*
 * deprecated as of 5.0. please use the following API call sequence:
 * 1) tibemsConnectionFactory_SetServerURL
 * 2) tibemsConnectionFactory_SetSSLParams
 * 3) tibemsConnectionFactory_SetPkPassword
 * 4) tibemsConnectionFactory_CreateXAConnection
 */
extern tibems_status
tibemsXATopicConnectionFactory_CreateXAConnectionSSL(
    tibemsXATopicConnectionFactory      factory,
    tibemsTopicConnection*              connection,
    const char*                         username,
    const char*                         password,
    tibemsSSLParams                     SSLparams,
    const char*                         pk_password);


/*
 * deprecated as of 5.0. please use the following API call sequence:
 * 1) tibemsConnectionFactory_SetServerURL
 * 2) tibemsConnectionFactory_SetSSLParams
 * 3) tibemsConnectionFactory_SetPkPassword
 * 4) tibemsConnectionFactory_CreateXAConnection
 */
extern tibems_status
tibemsXAQueueConnectionFactory_CreateXAConnectionSSL(
    tibemsXAQueueConnectionFactory      factory,
    tibemsQueueConnection*              connection,
    const char*                         username,
    const char*                         password,
    tibemsSSLParams                     SSLparams,
    const char*                         pk_password);

extern tibemsConnectionFactory
tibemsConnectionFactory_Create(void);

/* deprecated as of 5.0. */
extern tibems_status
tibemsConnectionFactory_SetType(
    tibemsConnectionFactory             factory,
    tibemsConnectionFactoryType         type);

extern tibems_status
tibemsConnectionFactory_SetServerURL(
    tibemsConnectionFactory             factory,
    const char*                         url);

extern tibems_status
tibemsConnectionFactory_SetClientID(
    tibemsConnectionFactory             factory,
    const char*                         cid);

extern tibems_status
tibemsConnectionFactory_SetMetric(
    tibemsConnectionFactory             factory,
    tibemsFactoryLoadBalanceMetric      metric);

extern tibems_status
tibemsConnectionFactory_SetConnectAttemptCount(
    tibemsConnectionFactory             factory,
    tibems_int                          connAttempts);

extern tibems_status
tibemsConnectionFactory_SetConnectAttemptDelay(
    tibemsConnectionFactory             factory,
    tibems_int                          delay);

extern tibems_status
tibemsConnectionFactory_SetConnectAttemptTimeout(
    tibemsConnectionFactory             factory,
    tibems_int                          connAttemptTimeout);

extern tibems_status
tibemsConnectionFactory_SetReconnectAttemptCount(
    tibemsConnectionFactory             factory,
    tibems_int                          connAttempts);

extern tibems_status
tibemsConnectionFactory_SetReconnectAttemptDelay(
    tibemsConnectionFactory             factory,
    tibems_int                          delay);

tibems_status
tibemsConnectionFactory_SetReconnectAttemptTimeout(
    tibemsConnectionFactory             factory,
    tibems_int                          reconnAttemptTimeout);

extern tibems_status
tibemsConnectionFactory_SetUserName(
    tibemsConnectionFactory        factory,
    const char*                    username);

extern tibems_status
tibemsConnectionFactory_SetUserPassword(
    tibemsConnectionFactory        factory,
    const char*                    password);

extern tibems_status
tibemsConnectionFactory_SetMulticastEnabled(
    tibemsConnectionFactory             factory,
    tibems_bool                         multicastEnabled);

extern tibems_status
tibemsConnectionFactory_SetMulticastDaemon(
    tibemsConnectionFactory             factory,
    const char*                         multicastDaemon);

/*
 * sslparams will be destroyed when factory is
 * destroyed in tibemsConnectionFactory_Destroy()
 */
extern tibems_status
tibemsConnectionFactory_SetSSLParams(
    tibemsConnectionFactory             factory,
    tibemsSSLParams                     sslparams);

extern tibems_status
tibemsConnectionFactory_SetPkPassword(
    tibemsConnectionFactory             factory,
    const char*                         pk_password);

extern tibems_status 
tibemsConnectionFactory_Print(
    tibemsConnectionFactory             factory);
#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemsconnfactory_h */
