/* 
 * Copyright 2001-2006 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: tmsg.h 21731 2006-05-01 21:41:34Z $
 * 
 */

#ifndef _INCLUDED_tibemstextmsg_h
#define _INCLUDED_tibemstextmsg_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status
tibemsTextMsg_Create(
    tibemsTextMsg*      message);

extern tibems_status
tibemsTextMsg_GetText(
    tibemsTextMsg       message,
    const char**        text);

extern tibems_status
tibemsTextMsg_SetText(
    tibemsTextMsg       message,
    const char*         text);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemstextmsg_h */
