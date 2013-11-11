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
// Comms debug utility (File and Serial logger) Client side internals header
// 
//

/**
 @file
 @internalTechnology
*/
 
#ifndef __COMSDBGSTD_H__
#define __COMSDBGSTD_H__

#include "comsdbgsvr.h"
#include "comsdbgaux.h"


const TInt KFLogSrvMajorVersionNumber=2;
const TInt KFLogSrvMinorVersionNumber=0;
const TInt KFLogSrvBuildVersionNumber=0;
const TBool KLoggingOnOffDefault = ETrue;	///< Needed to tie together default setting in the client and server.
const TText KFullStopChar='.';
const TText8 KFullStopChar8='.';

_LIT(KFLoggerServerName,"!Comsdbg server");	///< Identifier to symbian OS server system.




/**
 * Message types that are passed from client to server.
 *
 */
enum TFileLoggerOperations 
	{
	EStaticWriteToLog,
	EWriteToLog,
	ESetLogTag,
	EClearLog,
	EWriteBinary,
	EShutDownServer,
	ESetHeapFailure
	};


/**
 * FlogOverflow16 - class to quietly handle descriptor overflows 
 *
 * This class overrides the "Overflow" function in the
 * TDes16's own overflow class. The overflow function is
 * designed to quietly ignore any overflow which occurs when
 * copying/appending strings, thus
 * enabling flogger to implicitly truncate the
 * logging strings where necessary.
 */
class TFlogOverflow16  : public TDes16Overflow
	{
public:
	void Overflow(TDes16& /*aDes*/) { }
	};


/**
 * FlogOverflow8 - class to quietly handle descriptor overflows 
 *
 * This class overrides the "Overflow" function in the
 * TDes8's own overflow class. The overflow function is
 * designed to quietly ignore any overflow which occurs when
 * copying/appending strings, thus
 * enabling flogger to implicitly truncate the
 * logging strings where necessary.
 */
class TFlogOverflow8  : public TDes8Overflow
	{
public:
	void Overflow(TDes8& /*aDes*/) { }
	};

/**
 * RFileLoggerBody - class to provide all internal data for the client side 
 *
 * This class contains all data members which would otherwise be in the
 * RFileLogger class. They are instead in this file since that keeps the
 * size of RFileLogger to a minimum, and also means its size will not
 * change very often.
 * By keeping the RFileLogger class size to a minimum, it means in release
 * builds where no logging is to be included, only a skeleton of the
 * RFileLogger is built into each component, numbering a handful of bytes.
 */ 
class RFileLoggerBody : public RSessionBase
	{
public:
	RFileLoggerBody();
	
	// CreateSession is a protected member of RSessionBase, so we must derive from
	// it and provide a means to call this via pass-through inline functions.
	// In a normal client-server situation RFileLogger itself would derive from
	// RSessionBase.
	inline TInt DoCreateSession(const TDesC& aServer,const TVersion& aVersion,TInt aAsyncMessageSlots);
	inline TInt DoSendReceive(TInt aFunction,const TIpcArgs& aArgs) const;
	inline TInt DoSendReceive(TInt aFunction) const;
	inline void Close();
		
public:
	// we have no access methods to access these so they are public instead.
	TPckgBuf<TBool> iLoggingOnPckg;
	TFlogOverflow16 iFlogOverflow16;
	TFlogOverflow8 iFlogOverflow8;
	};

/**
FLogger - flogger server process startup class 
This class contains functions to start the flogger server.
@internalComponent 
 */ 
class FLogger
	{
public:
	class TSignal;
public:
	IMPORT_C static TInt Start();
	IMPORT_C static TInt Run();
private:
	static TInt Init();
	};

/**
 @removed Stub export for flogger.dll now that flogsvrl.dll contains the FLogger::Run()
 @internalComponent 
 */
IMPORT_C void ClientRunStubOrdinal1();

#include "comsdbg.inl"


#endif // __COMSDBGSTD_H__
