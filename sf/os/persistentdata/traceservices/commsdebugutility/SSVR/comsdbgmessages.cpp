// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the Flogger server side message processing
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgmessages.h"
#include "comsdbgwriter.h"
#include "comsdbgaux.h"

void CTimeUpdateMessage::Invoke(MLogMessageProtocol& aProtocol)
	{
	TRAP_IGNORE(aProtocol.SetTimeL(iTime));
	//Ignore error. Worst case would mean time doesn't make it into log.
	}

CTimeUpdateMessage::CTimeUpdateMessage(const TTime& aTime)
: iTime(aTime)
{}

/////////////////////////////////////////////////////////////////////////

void CMediaUpdateMessage::Invoke(MLogMessageProtocol& aProtocol)
	{
	aProtocol.MediaUpdate(iLogMediaSetting,iForceFlushSetting, iLogPathSetting);
	}

CMediaUpdateMessage::CMediaUpdateMessage(MIniLoggingMediaQuery& aQuery)
	{
	aQuery.LogMediaSetting(iLogMediaSetting);
	// if force flush value not yet known, false will be returned.
	aQuery.FlushingOn(iForceFlushSetting);
	aQuery.LogPathSetting(iLogPathSetting);
	}

////////////////////////////////////////////////////////////////////////

CClearLogMessage::CClearLogMessage(const TFullName& aName)
: iThreadName(aName)
	{}

void CClearLogMessage::Invoke(MLogMessageProtocol& aProtocol)
	{
	aProtocol.ClearLog(iThreadName);
	}

////////////////////////////////////////////////////////////////////////

void CLogStringMessage::Invoke(MLogMessageProtocol& aProtocol)
	{
	aProtocol.LogString(*iLogString, iSubsystem, iComponent, iThreadId);
	}

CLogStringMessage::CLogStringMessage(HBufC8* aString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId)
: iLogString(aString), iSubsystem(aSubSystem), iComponent(aComponent), iThreadId(aThreadId)
{}

CLogStringMessage::~CLogStringMessage()
	{
	delete iLogString;
	}

///////////////////////////////////////////////////////////////////////

void CLogBinaryString::Invoke(MLogMessageProtocol& aProtocol)
	{
	aProtocol.LogBinaryDump(*iBinaryString, iSubsystem, iComponent);
	}

CLogBinaryString::CLogBinaryString(HBufC8* aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
: iBinaryString(aBinaryString), iSubsystem(aSubSystem), iComponent(aComponent)
	{}

CLogBinaryString::~CLogBinaryString()
	{
	delete iBinaryString;
	}

////////////////////////////////////////////////////////////

CLogCommentMessage* CLogCommentMessage::NewL(const TDesC8& aComment)
	{
	CLogCommentMessage* self = new(ELeave) CLogCommentMessage;
	CleanupStack::PushL(self);
	self->iComment = aComment.AllocL();
	CleanupStack::Pop(self);
	return self;
	}

void CLogCommentMessage::Invoke(MLogMessageProtocol& aProtocol)
	{
	aProtocol.LogComment(*iComment);
	}

CLogCommentMessage::~CLogCommentMessage()
	{
	delete iComment;
	}

CLogCommentMessage::CLogCommentMessage()
	{}

///////////////////////////////////////////////////////////////

void CShutDownMessage::Invoke(MLogMessageProtocol& aProtocol)
	{
	aProtocol.ShutDown();
	}
