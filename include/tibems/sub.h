/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: sub.h 34047 2007-10-17 17:13:56Z $
 * 
 */

#ifndef _INCLUDED_tibemstsub_h
#define _INCLUDED_tibemstsub_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* deprecated as of 5.0. please use tibemsMsgConsumer_GetDestination */
extern tibems_status
tibemsTopicSubscriber_GetTopic(
    tibemsTopicSubscriber       topicSubscriber,
    tibemsTopic*                topic);

/* deprecated as of 5.0. please use tibemsMsgConsumer_GetNoLocal */
extern tibems_status
tibemsTopicSubscriber_GetNoLocal(
    tibemsTopicSubscriber       topicSubscriber,
    tibems_bool*                isNoLocal);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemstsub_h */
