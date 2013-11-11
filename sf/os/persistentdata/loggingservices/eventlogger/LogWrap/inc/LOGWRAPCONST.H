// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LOGWRAPCONST_H
#define LOGWRAPCONST_H
/** 
Big enough to contain any of the above strings
Defines a modifiable buffer descriptor into which a standard string from the 
resource in the logwrap.dll resource file can be safely put.

@see CLogClient::GetString()
@publishedAll
@released
*/
typedef TBuf<64> TLogString;

/** 
Type definitions
The unique event ID associated with a log event.

@see CLogEvent
@see CLogViewDuplicate
@see CLogViewRecent
@see CLogClient
@see CLogBase 
@publishedAll
@released
*/
typedef TInt32	TLogId;
/** 
The duration type of an event.

@see CLogEvent
@see CLogFilter
@publishedAll
@released
*/
typedef TInt8	TLogDurationType;
typedef TInt8	TLogRecentList;
typedef TUint16 TLogSize;
typedef TUint8	TLogRecentSize;
/** 
Duration of an event, expressed as the number of seconds since the time of 
the event.

@see CLogEvent
@publishedAll
@released
*/
typedef TUint32	TLogDuration;
typedef TUint32	TLogAge;
/** 
Link value relating a log event to an entity in another application.

@see CLogEvent
@publishedAll
@released
*/
typedef TUint32	TLogLink;
typedef TInt16	TLogStringId;
typedef TInt16	TLogTypeId;
/** 
The duration type of an event.

@see CLogEvent
@see CLogFilter
@publishedAll
@released
*/
typedef TInt8	TLogDurationType;
/** 
Event flags.

@see CLogEvent
@see CLogFilter
@publishedAll
@released
*/
typedef TUint8	TLogFlags;

#endif
