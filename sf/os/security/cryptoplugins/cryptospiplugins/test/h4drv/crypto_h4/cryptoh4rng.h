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
#ifndef __CRYPTOH4RNG__
#define __CRYPTOH4RNG__
#include "cryptojobs.h"

class DCryptoH4Chan;
class DLddChanRandom;
class CryptoH4JobRandom : public CryptoJobRandom
	{
public:
	CryptoH4JobRandom(DLddChanRandom &aLddChanRandom);
	virtual ~CryptoH4JobRandom();

	virtual void SetDfcQ(TDfcQue *aDfcQue);

	virtual void SetDetails(DCryptoJobScheduler *aJobScheduler, MCryptoJobCallbacks *aCallbacks,
							TUint32 aNumOfBytes);

	virtual void GetToPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore);
	virtual void BytesWrittenToPdd(TUint32 aBytes);

	virtual void GetFromPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore);
	virtual void BytesReadFromPdd(TUint32 aBytes);


private:	
	// Implementations of virtual base class methods to support DCryptoJobScheduler
	virtual void DoSlice(TBool aFirstSlice);
	virtual TBool DoSaveState();
	virtual void DoRestoreState();
	virtual void DoReleaseHw();
	
	TInt BytesAvailable() const;
	void RegionsAvailable(TUint8 * &aPtr1, TInt &aLen1, 
						  TUint8 * &aPtr2, TInt &aLen2) const;

	DLddChanRandom &iLddChanRandom;
	
	// Details of job
	TUint32 iRandomBuffer[64]; // Temporary buffer for PDD to write into
	TInt iJobSizeInBytes;
	TInt iSwReadByteOffset;
	TInt iHw32Index;

	// Hook/enable unhook/disable ISR in top level OMAP h/w
	// Done at job scheduling level
	TBool iIsrHooked;
	void HookIsr();
	void UnHookIsr();

	// Enable/disable RNG ISR in Mask register
	// Done for each byte of data.
	inline void EnableIsr();
	inline void DisableIsr();
	
	static void Isr(TAny *aPtr);

    static void RandomDfc(TAny* aPtr);
	void DoRandomDfc();
    TDfc iRandomDfc;	
	};
#endif
