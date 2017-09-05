/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: msgprod.h 32835 2007-06-18 21:41:10Z $
 * 
 */

#ifndef _INCLUDED_tibemsmsgprod_h
#define _INCLUDED_tibemsmsgprod_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status
tibemsMsgProducer_Close(
    tibemsMsgProducer           msgProducer);

extern tibems_status
tibemsMsgProducer_GetDestination(
    tibemsMsgProducer           msgProducer,
    tibemsDestination*          destination);

extern tibems_status
tibemsMsgProducer_GetDeliveryMode(
    tibemsMsgProducer           msgProducer,
    tibems_int*                 deliveryMode);

extern tibems_status
tibemsMsgProducer_GetDisableMessageID(
    tibemsMsgProducer           msgProducer,
    tibems_bool*                doDisableMessageID);

extern tibems_status
tibemsMsgProducer_GetDisableMessageTimestamp(
    tibemsMsgProducer           msgProducer,
    tibems_bool*                doDisableMessageTimeStamp);

extern tibems_status
tibemsMsgProducer_GetPriority(
    tibemsMsgProducer           msgProducer,
    tibems_int*                 priority);

extern tibems_status
tibemsMsgProducer_GetTimeToLive(
    tibemsMsgProducer           msgProducer,
    tibems_long*                timeToLive);

extern tibems_status
tibemsMsgProducer_SetDeliveryMode(
    tibemsMsgProducer           msgProducer,
    tibems_int                  deliveryMode);

extern tibems_status
tibemsMsgProducer_SetDisableMessageID(
    tibemsMsgProducer           msgProducer,
    tibems_bool                 doDisableMessageID);

extern tibems_status
tibemsMsgProducer_SetDisableMessageTimestamp(
    tibemsMsgProducer           msgProducer,
    tibems_bool                 doDisableMessageTimeStamp);

extern tibems_status
tibemsMsgProducer_SetPriority(
    tibemsMsgProducer           msgProducer,
    tibems_int                  priority);

extern tibems_status
tibemsMsgProducer_SetTimeToLive(
    tibemsMsgProducer           msgProducer,
    tibems_long                 timeToLive);

extern tibems_status
tibemsMsgProducer_Send(
    tibemsMsgProducer           msgProducer,
    tibemsMsg                   msg);

extern tibems_status
tibemsMsgProducer_SendEx(
    tibemsMsgProducer           msgProducer,
    tibemsMsg                   msg,
    tibems_int                  deliveryMode,
    tibems_int                  priority,
    tibems_long                 timeToLive);

extern tibems_status
tibemsMsgProducer_SendToDestination(
    tibemsMsgProducer           msgProducer,
    tibemsDestination           destination,
    tibemsMsg                   msg);

extern tibems_status
tibemsMsgProducer_SendToDestinationEx(
    tibemsMsgProducer           msgProducer,
    tibemsDestination           destination,
    tibemsMsg                   msg,
    tibemsDeliveryMode          deliveryMode,
    tibems_int                  priority,
    tibems_long                 timeToLive);

extern tibems_status
tibemsMsgProducer_SetNPSendCheckMode(
    tibemsMsgProducer           producer, 
    tibemsNpCheckMode           mode);

extern tibems_status
tibemsMsgProducer_GetNPSendCheckMode(
    tibemsMsgProducer           producer, 
    tibemsNpCheckMode          *mode);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemsmsgprod_h */
