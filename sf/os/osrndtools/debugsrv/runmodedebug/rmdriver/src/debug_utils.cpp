// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Purpose: Implementation of static functions for use by debug driver classes
//

#include "debug_logging.h"
#include "debug_utils.h"

/**
 * Given a thread ID, return a handle to the corresponding DThread. If the returned
 * pointer is non-NULL, it is the responsibility of the caller to close the handle.
 * 
 * @pre caller must be in thread critical section
 * @post if a non-NULL value is returned then a handle to the thread has been
 * opened on the callers behalf
 * @param aThreadId ID of the thread to return a handle for
 * @return a DThread* to the appropriate thread, or NULL if a handle could not be
 * opened to the specified thread
 */
DThread* DebugUtils::OpenThreadHandle(TUint64 aThreadId)
	{
	__ASSERT_CRITICAL;
	LOG_MSG2("DebugUtils::OpenThreadHandle(0x%lx)", aThreadId);

	DObjectCon& threads = *Kern::Containers()[EThread];  // Get containing holding threads
	threads.Wait();  // Obtain the container mutex so the list does get changed under us

	DThread* thread = Kern::ThreadFromId(aThreadId);

	// Open a handle to the thread so that it doesn't exit while we are processing
	if (thread)
		{
		// if opening a handle fails then set thread to NULL
		if(KErrNone != thread->Open())
			{
			LOG_MSG2("\tCould not open handle to thread %d", (TUint32)aThreadId);
			thread = NULL;
			}
		}
	else
		{
		LOG_MSG2("\tThread with ID %d is NULL", (TUint32)aThreadId);
		}

	threads.Signal();  // Release the container mutex

	return thread;
	}

/**
 * Given a process ID, return a handle to the corresponding DProcess. If the returned
 * pointer is non-NULL, it is the responsibility of the caller to close the handle.
 * 
 * @post if a non-NULL value is returned then a handle to the process has been
 * opened on the callers behalf
 * @param aProcessId ID of the process to return a handle for
 * @return a DProcess* to the appropriate process, or NULL if a handle could not be
 * opened to the specified process
 */
DProcess* DebugUtils::OpenProcessHandle(const TUint64 aProcessId)
	{
	// Commenting out this message as it gets printed out every time a RDebug::Printf statement is caught by the driver,
	// which makes looking at the serial cable output irritating. Replaced it with LOG_MSG statements below to indicate if
	// something amiss happened. By default then this function prints nothing out.
	//LOG_MSG("DebugUtils::OpenProcessHandle()");

	NKern::ThreadEnterCS();  // Prevent us from dying or suspending whilst holding a DMutex
	DObjectCon& processes = *Kern::Containers()[EProcess];  // Get containing holding threads
	processes.Wait();  // Obtain the container mutex so the list does get changed under us

	DProcess* process = Kern::ProcessFromId(aProcessId);

	// Open a handle to the process so that it doesn't exit while we are processing
	if (process)
		{
		// if opening a handle fails then set process to NULL
		if(KErrNone != process->Open())
			{
			LOG_MSG2("DebugUtils::OpenProcessHandle(): Could not open handle for 0x%lx", aProcessId);
			process = NULL;
			}
		}
	else
		{
		LOG_MSG2("DebugUtils::OpenProcessHandle(): Could not find process for 0x%lx", aProcessId);
		}

	processes.Signal();  // Release the container mutex
	NKern::ThreadLeaveCS();  // End of critical section

	return process;
	}

/**
 * Opens a reference to the first thread of the given process. Returns NULL if
 * there are no threads remaining in the process or if the thread couldn't be opened.
 * 
 * @pre Caller must be in thread context, in critical section, no fast mutexes held.
 * @post if result is non-NULL caller is responsible for closing the handle
 * @param aProcess The process whose first thread is to be opened
 * @return an Open()ed pointer to the first thread in the process, or NULL.
 */
DThread* DebugUtils::OpenFirstThreadForProcess(DProcess* aProcess)
	{
	__ASSERT_CRITICAL;
	// Copied from memspy's DMemSpyDriverOSAdaptionDProcess::OpenFirstThread()

	// It appears that the system lock needs to be held while manipulating the iThreadQ
	DThread* result = NULL;
	NKern::LockSystem();
	// We don't use DProcess::FirstThread() as that doesn't appear to do any checking of whether the list is empty, ie if there are no threads at all
	SDblQueLink* threadLink = aProcess->iThreadQ.First();
	if (threadLink != NULL && threadLink != &aProcess->iThreadQ.iA)
		{
		result = _LOFF(threadLink,DThread,iProcessLink);
		if (result->Open() != KErrNone)
			{
			result = NULL;
			}
		}
	NKern::UnlockSystem();
    return result;
    }
