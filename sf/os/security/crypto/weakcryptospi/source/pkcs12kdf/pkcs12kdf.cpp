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


#include <hash.h>
#include <bigint.h>
#include "pkcs12kdf.h"


EXPORT_C HBufC8* PKCS12KDF::GeneratePasswordLC(const TDesC& aDes)
/**
	Convert the supplied string to a byte string, as described
	in SB.1 of the PKCS 12 v1.0.
	
	Each character is converted to a big endian two-byte value,
	and a terminating NULL character is appended to the end.
	
	@param	aDes			String to use as password.
 */
	{
	const TInt len = aDes.Length();
	HBufC8* pwdBytes = HBufC8::NewMaxLC((len + 1) * 2);
	TPtr8 pbDes = pwdBytes->Des();
	
	TInt i = 0;
	while (i < len)
		{
		TUint16 ch = aDes[i];
		pbDes[i * 2] = ch >> 8;
		pbDes[(i * 2) + 1] = ch;
		++i;
		}
	pbDes[i * 2] = pbDes[(i * 2) + 1] = 0;
	
	return pwdBytes;
	}

static TInt CeilDiv(TInt aNumerator, TInt aDenominator)
/**
	Utility function returns ceil(aNumerator / aDenominator).

	@param	aNumerator		The numerator.
	@param	aDenominator	Denominator, which cannot be zero.
	@return					ceil(aNumerator / aDenominator)
 */
	{
	TInt result = aNumerator / aDenominator;
	if ((aNumerator % aDenominator) > 0)
		++result;
	return result;
	}

EXPORT_C void PKCS12KDF::DeriveKeyL(
	TDes8& aKey, TIDByteType aIDType,
	const TDesC8& aPasswd, const TDesC8& aSalt, const TUint aIterations)
/**
	Generate a key for the supplied password and salt.
	This implementation uses SHA1 as the hashing algorithm.
	
	@param	aKey			Descriptor which will hold key.  On entry
							its length must be set to the expected key length.
	@param	aIDType			Whether this function is being called to generate
							an (en|de)cryption key, an initialization vector,
							or a key for MAC-ing.  See SB.3 of spec.
	@param	aPasswd			Password string.  To comply with PKCS#12 spec,
							this must have 2-byte big-endian characters with
							a terminating null character.
	@param	aSalt			Used with aPasswd to generate key.
	@param	aIterations		Number of times to call the hash function for
							each block in the key.
	
	@panic	PKCS#12 16		Password is empty (debug only.)
	@panic	PKCS#12 17		Password does not contain an even number of bytes,
							and so can't use double-byte characters (debug only.)
	@panic	PKCS#12 18		The final two-byte character is not a null terminator,
							or a null terminator occurs before the end (debug only.)
 */
	{
	__ASSERT_DEBUG(aPasswd.Length() >= 2, Panic(EDKEmptyPswd));
	__ASSERT_DEBUG((aPasswd.Length() % 2) == 0, Panic(EDKOddPswdByteCount));
	TInt useCharCount = aPasswd.Length() / 2;
	TPtrC16 pswd16(reinterpret_cast<const TUint16*>(aPasswd.Ptr()), useCharCount);
	TInt nullPos = pswd16.Locate(L'\0');
	__ASSERT_DEBUG(nullPos == (useCharCount - 1), Panic(EDKBadNullTerminator));

	// use the same notation as the standard
	const TUint8 ID = static_cast<TUint8>(aIDType);
	const TInt u = 160;					// chaining variable length for SHA-1
	const TInt v = 512;					// message input length for SHA-1
	const TInt n = aKey.Length() * 8;	// number of bits required in key
	const TInt p = aPasswd.Length();
	const TInt s = aSalt.Length();
	const TInt r = aIterations;
	
	// (numbered steps are from the standard)
	// 1. Construct a string, D (the "diversifier"), by concatenating
	// v/8 copies of ID.
	const TInt D_LEN = v / 8;
	HBufC8* D_ = HBufC8::NewMaxLC(D_LEN);
	TPtr8 D = D_->Des();
	D.Fill(ID);
	
	// 2. Concatenate copies of the salt together to create a string S
	// of length v * ceil(s/v) bits (the final copy of the salt may be
	// truncated to create S).  Note that if the salt is the empty string,
	// then so is S.
	const TInt S_OVER_V_CEIL = CeilDiv(s, v);
	const TInt S_LEN = (v * S_OVER_V_CEIL) / 8;
	HBufC8* S_ = HBufC8::NewMaxLC(S_LEN);
	TPtr8 S = S_->Des();
	S.Repeat(aSalt);
	
	// 3. Concatenate copies of the password together to create a string P
	// of length v * ceil(p/v) bits (the final copy of the password may be
	// truncated to create P).  Note that if the password is the empty string
	// then so is P.
	const TInt P_OVER_V_CEIL = CeilDiv(p, v);
	const TInt P_LEN = (v * P_OVER_V_CEIL) / 8;
	HBufC8* P_ = HBufC8::NewMaxLC(P_LEN);
	TPtr8 P = P_->Des();
	P.Repeat(aPasswd);
	
	// 4. Set I=S||P to be the concatenation of S and P.
	const TInt I_LEN = S_LEN + P_LEN;
	HBufC8* I_ = HBufC8::NewLC(I_LEN);
	TPtr8 I = I_->Des();
	I.Copy(S);
	I.Append(P);
	
	// 5. Set c=ceil(n/u).
	const TInt c = CeilDiv(n, u);
	
	// ahead 7: allocate result buffer A
	// (Each Ai has SHA1_HASH bytes.)
	HBufC8* A_ = HBufC8::NewLC(c * SHA1_HASH);
	TPtr8 A = A_->Des();
	
	// 6. For i=1, 2, ..., c, do the following
	
	// pre-allocate SHA1 object, DI, and B buffers
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	
	const TInt DI_LEN = D_LEN + I_LEN;
	HBufC8* DI_ = HBufC8::NewLC(DI_LEN);
	TPtr8 DI = DI_->Des();
	
	const TInt B_LEN = v / 8;
	HBufC8* B_ = HBufC8::NewMaxLC(B_LEN);
	TPtr8 B = B_->Des();
	
	for (TInt i = 1; i <= c; ++i)
		{
		// 6a) Set Ai = H^r(D||I).  (i.e. the rth hash of D||I,
		// H(H(H(...H(D||I))))
		DI.Copy(D);
		DI.Append(I);
		
		sha1->Reset();
		TBuf8<SHA1_HASH> Ai(sha1->Final(DI));
		
		for (TInt iterCount = 2; iterCount <= r; ++iterCount)
			{
			Ai.Copy(sha1->Final(Ai));
			}
		
		// 6b) Concatenate copies of Ai to create a string B of length
		// v bits (the final copy of Ai may be truncated to create B).
		B.Repeat(Ai);
		
		// 6c) Treating I as a concatenation I0, I1, ..., Ik-1 of
		// v-bit blocks, where k=ceil(s/v)+ceil(p/v), modify I by
		// setting Ij=(Ij+B+1) mod 2^v for each j.

		const TInt k = S_OVER_V_CEIL + P_OVER_V_CEIL;
		for (TInt j = 0; j < k; ++j)
			{
			TPtr8 section = I.MidTPtr((v/8) * j, v/8);
			Process6cL(section, B, v);
			}
		
		// 7. Concatenate A1, A2, ..., Ac together to form a pseudo-random
		// bit string, A.
		A.Append(Ai);
		
		// stop building A if already have enough bits for key
		if (A.Length() >= n / 8)
			break;
		}

	// Use the first n bits of A as the output of this entire process.
	aKey.Copy(A.Left(n / 8));
	
	CleanupStack::PopAndDestroy(8, D_);	// B_, DI_, sha1, A_, I_, P_, S_, D_
	}

void PKCS12KDF::Process6cL(TDes8& Ij, const TDesC8& B, TInt v)
/**
	Helper function for DeriveKeyL modifies part of I,
	as described in step 6c of SB.2.
	
	@param	Ij		Section of I (S || P).
	@param	B		rth hash of D || I.
	@param	v		Number of bits to preserve in result.
 */
	{
	// 6c) Treating I as a concatenation I0, I1, ..., Ik-1 of
	// v-bit blocks, where k=ceil(s/v)+ceil(p/v), modify I by
	// setting Ij=(Ij+B+1) mod 2^v for each j.
	
	RInteger RI_Ij = RInteger::NewL(Ij);
	TCleanupItem ciIj = RI_Ij;
	CleanupStack::PushL(ciIj);
	
	RInteger RI_B = RInteger::NewL(B);
	TCleanupItem ciB = RI_B;
	CleanupStack::PushL(ciB);
	
	// these additions can leave
	RI_Ij += RI_B;
	RI_Ij += 1;
	
	HBufC8* result = RI_Ij.BufferLC();
	
	Ij.Zero();
	TInt resultLen = result->Length();
	
	TInt bytesToPreserve = v / 8;
	TInt leadingZeroes = bytesToPreserve - resultLen;
	if (leadingZeroes <= 0)
		Ij.Copy(result->Right(bytesToPreserve));
	else
		{
		Ij.FillZ(leadingZeroes);
		Ij.Append(*result);
		}
	
	CleanupStack::PopAndDestroy(3, &RI_Ij);	// result, ciB, ciIj
	}

#ifdef _DEBUG

void PKCS12KDF::Panic(PKCS12KDF::TPanic aPanic)
/**
	This function is used in debug builds to halt
	the current thread when a logic error is detected.
	
	The current thread is panicked with category "PKCS12KDF"
	and the supplied reason.
	
	@param	aPanic			Converted to numeric value and
							used for the panic reason.
 */
	{
	_LIT(KPanicCat, "PKCS12KDF");
	User::Panic(KPanicCat, aPanic);
	}

#endif

