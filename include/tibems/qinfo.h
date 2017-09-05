/* 
 * Copyright 2001-2008 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: qinfo.h 35530 2008-03-10 16:52:02Z $
 * 
 */

#ifndef _INCLUDED_tibems_qinfo_h
#define _INCLUDED_tibems_qinfo_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status 
tibemsQueueInfo_GetDeliveredMessageCount(
    tibemsQueueInfo             queueInfo,
    tibems_long*                count);

#define tibemsQueueInfo_Create(queueInfo, name) \
        tibemsDestinationInfo_Create((queueInfo), (name), TIBEMS_QUEUE)

#define tibemsQueueInfo_GetReceiverCount(queueInfo, count) \
        tibemsDestinationInfo_GetConsumerCount((queueInfo), (count))

#define tibemsQueueInfo_GetPendingMessageCount(queueInfo, size) \
        tibemsDestinationInfo_GetPendingMessageCount((queueInfo), (size))

#define tibemsQueueInfo_GetPendingMessageSize(queueInfo, size) \
        tibemsDestinationInfo_GetPendingMessageSize((queueInfo), (size))

#define tibemsQueueInfo_GetInboundStatistics(queueInfo, statData) \
        tibemsDestinationInfo_GetInboundStatistics((queueInfo), (statData))

#define tibemsQueueInfo_GetOutboundStatistics(queueInfo, statData) \
        tibemsDestinationInfo_GetOutboundStatistics((queueInfo), (statData))

#define tibemsQueueInfo_GetName(queueInfo, name, name_len) \
        tibemsDestinationInfo_GetName((queueInfo), (name), (name_len))

#define tibemsQueueInfo_GetFlowControlMaxBytes(queueInfo, maxBytes) \
        tibemsDestinationInfo_GetFlowControlMaxBytes((queueInfo), (maxBytes))

#define tibemsQueueInfo_GetMaxBytes(queueInfo, maxBytes) \
        tibemsDestinationInfo_GetMaxBytes((queueInfo), (maxBytes))

#define tibemsQueueInfo_GetMaxMsgs(queueInfo, maxMsgs) \
        tibemsDestinationInfo_GetMaxMsgs((queueInfo), (maxMsgs))

#define tibemsQueueInfo_GetOverflowPolicy(queueInfo, overflowPolicy ) \
        tibemsDestinationInfo_GetOverflowPolicy((queueInfo), (overflowPolicy))

#define tibemsQueueInfo_Destroy(queueInfo) \
        tibemsDestinationInfo_Destroy((queueInfo))

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibems_qinfo_h */
