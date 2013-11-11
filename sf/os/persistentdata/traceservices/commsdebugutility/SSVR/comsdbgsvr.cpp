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
// Implements the Flogger server side
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgsvr.h"
#include "comsdbgwriter.h"
#include "comsdbgstd.h"
#include "comsdbgmessages.h"

_LIT8(KOOMErrorString, "#Logs may be lost out of memory!! Further OOM conditions may not be recorded.\r\n");
_LIT8(KThreadDiedString, "#Something caused the secondary thread in comsdbgutil to die.\r\n");
_LIT8(KIniFileProblem, "#There is a problem with the ini file\r\n");
_LIT8(KIniFileUpdate, "#Ini file changes detected and noted.\r\n");
_LIT(KFloggerSecondaryThreadName, "Comsdbg2");





//
// CFileLoggerServer class definition
//


CFileLoggerServer* CFileLoggerServer::NewL()
	{

	CFileLoggerServer* r=new(ELeave) CFileLoggerServer();
	CleanupStack::PushL(r);
	r->ConstructL();
	r->StartL(KFLoggerServerName);
	CleanupStack::Pop();
	return r;
	}

CFileLoggerServer::CFileLoggerServer()
	: CServer2(EPriorityNormal,ESharableSessions)
	{}

void CFileLoggerServer::ConstructL()
/**
 *
 * @note Constructs the secondary thread, passing through a pointer to the log queue.
 * Constructs the file parser object and parses the file, attempting to copy the
 * file from the ROM if it is not found on C drive.
 * Constructs the time beat.
 * Performs the first UpdateMedia to ensure a media is set.
 */
	{
	User::LeaveIfError(iCriticalSection.CreateLocal());
	User::LeaveIfError(iCompletionSemaphore.CreateLocal(0));
	User::LeaveIfError(iFs.Connect());

#if defined (__WINS__)
	iDebugWriter = CDebugPortProtocol::NewL();
#endif

	User::LeaveIfError(iLogMessageArray.Append(NULL));
	iPreAllocatedErrorMessage = CLogCommentMessage::NewL(KOOMErrorString);
	iArrayHasSpaceForWrite = ETrue;
	
	TInt err(KErrNone);
	TPtrC iniFile(_L(""));
	
	/* We first check whether KFloggerIniFile (commsdbg.ini) exists,
	if it does we use that one, otherwise we use the old KFloggerIniOldFile
	(comsdbg.ini) instead (or rather, we try), else copy the default ini file. */
	TFindFile ff(iFs);
	
	err = ff.FindByDir(KFloggerIniFile, KFloggerIniDir);
		
	if(err != KErrNone)
		{
			// KFloggerIniFile was not there, lets use KFloggerIniOldFile
			err = ff.FindByDir(KFloggerIniOldFile, KFloggerIniDir);
		}
		
	if (err == KErrNone) 
      {
      // found the ini file to use, hold on to its name and location
      iniFile.Set (ff.File ());
      }
   else
		{
		// couldn't find an ini file with the new or old name, lets use ini file in the resource dir
		err = ff.FindByDir(KFloggerIniFile, KFloggerIniRscDir);
		
		// Get the correct system drive letter in a buffer.	
		TChar sysDriveLetter = RFs::GetSystemDriveChar();
		TBuf<1> sysDriveLetterBuf;
		sysDriveLetterBuf.Fill( sysDriveLetter, 1 );
			
       	// Get the full path to the default ini file including the system drive letter.
		TBuf<KMaxFileName> dfltIniFullPath( KFloggerDfltIniPartialPath );
		dfltIniFullPath.Insert( 0, sysDriveLetterBuf );
			
		if(err == KErrNone)
			{
			//only ini file is the default one in z:\resource
			CFileMan* fileMan = CFileMan::NewL(iFs);
			err = fileMan->Copy(KFloggerIniInROM, dfltIniFullPath, CFileMan::ERecurse);
			if (err==KErrNone)
				{
				err = fileMan->Attribs(dfltIniFullPath,0,KEntryAttReadOnly,TTime(0));
		   		iniFile.Set (dfltIniFullPath);
				}
			delete fileMan;
			}
		else
   			{
			// Get the full path to the old ini file including the system drive letter.
			TBuf<KMaxFileName> dfltIniOldIniFullPath( KFloggerIniOldFilePartialPath );
			dfltIniOldIniFullPath.Insert( 0, sysDriveLetterBuf );
			
  			// watch ini file, even tho it isnt there
   			// also create another file watched to watch for the old file for backward compatibility
   			iniFile.Set(dfltIniFullPath);
	 		iIniOldFileWatcher = CIniFileWatcher::NewL(iFs, *this, dfltIniOldIniFullPath);
			}
		}
	
	iIniFileParser = CIniFileParser::NewL(iFs);
   if (err==KErrNone)	//error getting the ini file or setting attributes
      {
   	err=iIniFileParser->ParseIniFile(iniFile);
      }
   else
      {
      // no default ini file to copy either, so lets just pretend there is an empty file
      err = KErrNone;
      }
			
	// create and set in motion the second thread
	// we can only create the second thread thread once we know whether we are running
	// synchronous or not. If we still don't know, then we'll force it to false.
	// When flogger first starts, the flush setting is not known because the ini file
	// parsing may fail. However, once client's start calling this function, we must
	// decide whether to flush or not since it is not easy to switch from
	// non-flushing to flushing. Thus, instead, the first time this function
	// is called, we make a decision
	iIniFileParser->FinaliseFlushSetting();
	TBool currentFlushSetting;
	iIniFileParser->FlushingOn(currentFlushSetting);
	// the second thread needs to have been created before we need to place anything
	// into the log queue, since the 2nd thread contains the semaphore we are using
	// to count the # of mesgs in the queue.
	iSecondaryThread = CSecondaryThread::NewL(*this,currentFlushSetting);

	// update the media regardless of whether the parsing worked
	// since we need to get the default set if needs be.
	UpdateMediaL();
	
	if (err!=KErrNone)	//Problem with ini file reading. Stick a message in the log...
		{
		CLogCommentMessage* message = CLogCommentMessage::NewL(KIniFileProblem);
		err = AppendAndGiveOwnership(message);
		if (err != KErrNone)
			{
			delete message;
			User::Leave(err);
			}
		}

	iIniFileWatcher = CIniFileWatcher::NewL(iFs, *this, iniFile);
	iTimeManager = CTimeManager::NewL(*this);
	}

void CFileLoggerServer::UpdateMediaL()
	{
	CMediaUpdateMessage* message = new(ELeave) CMediaUpdateMessage(*iIniFileParser);
	CleanupStack::PushL(message);
	if (AppendAndGiveOwnership(message)!=KErrNone)	//Problem => Leave media as it was.
		{
		CleanupStack::Pop(message);
		delete message;
		return;
		}
	CleanupStack::Pop(message);
	}

CFileLoggerServer::~CFileLoggerServer()
	{
	delete iSecondaryThread;
	delete iTimeManager;
	delete iIniFileWatcher;
	delete iIniOldFileWatcher;
	delete iIniFileParser;
	delete iPreAllocatedErrorMessage;
	iFs.Close();
	iCriticalSection.Close();
	iCompletionSemaphore.Close();
	iLogMessageArray.ResetAndDestroy();
#if defined (__WINS__)
	delete iDebugWriter;
#endif
	}


CSession2* CFileLoggerServer::NewSessionL(const TVersion &aVersion ,const RMessage2& /*aMessage*/) const
/**
 * Create a new server session. Check that client is using current or older interface and make a new session.
 * @note Called by kernel after RFileLogger::DoConnect().
 */
	{

	TVersion v(KFLogSrvMajorVersionNumber,KFLogSrvMinorVersionNumber,KFLogSrvBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);
		}
		
	return CFileLogSession::NewL(*(const_cast<CFileLoggerServer*>(this)), *iIniFileParser);
	}

void CFileLoggerServer::RePrepareForOOML()
/**
 * Called to ensure there is space for the OOM error msg in log queue.
 */
	{
	if (!iPreAllocatedErrorMessage)
		{
		iPreAllocatedErrorMessage = CLogCommentMessage::NewL(KOOMErrorString);
		}
	//Must reserve 1 space in array for error handling or leave.
	User::LeaveIfError(iLogMessageArray.Append(NULL));
	//We've got everything we need should we hit an OOM to guarentee we can get an error in the log.
	iArrayHasSpaceForWrite=ETrue;
	}

TInt CFileLoggerServer::AppendAndGiveOwnership(CLogMessageBase* aMessage)	
/**
 * Append a log package to the queue end.
 * @note Entry only allowed for primary (producer) thread.
 * @note If win32 and debug port logging on, package is written immediately to the debug port as well.
 * @note If no space has been allocated for the OOM error msg, then this is done first.
 * @note If flushing (synchronous) operation then waits for completion.
 @return KErrNoMemory
 */
	{
	iCriticalSection.Wait();
#if defined (__WINS__)
	if (iIniFileParser->Win32DebugEnabled())
		{
		aMessage->Invoke(*iDebugWriter);
		}
#endif
	TInt err(KErrNone);
	if (!iArrayHasSpaceForWrite) 	//We ran into low memmory a while ago, time to get set up again.
									//iArrayHasSpaceForWrite==ETrue <==> 1 free slot in the array
									//iArrayHasSpaceForWrite==EFalse <==> 0 free slots in the array
									//Above  MUST be true or we'll go horribly wrong!!
									//RePrepareForOOML must be done in CS since we'll be mucking about with the array.
		{
		TRAP(err, RePrepareForOOML());
		if (err!=KErrNone)
			{
			iCriticalSection.Signal();
			return err;
			}
		}
	//If we get to here then we must have 1 space in the array. First try to expand
	//then if that works stick the message in the array, otherwise return error.
	if ((err = iLogMessageArray.Append(NULL))==KErrNone)
		{
		const TInt arrayCount(iLogMessageArray.Count());
		iLogMessageArray[arrayCount-2] = aMessage;
		iSecondaryThread->SignalRequestSemaphore();
		}
	iCriticalSection.Signal();

	if (err==KErrNone)
		{
		TBool flushSetting;
		iIniFileParser->FlushingOn(flushSetting);

		if (flushSetting == EFlushOn)
			{
			iCompletionSemaphore.Wait(); // one for each signal of the request semaphore
			}
		}
	return err;
	}

void CFileLoggerServer::PutOOMErrorInLog()
	{
	//Best attempt to put error in log, if there's no space then we can't
	//We don't try to allocate space in the array. The reasons for this are left as
	//an excercise to whomever is unfortunate enough to be trying to debug this.
	//Basically, this is the only place the last slot of the array can ever be filled,
	//so if iArrayHasSpaceForWrite==EFalse we're guaranteed to have got an error in the
	//log already earlier on, and since
	//in the AppendAndGiveOwnership method we fail if we cant realocate the error message
	//we're guarenteed that the last line in the log is and error message already.
	if (iArrayHasSpaceForWrite)
		{
		iCriticalSection.Wait();
		const TInt arrayCount(iLogMessageArray.Count());
		iLogMessageArray[arrayCount-1] = iPreAllocatedErrorMessage;
		iPreAllocatedErrorMessage=NULL;
		iArrayHasSpaceForWrite=EFalse;
		iCriticalSection.Signal();
		iSecondaryThread->SignalRequestSemaphore();

		TBool flushSetting;
		iIniFileParser->FlushingOn(flushSetting);

		if (flushSetting == EFlushOn)
			{
			iCompletionSemaphore.Wait(); // one for each signal of the request semaphore
			}

		}

	}


void CFileLoggerServer::GetFirstMessageAndTakeOwnership(CLogMessageBase*& aMessage)	
/*
 * Remove the message at the head of the log queue.
 * @note Entry only allowed for secondary (consumer) thread.
 * @note Blocks if no messages in the log queue.
 */

	{
	// Wait on the request semaphore, since we are using that to communicate the number of
	// outstanding log items in the queue.
	User::WaitForAnyRequest();
	iCriticalSection.Wait();
	aMessage = iLogMessageArray[0];
	iLogMessageArray.Remove(0);
	iCriticalSection.Signal();
	}



void CFileLoggerServer::SignalCompletionSemaphore()
// signal the completion semaphore. Called by the slave/consumer thread when it is
// done with the current message. Since the slave thread doesn't know if flushing
// is on, it will call this regardless of whether we actually need to signal the
// semphore or not.
	{

	TBool flushSetting;
	iIniFileParser->FlushingOn(flushSetting);

	if (flushSetting == EFlushOn)
		{
		iCompletionSemaphore.Signal();
		}
	}



TInt CFileLoggerServer::RunError(TInt aError)
/**
 * Leave has occured in CFileLogSession::ServiceL.
 * Usually this is because the appending of the message to the queue has failed
 * due to the queue being filled.
 */
	{
	PutOOMErrorInLog();
	return CServer2::RunError(aError);
	}


void CFileLoggerServer::IniFileChanged(TDesC &aIniFile)
/**
Called by the file watcher when the ini file changes.
Any OOM problems are ignored
*/
	{
	CLogCommentMessage* message = NULL;
	TRAPD(err, message = CLogCommentMessage::NewL(KIniFileUpdate))
	if ((err == KErrNone) && (message))
		{
		err = AppendAndGiveOwnership(message);
		if (err != KErrNone)
			{
			delete message;
			}
		}
	err = iIniFileParser->ParseIniFile (aIniFile);

	// update media regardless of success, since we may want to
	// set the default media. update media will only leave if memory full.
	// in which case we cant output an error msg anyway.
	TRAPD(err2, UpdateMediaL());
	
	if (err!=KErrNone)	//problem parsing ini file, leave settings as they are and carry on.
		{
		CLogCommentMessage* message = NULL;
		TRAP(err, message = CLogCommentMessage::NewL(KIniFileProblem))
		if ((err == KErrNone) && (message))
			{
			err = AppendAndGiveOwnership(message);
			if (err != KErrNone)
				{
				delete message;
				}
			}
		return;
		} 
	else if (err2 != KErrNone)
		{
		// memory full, but cant even output msg to say this
		}


	//Ignore error. Above can only fail due to OOM, so carry on regardless.
	CSession2* p=NULL;
	iSessionIter.SetToFirst();
	while ((p=iSessionIter++)!=NULL)
		{
		static_cast<CFileLogSession*>(p)->IniFileChanged();
		}
	}

#ifdef _DEBUG
void CFileLoggerServer::__DbgKillTimeManager()
	{
	delete iTimeManager;
	iTimeManager=NULL;
	}
#endif	//_DEBUG
//
// CFileLogSession class definition
//

CFileLogSession* CFileLogSession::NewL(MLogArrayAccess& aArrayAccess, const MIniFlushModeAndLogValidQuery& aLogValidQuery)
/**
 * Construct new server end of session.
 * @note Only called from CFileLoggerServer::NewSessionL()
 */
	{

	CFileLogSession* self = new(ELeave) CFileLogSession(aArrayAccess, aLogValidQuery);
	return self;
	}

CFileLogSession::CFileLogSession(MLogArrayAccess& aArrayAccess, const MIniFlushModeAndLogValidQuery& aLogValidQuery)
	: iArrayAccess(aArrayAccess), iFlushModeLogValidQuery(aLogValidQuery), iLogValid(KLoggingOnOffDefault)
	{}


CFileLogSession::~CFileLogSession()
	{
	}

void CFileLogSession::ServiceL(const RMessage2& aMessage)
/**
 * Processes message from client-side (RFileLogger)
 * @note Most messages result in logs being added to the queue. If
 * synchronous logging is on, this function will wait until the queue is then emptied.
 */
	{
	RThread clientThread;
	TBool flushOn;
	iFlushModeLogValidQuery.FlushingOn(flushOn);

	// All of the common & performance-critical requests need the client thread id. Note that RFileLogger
	// sessions are shareable; the thread now logging may be other than that which created the session
	User::LeaveIfError(aMessage.Client(clientThread));
	iThreadId = clientThread.Id();



	switch(aMessage.Function())
		{
		case EStaticWriteToLog:
			{
			aMessage.ReadL(0, iSubsystem);
			aMessage.ReadL(1, iComponent);
			if (iFlushModeLogValidQuery.LogValid(iSubsystem, iComponent))
				{
				HBufC8* stringOnHeap =  HBufC8::NewLC(aMessage.Int3());
				TPtr8 ptrToString(stringOnHeap->Des());
				aMessage.ReadL(2, ptrToString);
				if (!flushOn)
					{
					//We're done with the client now, so we can complete its request.
					aMessage.Complete(KErrNone);
					}
				CLogStringMessage* logMessage = new(ELeave) CLogStringMessage(stringOnHeap, iSubsystem, iComponent, iThreadId);
				CleanupStack::Pop(stringOnHeap);
				TInt err = iArrayAccess.AppendAndGiveOwnership(logMessage);
				if(err != KErrNone)
					{
					delete logMessage;
					User::Leave(err);
					}
				if (flushOn)
					{
					aMessage.Complete(KErrNone);
					}
				}
			else
				{
				//We've done with the client now, so we can complete it's request.
				aMessage.Complete(KErrNone);
				}
			break;
			} 
		case EClearLog:
			{
			CheckClientHasSetTagsL(aMessage);
			CClearLogMessage* clearLogMessage = new(ELeave) CClearLogMessage(clientThread.FullName());
			TInt err = iArrayAccess.AppendAndGiveOwnership(clearLogMessage);
			if(err != KErrNone)
				{
				delete clearLogMessage;
				User::Leave(err);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ESetLogTag:
			{
			aMessage.ReadL(0, iSubsystem);
			aMessage.ReadL(1, iComponent);
			iSetLogMessage = aMessage;
			SetLoggingOnOffInClient();
			aMessage.Complete(KErrNone);
			break;
			}
		case EWriteToLog:
			{
			CheckClientHasSetTagsL(aMessage);
			HBufC8* stringOnHeap =  HBufC8::NewLC(aMessage.Int1());
			TPtr8 ptrToString(stringOnHeap->Des());
			aMessage.ReadL(0, ptrToString);
			if (!flushOn)
				{
				//We're done with the client now, so we can complete its request.
				aMessage.Complete(KErrNone);
				}
			CLogStringMessage* logMessage = new(ELeave) CLogStringMessage(stringOnHeap, iSubsystem, iComponent, iThreadId);
			CleanupStack::Pop(stringOnHeap);//Above call takes ownership of stringOnHeap
			TInt err = iArrayAccess.AppendAndGiveOwnership(logMessage);
			if(err != KErrNone)
				{
				delete logMessage;
				User::Leave(err);
				}
			if (flushOn)
				{
				aMessage.Complete(KErrNone);
				}
			
			break;
			}
		case EWriteBinary:
			{
			CheckClientHasSetTagsL(aMessage);
			HBufC8* stringOnHeap = HBufC8::NewLC(aMessage.Int1());
			TPtr8 ptrToString(stringOnHeap->Des());
			aMessage.ReadL(0, ptrToString);
			CLogBinaryString* logMessage = new(ELeave) CLogBinaryString(stringOnHeap, iSubsystem, iComponent);
			CleanupStack::Pop(stringOnHeap);//Above call takes ownership of stringOnHeap
			if (!flushOn)
				{
				//We're done with the client now, so we can complete its request.
				aMessage.Complete(KErrNone);
				}

			TInt err = iArrayAccess.AppendAndGiveOwnership(logMessage);
			if(err != KErrNone)
				{
				delete logMessage;
				User::Leave(err);
				}
			if (flushOn)
				{
				aMessage.Complete(KErrNone);
				}
			break;
			}
#ifdef _DEBUG	//These methods are only accessible in debug.
		case EShutDownServer:
			{
			CActiveScheduler::Stop();
			aMessage.Complete(KErrNone);
			break;
			}
		case ESetHeapFailure:
			{
			//we need to stop the timer otherwise server will
			//keep allocing when we're doing heap failure test which makes the test fail.
			const_cast<CFileLoggerServer*>(static_cast<const CFileLoggerServer*>(Server()))->__DbgKillTimeManager();
			__UHEAP_FAILNEXT(aMessage.Int0());
			aMessage.Complete(KErrNone);
			break;
			}
#endif	//_DEBUG	
		default:
			{
 			aMessage.Panic(KFloggerServerPanic, EBadMessageFunction);
			}
		}
	clientThread.Close();
	}

void CFileLogSession::CheckClientHasSetTagsL(const RMessage2& aMessage)
/**
 * Ensure for a connection that the client has set the tags, otherwise panic.
 * @param aMessage the current client message in progress
 * @note Tags are kept server side so that we don't need to store the tags in the client and pass them through with each request.
 */

	{
	if (iSetLogMessage.IsNull())
		{
		aMessage.Panic(KFloggerPanic, ESetLogTagsNotCalled);	//Client ain't called set log tags.
		User::Leave(KErrGeneral);
		}

	}

void CFileLogSession::SetLoggingOnOffInClient()
	{
	const TBool currentLoggingOnOffState = iLogValid;
	iLogValid = iFlushModeLogValidQuery.LogValid(iSubsystem, iComponent);
	if (currentLoggingOnOffState!=iLogValid)	//Logging on/off has changed. Set state in client
		{
		// ignore the error returned
		(void)iSetLogMessage.Write(2, TPckgBuf<TBool>(iLogValid));
		}
	}

void CFileLogSession::IniFileChanged()
	//We only need to update clients that have called SetLogTags
	//We don't update clients that are using the static API client side.
	{
	if (!iSetLogMessage.IsNull())
		{
		SetLoggingOnOffInClient();
		}
	}

/////////////////////////////////////////////////////////////////

CSecondaryThread* CSecondaryThread::NewL(MLogArrayAccess& aArrayAccess, TBool aFlushOn)
	{
	CSecondaryThread* self = new(ELeave) CSecondaryThread(aArrayAccess,aFlushOn);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSecondaryThread::CSecondaryThread(MLogArrayAccess& aArrayAccess, TBool aFlushOn)
: CActive(EPriorityHigh), iArrayAccess(aArrayAccess), iFlushingOn(aFlushOn)
	{}

void CSecondaryThread::RunL()
/**
 * Runs when second thread dies, and simply restarts it again.
 * @note On death a thread-death message is placed in the queue so that
 * a user is aware this happened.
 * @note StartSecondaryThread issues a request to be informed of thread death,
 * so starts active object again.
 */
	{
	StartSecondaryThreadL(ETrue);
	}

void CSecondaryThread::DoCancel()
/**
 * Appends a special shutdown message into the log array. When this reaches
 * the head and is run, it shuts down the second thread.
 * @note Logs onto second thread and waits for it to finish.
 */

	{
	iSecondaryThread.LogonCancel(iStatus);
	if (iArrayAccess.AppendAndGiveOwnership(iShutDownMessage)!=KErrNone)
		{	//Nothing else we can do here. We'll have to just kill the other thread.
		iSecondaryThread.Kill(KErrGeneral);
		}
	else
		{
		iShutDownMessage = NULL;
		TRequestStatus status(KRequestPending);
		iSecondaryThread.Logon(status);
		User::WaitForRequest(status);
		}
	}

void CSecondaryThread::StartSecondaryThreadL(TBool aRestarting)
/**
 * Start the second/consumer/slave thread and issue a request to be told when it dies.
 */
	{
	TRequestStatus stat;
	
	User::LeaveIfError(iSecondaryThread.Create(KFloggerSecondaryThreadName,CLogManager::ThreadEntryPoint,KDefaultStackSize,NULL,&iArrayAccess));

	iSecondaryThread.Rendezvous(stat);

	if (iFlushingOn)
		{
		iSecondaryThread.SetPriority(EPriorityAbsoluteHigh);  //was EPriorityMuchMore
		} 
	else
		{
		iSecondaryThread.SetPriority(EPriorityAbsoluteForeground);  // was EPriorityMuchLess
		}

	iSecondaryThread.Resume();

	User::WaitForRequest(stat);

	iSecondaryThread.Logon(iStatus);

	if (aRestarting)
		{	
		CLogCommentMessage* errorMessage = CLogCommentMessage::NewL(KThreadDiedString);
		TInt err = iArrayAccess.AppendAndGiveOwnership(errorMessage);
		if(err != KErrNone)
			{
			delete errorMessage;
			User::Leave(err);
			}
		}
		
	SetActive();

	}

void CSecondaryThread::ConstructL()
/**
 * Contructs and kicks off the secondary thread, while also issuing a request to be informed
 * if the thread dies, which will run the active object
 */
	{
	CActiveScheduler::Add(this);
	//Preallocate a shutdown message 
	iShutDownMessage = new(ELeave) CShutDownMessage;

	StartSecondaryThreadL(EFalse);
	}

TInt CSecondaryThread::RunError(TInt /*aError*/)
	{
	CActiveScheduler::Stop();	//What the hell happened! Shut the server down
	return KErrNone;
	}

CSecondaryThread::~CSecondaryThread()
	{
	Cancel();
	delete iShutDownMessage;
	iSecondaryThread.Close();
	}

