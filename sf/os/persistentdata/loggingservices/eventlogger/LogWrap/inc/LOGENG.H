// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __LOGENG_H__
#define __LOGENG_H__

#include <e32std.h>
#include <logwrap.h>
#include <logwrap.hrh>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <logengdurations.h>
	#include <logengevents.h>
	#include <logsmspdudata.h>
#endif

/**
Event types
@publishedAll
@released
*/
/** Voice call */
const TUid KLogCallEventTypeUid = {KLogCallEventType};
/** Data call. */
const TUid KLogDataEventTypeUid = {KLogDataEventType};
/** Fax call. */
const TUid KLogFaxEventTypeUid = {KLogFaxEventType};
/** SMS call. */
const TUid KLogShortMessageEventTypeUid = {KLogShortMessageEventType};
/** Email call. */
const TUid KLogMailEventTypeUid = {KLogMailEventType};
/** Task scheduler event. */
const TUid KLogTaskSchedulerEventTypeUid = {KLogTaskSchedulerEventType};

/**
Flags
Marks an event as having been "read".
@publishedAll
@released
*/
const TLogFlags KLogEventRead = 0x1;

#endif
