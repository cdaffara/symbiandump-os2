// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// system includes
#include <schtask.h>

// User includes
#include "SCHEXEC.H"
#include "SchLogger.h"
#include "SCHLOG.h"
#include "taskfile.h"

//Delay ensuring this thread is be preempted allowing client cleanup
//in case task file cannot be deleted
const TInt KClientCleanupDelay = 10000;

CTaskExecutor::CTaskExecutor(CSchLogManager& aSchLogManager)
:	CActive(EPriorityStandard), 
	iSchLogManager(aSchLogManager)
	{
	CActiveScheduler::Add(this);
	}


CTaskExecutor::~CTaskExecutor()
	{
	LOGSTRING("CTaskExecutor::~CTaskExecutor - start");
	iProcess.Close();

	//
	delete iClientFileName;
	delete iTaskFileName;
	delete iLogErrorMessage;
	LOGSTRING("CTaskExecutor::~CTaskExecutor - end");
	}


void CTaskExecutor::ConstructL(const TDesC& aTaskFileName, 
								const TDesC& aClientFileName, 
								const TDesC& aErrorMessage)
	{
	LOGSTRING("CTaskExecutor::ConstructL - start");
	// Store filename
	iTaskFileName = aTaskFileName.AllocL();
	iClientFileName = aClientFileName.AllocL();

	// In case there is an error...
	iLogErrorMessage = aErrorMessage.AllocL();
	
	LOGSTRING("CTaskExecutor::ConstructL - end");
	}


CTaskExecutor* CTaskExecutor::NewLC(const TDesC& aErrorMessage, 
									const TDesC& aTaskFileName, 
									const TDesC& aClientFileName,
									CSchLogManager& aSchLogManager)
	{
	LOGSTRING("CTaskExecutor::NewLC");
	CTaskExecutor* self = new(ELeave) CTaskExecutor(aSchLogManager);
	CleanupStack::PushL(self);
	self->ConstructL(aTaskFileName,	aClientFileName, aErrorMessage);
    return self;
	}

void CTaskExecutor::RunL()
	{
	LOGSTRING3("CTaskExecutor::RunL - task finished running [client: %S, task: %S]", iClientFileName, iTaskFileName);
	//
	// RunL is called when the process/thread terminates
	// so that any error conditions can be handled.
	TInt exitReason = iProcess.ExitReason();
	LOGSTRING2("CTaskExecutor::RunL - process exit reason was: %d", exitReason);

	// Close the process/thread
	iProcess.Close();
	
	// Check for error code
	if	(exitReason != KErrNone)
		{
		// Submit a log entry to record the error. 
		LOGSTRING2("CTaskExecutor::RunL - recording unclean process exit (%d) in the log engine", exitReason);
		if(iLogErrorMessage)
			iSchLogManager.LogError(*iLogErrorMessage,exitReason);
		else
			iSchLogManager.LogError(exitReason);			
		}

	// Clean up the file.  Only delete it here once task process has finished.
	// If task process never started then file is deleted in CClientProxy code.
	User::LeaveIfError(iFsSession.Connect());
	CleanupClosePushL(iFsSession);
	
	TInt fileDeleteErr = iFsSession.Delete(*iTaskFileName);

	// If unable to delete file wait and try again
	if (fileDeleteErr != KErrNone)
		{
		
		//Allow thread to be preempted to allow for cleanup of iProcess
		User::After(KClientCleanupDelay);
		
		fileDeleteErr = iFsSession.Delete(*iTaskFileName);
			
		// If still unable to delete file record the fact
		if (fileDeleteErr != KErrNone)
			{				
			if(iLogErrorMessage)
				{		
				iSchLogManager.LogError(*iLogErrorMessage, fileDeleteErr);
				}
			else
				{
				iSchLogManager.LogError(fileDeleteErr);
				}				
			}
		}
		
	//Calls iFsSession::Close() so no need to call explicitly
	CleanupStack::PopAndDestroy();

	// Delete outselves since we've finished
	LOGSTRING("CTaskExecutor::RunL - deleting ourself");
	delete this;
	}	

void CTaskExecutor::DoCancel()
	{
	LOGSTRING("CTaskExecutor::DoCancel - start");
	iProcess.LogonCancel(iStatus);
	// Delete file and ourselves since we can't do anything else.
	// We are in a bad state if we reach here but at least make the most of it.
	LOGSTRING("CTaskExecutor::DoCancel - deleting ourself");
	
	//Connect to file session
	TInt err = iFsSession.Connect();
	
	if(err == KErrNone)
		{
		err = iFsSession.Delete(*iTaskFileName);
		}
	
	// If unable to delete file record the fact
	if (err != KErrNone)
		{
		if(iLogErrorMessage)
			{
			iSchLogManager.LogError(*iLogErrorMessage, err);
			}
		else
			{
			iSchLogManager.LogError(err);
			}
		}
		
	//Close the file session
	iFsSession.Close();
	
	delete this;
	LOGSTRING("CTaskExecutor::DoCancel - end");
	}	

void CTaskExecutor::ExecuteL()
	{
	// If this leaves, CClientProxy should handle error....
	// CTaskScheduler::ExecuteClients() traps the leave and then calls
	// CClientProxy::FailToExecute() to handle the error.
#ifdef __SCHLOGGING__
	{
	TTime time; time.HomeTime();
	TDateTime due = time.DateTime();
	LOGSTRING8("CTaskExecutor::ExecuteL - Executing tasks at: [%02d/%02d/%d] @ %02d:%02d:%02d.%05d", due.Day(), (TInt) due.Month() + 1, due.Year(), due.Hour(), due.Minute(), due.Second(), due.MicroSecond());
	}
#endif

	// Create a new process and pass the name of the task file as the command line argument
	// (data for the target exe).
	LOGSTRING("CTaskExecutor::ExecuteL - creating process");
	TInt err = iProcess.Create(*iClientFileName, KNullDesC);
	
	// Will check the error, report the problem and leave (if err<KErrNone)
	// otherwise does nothing.
	LOGSTRING("CTaskExecutor::ExecuteL - checking process creation error code");
	CheckErrorAndLeaveL(err);
	
	// connect to new file session to avoid possible security
	// consequences if lauched process tries to use passed file in
	// subversive way.
	User::LeaveIfError(iFsSession.Connect());
	CleanupClosePushL(iFsSession);
	User::LeaveIfError(iFsSession.ShareProtected());
	
	User::LeaveIfError(iTaskFile.Open(iFsSession, *iTaskFileName, EFileRead));
	CleanupClosePushL(iTaskFile);
	
	// transfer file handle to launched process
	err = iTaskFile.TransferToProcess(iProcess, KTaskFsHandleIndex, KTaskFileHandleIndex);

	//Close task file and session handles
	//Calls iFsSession::Close() and iTaskFile::Close() so no need to call explicitly
	CleanupStack::PopAndDestroy(2);
	
	CheckErrorAndLeaveL(err);

	// Asynchronous logon: completes when process terminates with process exit code
	iProcess.Logon(iStatus);
	iProcess.Resume();

	SetActive();
	LOGSTRING("CTaskExecutor::ExecuteL - end");

	}

void CTaskExecutor::CheckErrorAndLeaveL(TInt aError)
	{
	if	(aError < KErrNone)
		{
		if(iLogErrorMessage)
			iSchLogManager.LogError(*iLogErrorMessage,aError);
		else
			iSchLogManager.LogError(aError);			
		User::Leave(aError);
		}
	}

