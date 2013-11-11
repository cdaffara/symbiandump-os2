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
//

/**
 @file
*/

#include <flogger.h>
#include "FLOGSTD.H"



#ifdef __WINS__
/**
stack for file logger

@internalComponent
*/
const TUint KFLoggerStackSize=0x3000;

/**
min heap size for file logger

@internalComponent
*/
const TUint KFLoggerMinHeapSize=0x1000;

/** 
define _FLOGGER_LARGE_HEAP if flogger is 
running out of memory (KErrNoMemory)
*/
#ifdef _FLOGGER_LARGE_HEAP

	/**
	Max heap size for file logger

	@internalComponent
	*/
	const TUint KFLoggerMaxHeapSize=0x100000;	/// 1M

#else

	/**
	Max heap size for file logger

	@internalComponent
	*/
	const TUint KFLoggerMaxHeapSize=0x40000;	/// 256K

#endif // _FLOGGER_LARGE_HEAP
#endif  //__WINS__

/** 
Uid for the Flogger server

@internalComponent
*/
const TInt KFLoggerServerUid=0x10004ee0;

/**
Literal which contains FLOGSVR

@internalComponent
*/
_LIT(KFLoggerServerExecutable,"FLOGSVR");

/**
FLogger class definition
*/

TInt FLogger::Start()
/**
Start the FLOGGER server.

@return KErrNone if successful 
*/
	{

	TRequestStatus stat;
	RProcess server;
	TInt ret=server.Create(KFLoggerServerExecutable,KNullDesC,TUidType(KNullUid,KNullUid,TUid::Uid(KFLoggerServerUid)),EOwnerThread);
	if (ret!=KErrNone)
		return ret;
	server.Rendezvous(stat);
	server.Resume();
	server.Close();
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt FLogger::Run(FLogger::TSignal& /*aSignal*/)
/**
Instantiates Active scheduler for the asynchronous request.

@param aSignal signal info
@return KErrNone if successful else KErrNoMemory
*/
	{

	__UHEAP_MARK;
	
	TInt ret = KErrNone;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		ret=KErrNoMemory;

	if (ret==KErrNone)
		ret=Init();

	RProcess::Rendezvous(ret);
	if (ret==KErrNone)
		CActiveScheduler::Start();

	delete CActiveScheduler::Current();
	delete cleanup;
	
	__UHEAP_MARKEND;

	return ret;
	}

TInt FLogger::Init()
/**
Initialises CFLoggerScheduler

@return KErrNone if successful
*/
	{

	TRAPD(ret,CFLoggerScheduler::NewL());
	return ret;
	}

/**
CFLoggerScheduler class definitions
*/

void CFLoggerScheduler::NewL()
/**
Installs CActiveScheduler
*/
	{

#ifdef __WINS__
// ensure the server thread has a handle on FLOGGER.DLL
	_LIT(KFLoggerLibraryName,"FLOGGER");
	RLibrary l;
	User::LeaveIfError(l.Load(KFLoggerLibraryName,KNullDesC));
#endif

	User::LeaveIfError(RThread::RenameMe(KFLoggerServerName));
//
	CFLoggerScheduler* self=new(ELeave) CFLoggerScheduler;
	CActiveScheduler::Install(self);
	self->iServer=CFileLoggerServer::NewL();
	}

CFLoggerScheduler::~CFLoggerScheduler()
/**
Destructor
*/
	{

	delete iServer;
	}

void CFLoggerScheduler::Error(TInt aError) const
/**
Message servicing failed

@param aError error value for failed message
*/
	{
	iServer->Error(aError);
	}

