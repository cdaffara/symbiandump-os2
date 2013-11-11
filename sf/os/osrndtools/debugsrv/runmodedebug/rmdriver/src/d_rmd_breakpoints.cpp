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
//


#include <e32def.h>
#include <e32def_private.h>
#include <e32cmn.h>
#include <e32cmn_private.h>
#include <u32std.h>
#include <kernel/kernel.h>
#include <kernel/kern_priv.h>
#include <nk_trace.h>
#include <arm.h>
#include <kernel/cache.h>
#include <platform.h>
#include <nkern.h>
#include <u32hal.h>

#include <rm_debug_api.h>
#include "d_rmd_breakpoints.h"
#include "d_process_tracker.h"
#include "d_rmd_stepping.h"
#include "rm_debug_kerneldriver.h"	// needed to access DRM_DebugChannel
#include "rm_debug_driver.h"
#include "debug_utils.h"
#include "debug_logging.h"

using namespace Debug;

/* @internalTechnology
 *
 * Checks whether aAddress is correctly aligned for placing a breakpoint of
 * cpu architecture aMode.
 *
 * @param aAddress - Virtual memory address to check
 * @param aMode - The CPU architecture mode of the breakpoint to be placed at aAddress
 * @return ETrue if aAddress is suitably aligned, EFalse otherwise.
 */
TBool D_RMD_Breakpoints::Aligned(TUint32 aAddress, Debug::TArchitectureMode aMode)
	{
	switch(aMode)
		{
		case Debug::EArmMode:
			// ARM breakpoints must be 32-bit aligned (lower two bits must be zero)
			if (aAddress & 0x3)
			{
				// Not 32-bit aligned.
				return EFalse;
			}
			break;
		case Debug::EThumbMode:
			// Thumb breakpoints must be 16-bit aligned (low bit must be zero)
			if (aAddress & 0x1)
			{
				// Not 16-bit aligned
				return EFalse;
			}
			break;
		case Debug::EThumb2EEMode:
			// Thumb-EE instructions are half-word aligned. See ARM ARM DDI0406A, section A3.2 Alignment Support
			// Note that some instructions need to be word-aligned, but this function does not know which ones.
			// It may also depend on the System Control register U bit.
			if (aAddress & 0x1)
			{
				// Not 16-bit aligned
				return EFalse;
			}
			break;
		default:
			{
			// No idea
			return EFalse;
			}
		}

	// Must be OK
	return ETrue;
	};

/* @internalTechnology
 *
 * Returns the size of a breakpoint of architecture aMode in bytes
 * 
 * @param aMode - The architure of the breakpoint
 * @return The size of the breakpoints in bytes. 0 if un-recognised architecture.
 */
TInt D_RMD_Breakpoints::BreakSize(Debug::TArchitectureMode aMode)
	{
	switch(aMode)
		{
		case Debug::EArmMode:
			{
				return 4;
			}
		case Debug::EThumbMode:
			{
				return 2;
			}
		case Debug::EThumb2EEMode:
			{
			// Only needs to be two bytes in size.
			return 2;
			}
		default:
			{
				// No idea
				return 0;
			}
		}
	};

/* @internalTechnology
 *
 * Checks whether two TBreakEntrys overlap
 *
 * @param aFirst - A TBreakEntry with valid iAddress and iMode fields.
 * @param aSecond  - A TBreakEntry with valid iAddress and iMode fields.
 * @return ETrue if the aFirst and aSecond overlap or the overlap cannot be determined
 *         , EFalse otherwise
 */
TBool D_RMD_Breakpoints::BreakpointsOverlap(TBreakEntry& aFirst, TBreakEntry& aSecond)
	{
	TInt firstSize = BreakSize(aFirst.iMode);
	TInt secondSize = BreakSize(aSecond.iMode);

	// Do we know the size of each breakpoint?
	if ((firstSize <= 0) || (secondSize <= 0))
		{
		// We don't know the size of the breakpoint, so assume they overlap
		return ETrue;
		}

	TInt firstStartAddress = aFirst.iAddress;
	TInt secondStartAddress = aSecond.iAddress;
	TInt firstEndAddress = firstStartAddress + firstSize - 1;
	TInt secondEndAddress = secondStartAddress + secondSize - 1;

	// If second breakpoint is past the end of the first then we're ok
	if(firstEndAddress < secondStartAddress)
		{
		return EFalse;
		}

	// If first breakpoint is past the end of the second then we're ok
	if(secondEndAddress < firstStartAddress)
		{
		return EFalse;
		}

	// The breakpoints overlap
	return ETrue;
	}

/* @internalTechnology
 * 
 * Returns the breakpoint bitpattern to use for each architecture type
 *
 * @param aMode - the cpu architecture type
 * @return The bit-pattern to use for the specified architecture, or 0 if unsupported.
 */
TUint32 D_RMD_Breakpoints::BreakInst(Debug::TArchitectureMode aMode)
	{
	switch(aMode)
		{
		case Debug::EArmMode:
			{
				return KArmBreakPoint;
			}
		case Debug::EThumbMode:
			{
				return KThumbBreakPoint;
			}
		case Debug::EThumb2EEMode:
			{
			return KT2EEBreakPoint;
			}
		default:
			{
				// No idea what the breakpoint should be
				return 0;
			}
		}
	};

/**
Constructor. Initialises its internal list of empty breakpoints.
*/
D_RMD_Breakpoints::D_RMD_Breakpoints(DRM_DebugChannel* aChannel)
: iBreakPointList(NUMBER_OF_TEMP_BREAKPOINTS, 0),
  iNextBreakId(NUMBER_OF_TEMP_BREAKPOINTS),
  iChannel(aChannel),
  iInitialised(EFalse)
	{
	iBreakPointList.Reset();	
	TBreakEntry emptyTempBreak;
	
	for (TInt i = 0; i < NUMBER_OF_TEMP_BREAKPOINTS; i++)
		{
		emptyTempBreak.iBreakId = i;
		
		if (KErrNone != iBreakPointList.Append(emptyTempBreak))
			{
			LOG_MSG("D_RMD_Breakpoints::D_RMD_Breakpoints() - Error appending blank temp break entry");
			}
		}
	}

/**
Destructor. Clears all the breakpoints in the system, deletes its internal list of breakpoints,
and closes the exclusivity semaphore.
*/
D_RMD_Breakpoints::~D_RMD_Breakpoints()
	{
	ClearAllBreakPoints();
	
	// close the breakpoint list and free the memory associated with it
	iBreakPointList.Close();

	if (iLock)
		iLock->Close(NULL);
	}

/**
Initialises the breakpoint list exclusion semaphore. This should be called once immediately after
the constructor.

@return KErrNone if successful, one of the other system wide error codes otherwise.
*/
TInt D_RMD_Breakpoints::Init()
	{
	TInt err = KErrNone;

	// Only create a semaphore if we are not initialised
	if(!iInitialised)
		{
		// Initialise the semaphore ensuring exclusive access to the breakpoint list
		err = Kern::SemaphoreCreate(iLock, _L("RM_DebugBreakpointLock"), 1 /* Initial count */);
		if (err == KErrNone)
			{
			iInitialised = ETrue;
			}
		}
	else
		{
		err = KErrNone;
		}

	return err;
	}

/** 
Public member function which sets a thread-specific breakpoint in the specified thread
and returns an opaque handle to the caller.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DoSetBreak

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;
 
@param aBreakId - Reference to a TUint32 into which the function will return a unique breakpoint Id.
@param aThreadId - The thread Id in which to place the breakpoint
@param aAddress - Address to place the breakpoint
@param aMode - The cpu instruction set architecture type breakpoint (e.g. EArmMode or EThumbMode)
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::DoSetBreak(TInt32 &aBreakId, const TUint64 aId, const TBool aThreadSpecific, const TUint32 aAddress, const TArchitectureMode aMode)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TInt err = priv_DoSetBreak(aBreakId, aId, aThreadSpecific, aAddress,aMode);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();
	
	return err;
	}
/**
Private member function which sets a thread-specific breakpoint in the specified thread
and returns an opaque handle to the caller.

@see DoSetBreak

@param aBreakId - Reference to a TUint32 into which the function will return a unique breakpoint Id.
@param aThreadId - The thread Id in which to place the breakpoint
@param aAddress - Address to place the breakpoint
@param aMode - The cpu instruction set architecture type breakpoint (e.g. EArmMode or EThumbMode)
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::priv_DoSetBreak(TInt32 &aBreakId, const TUint64 aId, const TBool aThreadSpecific, const TUint32 aAddress, const TArchitectureMode aMode)
	{
	LOG_MSG4("D_RMD_Breakpoints::priv_DoSetBreak(aThreadId = 0x%lx, aAddress = 0x%08x, aMode = %d)",aId,aAddress,aMode);

	// EThumb2EEMode breakpoints are not supported
	if (EThumb2EEMode == aMode)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - EThumb2EEMode breakpoints are not supported");
		return KErrNotSupported;
		}

	// Check how many breakpoints we have in existence
	if ((iBreakPointList.Count()+1) >= NUMBER_OF_MAX_BREAKPOINTS)
		{
		// Too many breakpoints are set!
		LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - Too many breakpoints set");
		return KErrOverflow;
		}

	// check the alignment of the breakpoint
	if (!Aligned(aAddress,aMode))
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - Unaligned address");
		return KErrArgument;
		}

	// make sure there is not already a breakpoint at this address
	for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
		{
		/* We need to check if the breakpoint overlaps the address at all,
		 * and this depends upon the size of the two breakpoints as well as 
		 * their address.
		 */

		// newInstSize = size in bytes of new breakpoint
		TInt newInstSize = BreakSize(aMode);
		if (newInstSize == 0)
			{
			LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - Unknown architecture type for new breakpoint");
			return KErrNotSupported;
			}

		// oldInstSize = size in bytes of the existing breakpoint
		TInt oldInstSize = BreakSize(iBreakPointList[i].iMode);
		if (oldInstSize == 0)
			{
			LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - : Unknown architecture type of existing breakpoint");
			return KErrNotSupported;
			}

		// Overlap checking - temp is used as the new breakpoint description for checking purposes only
		TBreakEntry temp;

		temp.iAddress = aAddress;
		temp.iMode = aMode;

		// do they overlap?
		if ( BreakpointsOverlap(temp,iBreakPointList[i]) )
			{
			// Yes
			if(iBreakPointList[i].iThreadSpecific && aThreadSpecific)
				{
				if(aId == iBreakPointList[i].iId)
					{
					LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - New thread specific breakpoint overlaps an existing thread specific breakpoint");
					return KErrAlreadyExists;
					}
				}
			else if(!iBreakPointList[i].iThreadSpecific && aThreadSpecific)
				{
				NKern::ThreadEnterCS();
				DThread* thread = DebugUtils::OpenThreadHandle(aId);
				TInt err = KErrNone;
				if (!thread)
					{
					err = KErrNotFound;
					}
				if (!err && thread->iOwningProcess->iId == iBreakPointList[i].iId)
					{
					LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - New thread specific breakpoint overlaps an existing breakpoint");
					err = KErrAlreadyExists;
					}
				thread->Close(NULL);
				NKern::ThreadLeaveCS();
				if (err) return err;
				}
			else if(iBreakPointList[i].iThreadSpecific && !aThreadSpecific)
				{
				NKern::ThreadEnterCS();
				DThread* thread = DebugUtils::OpenThreadHandle(iBreakPointList[i].iId);
				TInt err = KErrNone;
				if (!thread)
					{
					err = KErrNotFound;
					}
				if (!err && thread->iOwningProcess->iId == aId)
					{
					LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - New breakpoint overlaps an existing thread specific breakpoint");
					err = KErrAlreadyExists;
					}
				if (thread) thread->Close(NULL);
				NKern::ThreadLeaveCS();
				if (err) return err;
				}
			else // !iBreakPointList[i].iThreadSpecific && !aThreadSpecific
				{
				if(iBreakPointList[i].iId == aId)
					{
					LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - New breakpoint overlaps an existing breakpoint");
					return KErrAlreadyExists;
					}
				}
			}
		}

	// increment the break id
	aBreakId = iNextBreakId++;	

	// create the new breakpoint entry
	TBreakEntry breakEntry(aBreakId, aId, aThreadSpecific, aAddress, aMode);

	TInt err = priv_DoEnableBreak(breakEntry, ETrue);
	if (KErrNone != err)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - Could not enable the breakpoint");
		
		return err;
		}

	err = iBreakPointList.Append(breakEntry);
	if (err != KErrNone)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoSetBreak() - Failed to append breakpoint");
		}

	LOG_MSG2("D_RMD_Breakpoints::priv_DoSetBreak(breakId = 0x%08x) done",aBreakId);

	return err;
	}

/**
Public member function which enables a previously set breakpoint.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DoEnableBreak

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

Note 3
Historically, this function accepted a reference to a TBreakEntry in the class' own
iBreakPointList. It now checks whether the reference is to an element of its own list,
or one invented by the caller.

@param aEntry reference to a TBreakEntry datastructure describing the breakpoint to be re-enabled.
@param aSaveOldInstruction ETrue preserves the instruction at the breakpoint address, EFalse otherwise.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::DoEnableBreak(TBreakEntry &aEntry, TBool aSaveOldInstruction)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TInt err = priv_DoEnableBreak(aEntry,aSaveOldInstruction);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}

/**
Private member function which enables a previously set breakpoint, as per DoEnableBreak, but
does not serialise access.

@see DoEnableBreak

@param aEntry reference to a TBreakEntry datastructure describing the breakpoint to be re-enabled.
@param aSaveOldInstruction ETrue preserves the instruction at the breakpoint address, EFalse otherwise.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::priv_DoEnableBreak(TBreakEntry &aEntry, TBool aSaveOldInstruction)
	{
	LOG_MSG("D_RMD_Breakpoints::DoEnableBreak()");

	TUint32 inst = BreakInst(aEntry.iMode);	
	TInt instSize = BreakSize(aEntry.iMode);
	if (instSize == 0 || inst == 0)
		{
		// not supported
		LOG_MSG("D_RMD_Breakpoints::priv_DoEnableBreak - unsupported breakpoint architecture");
		return KErrNotSupported;
		}

	TInt err = KErrNone;

	// Get thread id
	TUint64 threadId = aEntry.iId + (aEntry.iThreadSpecific ? 0 : 1);
	NKern::ThreadEnterCS();
	DThread* threadObj = DebugUtils::OpenThreadHandle(threadId);
	if (!threadObj)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoEnableBreak - bad handle. Could not identify a threadObj");
		NKern::ThreadLeaveCS();
		return KErrBadHandle;
		}

	if (aSaveOldInstruction)
		{
		TUint32 instruction;

		// read the instruction at the address so we can store it in the break entry for when we clear this breakpoint
		// trap exceptions in case the address is invalid
		XTRAPD(r, XT_DEFAULT, err = iChannel->TryToReadMemory(threadObj, (TAny *)aEntry.iAddress, (TAny *)&instruction, instSize));

		//consider the leave as more important than the error code so store the leave if it's not KErrNone
		if(KErrNone != r)
			{
			err = r;
			}

		if(KErrNone != err)
			{
			threadObj->Close(NULL);
			NKern::ThreadLeaveCS();
			LOG_MSG("D_RMD_Breakpoints::priv_DoEnableBreak() - failed to read memory");
			return err;
			}

		aEntry.iInstruction.Copy((TUint8 *)&instruction, instSize);
		}

	TBool breakpointAlredySet = EFalse;
	for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
		{
		if(iBreakPointList[i].iAddress == aEntry.iAddress && !iBreakPointList[i].iDisabledForStep )
			{
			breakpointAlredySet = ETrue;
			break;
			}
		}
	if(!breakpointAlredySet)
		{
	
		LOG_MSG5("D_RMD_Breakpoints::DoEnableBreak() tId=0x%x, addr=0x%x, instSize=%d, inst=0x%x", 
			threadObj->iId, aEntry.iAddress, instSize, instSize == 4 ? (TUint32)inst : (TUint16)inst );
		XTRAPD(r, XT_DEFAULT, err = DebugSupport::ModifyCode(threadObj, aEntry.iAddress, instSize, inst, DebugSupport::EBreakpointGlobal));
		if(r != DebugSupport::EBreakpointGlobal)
			{
			err = r;
			}
		}
	else
		{
		LOG_MSG5("D_RMD_Breakpoints::DoEnableBreak() ALREADY SET: tId=0x%x, addr=0x%x, instSize=%d, inst=0x%x", 
				threadObj->iId, aEntry.iAddress, instSize, instSize == 4 ? (TUint32)inst : (TUint16)inst );

		}

	// Close the thread handle which has been opened by OpenThreadHandle
	threadObj->Close(NULL);
	NKern::ThreadLeaveCS();
	return err;
	}

/**
Public member function which clears a previously set breakpoint.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DoClearBreak

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

@param aBreakId A breakpoint Id as previously returned by DoSetBreak.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::DoClearBreak(const TInt32 aBreakId, TBool aIgnoreTerminatedThreads)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TInt err = priv_DoClearBreak(aBreakId, aIgnoreTerminatedThreads);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}

/**
Private member function which clears a previously set breakpoint, as per DoClearBreak, but
does not serialise access.

@see DoClearBreak

@param aBreakId A breakpoint Id as previously returned by DoSetBreak.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::priv_DoClearBreak(const TInt32 aBreakId, TBool aIgnoreTerminatedThreads)
	{
	LOG_MSG3("D_RMD_Breakpoints::priv_DoClearBreak(0x%08x), aIgnoreTerminatedThreads=%d",
	        aBreakId, aIgnoreTerminatedThreads);

	// find the break entry matching this id.  note that the breakpoints are already sorted in ascending order by id
	TBreakEntry entry;
	entry.iBreakId = aBreakId;
	TInt index = iBreakPointList.FindInSignedKeyOrder(entry);

	TInt err = KErrNone;
	if (index >= 0)
		{	
 		// if this breakpoint was set in a library and that library has already been unloaded, don't try to clear it
		if (!iBreakPointList[index].iObsoleteLibraryBreakpoint)
			{
			NKern::ThreadEnterCS();
			DThread* threadObj = DebugUtils::OpenThreadHandle(iBreakPointList[index].iId + (iBreakPointList[index].iThreadSpecific ? 0 : 1));
			if (threadObj)
				{
				LOG_MSG2("priv_DoClearBreak() OpenThreadHandle ret thread 0x%08x", threadObj->iId );
				TBool needToCallCodeModifier = ETrue;
				for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
					{
					if (i != index)
						{
						if ( BreakpointsOverlap(iBreakPointList[index],iBreakPointList[i]) )
							{
							needToCallCodeModifier = EFalse;
							break;
							}
						}
					}
				if(needToCallCodeModifier)
					{
					XTRAPD(r, XT_DEFAULT, err = DebugSupport::RestoreCode(threadObj, iBreakPointList[index].iAddress));
					if (r != KErrNone)
						{
						LOG_MSG2("D_RMD_Breakpoints::priv_DoClearBreak() - restore code trap harness returned error %d",r);
						}
					if (err == KErrNotFound)
						{
						LOG_MSG("restore code reported the breakpoint not found, continuing");
						err = KErrNone;
						}
					else if (err != KErrNone)
						{
						LOG_MSG2("D_RMD_Breakpoints::priv_DoClearBreak() - restore code returned error %d",err);
						}
					err = (KErrNone == r) ? err : r;
					}

				// Close the thread handle opened by OpenThreadHandle
				threadObj->Close(NULL);
				}
			else
				{
				LOG_MSG("D_RMD_Breakpoints::OpenThreadHandle ret null thread");
				err = KErrBadHandle;
				}
			NKern::ThreadLeaveCS();
			}
		
		LOG_MSG4("D_RMD_Breakpoints::priv_DoClearBreak() - Clearing breakpoint at address: %x, err: %d, ignore terminated: %d", iBreakPointList[index].iAddress, err, aIgnoreTerminatedThreads?1:0);
		if ((aIgnoreTerminatedThreads && KErrBadHandle == err) || KErrNone == err)
			{
			// if this is a temp breakpoint, just clear out the values, otherwise remove it from the list
			err = KErrNone;
			if (index < NUMBER_OF_TEMP_BREAKPOINTS)
				{
                // if this is a temp breakpoint, just clear out the values, otherwise remove it from the list
                LOG_MSG2("D_RMD_Breakpoints::priv_DoClearBreak() - Reseting temp breakpoint[%d]",index);
				iBreakPointList[index].Reset();
				}
			else
				{
				LOG_MSG2("D_RMD_Breakpoints::priv_DoClearBreak() - Removing breakpoint[%d]",index);
				iBreakPointList.Remove(index);
				}			
			}
		else
		    {
            LOG_MSG3("D_RMD_Breakpoints::priv_DoClearBreak() - *** Not removing breakpoint[%d] due to error=%d",index, err);
		    }
				
		return err;
		}

	LOG_MSG2("D_RMD_Breakpoints::priv_DoClearBreak() - Break Id %d not found", aBreakId);

	return KErrNotFound;
	}

/**
Public member function which modifies a previously set breakpoint.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DoModifyBreak

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

@param aBreakInfo A TModifyBreakInfo describing the breakpoint properties that are wanted.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::DoModifyBreak(TModifyBreakInfo* aBreakInfo)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock); 

	// Really do the work
	TInt err = priv_DoModifyBreak(aBreakInfo);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}

/**
Private member function which modifies a previously set breakpoint, as per DoModifyBreak, but
does not serialise access.

@see DoModifyBreak

@param aBreakInfo A TModifyBreakInfo describing the breakpoint properties that are wanted.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::priv_DoModifyBreak(TModifyBreakInfo* aBreakInfo)
	{
	LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak()");

	// Check arguments
	if (!aBreakInfo)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak() was passed a NULL argument");
		return KErrArgument;
		}

	//User side memory is not accessible directly
	TSetBreakInfo info;
	TInt err = Kern::ThreadRawRead(iChannel->iClientThread, aBreakInfo, (TUint8*)&info, sizeof(TSetBreakInfo));
	if (err != KErrNone)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak() was passed a bad argument");
		return err;
		}

	// EThumb2EEMode breakpoints are not supported
	if (EThumb2EEMode == info.iMode)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak() - EThumb2EEMode breakpoints are not supported");
		return KErrNotSupported;
		}

	// find the break entry matching this id.  note that the breakpoints are already sorted in ascending order by id
	TBreakEntry entry;
	entry.iBreakId = (TUint32)info.iBreakId;
	TInt index = iBreakPointList.FindInSignedKeyOrder(entry);
	if (index < 0)
		{
		// Could not find the breakpoint
		LOG_MSG2("D_RMD_Breakpoints::priv_DoModifyBreak() - Could not find the breakpoint id 0x%08x",(TUint32)info.iBreakId);
		return KErrNotFound;
		}

	// first check its not obsolete
	if (!iBreakPointList[index].iObsoleteLibraryBreakpoint)
		{
		// its still a valid breakpoint

		// remove the old breakpoint
		NKern::ThreadEnterCS();
		DThread* threadObj = DebugUtils::OpenThreadHandle(iBreakPointList[index].iId);
		if (threadObj)
			{
			LOG_MSG2("D_RMD_Breakpoints::priv_DoModifyBreak - Unsetting breakpoint at address 0x%08x",iBreakPointList[index].iAddress);

			XTRAPD(r, XT_DEFAULT, err = DebugSupport::RestoreCode(threadObj, iBreakPointList[index].iAddress));
			if (r != 0)
				{
				LOG_MSG("Failed to construct trap handler for DebugSupport::RestoreCode");
				}

			// Close the thread handle which has been opened by OpenThreadHandle
			threadObj->Close(NULL);
			NKern::ThreadLeaveCS();
			}
		else
			{
			// Bad handle
			LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak - Could not identify the breakpoint thread id");
			NKern::ThreadLeaveCS();
			return KErrBadHandle;
			}
		}

	// make sure there is not already a breakpoint at the new address
	for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
		{
		// Ignore data for the breakpoint entry being modified.
		if (i != index)
			{
			/* We need to check if the breakpoint overlaps the address at all,
			 * and this depends upon the size of the two breakpoints as well as 
			 * their address.
			 */

			// newInstSize = size in bytes of new breakpoint
			TInt newInstSize = BreakSize(info.iMode);
			if (newInstSize == 0)
			{
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak - Unknown architecture type for new breakpoint");
				return KErrNotSupported;
			}

			// oldInstSize = size in bytes of the existing breakpoint
			TInt oldInstSize = BreakSize(iBreakPointList[i].iMode);
			if (oldInstSize == 0)
			{
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak - Unknown architecture type of existing breakpoint");
				return KErrNotSupported;
			}

			// Overlap checking - temp is used as the new breakpoint description for checking purposes only
			TBreakEntry temp;

			temp.iAddress = info.iAddress;
			temp.iMode = info.iMode;

			// do they overlap?
			if ( BreakpointsOverlap(temp,iBreakPointList[i]) )
				{
				// Yes
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak() - New breakpoint overlaps an existing breakpoint");
				return KErrAlreadyExists;
				}
			}
		}

	// Prepare iBreakPointList[index] with the new information, then set the breakpoint
	iBreakPointList[index].iId = info.iId;
	iBreakPointList[index].iAddress = info.iAddress;
	iBreakPointList[index].iMode = info.iMode;

	TBreakEntry& newBreakEntry = iBreakPointList[index];

	// Decide the size of the breakpoint instruction
	TUint32 inst = BreakInst(newBreakEntry.iMode);
	TInt instSize = BreakSize(newBreakEntry.iMode);

	if (inst == 0 || instSize == 0)
		{
		// Unsupported architecture
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak - unsupported breakpoint architecture");
		return KErrNotSupported;
		}


	//if thread id is 0xFFFFFFFF, then the breakpoint is not thread specific
	if (newBreakEntry.iId != 0xFFFFFFFF)
		{
		newBreakEntry.iThreadSpecific = ETrue;
		}

	// Get thread id from the process that we are debugging
	TProcessInfo * proc = NULL;
	TUint64 threadId = NULL;

	threadId = newBreakEntry.iId;

	NKern::ThreadEnterCS();
	DThread* threadObj = DebugUtils::OpenThreadHandle(threadId);
	//if we don't have the right thread id for the address, 
	//then try with the thread id of the process that we are debugging 	
	if (!threadObj && iChannel->iDebugProcessList.Count())
		{
		proc = &iChannel->iDebugProcessList[0];
		if (proc)
			{
			threadId = proc->iId+1;	
			}
		threadObj = DebugUtils::OpenThreadHandle(threadId);
		}

	if(!threadObj)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyBreak() - bad handle. Could not identify a threadObj");
		NKern::ThreadLeaveCS();
		return KErrBadHandle;
		}

	// save the old instruction
	TUint32 instruction;

	// read the instruction at the address so we can store it in the break entry for when we clear this breakpoint
	// trap exceptions in case the address is invalid
	XTRAPD(r, XT_DEFAULT, err = iChannel->TryToReadMemory(threadObj, (TAny *)newBreakEntry.iAddress, (TAny *)&instruction, instSize));

	//consider the leave as more important than the error code so store the leave if it's not KErrNone
	if(KErrNone != r)
		{
		err = r;
		}
	if(KErrNone != err)
		{
		threadObj->Close(NULL);
		NKern::ThreadLeaveCS();
		return err;
		}

	newBreakEntry.iInstruction.Copy((TUint8 *)&instruction, instSize);

	newBreakEntry.iId = threadId; //set the thread ID here 
	LOG_MSG3("ModifyCode2 instSize:%d, inst: 0x%08x", instSize, inst);
	XTRAPD(s, XT_DEFAULT, err = DebugSupport::ModifyCode(threadObj, newBreakEntry.iAddress, instSize, inst, DebugSupport::EBreakpointGlobal));
	if(s != DebugSupport::EBreakpointGlobal)
		{
		err = s;
		}

	// Close the thread handle which has been opened by OpenThreadHandle
	threadObj->Close(NULL);
	NKern::ThreadLeaveCS();
	return err;
	}	

//
// D_RMD_Breakpoints::DoModifyProcessBreak
//
TInt D_RMD_Breakpoints::DoModifyProcessBreak(TModifyProcessBreakInfo* aBreakInfo)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock); 

	// Really do the work
	TInt err = priv_DoModifyProcessBreak(aBreakInfo);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}
	
TInt D_RMD_Breakpoints::priv_DoModifyProcessBreak(TModifyProcessBreakInfo* aBreakInfo)
	{	
	LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak()");

	// Check arguments
	if (!aBreakInfo)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() was passed a NULL argument");
		return KErrArgument;
		}

	//User side memory is not accessible directly
	TSetBreakInfo info;
	TInt err = Kern::ThreadRawRead(iChannel->iClientThread, aBreakInfo, (TUint8*)&info, sizeof(TModifyProcessBreakInfo));
	if (err != KErrNone)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() was passed a bad argument");
		return err;
		}

	// EThumb2EEMode breakpoints are not supported
	if (EThumb2EEMode == info.iMode)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - EThumb2EEMode breakpoints are not supported");
		return KErrNotSupported;
		}

	// find the break entry matching this id.  note that the breakpoints are already sorted in ascending order by id
	TBreakEntry entry;
	entry.iBreakId = (TUint32)info.iBreakId;
	TInt index = iBreakPointList.FindInSignedKeyOrder(entry);
	if (index < 0)
		{
		// Could not find the breakpoint
		LOG_MSG2("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - Could not find the breakpoint id 0x%08x",(TUint32)info.iBreakId);
		return KErrNotFound;
		}

	// first check its not obsolete
	if (!iBreakPointList[index].iObsoleteLibraryBreakpoint)
		{
		// its still a valid breakpoint

		// remove the old breakpoint
		NKern::ThreadEnterCS();
		DProcess *process = DebugUtils::OpenProcessHandle(iBreakPointList[index].iId);
		DThread* threadObj = NULL;
		if(process)
			{
			threadObj = DebugUtils::OpenFirstThreadForProcess(process);
			process->Close(NULL);
			}

		if (threadObj)
			{
			LOG_MSG2("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - Unsetting breakpoint at address 0x%08x",iBreakPointList[index].iAddress);

			XTRAPD(r, XT_DEFAULT, /*err =*/ DebugSupport::RestoreCode(threadObj, iBreakPointList[index].iAddress)); // Any error here is not important. The semantics of ModifyBreakpoint are such that if it fails the previous breakpoint location is removed, which means that a further call to ModifyBreakpoint shouldn't fail because the CodeModifier doesn't know about it.
			if (r != 0)
				{
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - Failed to construct trap handler for DebugSupport::RestoreCode");
				}

			// Close the thread handle which has been opened by OpenThreadHandle
			threadObj->Close(NULL);
			}
		else
			{
			// Bad handle
			LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - Could not identify the breakpoint process id");
			err = KErrBadHandle;
			}
		NKern::ThreadLeaveCS();
		if (err) return err;
		}

	// make sure there is not already a breakpoint at the new address
	for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
		{
		// Ignore data for the breakpoint entry being modified.
		if (i != index)
			{
			/* We need to check if the breakpoint overlaps the address at all,
			 * and this depends upon the size of the two breakpoints as well as 
			 * their address.
			 */

			// newInstSize = size in bytes of new breakpoint
			TInt newInstSize = BreakSize(info.iMode);
			if (newInstSize == 0)
				{
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - Unknown architecture type for new breakpoint");
				return KErrNotSupported;
				}

			// oldInstSize = size in bytes of the existing breakpoint
			TInt oldInstSize = BreakSize(iBreakPointList[i].iMode);
			if (oldInstSize == 0)
				{
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - : Unknown architecture type of existing breakpoint");
				return KErrNotSupported;
				}

			// Overlap checking - temp is used as the new breakpoint description for checking purposes only
			TBreakEntry temp;

			temp.iAddress = info.iAddress;
			temp.iMode = info.iMode;

			// do they overlap?
			if ( BreakpointsOverlap(temp,iBreakPointList[i]) )
				{
				// Yes
				LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - New breakpoint overlaps an existing breakpoint");
				return KErrAlreadyExists;
				}
			}
		}

	// Prepare iBreakPointList[index] with the new information, then set the breakpoint
	iBreakPointList[index].iId = info.iId;
	iBreakPointList[index].iAddress = info.iAddress;
	iBreakPointList[index].iMode = info.iMode;

	TBreakEntry& newBreakEntry = iBreakPointList[index];

	// Decide the size of the breakpoint instruction
	TUint32 inst = BreakInst(newBreakEntry.iMode);
	TInt instSize = BreakSize(newBreakEntry.iMode);

	if (inst == 0 || instSize == 0)
		{
		// Unsupported architecture
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - unsupported breakpoint architecture");
		return KErrNotSupported;
		}

	newBreakEntry.iThreadSpecific = EFalse;

	DThread* threadObj = NULL;
	NKern::ThreadEnterCS();
	DProcess* process = DebugUtils::OpenProcessHandle(newBreakEntry.iId);
	if (process)
		{
		threadObj = DebugUtils::OpenFirstThreadForProcess(process);
		if (!threadObj) err = KErrNotFound;
		process->Close(NULL);
		}
	else
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoModifyProcessBreak() - bad handle. Could not identify a process");
		err = KErrBadHandle;
		}

	if (err)
		{
		NKern::ThreadLeaveCS();
		return err;
		}

	// save the old instruction
	TUint32 instruction;

	// read the instruction at the address so we can store it in the break entry for when we clear this breakpoint
	// trap exceptions in case the address is invalid
	XTRAPD(r, XT_DEFAULT, err = iChannel->TryToReadMemory(threadObj, (TAny *)newBreakEntry.iAddress, (TAny *)&instruction, instSize));

	//consider the leave as more important than the error code so store the leave if it's not KErrNone
	if(KErrNone != r)
		{
		err = r;
		}
	if(KErrNone != err)
		{
		threadObj->Close(NULL);
		NKern::ThreadLeaveCS();
		return err;
		}

	newBreakEntry.iInstruction.Copy((TUint8 *)&instruction, instSize);

	XTRAPD(s, XT_DEFAULT, err = DebugSupport::ModifyCode(threadObj, newBreakEntry.iAddress, instSize, inst, DebugSupport::EBreakpointGlobal));
	if(s != DebugSupport::EBreakpointGlobal)
		{
		err = s;
		}

	// Close the thread handle which has been opened by OpenThreadHandle
	threadObj->Close(NULL);
	NKern::ThreadLeaveCS();
	return err;
	}

/**
Public member function which returns information about a previously set breakpoint.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DoBreakInfo

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

@param aBreakInfo Address of aBreakInfo structure in user-side memory within the DSS client thread. CAN ONLY BE ACCESSED VIA Kern::ThreadRawRead()
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::DoBreakInfo(TGetBreakInfo* aBreakInfo)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TInt err = priv_DoBreakInfo(aBreakInfo);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}

/**
Private member function function which returns information about a previously set breakpoint..

@see DoBreakInfo

@param aBreakInfo Address of aBreakInfo structure in user-side memory within the DSS client thread. CAN ONLY BE ACCESSED VIA Kern::ThreadRawRead()
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt D_RMD_Breakpoints::priv_DoBreakInfo(TGetBreakInfo* aBreakInfo)
	{
	LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo()");

	if (!aBreakInfo)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo() was passed a NULL argument");

		return KErrArgument;
		}

	//User side memory is not accessible directly
	TGetBreakInfo info;
	TInt err = Kern::ThreadRawRead(iChannel->iClientThread, aBreakInfo, (TUint8*)&info, sizeof(TGetBreakInfo));
	if (err != KErrNone)
		{
		LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo() was passed a bad argument");

		return err;
		}

	// find the break entry matching this id.  note that the breakpoints are already sorted in ascending order by id
	TBreakEntry entry;
	entry.iBreakId = (TUint32)info.iBreakId;
	TInt index = iBreakPointList.FindInSignedKeyOrder(entry);
	
	if (index >=0)
		{
		// get the thread id for this breakpoint
		TUint64 threadId = iBreakPointList[index].iId;

		err = Kern::ThreadRawWrite(iChannel->iClientThread,(TUint8*)info.iId,&threadId,sizeof(TUint64));
		if (err != KErrNone)
			{
			LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo() - failed to return breakpoint iThreadId information");
			return err;
			}

		// get the threadSpecific-ness
		TBool threadSpecific = iBreakPointList[index].iThreadSpecific;

		err = Kern::ThreadRawWrite(iChannel->iClientThread,(TUint8*)info.iThreadSpecific,&threadSpecific,sizeof(TBool));
		if (err != KErrNone)
			{
			LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo() - failed to return thread specific information");
			return err;
			}


		// get the address
		TUint32 address = iBreakPointList[index].iAddress;

		err = Kern::ThreadRawWrite(iChannel->iClientThread,(TUint8*)info.iAddress,&address,sizeof(TUint32));
		if (err != KErrNone)
			{
			LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo() - failed to return breakpoint iAddress information");
			return err;
			}


		// get the architecture
		TArchitectureMode mode = iBreakPointList[index].iMode;

		err = Kern::ThreadRawWrite(iChannel->iClientThread,(TUint8*)info.iMode,&mode,sizeof(TUint32));
		if (err != KErrNone)
			{
			LOG_MSG("D_RMD_Breakpoints::priv_DoBreakInfo() - failed to return breakpoint iMode information");
			return err;
			}

		return err;
		}

	LOG_MSG2("D_RMD_Breakpoints::priv_DoBreakInfo - Could not find the breakpoint id specified 0x%08x", entry.iBreakId);
	return KErrNotFound;
	}

/**
Public member function which clears all the breakpoints in the system. Generally used for shutting down
the debug device driver.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_ClearAllBreakPoints

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;
*/
void D_RMD_Breakpoints::ClearAllBreakPoints()
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	priv_ClearAllBreakPoints();
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();
	}

/**
Private member function which clears all the breakpoints in the system. Generally used for shutting down
the debug device driver. 

@see DoClearAllBreakPoints
*/
void D_RMD_Breakpoints::priv_ClearAllBreakPoints()
	{
	LOG_MSG("D_RMD_Breakpoints::priv_ClearAllBreakPoints()");

	TInt err = KErrNone;
	NKern::ThreadEnterCS();
	for (TInt i=0; i<iBreakPointList.Count(); i++)
		{
		if ((iBreakPointList[i].iAddress != 0) && !iBreakPointList[i].iObsoleteLibraryBreakpoint)
			{
			TUint32 id = iBreakPointList[i].iId + (iBreakPointList[i].iThreadSpecific ? 0 : 1);
	        LOG_MSG5(" Will try to clear breakpoint[%d] at address 0x%x, iId=0x%016lx, thrdSpec=%d", 
	                i, iBreakPointList[i].iAddress, iBreakPointList[i].iId, iBreakPointList[i].iThreadSpecific);

			DThread *threadObj = DebugUtils::OpenThreadHandle(id);
			if (threadObj)
				{
				XTRAPD(r, XT_DEFAULT, err = DebugSupport::RestoreCode(threadObj, iBreakPointList[i].iAddress));
				err = (KErrNone == r) ? err : r;
				threadObj->Close(NULL);
				}
			else
				{
                LOG_MSG(" OpenThreadHandle returned NULL handle");
				err = KErrBadHandle;
				}

			if (KErrNone != err)
				{
				LOG_MSG2("D_RMD_Breakpoints::priv_ClearAllBreakPoints() - Error 0x%08x while clearing breakpoint", err);
				}
			}
		else if(iBreakPointList[i].iAddress == 0)
		    {
            LOG_MSG3("Breakpoint[%d]: address is 0, iId=0x%016lx", i, iBreakPointList[i].iId );		
		    }
		else
		    {
            LOG_MSG4("Breakpoint[%d]: Obsoleted, address =0x%x, iId=0x%016lx", i, iBreakPointList[i].iAddress, iBreakPointList[i].iId );     
		    }
		}
	NKern::ThreadLeaveCS();

	iBreakPointList.Reset();
	}

/**
Public member function which disables the breakpoint at the specified address.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DisableBreakAtAddress

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

@param aAddress Address at which to disable breakpoints (all threads)
@return KErrNone if successful, one of the other system wide error codes otherwise.
*/
TInt D_RMD_Breakpoints::DisableBreakAtAddress(TUint32 aAddress)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TInt err = priv_DisableBreakAtAddress(aAddress);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}

/**
Private member function which clears all the breakpoints in the system. Generally used for shutting down
the debug device driver. 

@see DisableBreakAtAddress

@param aAddress clears the breakpoint at the specified address
@return KErrNone if successful, one of the other system wide error codes otherwise.
*/
TInt D_RMD_Breakpoints::priv_DisableBreakAtAddress(TUint32 aAddress)
	{
	LOG_MSG2("D_RMD_Breakpoints::priv_DisableBreakAtAddress(aAddress=0x%x)", aAddress);

	TInt err = KErrNone;

	for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
		{
		if (iBreakPointList[i].iAddress == aAddress)
			{
			iBreakPointList[i].iDisabledForStep = ETrue;
			LOG_MSG2("D_RMD_Breakpoints::priv_DisableBreakAtAddress - Disabling breakpoint at address 0x%x", iBreakPointList[i].iAddress);

			//clear the breakpoint with code modifier
			//code modifier will restore the org instruction and also frees the shadow page if necessary
			TUint64 id = iBreakPointList[i].iId + (iBreakPointList[i].iThreadSpecific ? 0 : 1);
			DThread* threadObj = NULL;
			NKern::ThreadEnterCS();
			if(iBreakPointList[i].iThreadSpecific)
				{
				threadObj = DebugUtils::OpenThreadHandle(id);
				}
			else
				{
				DProcess* process = DebugUtils::OpenProcessHandle(iBreakPointList[i].iId);
				if(process)
					{
					threadObj = DebugUtils::OpenFirstThreadForProcess(process);
					process->Close(NULL);
					}
				}

			if (threadObj)
				{
				XTRAPD(r, XT_DEFAULT, err = DebugSupport::RestoreCode(threadObj, aAddress));			
				if(KErrNone != err || KErrNone != r)
					{
					LOG_MSG3("Error from DebugSupport::RestoreCode: r: %d, err: %d", r, err);
					}
				err = (KErrNone == r) ? err : r;
				threadObj->Close(NULL);
				}
			else
				{
				err = KErrBadHandle;
				LOG_MSG2("Couldn't find thread for breakpoint id %d", iBreakPointList[i].iId);
				}
			NKern::ThreadLeaveCS();
			if (err) break;
			}
		}
		
	return err;
	}

/**
Public member function which enables previously disabled breakpoints within a given thread.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access. 

@see priv_DoEnableDisabledBreak

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

@param aThreadId Thread in which to enable all previously disabled breakpoints
@return KErrNone if successful, one of the system wide error codes otherwise.
*/
TInt D_RMD_Breakpoints::DoEnableDisabledBreak(TUint64 aThreadId)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return KErrNotReady;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TInt err = priv_DoEnableDisabledBreak(aThreadId);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();

	return err;
	}

/**
Private member function which enables previously disabled breakpoints within a given thread.

@see DoEnableDisabledBreak

@param aThreadId Thread in which to enable all previously disabled breakpoints
@return KErrNone if successful, one of the system wide error codes otherwise.
*/
TInt D_RMD_Breakpoints::priv_DoEnableDisabledBreak(TUint64 aThreadId)
	{
	LOG_MSG("D_RMD_Breakpoints::priv_DoEnableDisabledBreak()");
	NKern::ThreadEnterCS();
	DThread* thread = DebugUtils::OpenThreadHandle(aThreadId);
	if(!thread)
		{
		LOG_MSG2("Thread: 0x%08x does not exist", aThreadId);
		NKern::ThreadLeaveCS();
		return KErrNotFound;
		}
	TUint64 processId = thread->iOwningProcess->iId;
	thread->Close(NULL);
	NKern::ThreadLeaveCS();

	for (TInt i = NUMBER_OF_TEMP_BREAKPOINTS; i < iBreakPointList.Count(); i++)
		{
		TBool needsEnabling = EFalse;
		if(iBreakPointList[i].iDisabledForStep)
			{
			if(iBreakPointList[i].iThreadSpecific)
				{
				needsEnabling = (aThreadId == iBreakPointList[i].iId);
				}
			else
				{
				needsEnabling = (processId == iBreakPointList[i].iId);
				}
			}
		if (needsEnabling)
			{
			LOG_MSG2("Re-enabling breakpoint at address %x", iBreakPointList[i].iAddress);
			TInt err = priv_DoEnableBreak(iBreakPointList[i], EFalse);
			if(KErrNone != err)
				{
				LOG_MSG2("Error returned from DoEnableBreak: %d", err);
				iBreakPointList[i].iDisabledForStep = EFalse;
				return err;
				}
			}
		}
	
	return KErrNone;
	}

/**
Public member function which removes all the breakpoints within a given thread.

Note 1:
This function ensures exclusive access to the breakpoint data structures
by using a semaphore to serialise access.

@see priv_DoRemoveThreadBreaks

Note 2:
As implied by Note 1, the caller must have previously called Init() or this
function will return KErrNotReady;

@param aThreadId Thread from which to remove all existing breakpoints
@return KErrNone if successful, one of the system wide error codes otherwise.
*/
void D_RMD_Breakpoints::DoRemoveThreadBreaks(TUint64 aThreadId)
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	priv_DoRemoveThreadBreaks(aThreadId);

	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();
	}

/**
Private member function which removes all the breakpoints particular to a particular thread

@see DoRemoveThreadBreaks

@param aThreadId Thread from which to remove all existing breakpoints
@return KErrNone if successful, one of the system wide error codes otherwise.
*/
void D_RMD_Breakpoints::priv_DoRemoveThreadBreaks(TUint64 aThreadId)
	{
	LOG_MSG2("D_RMD_Breakpoints::priv_DoRemoveThreadBreaks(aThreadId = 0x%lx)\n",aThreadId);

	TInt err = KErrNone;
	TUint64 threadId;

	for (TInt i=iBreakPointList.Count()-1; i >= 0; i--)
		{
		if ((iBreakPointList[i].iAddress != 0) && !iBreakPointList[i].iObsoleteLibraryBreakpoint)
			{
			threadId = iBreakPointList[i].iId + (iBreakPointList[i].iThreadSpecific ? 0 : 1);
			if (threadId == aThreadId)
				{
				LOG_MSG5("D_RMD_Breakpoints::priv_DoRemoveThreadBreaks() - Clearing breakpoint[%d],idx=%x at address 0x%08x, iId=0x%016lx", 
				        i, iBreakPointList[i].iBreakId, iBreakPointList[i].iAddress, iBreakPointList[i].iId );

				err = priv_DoClearBreak(iBreakPointList[i].iBreakId, EFalse);

				if (err != KErrNone)
					{
					LOG_MSG2("D_RMD_Breakpoints::priv_DoRemoveThreadBreaks()  - failed to remove break id 0x%08x\n",iBreakPointList[i].iBreakId);
					return;
					}
				}
			}
        else if(iBreakPointList[i].iAddress == 0)
            {
            LOG_MSG3("Breakpoint[%d]: address is 0, iId=0x%016lx", i, iBreakPointList[i].iId );     
            }
        else
            {
            LOG_MSG4("Breakpoint[%d]: Obsoleted, address =0x%x, iId=0x%016lx", i, iBreakPointList[i].iAddress, iBreakPointList[i].iId );     
            }		
		}	
	}

// Remove the process breakpoints for process with PID aProcessId in the range [aCodeAddress, aCodeAddress + aCodeSize)
void D_RMD_Breakpoints::RemoveBreaksForProcess(TUint64 aProcessId, TUint32 aCodeAddress, TUint32 aCodeSize)
	{
	LOG_MSG4("D_RMD_Breakpoints::RemoveBreaksForProcess(), aProcId=0x%016lx, codeAddr=0x%x, codeSize=0x%x", 
	        aProcessId,aCodeAddress, aCodeSize);
	NKern::ThreadEnterCS();
	for (TInt i=iBreakPointList.Count() - 1; i>=0; i--)
		{
        TBool remove = EFalse;
		TBreakEntry& breakEntry = iBreakPointList[i];
		
		if( breakEntry.iId == 0 || breakEntry.iAddress == 0 )
		    {
            breakEntry.Reset();
		    continue;
		    }
		
		LOG_MSG5(" break[%d], iId=0x%016lx, threadSpec=%d, aProcessId=0x%016lx", 
		        i, breakEntry.iId, breakEntry.iThreadSpecific, aProcessId);
		
		if(!breakEntry.iThreadSpecific && breakEntry.iId == aProcessId)
		    {
		    remove = ETrue;
		    }
		else if(breakEntry.iThreadSpecific)
		    {
            //breakEntry.iId is thread id. Get its pid, then check if aProcessId is same, then remove
            DThread* thread = DebugUtils::OpenThreadHandle(breakEntry.iId);
            if(!thread)
                {
                LOG_MSG2("Could not open handle to thread (aThreadId = 0x%016lx)",breakEntry.iId);
                continue;
                }
            
            LOG_MSG2(" thread->iOwningProcess->iId=0x%016lx", thread->iOwningProcess->iId );
            
            if( thread->iOwningProcess->iId == aProcessId )
                {
                LOG_MSG3("Thread spec breakpoint @ index[%d] matches aProcessId= 0x%016lx. Removing",i, aProcessId);
                remove = ETrue;
                }
            
            thread->Close(NULL);
		    }
		    
        if ( remove && (breakEntry.iAddress >= aCodeAddress) && (breakEntry.iAddress < (aCodeAddress + aCodeSize)))
            {
            LOG_MSG2("Removing process breakpoint at address %x", (TUint32)breakEntry.iAddress);
            TInt err = DoClearBreak(breakEntry.iBreakId, ETrue);
            if(KErrNone != err)
                {
                LOG_MSG2("Error removing breakpoint: %d", err);
                }
            }
        else
            {
            LOG_MSG4("Not removing breakpoint at index[%d], id=0x%016lx, address=0x%x", 
                    i, breakEntry.iId, (TUint32)breakEntry.iAddress);
            }
		}
	NKern::ThreadLeaveCS();
	}

// mark the breakpoints in the range [aCodeAddress, aCodeAddress + aCodeSize)
void D_RMD_Breakpoints::InvalidateLibraryBreakPoints(TUint32 aCodeAddress, TUint32 aCodeSize)
	{
	LOG_MSG3("D_RMD_Breakpoints::InvalidateLibraryBreakPoints(aCodeAddress=0x%x, aCodeSize=0x%x)",
	        aCodeAddress, aCodeSize );
	
	for (TInt i=0; i<iBreakPointList.Count(); i++)
		{
		if ((iBreakPointList[i].iAddress >= aCodeAddress) && (iBreakPointList[i].iAddress < (aCodeAddress + aCodeSize)))
			{
			LOG_MSG2("Obsoleting library breakpoint at address %x", iBreakPointList[i].iAddress);
			iBreakPointList[i].iObsoleteLibraryBreakpoint = ETrue;
			}
		}
	}

TInt D_RMD_Breakpoints::BreakPointCount() const
	{
	return iBreakPointList.Count();
	}

/**
  Gets next breakpoint in list.
  @param aBreakEntry The break entry to get the successor of. If NULL then returns the first entry.
  @return A pointer to the next break entry, or NULL if the end of the list has been reached
  */
TBreakEntry* D_RMD_Breakpoints::GetNextBreak(const TBreakEntry* aBreakEntry) const
	{
	if(!aBreakEntry)
		{
		return (TBreakEntry*)&(iBreakPointList[0]);
		}
	TInt index = iBreakPointList.FindInSignedKeyOrder(*aBreakEntry) + 1;
	return (index < BreakPointCount()) ? (TBreakEntry*)&(iBreakPointList[index]) : NULL;
	}

TBool D_RMD_Breakpoints::IsTemporaryBreak(const TBreakEntry& aBreakEntry) const
	{
	// Ensure we have a valid semaphore
	if (!iInitialised || !iLock)
		{
		return EFalse;
		}

	// Acquire the lock
	NKern::ThreadEnterCS();
	Kern::SemaphoreWait(*iLock);

	// Really do the work
	TBool tempBreak = priv_IsTemporaryBreak(aBreakEntry);
	
	// Release the lock
	Kern::SemaphoreSignal(*iLock);
	NKern::ThreadLeaveCS();
	
	return tempBreak;
	}

/**
Private member function which tells us if a breakpoint is temporary

@see IsTemporaryBreak

@param aBreakEntry
@return TBool indicating if the break is temporary or not
*/
TBool D_RMD_Breakpoints::priv_IsTemporaryBreak(const TBreakEntry& aBreakEntry) const 
	{
	return aBreakEntry.iBreakId < NUMBER_OF_TEMP_BREAKPOINTS;
	}


// End of file - d_rmd_breakpoints.cpp
