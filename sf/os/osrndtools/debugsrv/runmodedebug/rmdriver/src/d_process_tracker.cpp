// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Purpose: The DProcessTracker object tracks which processes are being
// debugged. The DProcessTracker class uses a DTargetProcess object for
// each process being debugged.
// Note: Although TheDProcessTracker object is a global, it will be unique
// as only the Debug Security Server can load and use rm_debug.ldd.
// 
//

#include <e32def.h>
#include <e32def_private.h>
#include <e32cmn.h>
#include <e32cmn_private.h>
#include <kernel/kernel.h>
#include <kernel/kern_priv.h>

#include <rm_debug_api.h>
#include "debug_logging.h"
#include "d_process_tracker.h"
#include "debug_utils.h"

// Global Run-mode debugged process tracking object
DProcessTracker TheDProcessTracker;

// ctor
DProcessTracker::DProcessTracker()
	{
	}

/**
 * dtor
 * Go through forzen thread list and resume each one before clearing our structures
 * @internalTechnology
 */
DProcessTracker::~DProcessTracker()
	{

	for(TInt i=0; i<iFrozenThreadSemaphores.Count(); i++)
		{
		LOG_MSG2("~DProcessTracker Resuming frozen NThread 0x%08x via FSSignal ", iFrozenThreadSemaphores[i]->iOwningThread );
		NKern::FSSignal(iFrozenThreadSemaphores[i]);
		}

	NKern::ThreadEnterCS();
	// The ResetAndDestroy() will call the individual deletes for all objects in the containers
	iFrozenThreadSemaphores.ResetAndDestroy();
	iProcesses.ResetAndDestroy();
	iAgentsAttachedToAll.ResetAndDestroy();
	NKern::ThreadLeaveCS();
	}

/**
 * @internalTechnology
 *
 * Creates and stores an internal mapping of debug agent to debugged process.
 * Note that an individual process may be mapped to a number of debug agents.
 *
 * @param aProcessName - The fullly qualified path of the debugged process. E.g. z:\sys\bin\hello_world.exe
 * @param aAgentId - The process id of the debug agent which is attaching to aProcessName, as returned by RProcess.Id()
 * @return KErrNone if there are no errors. KErrArgument if the processname is too long/short for a valid filepath.
 *  KErrNoMemory if there is insufficient memory.
 */
TInt DProcessTracker::AttachProcess(const TDesC8& aProcessName,TUint64 aAgentId)
	{
	LOG_MSG3("DProcessTracker::AttachProcess name=%S agentId=0x%lx", 
		&aProcessName, aAgentId);

	// Valid ProcessName?
	if (aProcessName.Length() < 1 || aProcessName.Length() >= KMaxPath)
		{
		return KErrArgument;
		}

	if (aProcessName == _L8("*"))
		{
		DDebugAgent* agent = FindAgentForProcessAndId( aProcessName, aAgentId );
		if(agent != NULL)
			{
			LOG_MSG("Found agent already attached to all");
			return KErrAlreadyExists;
			}

		agent = DDebugAgent::New(aAgentId);
		if(agent == NULL)
			{
			LOG_MSG("DProcessTracker::AttachProcess() couldn't allocate memory for DDebugAgent");
			return KErrNoMemory;
			}
		
		return iAgentsAttachedToAll.Append(agent);

		}

	// Not attach all, but for a specific process/exe

	// Create an DTargetProcess to store
	DTargetProcess* tmpProcess = new DTargetProcess;
	if (tmpProcess == 0)
		{
		return KErrNoMemory;
		}
	LOG_MSG2(" AttachProcess: < new DTargetProcess=0x%08x", tmpProcess );
	
	// Set the name
	TInt err = KErrNone;
	err = tmpProcess->SetProcessName(aProcessName);
	if (err != KErrNone)
		{
		LOG_MSG2(" AttachProcess: < SetProcessName returned %d", err );
		return err;
		}

	// Is this process being debugged (ie already attached?)
	TInt found = KErrNotFound;
	const TInt numberOfProcesses = iProcesses.Count();
	for (TInt index = 0; index < numberOfProcesses; index++)
		{
		if (iProcesses[index]->ProcessName().CompareF(aProcessName) == 0)
			{
			LOG_MSG3(" Proc count=%d, found proc in iProcesses at %d. Count=%d",
				index, iProcesses.Count() );
			found = index;
			break;
			}
		}

	if (found != KErrNotFound)
		{
		// Yes, it is being debugged

		// Add the agent to the list of agents for this process
		LOG_MSG3(" > AddAgent(agent id %d) to existing iProcesses[%d]", I64LOW(aAgentId), found); 

		iProcesses[found]->AddAgent(aAgentId);

		return KErrNone;
		}
	else
		{
		// No, it is not being debugged
			
		// Add the agent to the list of agents for this process
		LOG_MSG2(" > AddAgent(agent %d) to new proc at index 0", I64LOW(aAgentId) ); 

		tmpProcess->AddAgent(aAgentId);

		// Add the process to the list of processes being debugged
		return iProcesses.Append(tmpProcess);
		}
	}

/**
 * @internalTechnology
 * 
 * Removes a previously created mapping between a debug agent and a debugged process,
 * as created by AttachProcess.
 *
 * @param aProcessName - The fully qualified path of the debugged process. E.g. z:\sys\bin\hello_world.exe
 * @param aAgentId - The process id of the debug agent which is attaching to aProcessName, as returned by RProcess.Id()
 * @return KErrNone if there are no problems. KErrArgument if the processname is too long/short for a valid filepath.
 * KErrNotFound if the mapping does not exist (and therefore cannot be removed).
 */
TInt DProcessTracker::DetachProcess(const TDesC8& aProcessName, TUint64 aAgentId)
	{
	LOG_MSG3("DProcessTracker::DetachProcess name=%S agentId=0x%lx", 
		&aProcessName, aAgentId);

	// Valid ProcessName?
	if (aProcessName.Length() < 1 || aProcessName.Length() >= KMaxPath)
		{
		return KErrArgument;
		}

	if (aProcessName == _L8("*"))
		{
		TInt const agentCount = iAgentsAttachedToAll.Count();
		LOG_MSG2(" iAgentsAttachedToAll size=%d", agentCount );

		for (TInt i = 0; i < agentCount; i++)
			{
			if (iAgentsAttachedToAll[i]->Id() == aAgentId)
				{
				LOG_MSG2(" Agent id found at index %d, deleting it", i);
				delete iAgentsAttachedToAll[i];
				iAgentsAttachedToAll.Remove(i);
				return KErrNone;
				}
			}

		//Not found, so error condition
		return KErrNotFound;
		}

	// Are we debugging this process?
	const TInt numberOfProcesses = iProcesses.Count();
	TInt foundIdx = KErrNotFound;
	for(TInt i = 0; i < numberOfProcesses; i++)
		{
		if (iProcesses[i]->ProcessName().CompareF(aProcessName) == 0)
			{
			foundIdx = i;
			break;
			}
		}

	if (foundIdx == KErrNotFound)
		{
		return KErrNotFound;
		}

	// remove the agent from the process
	iProcesses[foundIdx]->RemoveAgent(aAgentId);

	// Found it, are there any more attached agents, or suspended threads in the process?

	
	if (iProcesses[foundIdx]->AgentCount() == 0)
		{
		// Delete the process as no more agents are still attached
		delete iProcesses[foundIdx];

		// Remove the now obsolete pointer from our array.
		iProcesses.Remove(foundIdx);
		}

	return KErrNone;
	}

/**
 * @internalTechnology
 *
 * Detachs a debug agent from every process being debugged. Used when a debug agent is being detached
 * from the debug security server and has not supplied a specific process name from which to detach.
 */
TInt DProcessTracker::DetachAgent(const TUint64 aAgentId)
	{
	
	LOG_MSG2("DProcessTracker::DetachAgent 0x%lx", aAgentId);
	
	// Remove this agent from all the processes being tracked.
	TInt numberOfProcesses = iProcesses.Count();
	for(TInt i=0; i<numberOfProcesses; i++)
		{
		// remove the agent from the process (we don't care about the return code)
		iProcesses[i]->RemoveAgent(aAgentId);
		}

	// Increment down through the array as we then don't have to worry about
	// missing entries which have been shifted after deletes.
	// The initial value of i correspnds to the index of the final element 
	// in the array.
	for(TInt i = iProcesses.Count()-1; i>=0; i--)
		{
		if (iProcesses[i]->AgentCount() == 0)
			{
			// No agents remain for this process. Delete the
			// process object and remove the pointer from the array
			delete iProcesses[i];
			iProcesses.Remove(i);
			}
		}

	TInt const agentCount = iAgentsAttachedToAll.Count();
	for (TInt i = 0; i < agentCount; i++)
		{
		if (iAgentsAttachedToAll[i]->Id() == aAgentId)
			{
			LOG_MSG2(" Agent id found at index %d, deleting it", i);
			delete iAgentsAttachedToAll[i];
			iAgentsAttachedToAll.Remove(i);
			}
		}
		
	return KErrNone;
	}

/**
 * @internalTechnology
 *
 * Returns a pointer to a DTargetProcess object representing the mapping of a debugged process
 * with all the relevant debug agents interested in that process, as determined
 * by AttachProcess.
 *
 * @param aProcessName - The fully qualified path of the debugged process. E.g. z:\sys\bin\hello_world.exe
 * @return DTargetProcess* pointer to an object representing the internal mapping of a process to all associated
 * debug agents. Returns 0 if the mapping cannot be found or the aProcessName is invalid.
 */
DTargetProcess* DProcessTracker::FindProcess(const TDesC8& aProcessName) const
	{
	// Valid ProcessName?
	if (aProcessName.Length() < 1 || aProcessName.Length() >= KMaxPath)
		{
		return NULL;
		}

	// Can we find this in the array?

	// Are we debugging this process?
	const TInt numberOfProcesses = iProcesses.Count();
	DTargetProcess* found = NULL;
	for(TInt i = 0; i < numberOfProcesses; i++)
		{
		if (iProcesses[i]->ProcessName().CompareF(aProcessName) == 0)
			{
			found = iProcesses[i];
			LOG_EVENT_MSG3("DProcessTracker::FindProcess(%S) found at list pos %i", 
				&aProcessName, i);
			break;
			}
		}

	if (found == NULL)
		{
		LOG_EVENT_MSG2("DProcessTracker::FindProcess(%S), not found", &aProcessName);
		}

	return found;
	}

/**
 * @internalTechnology
 *
 * Returns a pointer to a DTargetProcess object representing the mapping of a debugged process
 * with all the relevant debug agents interested in that process, as determined
 * by AttachProcess.
 *
 * Note: This does not attempt an exact match, because the AddProcess event does not provide
 * a fully-qualified path, it provides something like [t_rmdebug_security0.exe].
 *
 * So for the purposes of dealing with this event, we need a "fuzzier" match which does not use the complete
 * path.
 *
 * @param aProcessName - The fully qualified path of the debugged process. E.g. z:\sys\bin\hello_world.exe
 * @return DTargetProcess* pointer to an object representing the internal mapping of a process to all associated
 * debug agents. Returns 0 if the mapping cannot be found or the aProcessName is invalid.
 */
DTargetProcess*	DProcessTracker::FuzzyFindProcess(const TDesC8& aProcessName)
	{
	// Valid ProcessName?
	if (aProcessName.Length() < 1 || aProcessName.Length() >= KMaxPath)
		{
		return 0;	// not found
		}

	// Can we find this in the array?
	TBool found = EFalse;
	DTargetProcess* foundProcess = 0;
	const TChar KBackSlash('\\');

	TInt numberOfProcesses = iProcesses.Count();
	for(TInt i=0; i < numberOfProcesses; i++)
		{
		foundProcess = iProcesses[i];

		TInt procListBackSlash = foundProcess->ProcessName().LocateReverse( KBackSlash );
		if( procListBackSlash == KErrNotFound )
			{
			procListBackSlash = 0;
			}
		else
			{
			//Now move to the char after the backlash
			procListBackSlash++;
			}

		TInt eventBackSlash = aProcessName.LocateReverse( KBackSlash );
		if( eventBackSlash == KErrNotFound )
			{
			eventBackSlash = 0;
			}
		else
			{
			//Now move to the char after the backlash
			eventBackSlash++;
			}

		if( ( procListBackSlash == 0 ) && ( eventBackSlash == 0 ) )
			{
			//There were no backslashes on either name, so no point in continuing
			break;
			}

		TPtrC8 eventCleanName( aProcessName.Mid( eventBackSlash ) );		
		TPtrC8 procListCleanName( foundProcess->ProcessName().Mid( procListBackSlash ) );

		if ( eventCleanName.CompareF( procListCleanName ) == 0 )
			{
			LOG_MSG2("DProcessTracker::FuzzyFindProcess() found a match : process list[%d]", i );
			found = ETrue;
			break;
			}
		}

	if (found == EFalse)
		{
		return 0;	// not found
		}

	return foundProcess;
	}

/**
  Freeze the current thread

  @return KErrNone if the thread is successfully suspended,
  KErrAlreadyExists if the agent has already suspended the thread,
  or one of the other system wide error codes

  This marks the current thread for waiting on a Fast Semaphore
  when exception handling for this thread has completed - see
  rm_debug_eventhandler.cpp for details.
  */
TInt DProcessTracker::FreezeThread()
	{
	// create and store a fast semaphore to stop the thread on
	TInt err = KErrGeneral;
	NKern::ThreadEnterCS();
	NFastSemaphore* sem = new NFastSemaphore( &(Kern::CurrentThread().iNThread) );
	if( sem != NULL )
		{
		LOG_MSG3("DProcessTracker::FreezeThread(): new NFastSemaphore(curr NThread=0x%08x), DThread=0x%08x", 
			sem->iOwningThread, &(Kern::CurrentThread()) );
		err = iFrozenThreadSemaphores.Append(sem); 
		}
	else
		{
		LOG_MSG("DProcessTracker::FreezeThread(): Error : could not allocate NFastSemaphore"); 
		err = KErrNoMemory;
		}

	NKern::ThreadLeaveCS();
	return err;
	}

/**
 Waits the current thread on a Fast Semaphore.

 This is useful for situations where the current thread
 has hit a breakpoint within a critical section, and
 otherwise could not be suspended at this point.

 Note that the Fast Semaphore structure on which the thread
 waits must be a member data item of this class instance,
 as it needs to be FSSignal()'d by another thread to resume
 again.
 */
void DProcessTracker::FSWait()
	{
	NThread* currentNThread = &(Kern::CurrentThread().iNThread);	
	for(TInt i=0; i<iFrozenThreadSemaphores.Count(); i++)
		{
		if(iFrozenThreadSemaphores[i]->iOwningThread == currentNThread)
			{
			LOG_MSG4("DProcessTracker::FSWait(): > FSWait frozen sem %d, currentNThread=0x%08x, id=0x%x", 
				i, currentNThread, Kern::CurrentThread().iId );
			NKern::FSWait(iFrozenThreadSemaphores[i]);
			return;
			}
		}
	}
	
/**
  Resume the specified frozen thread

  @param aThread thread to resume

  @return KErrNone if the thread has previously been suspended and is resumed,
  KErrNotFound if the thread has not previously been suspended
  */
TInt DProcessTracker::ResumeFrozenThread(DThread* aThread)
	{
	for(TInt i=0; i<iFrozenThreadSemaphores.Count(); i++)
		{
		if(iFrozenThreadSemaphores[i]->iOwningThread == &(aThread->iNThread))
			{
			LOG_MSG2("DProcessTracker::ResumeFrozenThread 0x%08x, signalling then deleting this FastSem", aThread->iId );
			NKern::FSSignal(iFrozenThreadSemaphores[i]);
			NKern::ThreadEnterCS();
			delete iFrozenThreadSemaphores[i];
			NKern::ThreadLeaveCS();
			iFrozenThreadSemaphores.Remove(i);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}
	
TInt DProcessTracker::SuspendThread(DThread* aTargetThread, TBool aFreezeThread)
	{
	LOG_MSG5("DProcessTracker::SuspendThread() id 0x%08x, iCsCount=%d, , iCsFunction=%d, iSuspendCount=%d ", 
			aTargetThread->iId, aTargetThread->iNThread.iCsCount, aTargetThread->iNThread.iCsFunction, aTargetThread->iNThread.iSuspendCount );
	if( !aFreezeThread )
		{		
		if(!aTargetThread)
			{
			LOG_MSG("DProcessTracker::SuspendThread()  > Kern::ThreadSuspend NullThrd Ptr!!");
			return KErrBadHandle;
			}
		
		Kern::ThreadSuspend(*aTargetThread, 1);
		return KErrNone;
		}

	if( Kern::CurrentThread().iId != aTargetThread->iId )
		{
		LOG_MSG2("DProcessTracker::SuspendThread() Error: Freeze for thread 0x%08x, but different from current thread", 
				aTargetThread->iId);
		return KErrBadHandle;
		}

	return FreezeThread();
	}


TInt DProcessTracker::ResumeThread(DThread* aTargetThread)
	{
	LOG_MSG5("DProcessTracker::ResumeThread() id 0x%08x, iCsCount=%d, , iCsFunction=%d, iSuspendCount=%d ", 
			aTargetThread->iId, aTargetThread->iNThread.iCsCount, aTargetThread->iNThread.iCsFunction, aTargetThread->iNThread.iSuspendCount );

	TInt err = ResumeFrozenThread( aTargetThread );
	if( err == KErrNotFound ) 
		{
		LOG_MSG(" ResumeThread() : not found in frozen list. Using Kern::ThreadResume" );
		Kern::ThreadResume(*aTargetThread);
		return KErrNone;
		}

	return err;
	}

/**
  Get a thread's originating file name

  @param aThread the thread to get the file name for

  @return a pointer to the thread's file name, if there are problems accessing
  the file name then NULL will be returned
  */
const TDesC* DProcessTracker::GetFileName(DThread* aThread) const
	{
	//check if the thread is NULL and return if so
	if(!aThread)
		{
		return NULL;
		}

	//get the owning process and return if it is NULL
	DProcess* process = aThread->iOwningProcess;
	if(!process)
		{
		return NULL;
		}

	//get the process' code seg and return if it is NULL
	DCodeSeg* codeSeg = process->iCodeSeg;
	if(!codeSeg)
		{
		return NULL;
		}

	//return the code seg's stored file name (which could theoretically be NULL)
	return codeSeg->iFileName;
	}

/**
If any agent has called AttachToAll, return the most recently attached one.
*/
DDebugAgent* DProcessTracker::GetCurrentAgentAttachedToAll() const
	{
	if (iAgentsAttachedToAll.Count() > 0)
		{
		return iAgentsAttachedToAll[iAgentsAttachedToAll.Count()-1];
		}
	else
		{
		return NULL;
		}
	}

/**
Returns ETrue if at least one agent was found for this process (either a specifically-attached 
one or a current attached to all). Search specifically attached first, since these have 
priority over attach all.
*/
TBool DProcessTracker::NotifyAgentsForProcessEvent(const TDesC8& aProcessName, const TDriverEventInfo& aEvent, TBool aAllowFuzzy)
	{
	TBool foundAgent = EFalse;

	DTargetProcess* process = FindProcess(aProcessName);
	if (process == NULL && aAllowFuzzy)
		{
		process = FuzzyFindProcess(aProcessName);
		}

	if (process)
		{
		LOG_MSG3("DProcessTracker::NotifyAgentsForProcessEvent name=%S eventtype=%d", 
			&aProcessName, aEvent.iEventType);
		process->NotifyEvent(aEvent);
		return ETrue;
		}

	// Since no specifically attached agents were found, try the attach all

	DDebugAgent* currentAll = GetCurrentAgentAttachedToAll();
	if (currentAll)
		{
		foundAgent = ETrue;
		LOG_MSG4("DProcessTracker::NotifyAgentsForProcessEvent via AttachAll name=%S eventtype=%d, agent 0x%lx", 
			&aProcessName, aEvent.iEventType, currentAll->Id());
		currentAll->NotifyEvent(aEvent);
		}

	return foundAgent;
	}

/**
 * Find the agent that matches this exe/proc name. Name could be the attachall indicator "*", 
 * in which case it returns the agent that matched the pid from the attach all list
 */
DDebugAgent* DProcessTracker::FindAgentForProcessAndId(const TDesC8& aProcessName, TUint64 aAgentId) const
	{

	if (aProcessName == _L8("*"))
		{
		TInt const agentCount = iAgentsAttachedToAll.Count();
		
		LOG_MSG3("FindAgentForProcessAndId : Searching for agent id 0x%lx, iAgentsAttachedToAll size=%d", 
			aAgentId, agentCount );
		
		// Then check the attached to all list. Should not have more than one entry
		// for each agent, but just in case we search backwards to match the append
		//
		for (TInt i = agentCount - 1 ; i >= 0; i--)
			{
			DDebugAgent* agent = iAgentsAttachedToAll[i];
			if (agent->Id() == aAgentId)
				{
				return agent;
				}
			}
		}
	else
		{
		DTargetProcess* process = FindProcess(aProcessName);
		if (process)
			{
			return process->Agent(aAgentId);
			}
		}
	return NULL;
	}
