/* 
 * Copyright 2001-2008 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tinfo.h 35530 2008-03-10 16:52:02Z $
 * 
 */

#ifndef _INCLUDED_tibems_tinfo_h
#define _INCLUDED_tibems_tinfo_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status 
tibemsTopicInfo_GetActiveDurableCount(
    tibemsTopicInfo             topicInfo,
    tibems_int*                 count);

extern tibems_status 
tibemsTopicInfo_GetDurableCount(
    tibemsTopicInfo             topicInfo,
    tibems_int*                 count);

#define tibemsTopicInfo_Create(topicInfo, name) \
        tibemsDestinationInfo_Create((topicInfo), (name), TIBEMS_TOPIC)

#define tibemsTopicInfo_GetSubscriberCount(topicInfo, count) \
        tibemsDestinationInfo_GetConsumerCount((topicInfo), (count))

#define tibemsTopicInfo_GetPendingMessageCount(topicInfo, size) \
        tibemsDestinationInfo_GetPendingMessageCount((topicInfo), (size))

#define tibemsTopicInfo_GetPendingMessageSize(topicInfo, size) \
        tibemsDestinationInfo_GetPendingMessageSize((topicInfo), (size))

#define tibemsTopicInfo_GetInboundStatistics(topicInfo, statData) \
        tibemsDestinationInfo_GetInboundStatistics((topicInfo), (statData))

#define tibemsTopicInfo_GetOutboundStatistics(topicInfo, statData) \
        tibemsDestinationInfo_GetOutboundStatistics((topicInfo), (statData))

#define tibemsTopicInfo_GetName(topicInfo, name, name_len) \
        tibemsDestinationInfo_GetName((topicInfo), (name), (name_len))

#define tibemsTopicInfo_GetFlowControlMaxBytes(topicInfo, maxBytes) \
        tibemsDestinationInfo_GetFlowControlMaxBytes((topicInfo), (maxBytes))

#define tibemsTopicInfo_GetMaxBytes(topicInfo, maxBytes) \
        tibemsDestinationInfo_GetMaxBytes((topicInfo), (maxBytes))

#define tibemsTopicInfo_GetMaxMsgs(topicInfo, maxMsgs) \
        tibemsDestinationInfo_GetMaxMsgs((topicInfo), (maxMsgs))

#define tibemsTopicInfo_GetOverflowPolicy(topicInfo, overflowPolicy ) \
        tibemsDestinationInfo_GetOverflowPolicy((topicInfo), (overflowPolicy))

#define tibemsTopicInfo_Destroy(topicInfo) \
        tibemsDestinationInfo_Destroy((topicInfo))

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibems_tinfo_h */
