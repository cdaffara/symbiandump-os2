// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "FLOGSVR.H"
#include "FLOGMAN.H"
#include "FLOGSTD.H"

/**
@internalComponent
*/
const TInt KOpenLogFilesGranularity=5;

/* Internal error. Use positive number to ensure no conflict with other error code
    Indicate that client message has panic'ed and should not be completed.
*/
const TInt KErrMessagePanic = 1;  

/**
panics for log client

@internalComponent
*/
enum TLogClientPanic 
    {
	ELogDirectoryNameDoesNotExist =1,   ///<  panic if no log directory name 
	ELogFileNameDoesNotExist	        ///< panic if file name doesnot exist 
	};

/**
CFileLoggerServer class definition
*/

CFileLoggerServer* CFileLoggerServer::NewL()
/**
Creates new CFileLoggerServer object

@return Pointer to CFileLoggerServer object
*/
	{

	CFileLoggerServer* r=new(ELeave) CFileLoggerServer();
	CleanupStack::PushL(r);
	r->ConstructL();
	r->StartL(KFLoggerServerName);
	CleanupStack::Pop();
	return r;
	}

CFileLoggerServer::CFileLoggerServer()
 	: CServer2(EPriority)
 	{}

void CFileLoggerServer::ConstructL()
	{

	iLoggerManager=CFileLoggerManager::NewL();
	iSessionCounter=CFileLogSessionCounter::NewL();
	}

CFileLoggerServer::~CFileLoggerServer()
/**
Destructor
*/
	{

	delete iSessionCounter;
	delete iLoggerManager;
	}

CFileLogSessionCounter* CFileLoggerServer::SessionCounter() const
	{

	return iSessionCounter;
	}

void CFileLoggerServer::Error(TInt aError)
	{

	Message().Complete(aError);
	ReStart();
	}

CSession2* CFileLoggerServer::NewSessionL(const TVersion &aVersion, const RMessage2& /* aMessage */) const
/**
Create a new server session. Check we're the right version and make a new session

@param aVersion version of new session
*/
	{

	TVersion v(KFLogSrvMajorVersionNumber,KFLogSrvMinorVersionNumber,KFLogSrvBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);
	
	iSessionCounter->CancelTimer();
	return CFileLogSession::NewL(CONST_CAST(CFileLoggerServer*,this),iLoggerManager);
	}
/**
CFileLogSessionCounter class definition 
*/

CFileLogSessionCounter* CFileLogSessionCounter::NewL()
/**
Creates new CFileLogSessionCounter object

@return Pointer to CFileLogSessionCounter object
*/
	{
	
	CFileLogSessionCounter* r=new(ELeave) CFileLogSessionCounter();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CFileLogSessionCounter::~CFileLogSessionCounter()
/**
Destructor
*/
	{

	delete iShutdownTimer;
	}

CFileLogSessionCounter::CFileLogSessionCounter()
	: iSessionCount(0)
/**
Default Constructor
*/
	{}

void CFileLogSessionCounter::ConstructL()
	{

	iShutdownTimer=CShutdownTimer::NewL();
	}
	
void CFileLogSessionCounter::IncrementSessionCount()
/**
Increments the Session count 
*/
	{

	iSessionCount++;
	}

void CFileLogSessionCounter::DecrementSessionCount()
/**
Decrements the Session count
*/
	{

	iSessionCount--;
	if (iSessionCount<=0)
		iShutdownTimer->After(KShutdownPause);
	}

void CFileLogSessionCounter::CancelTimer()
/**
Cancels the timer 
*/
	{

	iShutdownTimer->Cancel();
	}	

/**
CShutdownTimer class definitions
*/

CShutdownTimer* CShutdownTimer::NewL()
/**
Creates new CShutdownTimer object
@return Pointer to CShutdownTimer object
*/
	{

	CShutdownTimer* r=new(ELeave) CShutdownTimer();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CShutdownTimer::~CShutdownTimer()
/**
Destructor
*/
	{}

CShutdownTimer::CShutdownTimer()
	: CTimer(EPriorityIdle)
/**
Default Constructor
*/
	{
	CActiveScheduler::Add(this);
	}

void CShutdownTimer::RunL()
/**
*/
	{

	CActiveScheduler::Stop();
	}

/**
CFileLogSession class definition
*/

CFileLogSession* CFileLogSession::NewL(CFileLoggerServer* aServer, CFileLoggerManager* aManager)
 	{
   
 	CFileLogSession* r=new(ELeave) CFileLogSession(aServer,aManager);
 	CleanupStack::PushL(r);
 	r->ConstructL();
 	CleanupStack::Pop();
 	return r;
 	}

CFileLogSession::CFileLogSession( CFileLoggerServer* aServer, CFileLoggerManager* aManager)
 	: CSession2(), iLoggerServer(aServer), iLoggerManager(aManager)
 	{}

void CFileLogSession::ConstructL()
	{

	iLoggerServer->SessionCounter()->IncrementSessionCount();		// should be done first because it will be decremented in the destructor
	iOpenLogFiles=new(ELeave) CArrayFixFlat<TLogFile>(KOpenLogFilesGranularity);
 	}

CFileLogSession::~CFileLogSession()
/**
Destructor
*/
	{
	if  (iOpenLogFiles)
		{
	TInt count=iOpenLogFiles->Count();
	TInt i;
	for (i=0; i<count; i++)
		{
		iLoggerManager->CloseLog(iOpenLogFiles->At(i));
		}
	iOpenLogFiles->Delete(0,count);
	delete iOpenLogFiles;
		}
	iLoggerServer->SessionCounter()->DecrementSessionCount();		// starts the timer if this is the last session
	}

void CFileLogSession::ServiceL(const RMessage2& aMessage)
/**
@internalTechnology
*/
	{
  
 	TRAPD(ret,DispatchMessageL(aMessage));
 	if (ret!=KErrMessagePanic)
 	    {
        aMessage.Complete(ret);
 	    }
 	}

void CFileLogSession::DispatchMessageL(const RMessage2& aMessage)
 	{
 
 	TInt func=aMessage.Function();
 	if (func!=ECreateLog && func!=EWriteLog && func!=ECloseLog && func!=ECreateWriteAndCloseLog)
 		User::Leave(KErrNotSupported);
 
 	TLogFile log;
 	TPckg<TLogFile> logPckg(log);
 	aMessage.ReadL(0,logPckg);
 	if (log.Directory().Length()<=0)
        {
        aMessage.Panic(KFLoggerServerName,ELogDirectoryNameDoesNotExist);
        User::Leave(KErrMessagePanic);
        }
    
    if (log.Name().Length()<=0)
        {
        aMessage.Panic(KFLoggerServerName,ELogFileNameDoesNotExist);
        User::Leave(KErrMessagePanic);
        }
 
 //	TBuf8<KLogBufferSize> buf;
 	TBuf8<1600> buf;
 	if (func==EWriteLog || func==ECreateWriteAndCloseLog)
 	    {
 		aMessage.ReadL(1,buf);
 	    }
 
 	switch (func)
 		{
 	case ECreateLog:
 		OpenLogL(log);
 		aMessage.WriteL(0,logPckg);
 		break;
 
 	case EWriteLog:
 		iLoggerManager->WriteToLogL(log,buf);
 		break;
 
 	case ECloseLog:
 		CloseLog(log);
 		break;
 
 	case ECreateWriteAndCloseLog:
  		{
  		OpenLogL(log); // Ok to leave here; assume that log not left open
 		TInt rc = aMessage.Write(0,logPckg);
  		if (rc == KErrNone && log.Valid())
  			TRAP(rc,iLoggerManager->WriteToLogL(log,buf));
    		CloseLog(log);
  		User::LeaveIfError(rc);
    		break;
  		}
 
 	default:
 		User::Leave(KErrNotSupported);
 		break;
 		}
 	}
 

void CFileLogSession::OpenLogL(TLogFile& aLogFile)
/**
Opens log file

@param aLogFile Log file name
*/
	{

	iOpenLogFiles->AppendL(aLogFile);
	iLoggerManager->FindOrCreateLogL(aLogFile);
	}

void CFileLogSession::CloseLog(TLogFile& aLogFile)
/**
Closes the log file

@param aLogFile Log file name
*/
	{

	iLoggerManager->CloseLog(aLogFile);
	TInt count=iOpenLogFiles->Count();
	TInt i=0;
	for (i=0; i<count; i++)
		{
		if (iOpenLogFiles->At(i)==aLogFile)
			{
			iOpenLogFiles->Delete(i,1);
			break;
			}
		}	

	__ASSERT_DEBUG(i<=count, User::Invariant());
	}
			
