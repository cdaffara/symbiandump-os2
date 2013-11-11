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
#include "cryptoh4aes.h"

#if 0
#undef __MARM__
#ifndef __MARM__
#warning "h/w disabled"
#endif
#endif

#ifdef DUMPBUFFER
LOCAL_D void dumpBuffer(const char *aName, TUint32 *aBuf, TUint32 aLen);
#else
#define dumpBuffer(aName, aBuf, aLen)
#endif

CryptoH4JobAes::CryptoH4JobAes(DLddChanAes &aLddChanAes)
	: iLddChanAes(aLddChanAes),
	  iEncrypt(EFalse),
	  iKeyLengthBytes(0),
	  iSwWriteByteOffset(0),
	  iHwReadIndex(0),
	  iHwWriteIndex(0),
	  iSwReadByteOffset(0),
	  iHwRunning(EFalse),
	  iDmaToHwPending(0),
	  iDmaFromHwPending(0),
#ifdef FAKE_DMA
	  iFakeDmaToHwQueued(0),
	  iFakeDmaFromHwQueued(0),
#endif
	  iDmaToHwCompleteDfc(DmaToHwCompleteDfc, this, 1), // DFC is priority '1'
	  iDmaFromHwCompleteDfc(DmaFromHwCompleteDfc, this, 1)
	{
	TRACE_FUNCTION("CryptoH4JobAes");
	}

CryptoH4JobAes::~CryptoH4JobAes()
	{
	TRACE_FUNCTION("~CryptoH4JobAes");
	StopHw();
	}


void CryptoH4JobAes::SetDfcQ(TDfcQue *aDfcQue)
	{
	TRACE_FUNCTION("SetDfcQ");
	iDmaToHwCompleteDfc.SetDfcQ(aDfcQue);
	iDmaFromHwCompleteDfc.SetDfcQ(aDfcQue);
	}

TUint8 *CryptoH4JobAes::GetKeyBuffer()
	{
	TRACE_FUNCTION("GetKeyBuffer");
	return (TUint8 *) &iKey;
	}

TUint8 *CryptoH4JobAes::GetIVBuffer()
	{
	TRACE_FUNCTION("GetIVBuffer");
	return (TUint8 *) &iIV;
	}
	
TUint32 CryptoH4JobAes::MaxBytes() const
	{
	TRACE_FUNCTION("MaxBytes");
	return sizeof(iAesBuffer); // return size in bytes
	}

TUint8 *CryptoH4JobAes::GetIOBuffer()
	{
	TRACE_FUNCTION("GetIOBuffer");
	return (TUint8 *) &iAesBuffer;
	}

void CryptoH4JobAes::GetToPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore)
	{
	TRACE_FUNCTION("GetToPddBuffer");
	CheckIndexes();
	TUint8 *p = (TUint8 *) iAesBuffer;
	aBuf = &p[iSwWriteByteOffset];

	if(iSwReadByteOffset > iSwWriteByteOffset)
		{
		// Available buffer is contiguous
		aBufLen = iSwReadByteOffset - iSwWriteByteOffset;
		if(aBufLen) --aBufLen; // Never use all space to stop index collision
		aMore = EFalse;
		return;
		}
	else
		{
		// Available data crosses buffer end so return two regions
		// OR indexes are equal
		aBufLen = sizeof(iAesBuffer) - iSwWriteByteOffset;
		if(iSwReadByteOffset == 0)
			{
			// Do not fill to end of buffer because index would wrap and collid
			--aBufLen;
			aMore = EFalse;
			return;
			}
		aMore = (iSwReadByteOffset != iSwWriteByteOffset); // Another region to read
		return;
		}
	// Never gets here
	}

void CryptoH4JobAes::BytesWrittenToPdd(TUint32 aBytes)
	{
	TRACE_FUNCTION("BytesWrittenToPdd");
	CheckIndexes();
	iSwWriteByteOffset += aBytes;
	if(iSwWriteByteOffset >= sizeof(iAesBuffer))
		{
		iSwWriteByteOffset -= sizeof(iAesBuffer);
		}
		
	CheckIndexes();
	}

void CryptoH4JobAes::GetFromPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore)
	{
	TRACE_FUNCTION("GetFromPddBuffer");
	CheckIndexes();
	TInt hwWrite8Index = iHwWriteIndex * 4;
	TUint8 *p = (TUint8 *) iAesBuffer;
	aBuf = &p[iSwReadByteOffset];

	TInt len = hwWrite8Index - iSwReadByteOffset;
	if(len >= 0)
		{
		aBufLen = len;
		aMore = EFalse;
		}
	else
		{
		// Wrap round condition, but can only return contiguous bytes
		aBufLen = sizeof(iAesBuffer) - iSwReadByteOffset;
		aMore = (hwWrite8Index != 0);
		}
	CheckIndexes();
	}

void CryptoH4JobAes::BytesReadFromPdd(TUint32 aBytes)
	{
	TRACE_FUNCTION("BytesReadFromPdd");
	CheckIndexes();
	iSwReadByteOffset += aBytes;
	if(iSwReadByteOffset >= sizeof(iAesBuffer))
		{
		iSwReadByteOffset -= sizeof(iAesBuffer);
		}
	CheckIndexes();
	iReadRequestLength -= aBytes;
	}



TInt CryptoH4JobAes::SetDetails(DCryptoJobScheduler *aJobScheduler, 
								MCryptoJobCallbacks *aCallbacks,
								TBool aEncrypt, 
								TInt aKeyLengthBytes,
								RCryptoDriver::TChainingMode aMode)
	{
	TRACE_FUNCTION("TChainingMode");
	//	Kern::Printf("AES Details %s: Key len %d, Mode %s (%d)",
	//				 aEncrypt?"Encrypt":"Decrypt", aKeyLengthBytes, (aMode == RCryptoDriver::ECbcMode)?"CBC":"ECB", aMode);

	if(State() != ECreated)
		{
        return KErrArgument;
		}
	
	iJobScheduler = aJobScheduler;
	iCallbacks = aCallbacks;
	iEncrypt = aEncrypt;
	iKeyLengthBytes = aKeyLengthBytes;

	if((aMode != RCryptoDriver::EEcbMode) && (aMode != RCryptoDriver::ECbcMode))
		{
		return KErrArgument;
		}
	iMode = aMode;
	if(iMode == RCryptoDriver::ECbcMode)
		{
		// For CBC we need to save the IV incase we need to
		// re-initialise the h/w mid-job
		TUint32 *from;
		TUint32 *to;
		if(iEncrypt)
			{
			// For encryption - DoSaveState saves the last encrypted
			// block. We set this to the IV to handle the case where
			// we do not encrypt any blocks before being suspended.
			from = &iIV[0];
			to = &iAesBuffer[((sizeof(iAesBuffer)-16)/4)];
			}
		else
			{
			// For decryption - MaybeSetupWriteDmaToHw maintains
			// iSavedState as a copy of the last ciphertext
			// (pre-decryption) so DoSaveState does not need to do
			// anything.
			//
			// To cover the case where we do not decrypt any blocks
			// before being suspended, we initialise iSavedState to the IV.
			from = &iIV[0];
			to = &iSavedState[0];
			}
		// Save the IV
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		if(iEncrypt)
			{
			dumpBuffer("SetDetails - end of iAesBuffer", to-4, 4);
			}
		else
			{
			dumpBuffer("SetDetails - iSavedState", iSavedState, 4);
			}
		}

	// Reset indexes
	iSwWriteByteOffset = 0;
	iHwReadIndex = 0,
	iHwWriteIndex = 0,
	iSwReadByteOffset = 0;

	return KErrNone;
	}

void CryptoH4JobAes::DoSlice(TBool aFirstSlice)
	{
	TRACE_FUNCTION("DoSlice");
	//	Kern::Printf("DoSlice %s", aFirstSlice?"FIRST":"");
	if(aFirstSlice)
		{
		SetupHw(EFalse);
		}
	
	// Push any available data to user
	TInt r = iCallbacks->DataAvailable();
	if(r != KErrNone)
		{
		iJobScheduler->JobComplete(this,r);
		return;
		}
	// Read available data from user
	r = iCallbacks->DataRequired();
	if(r != KErrNone)
		{
		iJobScheduler->JobComplete(this,r);
		return;
		}
	
	// Setup to read data (if enough is available).
	// 	Kern::Printf("DoSlice - calling MaybeSetupWriteDmaToHw");
	MaybeSetupWriteDmaToHw();

	FAKE_DMA();

	if(!iDmaToHwPending && !iDmaFromHwPending)
		{
		Stalled();
		}

	return;
	}

TBool CryptoH4JobAes::DoSaveState()
	{
	TRACE_FUNCTION("DoSaveState");

	if((iMode == RCryptoDriver::ECbcMode) && iEncrypt)
		{
		// Doing CBC encryption - Need to save a copy of the last
		// ciphertext block (after encryption) so we can use it as the
		// IV if we are later resumed.
		//
		// Last block processed by h/w just BEFORE iHwWriteIndex. If
		// we have not processed any data, then SetDetails will have
		// initialised this to the IV
		TInt32 fromIndex = (iHwWriteIndex!=0) ? (iHwWriteIndex-4) : ((sizeof(iAesBuffer)-16)/4);
		TUint32 *from = &iAesBuffer[fromIndex];
		TUint32 *to = &iSavedState[0];
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		dumpBuffer("DoSaveState - iSavedState", iSavedState, 4);
		}

	StopHw();
	return ETrue; // We want DoRestoreState to be called
	}

void CryptoH4JobAes::DoRestoreState()
	{
	TRACE_FUNCTION("DoRestoreState");
	SetupHw(ETrue);
	}

void CryptoH4JobAes::DoReleaseHw()
	{
	TRACE_FUNCTION("DoReleaseHw");
	StopHw();
#ifndef FAKE_DMA
	// Cancel DFCs - Doesn't work for FAKE_DMA case....
	iDmaToHwCompleteDfc.Cancel();
	iDmaFromHwCompleteDfc.Cancel();
#endif
	}

void CryptoH4JobAes::MaybeSetupWriteDmaToHw()
	{
	TRACE_FUNCTION("MaybeSetupWriteDmaToHw");
	if(!iDmaToHwPending)
		{
		// Calculate space between H/W read index and S/W write index or end of buffer
		TInt hwReadIndex8 = iHwReadIndex*4;
		TInt avail = (iSwWriteByteOffset >= hwReadIndex8) ? (iSwWriteByteOffset - hwReadIndex8) : (sizeof(iAesBuffer) - hwReadIndex8);
		
		if(avail >= 16)
			{
			// At least another block of data is available.
			if((avail <= 31) && (iMode == RCryptoDriver::ECbcMode) && !iEncrypt)
				{
				// Only one complete block is available

				// Doing CBC decryption, so need to save a copy of the
				// last ciphertext block (before it is decrypted) so we
				// can use it as the IV if we are kicked off the h/w
				// and have to reconfigure.
				// Last block available for h/w is at hwReadIndex8
				TUint32 *from = &iAesBuffer[iHwReadIndex];
				TUint32 *to = &iSavedState[0];
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				dumpBuffer("MaybeSetupWriteDmaToHw - iSavedState", iSavedState, 4);
				}
			SetupDma((TUint32)&iAesBuffer[iHwReadIndex], ETrue);
			}
		}
	}


#ifdef FAKE_DMA
void CryptoH4JobAes::FakeDma()
	{
	TRACE_FUNCTION("FakeDma");
	if(iFakeDmaToHwQueued < iDmaToHwPending)
		{
		// Calculate number of 32 bit values in the h/w
		TInt inHw32 = iHwReadIndex - iHwWriteIndex;
		if(inHw32 < 0)
			{
			inHw32 += sizeof(iAesBuffer)/sizeof(iAesBuffer[0]);
			}
		// Convert to number of 16 byte blocks in h/w
		TInt inHwBlocks = inHw32/4;

		if((inHwBlocks + iFakeDmaToHwQueued) < 2)
			{
			// Pipeline is not full, so the next DMA to complete would be a "to h/w"
			// Wait for h/w to be ready
#ifdef __MARM__
			//		Kern::Printf("CryptoH4JobAes::FakeDma - Start waiting for h/w input ready (%x)", TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
			while(! (TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL) & KHtAesCtrlInputReady))
				{
				Kern::Printf("CryptoH4JobAes::FakeDma - Waiting for h/w input ready (%x)", TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
				}
#endif			
			// Queue the fake "to dma" complete DFC
			iDmaToHwCompleteDfc.Enque();
			++iFakeDmaToHwQueued;
			return;
			}
		}

	// Either pipeline is full, or we are out of input data.

	// Check for output
	if(iFakeDmaFromHwQueued < iDmaFromHwPending)
		{
#ifdef __MARM__
		//		Kern::Printf("CryptoH4JobAes::FakeDma - Start waiting for output ready (%x)", TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
		while(! (TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL) & KHtAesCtrlOutputReady))
			{
			Kern::Printf("CryptoH4JobAes::FakeDma - waiting for output ready (%x)",TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
			}
#endif
		// Queue the fake "from dma" complete DFC
		iDmaFromHwCompleteDfc.Enque();
		++iFakeDmaFromHwQueued;
		return;
		}

	return;
	}
#endif




void CryptoH4JobAes::SetupHw(TBool aUseSavedState)
	{
	TRACE_FUNCTION("SetupHw");
	//	Kern::Printf("SetupHw");
#ifdef __MARM__
	// AES_MASK
#ifdef FAKE_DMA
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_MASK, KHtAesMaskAutoIdle);
#else
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_MASK, 
						 KHtAesMaskDmaReqIn | KHtAesMaskDmaReqOut | KHtAesMaskAutoIdle);
#endif
	iHwRunning = EFalse; // Previous MASK register write cleared the start bit.
	
	TUint32 ctrl = 0;
	if(iEncrypt)
		{
			ctrl |= KHtAesCtrlDirection;
		}

	switch(iKeyLengthBytes)
		{
		case 32:
			// KEYS
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY1_L, iKey[0]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY1_H, iKey[1]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY2_L, iKey[2]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY2_H, iKey[3]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY3_L, iKey[4]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY3_H, iKey[5]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY4_L, iKey[6]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY4_H, iKey[7]);
			ctrl |= KHtAesCtrlKeySize256;
			break;
		case 24:
			// KEYS
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY1_L, iKey[0]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY1_H, iKey[1]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY2_L, iKey[2]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY2_H, iKey[3]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY3_L, iKey[4]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY3_H, iKey[5]);
			ctrl |= KHtAesCtrlKeySize192;
			break;
		case 16:
			// KEYS
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY1_L, iKey[0]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY1_H, iKey[1]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY2_L, iKey[2]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_KEY2_H, iKey[3]);
			ctrl |= KHtAesCtrlKeySize128;
			break;
		}
	
			
	
	// IV (CBC only)
	if(iMode == RCryptoDriver::ECbcMode)
		{
		if(!aUseSavedState)
			{
			//		Kern::Printf("Setting IV");
			// Set IV
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_1, iIV[0]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_2, iIV[1]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_3, iIV[2]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_4, iIV[3]);
			dumpBuffer("SetupHw(EFalse) - iIV", iIV, 4);
			}
		else
			{
			// Set IV to saved state
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_1, iSavedState[0]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_2, iSavedState[1]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_3, iSavedState[2]);
			TOmap::SetRegister32(KHwBaseAesReg + KHoAES_IV_4, iSavedState[3]);
			dumpBuffer("SetupHw(ETrue) - iSavedState", iSavedState, 4);
			}
		
		ctrl |= KHsAesCtrlCBC;
		}
	
	// AES_CTRL
	//	Kern::Printf("Setting crtl to %x", ctrl);
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_CTRL, ctrl);

	// AES_MASK START bit to start DMA
	// This is done by SetupDma
#else
	(void)aUseSavedState;

#endif
	}

void CryptoH4JobAes::SetupDma(TUint32 aPtr, TBool aToHw)
	{
	TRACE_FUNCTION("SetupDma");
	//	Kern::Printf("\t\tSetupDMA - %s, iHwReadIndex %d iHwWriteIndex %d", 
	//				 aToHw?"toHw":"fromHw", iHwReadIndex, iHwWriteIndex);
	// Start the h/w
	if(!iHwRunning)
		{
		//		Kern::Printf("SetupDma - starting h/w");
#ifdef __MARM__
		// If h/w is not enabled yet, then set the start bit. This is
		// required even when NOT using DMA...
		TUint32 mask = TOmap::Register32(KHwBaseAesReg + KHoAES_MASK);
		//		Kern::Printf("mask is %x", mask);
		mask |= KHtDesMaskDmaReqStart;
		TOmap::SetRegister32(KHwBaseAesReg + KHoAES_MASK, mask);
		//		Kern::Printf("changed to %x", TOmap::Register32(KHwBaseAesReg + KHoAES_MASK));
#else
		(void)aPtr;
#endif
		iHwRunning = ETrue;
		}

	if(aToHw)
		{
		++iDmaToHwPending;
		SetRunning(ETrue);
		}
	else
		{
		++iDmaFromHwPending;
		SetRunning(ETrue);
		}
	
	}


void CryptoH4JobAes::StopHw()
	{
	TRACE_FUNCTION("StopHw");
#ifdef __MARM__
	// Disable h/w
	TUint32 mask = TOmap::Register32(KHwBaseAesReg + KHoAES_MASK);
	mask &= ~KHtDesMaskDmaReqStart;
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_MASK, mask);
#endif
	iHwRunning = EFalse;
	}



/**
  Called when the current h/w opperation is complete
*/
void CryptoH4JobAes::DmaComplete(DDmaRequest::TResult aResult, TAny *aPtr)
	{
	TRACE_FUNCTION("TResult");
	(void)aResult;
	// Queue our DFC to action the DMA complete notification in our thread.
	reinterpret_cast<TDfc *>(aPtr)->Enque();
	}




void CryptoH4JobAes::DmaToHwCompleteDfc(TAny* aPtr)
    {
    ((CryptoH4JobAes*)aPtr)->DoDmaToHwCompleteDfc();
    }


void CryptoH4JobAes::DoDmaToHwCompleteDfc()
	{
	TRACE_FUNCTION("DoDmaToHwCompleteDfc");
	//	Kern::Printf("**DoDmaToHwCompleteDfc iHwReadIndex %d, iHwWriteIndex %d",iHwReadIndex, iHwWriteIndex);
	--iDmaToHwPending;
	if(iDmaToHwPending < 0) Kern::Fault("DoDmaToHwCompleteDfc - iDmaToHwPending is negative",1);

#ifdef FAKE_DMA
	--iFakeDmaToHwQueued;
	if(iFakeDmaToHwQueued < 0) Kern::Fault("DoDmaToHwCompleteDfc - iFakeDmaToHwQueued is negative",2);
#endif

	CheckIndexes();

#ifdef __MARM__
	if(! (TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL) & KHtAesCtrlInputReady))
		{
		Kern::Fault("DoDmaToHwCompleteDfc - h/w not ready for input!",3);
		}
	//		Kern::Printf("DoDmaToHwCompleteDfc - Writing data into h/w index %d (%x)", iHwReadIndex, TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_1, iAesBuffer[iHwReadIndex]);
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_2, iAesBuffer[iHwReadIndex+1]);
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_3, iAesBuffer[iHwReadIndex+2]);
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_4, iAesBuffer[iHwReadIndex+3]);
#endif

	// Update index to point at next block to be passed to the h/w
	iHwReadIndex += 4; // 4x32bit == 16bytes == block length
	if(iHwReadIndex == sizeof(iAesBuffer)/sizeof(TUint32))
		{
		iHwReadIndex = 0;
		}

	if(!iDmaFromHwPending)
		{
		SetupDma((TUint32)&iAesBuffer[iHwWriteIndex], EFalse);
		}
	
	CheckIndexes();
	
	// Setup to read data (if enough is available).
	MaybeSetupWriteDmaToHw();
	
	FAKE_DMA();
	}

void CryptoH4JobAes::DmaFromHwCompleteDfc(TAny* aPtr)
    {
    ((CryptoH4JobAes*)aPtr)->DoDmaFromHwCompleteDfc();
    }


void CryptoH4JobAes::DoDmaFromHwCompleteDfc()
	{
	TRACE_FUNCTION("DoDmaFromHwCompleteDfc");
	//	Kern::Printf("**DoDmaFromHwCompleteDfc iHwReadIndex %d, iHwWriteIndex %d", iHwReadIndex, iHwWriteIndex);

	--iDmaFromHwPending;
	if(iDmaFromHwPending < 0) Kern::Fault("DoDmaFromHwCompleteDfc - iDmaFromHwPending is negative",1);

#ifdef FAKE_DMA
	--iFakeDmaFromHwQueued;
	if(iFakeDmaFromHwQueued < 0) Kern::Fault("iFakeDmaFromHwQueued - iFakeDmaFromHwQueued is negative",2);
#endif

	CheckIndexes();

#ifdef __MARM__
	if(! (TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL) & KHtAesCtrlOutputReady))
		{
		Kern::Fault("DoDmaToHwCompleteDfc - h/w not ready for output!",3);
		}

	//	Kern::Printf("DoDmaFromHwCompleteDfc - Reading data from h/w index %d (%x)", iHwWriteIndex, TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
	iAesBuffer[iHwWriteIndex] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_1);
	iAesBuffer[iHwWriteIndex+1] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_2);
	iAesBuffer[iHwWriteIndex+2] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_3);
	iAesBuffer[iHwWriteIndex+3] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_4);
#endif

	// Update index to point at next block to be read from the h/w
	iHwWriteIndex += 4; // 4x32bit == 16bytes == block length
	if(iHwWriteIndex == sizeof(iAesBuffer)/sizeof(TUint32))
		{
		iHwWriteIndex= 0;
		}

	CheckIndexes();



	TInt hwWrite8Index = iHwWriteIndex * 4;
	TInt hwRead8Index = iHwReadIndex * 4;

	// Check if we either have enough data to finish the current LDD
	// user read request, or if we are running out of space
	//
	// Calculate data available for xfer to user
	TInt availableForUser = hwWrite8Index - iSwReadByteOffset;
	if(availableForUser < 0)
		{
		availableForUser += sizeof(iAesBuffer);
		}

	if((availableForUser >= sizeof(iAesBuffer) - 32) ||
	   (availableForUser >= iReadRequestLength))
		{
		// Pass available data to user
		TInt r = iCallbacks->DataAvailable();
		if(r != KErrNone)
			{
			iJobScheduler->JobComplete(this,r);
			return;
			}
		}

	// Are we running short of data?
	TInt availableForHw = iSwWriteByteOffset - hwRead8Index;
	if(availableForHw < 0)
		{
		availableForHw += sizeof(iAesBuffer);
		}
	
	if(availableForHw < 16)
		{
		TInt r = iCallbacks->DataRequired();
		if(r != KErrNone)
			{
			iJobScheduler->JobComplete(this,r);
			return;
			}
		}

	// Kick off a new to h/w DMA if one is not already running
	MaybeSetupWriteDmaToHw();
		
	// Current h/w -> iAesBuffer DMA request has completed
	if(iHwWriteIndex != iHwReadIndex)
		{
		SetupDma((TUint32)&iAesBuffer[iHwWriteIndex], EFalse);
		}

	if(!iDmaToHwPending && ! iDmaFromHwPending)
		{
		//		Kern::Printf("\t\tDoDmaFromHwCompleteDfc STALLED (underrun), iHwReadIndex %d iHwWriteIndex %d",
		//					 iHwReadIndex, iHwWriteIndex);
		// Run out of data to process!
		// Tell the scheduler that we are stalled & therefore this slice is done
		Stalled();
		return;
		}


	CheckIndexes();

	FAKE_DMA();
	}

void CryptoH4JobAes::CheckIndexes() const
	{
	TRACE_FUNCTION("CheckIndexes");
	if(iSwWriteByteOffset < 0 || iSwWriteByteOffset > sizeof(iAesBuffer)) Kern::Fault("CryptoH4JobAes::checkIndexes", 1);

	if(iHwReadIndex < 0 || iHwReadIndex > sizeof(iAesBuffer)/sizeof(iAesBuffer[0])) Kern::Fault("CryptoH4JobAes::checkIndexes", 2);

	if(iHwWriteIndex < 0 || iHwWriteIndex > sizeof(iAesBuffer)/sizeof(iAesBuffer[0])) Kern::Fault("CryptoH4JobAes::checkIndexes", 3);

	if(iSwReadByteOffset < 0 || iSwReadByteOffset > sizeof(iAesBuffer)) Kern::Fault("CryptoH4JobAes::checkIndexes", 4);
	
	
	TInt32 d = iSwWriteByteOffset;
	TInt32 c = iHwReadIndex * 4;
	TInt32 b = 	iHwWriteIndex * 4;
	TInt32 a = 	iSwReadByteOffset;

	//	Kern::Printf("%d %d %d %d", a, b, c, d);
	
	TInt32 offset = 0;
	if(b < a) offset = sizeof(iAesBuffer);
	b += offset;
	if(c < b) offset = sizeof(iAesBuffer);
	c += offset;
	if(d < c) offset = sizeof(iAesBuffer);
	d += offset;
	
	if(a>b) Kern::Fault("CryptoH4JobAes::CheckIndexes", 5);
	if(b>c) Kern::Fault("CryptoH4JobAes::CheckIndexes", 6);
	if(c>d) Kern::Fault("CryptoH4JobAes::CheckIndexes", 7);
	}


void CryptoH4JobAes::NotifyReadRequestLength(TUint32 aReadRequestLength)
	{
	TRACE_FUNCTION("NotifyReadRequestLength");
	iReadRequestLength = aReadRequestLength;
	}

/**
   HwPerfCheck

   This function uses 100% of the CPU power to attempt to drive
   the AES h/w as fast as possible.

   This will give some indication of the maximum achievable speed of the h/w
   excluding the overhead of (almost all of) the driver framework.
 */
void CryptoH4JobAes::HwPerfCheck()
	{
	TRACE_FUNCTION("HwPerfCheck");
	SetupHw(EFalse);

	// Start h/w
#ifdef __MARM__
	TUint32 mask = TOmap::Register32(KHwBaseAesReg + KHoAES_MASK);
	mask |= KHtDesMaskDmaReqStart;
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_MASK, mask);
#endif

	// Reset indexes
	iSwWriteByteOffset = 0;
	iHwReadIndex = 0,
	iHwWriteIndex = 0,
	iSwReadByteOffset = 0;

	// Read data
	iCallbacks->DataRequired();
	// Process all data
	while(iHwWriteIndex*4 < iSwWriteByteOffset)
		{
#ifdef __MARM__
		//		Kern::Printf("Ctrl %08x", TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL));
#endif
		// Have we got more data to write to h/w?
		if(iHwReadIndex < iSwWriteByteOffset/4)
			{
			// Yes, but is h/w ready for it?
#ifdef __MARM__
			if(TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL) & KHtAesCtrlInputReady)
				{
				//				Kern::Printf("toHw iHwReadIndex=%d", iHwReadIndex);
				// ok, write data to h/w
				TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_1, iAesBuffer[iHwReadIndex]);
				TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_2, iAesBuffer[iHwReadIndex+1]);
				TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_3, iAesBuffer[iHwReadIndex+2]);
				TOmap::SetRegister32(KHwBaseAesReg + KHoAES_DATA_4, iAesBuffer[iHwReadIndex+3]);
				iHwReadIndex += 4;
				}
#else
			iHwReadIndex += 4;
#endif
			}
		// Do we expect more data from the h/w?
		if(iHwWriteIndex < iSwWriteByteOffset/4)
			{
			// Yes, but is h/w ready?
#ifdef __MARM__
			if(TOmap::Register32(KHwBaseAesReg + KHoAES_CTRL) & KHtAesCtrlOutputReady)
				{
				//				Kern::Printf("ReadHw to iHwWriteIndex=%d", iHwWriteIndex);
				iAesBuffer[iHwWriteIndex] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_1);
				iAesBuffer[iHwWriteIndex+1] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_2);
				iAesBuffer[iHwWriteIndex+2] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_3);
				iAesBuffer[iHwWriteIndex+3] = TOmap::Register32(KHwBaseAesReg + KHoAES_DATA_4);
				iHwWriteIndex += 4;
				}
#else
			iHwWriteIndex += 4;
#endif
			}
		}
	
	// Write data back to user
	iCallbacks->DataAvailable();
	}



	
#ifdef TDFC_WRAPPER
TDfcWrapper::TDfcWrapper(const TDfcWrapper &aOrig)
	: TDfc(DfcWrapperFunc, this, aOrig.iPriority)
	{
	TRACE_FUNCTION("TDfcWrapper");
	iRealFunction = aOrig.iRealFunction,
	iRealPtr = aOrig.iRealPtr;
	SetDfcQ(aOrig.iDfcQ);
	}


TDfcWrapper::TDfcWrapper(TDfcFn aFunction, TAny* aPtr, TInt aPriority)
	: TDfc(DfcWrapperFunc, this, aPriority),
	  iRealFunction(aFunction),
	  iRealPtr(aPtr)
	{
	TRACE_FUNCTION("TDfcWrapper");
	}

void TDfcWrapper::Enque()
	{
	TRACE_FUNCTION("Enque");
	// Clone self and queue the clone
	TDfcWrapper *p = new TDfcWrapper(*this);
	p->BaseEnque();
	}

void TDfcWrapper::BaseEnque()
	{
	TRACE_FUNCTION("BaseEnque");
	TDfc::Enque();
	}


void TDfcWrapper::DfcWrapperFunc(TAny* aPtr)
	{
	TRACE_FUNCTION("DfcWrapperFunc");
	TDfcWrapper *p = (TDfcWrapper *) aPtr;
	p->iRealFunction(p->iRealPtr);
	delete p;
	}
#endif

#ifdef DUMPBUFFER
LOCAL_D void dumpBuffer(const char *aName, TUint32 *aBuf, TUint32 aLen)
	{
	Kern::Printf("%s =", aName);
	TUint8 *buf8 = reinterpret_cast<TUint8 *>(aBuf);
	for(TInt i = 0 ; i < aLen*4; ++i)
		{
		if(i%16 == 0)
			{
			Kern::Printf("\n    ");
			}
		Kern::Printf("%02x ", buf8[i]);
		}
	Kern::Printf("\n");
	}
#endif

// End of file
