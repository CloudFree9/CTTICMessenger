/* 
 * Copyright 2001-2008 TIBCO Software Inc.
 * All rights reserved.
 * For more information, please contact:
 * TIBCO Software Inc., Palo Alto, California, USA
 * 
 * $Id: ddeststat.h 35530 2008-03-10 16:52:02Z $
 * 
 */

#ifndef _INCLUDED_tibems_ddeststat_h
#define _INCLUDED_tibems_ddeststat_h

#include "types.h"
#include "status.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern tibems_status 
tibemsDetailedDestStat_GetDestinationName(
    tibemsDetailedDestStat      detailedDestStat,
    char*                       destName,
    tibems_int                  name_len);

extern tibems_status 
tibemsDetailedDestStat_GetDestinationType(
    tibemsDetailedDestStat      detailedDestStat,
    tibemsDestinationType*      destType);

extern tibems_status 
tibemsDetailedDestStat_GetStatData(
    tibemsDetailedDestStat      detailedDestStat,
    tibemsStatData*             stat);

#ifdef  __cplusplus
}
#endif

#endif /* _INCLUDED_tibems_ddeststat_h */
