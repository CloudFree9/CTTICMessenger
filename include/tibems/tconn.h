/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tconn.h 33866 2007-10-02 17:30:51Z $
 * 
 */

#ifndef _INCLUDED_tibemstconn_h
#define _INCLUDED_tibemstconn_h

#include "conn.h"
#include "types.h"
#include "status.h"
#include "sess.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* deprecated as of 5.0. please use tibemsConnection_Create */
extern tibems_status
tibemsTopicConnection_Create(
    tibemsTopicConnection*              topicConnection,
    const char*                         brokerURL,
    const char*                         clientId,
    const char*                         username,
    const char*                         password);

/* deprecated as of 5.0. please use tibemsConnection_CreateSession */
extern tibems_status
tibemsTopicConnection_CreateTopicSession(
    tibemsTopicConnection               topicConnection,
    tibemsTopicSession*                 topicSession,
    tibems_bool                         transacted,
    tibemsAcknowledgeMode               ackowledgeMode);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemstconn_h */
