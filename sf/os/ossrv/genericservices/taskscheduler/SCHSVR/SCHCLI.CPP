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

// System includes
#include <logwraplimits.h>
#include <logwrap.h>
#include <schtask.h>


// User includes
#include "SCHCLI.H"
#include "SchLogger.h"
#include "SCHEXEC.H"
#include "SCHLOG.h"


// Constants
#define KEllipsis 0x2026
const TInt KTaskArrayGranularity = 1;
_LIT(KTaskNameSeparator, ", ");



//
// CClientProxy
//
CClientProxy::CClientProxy(RFs& aFsSession, CSchLogManager& aLogManager)
:	iFsSession(aFsSession), 
	iTasks(CScheduledTask::Offset()),
	iSchLogManager(aLogManager)
	{
	}

CClientProxy::CClientProxy(RFs& aFsSession, TInt aPriority, CSchLogManager& aLogManager)
:	iFsSession(aFsSession), 
	iTasks(CScheduledTask::Offset()),
	iSchLogManager(aLogManager)
	{
	iPriLink.iPriority = aPriority;
	}

CClientProxy::~CClientProxy()
	{
	LOGSTRING("CClientProxy::~CClientProxy - start");

	TDblQueIter<CScheduledTask> taskIter(iTasks);
	taskIter.SetToFirst();
	CScheduledTask* task;
	while ((task = taskIter++) != NULL)
		{
		RemoveTask(task);
		}
	delete iFileName;
	LOGSTRING("CClientProxy::~CClientProxy - end");
	} 

void CClientProxy::ConstructL(const TDesC& aFileName)
	{
	iFileName = aFileName.AllocL();
	}

CClientProxy* CClientProxy::NewL(RFs& aFsSession, 
								const TDesC& aFileName, 
								TInt aPriority,
								CSchLogManager& aLogManager)
	{
	CClientProxy* self = new(ELeave) CClientProxy(aFsSession, aPriority,aLogManager);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop();
	return self;
	}

CClientProxy* CClientProxy::NewL(RFs& aFsSession, 
								RReadStream& aStream,
								CSchLogManager& aLogManager)
	{
	CClientProxy* self = new(ELeave) CClientProxy(aFsSession,aLogManager);
	CleanupStack::PushL(self);
	aStream >> *self;
	CleanupStack::Pop();
	return self;
	}

TBool CClientProxy::IsEqual(const TDesC& aFilename, TInt aPriority) const
	{
	// Equality here means: same DLL filename, DLL ordinal & priority
	return ((*iFileName == aFilename) && (iPriLink.iPriority == aPriority));
	}

void CClientProxy::AddTask(CScheduledTask& aTask)
	{
	LOGSTRING("CClientProxy::AddTask - start");
	iTasks.Add(aTask);
	iUsers++;
	LOGSTRING("CClientProxy::AddTask - end");
	}

void CClientProxy::RemoveTask(CScheduledTask* aTask)
	{
	LOGSTRING("CClientProxy::RemoveTask - start");
	iUsers--;
	aTask->Remove();
	delete aTask;
	LOGSTRING("CClientProxy::RemoveTask - end");
	}

//
static void BuildTaskErrorMessage(const CArrayPtr<CScheduledTask>& aTasks, TDes& aErrorMessage)
//
//	Establish the names of all the tasks that potentially might fail
//	when executing the task. This is done *before* actually executing the
//	tasks as its not possible to discern (at failure time) which tasks
//	were actually executing (because we don't know at what time the task
//	executor was spawned).
//
	{
	const TInt count = aTasks.Count();
	for(TInt i=0; i<count; i++)
		{
		const TDesC& taskName = aTasks.At(i)->Info().iName;
		if	(aErrorMessage.Length() + taskName.Length() > KLogMaxSubjectLength - KTaskNameSeparator().Length())
			{
			// The task name that still needs adding to the subject line (of the
			// log entry) is too big to fit within KLogCallEventTypeUid characters.
			// Trim the task name so that it will fit, and append the ellipsis
			// character to indicate that more tasks were present but unable to be
			// shown.
			TInt lengthOfTaskNameToAccept = (KLogMaxSubjectLength - aErrorMessage.Length()) - KTaskNameSeparator().Length();
			if	(lengthOfTaskNameToAccept <= 0)
				break;
			aErrorMessage += taskName.Left(lengthOfTaskNameToAccept);
			aErrorMessage.Append(KEllipsis);
			
			// Exit the 'for' loop as there isn't any more room to hold subsequent
			// tasks that potentially failed...
			break;
			}
		else
			{
			aErrorMessage += taskName;
			}

		if	(i<count-1) // not the last taskName
			{
			// Append a comma
			aErrorMessage += KTaskNameSeparator;
			}
		}
	}
	
void CClientProxy::ExecuteTasks()
	{
	// generate an array of due tasks
	CArrayPtr<CScheduledTask>* tasks = NULL;
	TRAPD(err, tasks = DueTasksL());
	if(err)
		{
		iSchLogManager.LogError(err);
		return;
		}
	
	// Create the task error message, just in case an error occurs
	TBuf<KLogMaxSubjectLength> errorMessage;
	BuildTaskErrorMessage(*tasks, errorMessage);

	TInt error =DoExecuteTasks(*tasks, errorMessage);
	if(error)
		iSchLogManager.LogError(errorMessage,error);
	delete tasks;
	}

TInt CClientProxy::DoExecuteTasks(const CArrayPtr<CScheduledTask>& aTasks,
									const TDesC& aErrorMessage)
	{
	// Generate a unique task name
	TTime timenow;
	timenow.UniversalTime();
	TBuf<24> filename; // 20 is the longest decimal char length of a 64-bit number + 4 for extension
	filename.Format(_L("%Ld.tmp"),timenow.Int64());
	
	// Save & perform execution
	TRAPD(error, DoExecuteTasksL(aTasks, filename, aErrorMessage));

	if (error != KErrNone)
		{
		LOGSTRING2("CClientProxy::ExecuteTasksL - attempted to execute tasks, error was %d", error);
		// Cleanup the file that was created if the execution fails in any way. Note
		// that it might not have been created

		// If a debug build - record error
		TInt fileDeleteErr=iFsSession.Delete(filename);
		if (fileDeleteErr != KErrNone)
			{
			LOGSTRING2("CClientProxy::DoExecuteTasks - Failed to delete file. Error = %d", fileDeleteErr);
			}
		}
	return error;
	}

void CClientProxy::DoExecuteTasksL(const CArrayPtr<CScheduledTask>& aTasks, 
									const TDesC& aFileName,
									const TDesC& aErrorMessage)
	{
	// Save the tasks to disk
	SaveTasksToFileL(aTasks, aFileName);
	
	// Spawn the task executor active object (deletes itself upon completion)
	CTaskExecutor* executor = CTaskExecutor::NewLC(aErrorMessage,aFileName,*iFileName,iSchLogManager);
	executor->ExecuteL();
	CleanupStack::Pop(executor);
	// Don't delete executor here as it does it itself once 
	// the task has finished (delete this in its RunL() )
	}


CArrayPtr<CScheduledTask>* CClientProxy::DueTasksL()
//
//	Returns a copy of any due tasks
//
	{
	CArrayPtrFlat<CScheduledTask>* tasks = new(ELeave)CArrayPtrFlat<CScheduledTask> (KTaskArrayGranularity);
	CleanupStack::PushL(tasks);
	TDblQueIter<CScheduledTask> taskIter(iTasks);
	taskIter.SetToFirst();
	CScheduledTask* task;
	while ((task=taskIter++)!=NULL)
		{
		if (task->Due())
			{
			tasks->AppendL(task);
			}
		}
	CleanupStack::Pop(tasks);
	return tasks;
	}
	

CFileStore* CClientProxy::CreateStoreL(const TDesC& aName,TUint aFileMode)
//
//	To make the code clearer - called within a trap frame that shouldn't leave anything 
//	on cleanup stack.
//
	{
	CFileStore* store = CDirectFileStore::CreateLC(iFsSession, aName, aFileMode);
	CleanupStack::Pop(store);
	return store;
	}

void CClientProxy::SaveTasksToFileL(const CArrayPtr<CScheduledTask>& aTasks, const TDesC& aFileName)
	{
	LOGSTRING("CClientProxy::SaveTasksToFileL - start");

	// Create the file store
	LOGSTRING("CClientProxy::SaveTasksToFileL - creating store");
	CFileStore* store = CreateStoreL(aFileName, EFileWrite);
	CleanupStack::PushL(store);

	// Save the actual tasks to the store
	LOGSTRING("CClientProxy::SaveTasksToFileL - saving tasks");
	SaveTasksL(*store, aTasks);
	CleanupStack::PopAndDestroy(store);

	LOGSTRING("CClientProxy::SaveTasksToFileL - end");
	}

void CClientProxy::SaveTasksL(CFileStore& aStore, const CArrayPtr<CScheduledTask>& aTasks)
	{
	LOGSTRING("CClientProxy::SaveTasksL - start");
	aStore.SetTypeL(KDirectFileStoreLayoutUid);	

	// Save the tasks
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(aStore);
	const TInt count = aTasks.Count();
	LOGSTRING2("CClientProxy::SaveTasksL - saving %d tasks", count);
	outstream.WriteInt32L(count);
	for (TInt i=0;i<count;i++)
		{
		CScheduledTask* task = aTasks.At(i);
		task->ExternalizeL(outstream);
		} 
	outstream.CommitL();
	CleanupStack::PopAndDestroy(); // outstream

	aStore.SetRootL(id);							
	aStore.CommitL();							
	LOGSTRING("CClientProxy::SaveTasksL - end");
	}

void CClientProxy::RemoveDueTasks()
	{
	LOGSTRING("CClientProxy::RemoveDueTasks - start");

	TDblQueIter<CScheduledTask> taskIter(iTasks);
	taskIter.SetToFirst();
	CScheduledTask* task;
	//
	while ((task=taskIter++)!=NULL)
		{
		if (task->Due())
			{
			LOGSTRING3("CClientProxy::RemoveDueTasks - found due task %S, %d", &task->Info().iName, task->Info().iTaskId);

			if	(!task->Info().iRepeat)
				{
				LOGSTRING("CClientProxy::RemoveDueTasks - task has no repeats left - it's being removed");
				RemoveTask(task);
				}
			else
				{
				LOGSTRING("CClientProxy::RemoveDueTasks - task still has some repeats, setting as 'no longer due'");
				task->SetDue(EFalse);
				}
			}
		}
	iReadyToExecute = EFalse;
	LOGSTRING("CClientProxy::RemoveDueTasks - end");
	}

void CClientProxy::TransferTasks(CClientProxy& aTargetClient)
	{
	TDblQueIter<CScheduledTask> taskIter(iTasks);
	taskIter.SetToFirst();
	CScheduledTask* task;
	while ((task=taskIter++)!=NULL)
		{
		task->Remove();
		aTargetClient.AddTask(*task);
		}
	}

void CClientProxy::InternalizeL(RReadStream& aReadStream)
	{
	LOGSTRING("CClientProxy::InternalizeL - start");

	HBufC* fileName = HBufC::NewL(aReadStream, KMaxFileName);
	delete iFileName;
	iFileName = fileName;
	//
	iPriLink.iPriority = aReadStream.ReadInt32L();

	const TInt count = aReadStream.ReadInt32L();
	for(TInt i=0; i<count; i++)
		{
		CScheduledTask* task = CScheduledTask::NewLC(aReadStream);
		AddTask(*task);
		CleanupStack::Pop(task);
		}

	LOGSTRING("CClientProxy::InternalizeL - end");
	}

void CClientProxy::ExternalizeL(RWriteStream& aWriteStream) const
	{
	LOGSTRING("CClientProxy::ExternalizeL - start");

	aWriteStream << *iFileName;
	aWriteStream.WriteInt32L(iPriLink.iPriority);

	TDblQueIter<CScheduledTask> taskIter(const_cast<CClientProxy*>(this)->iTasks);
	taskIter.SetToFirst();
	CScheduledTask* task;

	// Count tasks
	TInt count = 0;
	while ((task=taskIter++)!=NULL)
		{
		if(task->Persists())
			{	
			count++;
			}
		}

	// Store leading count
	aWriteStream.WriteInt32L(count);

	// Store tasks
	taskIter.SetToFirst();
	while ((task=taskIter++)!=NULL)
		{
		if(task->Persists())
		  {
		  aWriteStream << *task;
		  }
		}

	LOGSTRING("CClientProxy::ExternalizeL - end");
	}


TDblQueIter<CScheduledTask> CClientProxy::TaskIterator()
	{
	return TDblQueIter<CScheduledTask>(iTasks);
	}



