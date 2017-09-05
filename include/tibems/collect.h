/* 
 * Copyright 2001-2007 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: collect.h 32668 2007-06-01 22:58:17Z $
 * 
 */

#ifndef _INCLUDED_tibems_collect_h
#define _INCLUDED_tibems_collect_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status 
tibemsCollection_GetFirst(
    tibemsCollection            collection,
    tibemsCollectionObj*        obj);

extern tibems_status 
tibemsCollection_GetNext(
    tibemsCollection            collection,
    tibemsCollectionObj*        obj);

extern tibems_status 
tibemsCollection_GetCount(
    tibemsCollection            collection,
    tibems_int*                 count);

extern tibems_status 
tibemsCollection_Destroy(
    tibemsCollection            collection);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibems_collect_h */
