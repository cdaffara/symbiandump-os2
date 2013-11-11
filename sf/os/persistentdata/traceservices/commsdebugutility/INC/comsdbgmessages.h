// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms debug utility (File and Serial logger) message declaration header
// 
//

/**
 @file
 @internalTechnology
*/
 
#ifndef __COMSDBGMESSAGES_H__
#define __COMSDBGMESSAGES_H__


#include <e32base.h>
#include "comsdbgsvr.h"

class MLogMessageProtocol;

class CLogMessageBase : public CBase
	{
public:
	void virtual Invoke(MLogMessageProtocol& aProtocol) = 0;
	};

class CLogStringMessage : public CLogMessageBase
	{
public:
	virtual void Invoke(MLogMessageProtocol& aProtocol);
	CLogStringMessage(HBufC8* aString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId);
	~CLogStringMessage();
private:
	HBufC8* iLogString;
	TBufC8<KMaxTagLength> iSubsystem;
	TBufC8<KMaxTagLength> iComponent;
	TThreadId iThreadId;
	};

class CLogBinaryString : public CLogMessageBase
	{
public:
	virtual void Invoke(MLogMessageProtocol& aProtocol);
	CLogBinaryString(HBufC8* aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	~CLogBinaryString();
private:
	HBufC8* iBinaryString;
	TBuf8<KMaxTagLength> iSubsystem;
	TBuf8<KMaxTagLength> iComponent;
	};

class CTimeUpdateMessage : public CLogMessageBase
	{
public:
	CTimeUpdateMessage(const TTime& aTime);
	virtual void Invoke(MLogMessageProtocol& aProtocol);	
private:
	TTime iTime;
	};

class MIniLoggingMediaQuery;

class CMediaUpdateMessage : public CLogMessageBase
	{
public:
	CMediaUpdateMessage(MIniLoggingMediaQuery& aQuery);
	virtual void Invoke(MLogMessageProtocol& aProtocol);
private:
	TBuf8<KMaxMediaSettingLength> iLogMediaSetting;
	TBool       iForceFlushSetting;
	TFilePath iLogPathSetting;
	};

class CClearLogMessage : public CLogMessageBase
	{
public:
	CClearLogMessage(const TFullName& aName);
	virtual void Invoke(MLogMessageProtocol& aProtocol);
private:
	TFullName iThreadName;
	};

class CLogCommentMessage : public CLogMessageBase
	{
public:
	static CLogCommentMessage* NewL(const TDesC8& aComment);
	virtual void Invoke(MLogMessageProtocol& aProtocol);
	~CLogCommentMessage();
private:
	CLogCommentMessage();
private:
	HBufC8* iComment;
	};

class CShutDownMessage : public CLogMessageBase
	{
public:
	virtual void Invoke(MLogMessageProtocol& aProtocol);
private:
private:
	};

#endif //__COMSDBGMESSAGES_H__
