/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: qsess.h 33866 2007-10-02 17:30:51Z $
 * 
 */

#ifndef _INCLUDED_tibemsqsess_h
#define _INCLUDED_tibemsqsess_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* deprecated as of 5.0. please use tibemsSession_CreateBrowser */
extern tibems_status
tibemsQueueSession_CreateBrowser(
    tibemsQueueSession          session,
    tibemsQueueBrowser*         browser,
    tibemsQueue                 queue,
    const char*                 optionalSelector);

/* deprecated as of 5.0. please use tibemsSession_CreateConsumer */
extern tibems_status
tibemsQueueSession_CreateReceiver(
    tibemsQueueSession          session,
    tibemsQueueReceiver*        queueReceiver,
    tibemsQueue                 queue,
    const char*                 optionalSelector);

/* deprecated as of 5.0. please use tibemsSession_CreateProducer */
extern tibems_status
tibemsQueueSession_CreateSender(
    tibemsQueueSession          session,
    tibemsQueueSender*          queueSender,
    tibemsQueue                 queue);

/* deprecated as of 5.0. please use tibemsSession_CreateTemporaryQueue */
extern tibems_status
tibemsQueueSession_CreateTemporaryQueue(
    tibemsQueueSession          session,
    tibemsTemporaryQueue*       tmpQueue);

/* deprecated as of 5.0. please use tibemsSession_DeleteTemporaryQueue */
extern tibems_status
tibemsQueueSession_DeleteTemporaryQueue(
    tibemsQueueSession          session,
    tibemsTemporaryQueue        tmpQueue);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemsqsess_h */
