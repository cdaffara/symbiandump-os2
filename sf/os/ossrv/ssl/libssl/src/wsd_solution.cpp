/*
Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
  may be used to endorse or promote products derived from this software 
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Description:  Contains the WSD solution
*/


// INCLUDE FILES
#include "libssl_wsd_defs.h"

#if EMULATOR

#include <e32std.h>
#include <pthread.h>
#include <windows.h>
#include "wsd_solution.h"

//definition of all WSD related funcitons 

#define MAX_NUMBER_OF_PROCESSES	100

struct TWsdArray
{
	TUint 		iPid;
	TFullName 	iFullName;
	void* 		iPtr;
};

#pragma data_seg(".wsd")
TInt NumOfProcesses = 0;
#pragma data_seg()

#pragma bss_seg(".wsd")
struct TWsdArray wsdArray[MAX_NUMBER_OF_PROCESSES];
#pragma bss_seg()

// This function frees up the used slots within the wsdArray
// for use by other processes; internal to the implementation
// that provides WSD support
LOCAL_C TInt FindSlot();

void *Pls()
{
	HANDLE mutexHandle = NULL;
	unsigned short p[] = 
	{
		'l', 'i', 'b', 's', 's', 'l', '_', 'm','u','t','e','x'
	};
	
	if( !(mutexHandle = CreateMutex(NULL, TRUE, p)) )
	{
		// Mutex already created. Try to acquire it
		mutexHandle = OpenMutex(SYNCHRONIZE, FALSE, p);
		WaitForSingleObject(mutexHandle, INFINITE);
	}

	TProcessId procId = RProcess().Id();
	TFullName fullName = RProcess().FullName();
	TInt pid = *(TUint *)&procId;
	TInt slot = -1;

	for(int i=0 ; i < MAX_NUMBER_OF_PROCESSES ; ++i )
	{
		if(!wsdArray[i].iPid)
		{
			if(slot == -1)
				slot = i;
			
			continue;
		}
		else if(wsdArray[i].iPid == pid)
		{
			ReleaseMutex(mutexHandle);
			return wsdArray[i].iPtr;
		}
	}
	
	// NEW PROCESS
	
	if(MAX_NUMBER_OF_PROCESSES == NumOfProcesses)
	{
		// Find out if one of the slots reserved for previous processes
		// could be reused.
		TInt returnValue = -1;
		returnValue = FindSlot();
		if(returnValue != -1)
		{
			slot = returnValue;
		}
		else
		{
			User::Panic(_L("Pls() Reached the naximum number for the processes"),KErrNoMemory);
		}
	}

	wsdArray[slot].iPid = pid;
	wsdArray[slot].iFullName = fullName;
	wsdArray[slot].iPtr = NULL;
	
	// Increment the count for the number of processes
	++NumOfProcesses;
	
	// Release the mutex
	ReleaseMutex(mutexHandle);
	return wsdArray[slot].iPtr;
}

LOCAL_C TInt FindSlot()
{
	TInt slot = -1;
	TFullName fullName;
	TInt currentCount = 0;
	
	for(int i = 0 ; i < MAX_NUMBER_OF_PROCESSES ; ++i)
	{
		TFindProcess search(wsdArray[i].iFullName);
		if(search.Next(fullName) == KErrNone)
		{
			++currentCount;
			continue;
		}
		
		// Process with the given name does not exist in
		// the system. So the slot could be reused.
		wsdArray[i].iPid = 0;

		// Free the VAS associated with this "process" (terminated)
		VirtualFree(wsdArray[i].iPtr, 0, MEM_RELEASE);
		wsdArray[i].iPtr = NULL;

		if(slot == -1)
		{
			// Update
			slot = i;
		}
	}
	NumOfProcesses = currentCount;
	return slot;
}

TInt SetPls(void *aArg)
{
	HANDLE mutexHandle = NULL;
	unsigned short p[] = 
	{
		'l', 'i', 'b', 's', 's', 'l', '_', 'm','u','t','e','x'
	};
	
	if( !(mutexHandle = CreateMutex(NULL, TRUE, p)) )
	{
		// Mutex already created. Try to acquire it
		mutexHandle = OpenMutex(SYNCHRONIZE, FALSE, p);
		WaitForSingleObject(mutexHandle, INFINITE);
	}

	TProcessId procId = RProcess().Id();
	TFullName fullName = RProcess().FullName();
	TInt pid = *(TUint *)&procId;

	for(int i=0 ; i < MAX_NUMBER_OF_PROCESSES ; ++i )
	{
		if(wsdArray[i].iPid == pid)
		{
			wsdArray[i].iPtr = aArg;
			ReleaseMutex(mutexHandle);
			return KErrNone;
		}
	}
	
	ReleaseMutex(mutexHandle);
	return KErrNotFound;
}

void* AllocatePls(TInt aSize)
{
	void *r = VirtualAlloc(NULL, 
						   aSize,
						   MEM_COMMIT | MEM_RESERVE,
						   PAGE_READWRITE);
	if(!r)
	{
		User::Panic(_L("AllocatePls() VIRTUALALLOC"),KErrNoMemory);
	}
	return r;
}

#endif
