/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tsess.h 33866 2007-10-02 17:30:51Z $
 * 
 */

#ifndef _INCLUDED_tibemstsess_h
#define _INCLUDED_tibemstsess_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* deprecated as of 5.0. please use tibemsSession_CreateConsumer */
extern tibems_status
tibemsTopicSession_CreateSubscriber(
    tibemsTopicSession          session,
    tibemsTopicSubscriber*      topicSubscriber,
    tibemsTopic                 topic,
    const char*                 optionalSelector,
    tibems_bool                 noLocal);

/* deprecated as of 5.0. please use tibemsSession_CreateDurableSubscriber */
extern tibems_status
tibemsTopicSession_CreateDurableSubscriber(
    tibemsTopicSession          session,
    tibemsTopicSubscriber*      topicSubscriber,
    tibemsTopic                 topic,
    const char*                 subscriberName,
    const char*                 optionalSelector,
    tibems_bool                 noLocal);

/* deprecated as of 5.0. please use tibemsSession_CreateProducer */
extern tibems_status
tibemsTopicSession_CreatePublisher(
    tibemsTopicSession          session,
    tibemsTopicPublisher*       topicPublisher,
    tibemsTopic                 topic);

/* deprecated as of 5.0. please use tibemsSession_Unsubscribe */
extern tibems_status
tibemsTopicSession_Unsubscribe(
    tibemsTopicSession          session,
    const char*                 subscriberName);

/* deprecated as of 5.0. please use tibemsSession_CreateTemporaryTopic */
extern tibems_status
tibemsTopicSession_CreateTemporaryTopic(
    tibemsTopicSession          session,
    tibemsTemporaryTopic*       tmpTopic);

/* deprecated as of 5.0. please use tibemsSession_DeleteTemporaryTopic */
extern tibems_status
tibemsTopicSession_DeleteTemporaryTopic(
    tibemsTopicSession          session,
    tibemsTemporaryTopic        tmpTopic);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemstsess_h */
