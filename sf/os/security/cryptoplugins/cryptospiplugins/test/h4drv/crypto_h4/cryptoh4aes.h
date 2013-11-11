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
#ifndef __CRYPTOH4AES__
#define __CRYPTOH4AES__
#include "cryptojobs.h"
#include <drivers/dma.h>

#define FAKE_DMA FakeDma

//#define TDFC_WRAPPER

#ifdef TDFC_WRAPPER
class TDfcWrapper : public TDfc
	{
public:
	TDfcWrapper(const TDfcWrapper &aOrig);
	TDfcWrapper(TDfcFn aFunction, TAny* aPtr, TInt aPriority);
	void Enque();
private:
	TDfcWrapper &operator=(const TDfcWrapper &aRHS);
	void BaseEnque();
    static void DfcWrapperFunc(TAny* aPtr);
	TDfcFn iRealFunction;
	TAny *iRealPtr;
	};
#endif

class DCryptoH4Chan;
class DLddChanAes;
class CryptoH4JobAes : public CryptoJobAes
	{
public:
	CryptoH4JobAes(DLddChanAes &aLddChanAes);
	~CryptoH4JobAes();

	void SetDfcQ(TDfcQue *aDfcQue);

	virtual TUint8 *GetKeyBuffer(); // Max 32 bytes long (256bits)
	virtual TUint8 *GetIVBuffer(); // 16 bytes long
	virtual TUint32 MaxBytes() const;
	virtual TUint8 *GetIOBuffer(); // MaxBytes() long

	virtual void GetToPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore);
	virtual void BytesWrittenToPdd(TUint32 aBytes);

	virtual void GetFromPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore);
	virtual void BytesReadFromPdd(TUint32 aBytes);

	// Must be called after setting the IV (if used)
	virtual TInt SetDetails(DCryptoJobScheduler *aJobScheduler, 
							MCryptoJobCallbacks *aCallbacks,
							TBool aEncrypt, 
							TInt aKeyLength,
							RCryptoDriver::TChainingMode aMode);
	virtual void NotifyReadRequestLength(TUint32 aReadRequestLength);
	virtual void HwPerfCheck();
	
private:	
	// Implementations of virtual base class methods to support DCryptoJobScheduler
	virtual void DoSlice(TBool aFirstSlice);
	virtual TBool DoSaveState();
	virtual void DoRestoreState();
	virtual void DoReleaseHw();

	void MaybeSetupWriteDmaToHw();
#ifdef FAKE_DMA
	void FakeDma();
#endif

	DLddChanAes &iLddChanAes;
	
	// Details of job
	TBool iEncrypt;
	RCryptoDriver::TChainingMode iMode;

	TUint32 iKeyLengthBytes;
	TUint32 iKey[8]; // 128, 192 or 256 bits
	TUint32 iIV[4]; // 128 bits
	TUint32 iSavedState[4]; // Used for CBC chaining

	TUint32 iReadRequestLength;

	// The AES data buffer
	//	TUint32 iAesBuffer[2*4+4]; // Temporary buffer 8 = 2x16 bytes blocks, +4 space (which never gets used)
	TUint32 iAesBuffer[400*4+4]; // Temporary buffer 8 = 2x16 bytes blocks, +4 space (which never gets used)
	// There are 4 indexes into the above buffer, and after converting
	// all to bytes indexes, and allowing for wrapping the following
	// condition should always hold iSwReadIndex <= iHwWriteIndex <=
	// iHwReadIndex <= iSwWriteByteOffset
	TInt32 iSwWriteByteOffset;
	TInt32 iHwReadIndex;
	TInt32 iHwWriteIndex;
	TInt32 iSwReadByteOffset;

	TBool iHwRunning;
	TInt iDmaToHwPending;
	TInt iDmaFromHwPending;

#ifdef FAKE_DMA
	TInt iFakeDmaToHwQueued;
	TInt iFakeDmaFromHwQueued;
#endif
	
	void SetupHw(TBool aUseSavedState);
	void SetupDma(TUint32 aPtr, TBool aToHw);
	void StopHw();
	static void DmaComplete(DDmaRequest::TResult aResult, TAny *aPtr);

    static void DmaToHwCompleteDfc(TAny* aPtr);
	void DoDmaToHwCompleteDfc();
#ifdef TDFC_WRAPPER
	TDfcWrapper iDmaToHwCompleteDfc;
#else
    TDfc iDmaToHwCompleteDfc;	
#endif

	void CheckIndexes() const;
	void CompareIndexes(const char *aName, TInt32 aP1, TInt32 aP2) const;

    static void DmaFromHwCompleteDfc(TAny* aPtr);
	void DoDmaFromHwCompleteDfc();
#ifdef TDFC_WRAPPER
    TDfcWrapper iDmaFromHwCompleteDfc;	
#else
    TDfc iDmaFromHwCompleteDfc;	
#endif
	};
#endif
