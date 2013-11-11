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


#ifndef __CRYPTO_SHA224AND256_H_
#define __CRYPTO_SHA224AND256_H_


#include "shacommon.h"

const TInt KSHA256BlockSize	= 64;
const TInt KSHA224HashSize	= 28;
const TInt KSHA256HashSize	= 32;
		
NONSHARABLE_CLASS(CSHA224And256) : public CBase, public MSHA2Impl
	{	
public:
	//NewL	
	static CSHA224And256* NewL();
	CSHA224And256(const CSHA224And256& aSHAImpl);
	
	// Functions from MSHA2Impl
	virtual void RestoreState();
	virtual void StoreState();
	virtual void Reset(const TAny*);
	virtual void Update(const TUint8* aData,TUint aLength);
	virtual const TDes8& Final(void);

private:
	//Constructors
	CSHA224And256();
	inline void AddLength(const TUint aLength);
	inline void CopyWordToHash(TUint aVal, TUint aIndex);
	void Block();
	void PadMessage();

private:
	TBuf8<KSHA256HashSize> iHash;
	TUint iA;
	TUint iB;
	TUint iC;
	TUint iD;
	TUint iE;
	TUint iF;
	TUint iG;
	TUint iH;
	TUint iData[KSHA256BlockSize];

	TUint iACopy;
	TUint iBCopy;
	TUint iCCopy;
	TUint iDCopy;
	TUint iECopy;
	TUint iFCopy;
	TUint iGCopy;
	TUint iHCopy;
	TUint iNlCopy;
	TUint iNhCopy;	
	TUint iDataCopy[KSHA256BlockSize];
	
	TUint iNl;
	TUint64 iNh;
	};

#endif // __CRYPTO_SHA224AND256_H_
