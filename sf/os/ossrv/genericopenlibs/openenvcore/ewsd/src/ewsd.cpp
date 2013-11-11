// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ewsd.cpp
// Part of     : ewsd library
// Contains the definitions of the APIs of the emulator WSD library 
//



#ifdef __WINSCW__

#include <e32std.h>
#include <windows.h>

// Constant declarations
const TInt KMaxNumberOfProcesses = 100;
const TInt KMaxNumberOfLibraries = 50;

// Panic strings
_LIT(KMutexGetFailure, "WSD mutex get failed");
_LIT(KMutexReleaseFailure, "WSD mutex release failed");

// Mutex name
unsigned short KMutexName[] = 
	{
		'E', 'W', 'S', 'D', 'M', 'u', 't', 'e', 'x'
	};

// LOCAL STRUCTURE DECLARATIONS

// Data structure to store the WSD info associated with a particular DLL
struct TDllData
	{
	TUid  iLibraryUid;
	TAny* iPtr;
	};

// Data structure to store the DLL information associated with a process
struct TWsdNode
	{
	TProcessId  iPid;
	TInt  		iFirstUnusedSlot;
	struct TDllData iDllData[KMaxNumberOfLibraries];
	};
	
// WSD array variables
struct TWsdNode WsdArray[KMaxNumberOfProcesses];
TInt LastUsedProcessSlot = -1;

// LOCAL FUNCTION DECLARATIONS

LOCAL_C TInt CleanupDeadProcessSlots();
LOCAL_C void StorePls(const TInt& aProcessSlot, const TInt& aLibrarySlot, TAny* aPls, const TUid& aLibraryUid);  
	
// EXPORTED FUNCTION DEFINITIONS

EXPORT_C TAny* CheckPls(const TUid& aLibraryUid)
	{
	TProcessId procId = RProcess().Id();
	
	for (TInt i = 0; i <= LastUsedProcessSlot; ++i)
		{
		if (WsdArray[i].iPid == procId)
			{
			// This process has a slot in the WsdArray - now check
			// whether its DLL array contains the specified library
			TInt firstUnused = WsdArray[i].iFirstUnusedSlot;
			for (TInt j = 0; j < firstUnused; ++j)	
				{
				if (aLibraryUid == WsdArray[i].iDllData[j].iLibraryUid)
					{
					// The specified library is present so return the PLS object
					return WsdArray[i].iDllData[j].iPtr;
					}
				}
			break;
			}
		}
	
	// The PLS object hasn't yet been stored	
	return NULL;
	}

EXPORT_C TInt SetPls(TAny* aPls, const TUid& aLibraryUid)   		 
	{
	TProcessId procId = RProcess().Id();
	TInt slot = -1;

	if (LastUsedProcessSlot >= 0)
		{		
		for (TInt i = 0; i <= LastUsedProcessSlot; ++i)
			{
			if (!WsdArray[i].iPid.Id())
				{
				if (slot == -1)
					{
					 slot = i;
					}
					
				continue;
				}
			else if (WsdArray[i].iPid == procId)	
				{
					// We are about to set the Pls of a new library for this process
					TInt firstUnused = WsdArray[i].iFirstUnusedSlot;
					if (KMaxNumberOfLibraries == firstUnused)
						{
						// The library array is full for this process
						return KErrNoMemory;
						}
					else
						{
						// Store the PLS for the specified library
						StorePls(i, firstUnused, aPls, aLibraryUid);
						return KErrNone;
						}
				}
			}
		
		// The remainder of WsdArray is not used, so the process is
		// not in it. If an empty slot hasn't yet been found then...			
		if (slot == -1)
			{
			// ...  use the next unused slot in the array, if there is one
			if (LastUsedProcessSlot < (KMaxNumberOfProcesses - 1))
				{
				slot = LastUsedProcessSlot + 1;
				}
			else
				{
				// ... the array is full (there are no empty slots). 
				// Perform a clean up of the array to free any process slots that
				// belong to dead processes to see if there is a slot that we can reuse
				TInt slotToReuse = CleanupDeadProcessSlots();
				if (slotToReuse == -1)
					{
					// There are no reusable slots in the process array
					return KErrNoMemory;
					}
				slot = slotToReuse;
				}
			}
		}
	else
		{
		slot = 0;
		}
	
	// Store the PLS for the specified library	
	StorePls(slot, 0, aPls, aLibraryUid);
	
	// Store the process details too as this process is new to WsdArray
	WsdArray[slot].iPid = procId;
	
	// Update the LastUsedProcessSlot if necessary		    
	if (slot > LastUsedProcessSlot)
		{
		 LastUsedProcessSlot = slot;
		}
	
	return KErrNone;
	}
	
EXPORT_C TAny* AllocatePls(const TInt& aSize)
	{
	return VirtualAlloc(NULL, 
						aSize,
						MEM_COMMIT | MEM_RESERVE,
						PAGE_READWRITE);
	}

EXPORT_C TInt FreePls(TAny* aPls)
	{
	if (!VirtualFree(aPls, 0, MEM_RELEASE))
		{
		return KErrAccessDenied;
		}
	aPls = NULL;
	return KErrNone;
	}

EXPORT_C TAny* ObtainPlsMutex()
	{
	// Get a handle to the mutex (the mutex will be created 
	// if it doesn't already exist) and then wait to acquire 
	// ownership of the mutex
	HANDLE mutexHandle = CreateMutex(NULL, FALSE, KMutexName);
	if ((!mutexHandle) || (WaitForSingleObject(mutexHandle, INFINITE) == WAIT_FAILED)) 
		{
		User::Panic(KMutexGetFailure, KErrAccessDenied);
		}
		
	return mutexHandle;
	}
	
EXPORT_C void ReleasePlsMutex(TAny* aMutexHandle)
	{
	if (!ReleaseMutex(aMutexHandle))
		{
		 User::Panic(KMutexReleaseFailure, KErrAccessDenied);
		}
	}
	
// LOCAL FUNCTIONS DEFINITIONS

/**  
Iterates through the WSD array and frees each slot that belongs to a dead process
(the stored data is reset and all associated PLS memory is freed).
Returns the index of the first freed slot if at least one slot was freed,
otherwise -1
*/
LOCAL_C TInt CleanupDeadProcessSlots()
	{
	TInt firstReusableSlot = -1;
	TUid nullUid = TUid::Null();
	LastUsedProcessSlot = -1;
	RProcess proc;
	for (TInt i = 0; i < KMaxNumberOfProcesses; ++i)
		{
		if(proc.Open(WsdArray[i].iPid) == KErrNone)
			{
			if(proc.ExitType()==EExitPending)
				{
				LastUsedProcessSlot = i;
				proc.Close();
				continue;	
				}
			else
				proc.Close();
			
			}
		
		// Process with the given name does not exist in
		// the system, so the slot could be reused
		WsdArray[i].iPid = 0;
		

		// Free all of the PLS memory associated with this process
		TInt firstUnused = WsdArray[i].iFirstUnusedSlot;
		WsdArray[i].iFirstUnusedSlot = 0;
		for (TInt j = 0; j < firstUnused; ++j)
			{
			FreePls(WsdArray[i].iDllData[j].iPtr);
			WsdArray[i].iDllData[j].iLibraryUid = nullUid;
			}

		if (firstReusableSlot == -1)
			{
			// Set the slot to reuse
			firstReusableSlot = i;
			}
		}
		
	return firstReusableSlot;
}	

/**  
Stores the specified PLS object and library TUid at the specified location in the WSD array.
Takes as parameters the process slot to use in the WSD array, the slot to use in the library 
array associated with the given process, the PLS object to be stored and the TUid of the 
library DLL that the PLS object belongs to
*/
LOCAL_C void StorePls(const TInt& aProcessSlot, const TInt& aLibrarySlot, TAny* aPls, const TUid& aLibraryUid)
	{
	 WsdArray[aProcessSlot].iDllData[aLibrarySlot].iPtr = aPls;
	 WsdArray[aProcessSlot].iDllData[aLibrarySlot].iLibraryUid = aLibraryUid;
	 ++WsdArray[aProcessSlot].iFirstUnusedSlot;	
	}

#endif // __WINSCW__
