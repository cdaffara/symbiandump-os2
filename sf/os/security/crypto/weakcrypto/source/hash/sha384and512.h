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


#ifndef __CRYPTO_SHA384AND512_H_
#define __CRYPTO_SHA384AND512_H_


#include "shacommon.h"

const TInt KSHA512BlockSize	= 128;
const TInt KSHA384HashSize	= 48;
const TInt KSHA512HashSize	= 64;
		
NONSHARABLE_CLASS(CSHA384And512) : public CBase, public MSHA2Impl
	{	
public:
	//NewL	
	static CSHA384And512* NewL();
	CSHA384And512(const CSHA384And512& aSHAImpl);
	
	//From MSHAImpl
	virtual void Reset(const TAny* aValueArr);
	virtual const TDesC8& Final(void);
	virtual void Update(const TUint8* aData,TUint aLength);
	virtual void StoreState();
	virtual void RestoreState();

private:
	//Constructors
	CSHA384And512();

private:
	// Private member functions
	/**
	 * @param aLength - Length of message added in bytes.
	 */ 
	inline void AddLength(const TUint64 aLength);
	inline void CopyWordToHash(TUint64 aVal, TUint aIndex);
	void Block();
	void PadMessage();

private:
	TBuf8<KSHA512HashSize> iHash;
	TUint64 iA;
	TUint64 iB;
	TUint64 iC;
	TUint64 iD;
	TUint64 iE;
	TUint64 iF;
	TUint64 iG;
	TUint64 iH;
	TUint64 iData[KSHA512BlockSize];

	TUint64 iACopy;
	TUint64 iBCopy;
	TUint64 iCCopy;
	TUint64 iDCopy;
	TUint64 iECopy;
	TUint64 iFCopy;
	TUint64 iGCopy;
	TUint64 iHCopy;
	TUint64 iNlCopy;
	TUint64 iNhCopy;
	TUint	iBlockIndexCopy;
	TUint64 iDataCopy[KSHA512BlockSize];
	
	TUint64 iNl; // Lower word of Message Length
	TUint64 iNh; // Higher word of Message Length
	TUint iBlockIndex; // Message Length in the current block
	};

#endif // __CRYPTO_SHA384AND512_H_
