/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent
 @released
*/
#include <kernel/kern_priv.h>
#include "cryptodriver.h"
#ifdef __MARM__
#include <omap_hrp/assp/shared/omap_reg.h>
#include <omap_hrp/assp/shared/omap_interrupt.h>
#endif
//#include "cryptoh4.h"
#include "cryptoldd.h"
#include "cryptoh4rng.h"

inline void CryptoH4JobRandom::EnableIsr()
	{
	TRACE_FUNCTION("EnableIsr");
	//	Kern::Printf("EI");
	SetRunning(ETrue);
#ifdef __MARM__
	// Enable RNG interrupt. This interrupt will then queue the
	// "random number ready" DFC
	TInt32 tmp = TOmap::Register32(KHwBaseRngReg + KHoRng_Mask);
	tmp |= 4;
	TOmap::SetRegister32(KHwBaseRngReg + KHoRng_Mask, tmp);
#else
	// Not on real h/w so just queue the DFC...
	// Queue the "random number ready" DFC
	iRandomDfc.Enque(); // Queue from task level
#endif
	}

inline void CryptoH4JobRandom::DisableIsr()
	{
	TRACE_FUNCTION("DisableIsr");
	//	Kern::Printf("DI");
#ifdef __MARM__
	TInt32 tmp = TOmap::Register32(KHwBaseRngReg + KHoRng_Mask);
	tmp &= ~4;
	TOmap::SetRegister32(KHwBaseRngReg + KHoRng_Mask, tmp);
#endif
	}



CryptoH4JobRandom::CryptoH4JobRandom(DLddChanRandom &aLddChanRandom)
	: iLddChanRandom(aLddChanRandom),
	  iJobSizeInBytes(0),
	  iSwReadByteOffset(0),
	  iHw32Index(0),
	  iIsrHooked(EFalse),
	  iRandomDfc(RandomDfc, this, 1) // DFC is priority '1'
	{
	TRACE_FUNCTION("CryptoH4JobRandom");
	//	Kern::Printf("CryptoH4JobRandom::CryptoH4JobRandom %x", this);
	}

CryptoH4JobRandom::~CryptoH4JobRandom()
	{
	TRACE_FUNCTION("~CryptoH4JobRandom");
	//	Kern::Printf("CryptoH4JobRandom::~CryptoH4JobRandom %x", this);
	UnHookIsr();
	}


void CryptoH4JobRandom::SetDfcQ(TDfcQue *aDfcQue)
	{
	TRACE_FUNCTION("SetDfcQ");
	iRandomDfc.SetDfcQ(aDfcQue);
	}

void CryptoH4JobRandom::SetDetails(DCryptoJobScheduler *aJobScheduler, 
								   MCryptoJobCallbacks *aCallbacks,
								   TUint32 aNumOfBytes)
	{
	TRACE_FUNCTION("SetDetails");
	//	Kern::Printf("CryptoH4JobRandom::SetDetails");
	iJobScheduler = aJobScheduler;
	iCallbacks = aCallbacks;
	iJobSizeInBytes = aNumOfBytes;
	}

void CryptoH4JobRandom::GetToPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore)
	{
	TRACE_FUNCTION("GetToPddBuffer");
	aBuf = 0;
	aBufLen = 0;
	aMore = EFalse;
	}

void CryptoH4JobRandom::BytesWrittenToPdd(TUint32)
	{
	TRACE_FUNCTION("BytesWrittenToPdd");
	}

void CryptoH4JobRandom::GetFromPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore)
	{
	TRACE_FUNCTION("GetFromPddBuffer");
	
	TInt hw8Index = iHw32Index * 4;
	TUint8 *p = (TUint8 *) iRandomBuffer;
	aBuf = &p[iSwReadByteOffset];

	TInt len = hw8Index - iSwReadByteOffset;
	if(len >= 0)
		{
		aBufLen = len;
		aMore = EFalse;
		}
	else
		{
		// Wrap round condition, but can only return contiguous bytes
		aBufLen = sizeof(iRandomBuffer) - iSwReadByteOffset;
		aMore = ETrue;
		return;
		}
	}

void CryptoH4JobRandom::BytesReadFromPdd(TUint32 aBytes)
	{
	TRACE_FUNCTION("BytesReadFromPdd");
	iSwReadByteOffset += aBytes;
	if(iSwReadByteOffset >= sizeof(iRandomBuffer))
		{
		iSwReadByteOffset -= sizeof(iRandomBuffer);
		}
	iJobSizeInBytes -= aBytes;
	}



void CryptoH4JobRandom::DoSlice(TBool aFirstSlice)
	{
	TRACE_FUNCTION("DoSlice");
	//	Kern::Printf("DoSlice(%d)", aFirstSlice);
	if(aFirstSlice)
		{
		HookIsr();
		}
	
	// Enable RNG interrupt. The interrupt will then queue the
	// "random number ready" DFC when the h/w is ready.
	// (when not on h/w, this immediately queues a DFC)
	EnableIsr();
	}

TBool CryptoH4JobRandom::DoSaveState()
	{
	TRACE_FUNCTION("DoSaveState");
	UnHookIsr();
	return ETrue; // We want DoRestoreState to be called
	}

void CryptoH4JobRandom::DoRestoreState()
	{
	TRACE_FUNCTION("DoRestoreState");
	HookIsr();
	}

void CryptoH4JobRandom::DoReleaseHw()
	{
	TRACE_FUNCTION("DoReleaseHw");
	// Disable RNG interrupt
	DisableIsr();

	// Disable/unhook ISR
	UnHookIsr();

	// Cancel DFC
	iRandomDfc.Cancel();
	
	}

TInt CryptoH4JobRandom::BytesAvailable() const
	{
	TRACE_FUNCTION("BytesAvailable");
	TInt hw8Index = iHw32Index * 4;
	TInt available = hw8Index - iSwReadByteOffset;
	if(available < 0)
		{
		available += sizeof(iRandomBuffer);
		}
	return available;	
	}

void CryptoH4JobRandom::RegionsAvailable(TUint8 * &aPtr1, TInt &aLen1, 
										 TUint8 * &aPtr2, TInt &aLen2) const
	{
	TRACE_FUNCTION("RegionsAvailable");
	TInt hw8Index = iHw32Index * 4;
	TUint8 *p = (TUint8 *) iRandomBuffer;
	aPtr1 = &p[iSwReadByteOffset];

	TInt len = hw8Index - iSwReadByteOffset;
	if(len < 0)
		{
		// Available data crosses buffer end so return two regions
		aLen1 = sizeof(iRandomBuffer) - iSwReadByteOffset;
		aPtr2 = &p[0];
		aLen2 = hw8Index;
		}
	else
		{
		// Available buffer is contiguous
		aLen1 = len;
		aPtr2 = 0;
		aLen2 = 0;
		}
	}



void CryptoH4JobRandom::HookIsr()
	{
	TRACE_FUNCTION("HookIsr");
	//	Kern::Printf("CryptoH4JobRandom::HookIsr iIsrHooked=%d this=%x", iIsrHooked, this);
#ifdef __MARM__
	if(!iIsrHooked)
		{
		TInt r = Interrupt::Bind(EIrqRng, Isr, this);
		if(r != KErrNone) Kern::Fault("CryptoH4JobRandom::HookIsr Bind failed", r);
		r = Interrupt::Enable(EIrqRng);
		if(r != KErrNone) Kern::Fault("CryptoH4JobRandom::HookIsr Enable failed", r);
		iIsrHooked = ETrue;
		}
#endif
	}

void CryptoH4JobRandom::UnHookIsr()
	{
	TRACE_FUNCTION("UnHookIsr");
	//	Kern::Printf("CryptoH4JobRandom::UnHookIsr iIsrHooked=%d this=%x", iIsrHooked, this);
#ifdef __MARM__
	if(iIsrHooked)
		{
		Interrupt::Disable(EIrqRng);
		Interrupt::Unbind(EIrqRng);
		iIsrHooked = EFalse;
		}
#endif
	}



#ifdef __MARM__
void CryptoH4JobRandom::Isr(TAny *aPtr)
	{
	TRACE_FUNCTION("Isr");
	CryptoH4JobRandom *p = static_cast<CryptoH4JobRandom *>(aPtr);
	// Disable RNG interrupt so DFC can run.
	p->DisableIsr();
	// Queue DFC to read the RNG
	p->iRandomDfc.Add();
	}
#endif

/**
  Called when the current h/w opperation is complete
*/
void CryptoH4JobRandom::RandomDfc(TAny* aPtr)
    {
    ((CryptoH4JobRandom*)aPtr)->DoRandomDfc();
    }

void CryptoH4JobRandom::DoRandomDfc()
	{
	TRACE_FUNCTION("DoRandomDfc");
	// Set state to not using hw, if we continue using the h/w we will
	// call EnableIsr which will change the state back to ERunning.
	SetRunning(EFalse);
	//	Kern::Printf("DoRandomDfc");
#ifdef __MARM__
	// Read h/w
	iRandomBuffer[iHw32Index] = TOmap::Register32(KHwBaseRngReg + KHoRng_Out);
#else
	static TUint32 n = 0;
	iRandomBuffer[iHw32Index]= n++;
#endif

	++iHw32Index;
	if(iHw32Index >= sizeof(iRandomBuffer)/sizeof(iRandomBuffer[0]))
		{
		iHw32Index = 0;
		}

	TInt outputAvailable = BytesAvailable();
	TInt space = sizeof(iRandomBuffer) - outputAvailable - 4;
	if((outputAvailable >= iJobSizeInBytes) || (space <= 0))
		{
		// Either have enough data to finish job, or out of buffer
		// space to read more.  We pass available data to the LDD, and
		// declare the slice/job done and return.

		//
		// Pass available data to LDD
		//
		// LDD will call GetFromPddBuffer/BytesReadFromPdd to read the data.
		iLddChanRandom.DataAvailable();
		}
	
	// Are we done yet?
	if(iJobSizeInBytes <= 0)
		{
		// Tell the scheduler that this slice is done
		iJobScheduler->JobComplete(this, KErrNone);
		return;
		}

	// Re-calculate output available and space
	outputAvailable = BytesAvailable();
	space = sizeof(iRandomBuffer) - outputAvailable - 4;

	if((space != 0) && (iJobSizeInBytes-outputAvailable > 0))
		{
		// We have some space and we need more data
		
		// Enable RNG interrupt. The interrupt will then queue the
		// "random number ready" DFC when the h/w is ready.
		// (when not on h/w, this immediately queues a DFC)
		EnableIsr();
		}
	else
		{
		// Job stalled - either out of space or already have enough
		// data but LDD has not take it
		Stalled();
		}
		
	return;
	}

	


// End of file
