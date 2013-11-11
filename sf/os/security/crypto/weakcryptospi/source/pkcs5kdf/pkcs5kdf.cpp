/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "pkcs5kdf.h"

/* Before complaining about the variable names in this file, 
 * read the pkcs5 spec and all will become clear.
 */

EXPORT_C void TPKCS5KDF::DeriveKeyL(TDes8& aKey, const TDesC8& aPasswd, const TDesC8& aSalt, 
	const TUint aIterations)
{
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	CHMAC* hmac = CHMAC::NewL(aPasswd, sha1);
	CleanupStack::Pop(sha1); //hmac now owns it
	CleanupStack::PushL(hmac);

	TUint hashBytes = hmac->HashSize();
	TUint c = aIterations;
	TUint l = aKey.Length() / hashBytes; 
	if(aKey.Length() % hashBytes != 0) //round up if mod !=0
		{
		l+=1;
		}
	TUint r = aKey.Length() - (l-1) * hashBytes; //r == length of last block
	
	HBufC8* TiTemp = HBufC8::NewLC(hashBytes);
	TUint32* Ti = (TUint32*)(TiTemp->Ptr());
	aKey.SetLength(0); //we've already saved the length we want

	HBufC8* STemp = HBufC8::NewLC(aSalt.Length() + sizeof(TUint32));
	TUint32* S = (TUint32*)(STemp->Ptr());

	HBufC8* UiTemp = HBufC8::NewLC(hashBytes);
	TUint32* Ui = (TUint32*)(UiTemp->Ptr());

	const TUint32* salt = (TUint32*)(aSalt.Ptr());
	TUint saltBytes = aSalt.Length();

	for(TUint i = 1; i<=l; i++)
		{
		F(*hmac, Ti, S, Ui, hashBytes, salt, saltBytes, c, i);
		if(i == l)
			aKey.Append((TUint8*)Ti, r);
		else 
			aKey.Append((TUint8*)Ti, hashBytes);
		}

	CleanupStack::PopAndDestroy(UiTemp);
	CleanupStack::PopAndDestroy(STemp);
	CleanupStack::PopAndDestroy(TiTemp);
	CleanupStack::PopAndDestroy(hmac);
	}

void TPKCS5KDF::F(CMessageDigest& aDigest, TUint32* aAccumulator, 
	TUint32* S, TUint32* Ui, TUint aHashBytes, const TUint32* aSalt, 
	TUint aSaltBytes, TUint c, TUint i)
	{
	TUint8 itmp[4];
	itmp[0] = (TUint8)((i >> 24) & 0xff);
	itmp[1] = (TUint8)((i >> 16) & 0xff);
	itmp[2] = (TUint8)((i >> 8) & 0xff);
	itmp[3] = (TUint8)(i & 0xff);
	TUint8* endOfS = Mem::Copy(S, aSalt, aSaltBytes);
	Mem::Copy((TUint32*)endOfS, (TUint32*)&itmp, 4);

	TPtr8 sptr((TUint8*)S, aSaltBytes+4);
	sptr.SetLength(aSaltBytes+4);
	Mem::Copy(aAccumulator, (TUint32*)((aDigest.Final(sptr)).Ptr()),aHashBytes);
	Mem::Copy(Ui, aAccumulator, aHashBytes);

	for(TUint j=1; j<c; j++)
		{
		TPtr8 uiptr((TUint8*)Ui, aHashBytes);
		uiptr.SetLength(aHashBytes);
		Mem::Copy(Ui, (TUint32*)((aDigest.Final(uiptr)).Ptr()), aHashBytes);
		XORString(Ui, aAccumulator, aHashBytes);
		}
	}

inline void TPKCS5KDF::XORString(const TUint32* aOp1, TUint32* aOp2,
	TUint aLength)
	{
	const TUint32* i = aOp1;

	//this will overflow the whole final word if aLength % 4 != 0 
	//but I can't see this mattering cuz all memory allocation is on a word by word basis
	//i don't want to do this byte by byte as it'll be way slower
	//also, every sane digest is going to be a multiple of 4 -- so this isn't a problem
	for( ; aOp1 != (TUint32*)((TUint8*)i + aLength); )
		{
			*aOp2++ ^= *aOp1++;	
		}
	}

