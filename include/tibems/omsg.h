/* 
 * Copyright 2001-2006 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: omsg.h 21731 2006-05-01 21:41:34Z $
 * 
 */

#ifndef _INCLUDED_tibemsobjectmsg_h
#define _INCLUDED_tibemsobjectmsg_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status
tibemsObjectMsg_Create(
    tibemsObjectMsg*    message);

extern tibems_status
tibemsObjectMsg_GetObjectBytes(
    tibemsObjectMsg     message,
    void**              bytes,
    tibems_uint*        byteSize);

extern tibems_status
tibemsObjectMsg_SetObjectBytes(
    tibemsObjectMsg     message,
    const void*         bytes,
    tibems_uint         byteSize);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibemsobjectmsg_h */
