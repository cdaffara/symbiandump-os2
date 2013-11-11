/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <errno.h>
#include "stringconv.h"
#include "logger.h"
#include <f32file.h>

void Panic(TInt aCode)
{
	dbg << Log::Indent() << "Panic(" << aCode << ")" << Log::Endl();
	FatalError();
}

void User::LeaveIfError(TInt aError)
{
	if(aError<0) User::Leave(aError);
}

void User::Invariant()
{ 
	FatalError();
}
void User::Panic(const TDesC &aCategory, TInt aReason) 
{ 
	dbg << Log::Indent() << "User::Panic('" << stringFromUtf16(aCategory) << "'," << aReason << ") called" << Log::Endl();
	FatalError(); 
}

void User::Leave(TInt aReason) 
{ 
	dbg << Log::Indent() << "User::Leave(" << aReason << ") - Not supported by this port" << Log::Endl();
	FatalError(); 
}

TDesC8::TDesC8()
	: iCurrentLength(0)
{
}

TDesC8::TDesC8(const TDesC8 &aRef)
	: iCurrentLength(aRef.iCurrentLength)
{
}

TDesC8::TDesC8(TInt aLength)
	: iCurrentLength(aLength)
{
}

TBool TDesC8::operator==(const TDesC8 &aDes) const
{
	if(Length() != aDes.Length())
		{
		return EFalse;
		}
	
	if(memcmp(Ptr(), aDes.Ptr(), Length()) == 0)
		{
		return ETrue; // Identical
		}
	return EFalse;
}



TDes8::TDes8()
	: TDesC8(0), iMaxLength(0)
{
}

TDes8::TDes8(const TDes8 &aRef)
	: TDesC8(aRef), iMaxLength(aRef.iMaxLength)
{
}



TDes8::TDes8(TInt aLength,TInt aMaxLength)
	: TDesC8(aLength), iMaxLength(aMaxLength)
{
}

void TDes8::Copy(const TDesC16 &aDes)
{
	TInt len=aDes.Length();
	SetLength(len);
	const TUint16 *pS=aDes.Ptr();
	const TUint16 *pE=pS+len;
	TUint8 *pT=const_cast<TUint8 *>(Ptr());
	while (pS<pE)
		{
		TUint c=(*pS++);
		if (c>=0x100)
			c=1;
		*pT++=(TUint8)c;
		}
}



void TDes8::SetLength(TInt aLength)
{
	if(aLength < 0 || aLength > iMaxLength) FatalError();
	iCurrentLength = aLength;
}
	

TPtr8::TPtr8(const TPtr8 &aRef)
	: TDes8(aRef), iPtr(aRef.iPtr)
{
}

TPtr8 &TPtr8::operator=(const TPtr8 &aRhs)
{
	if(this == &aRhs) return *this; // handle self assignment
	if(aRhs.Length() > MaxLength()) FatalError();
	memcpy(iPtr, aRhs.Ptr(), aRhs.Length());
	SetLength(aRhs.Length());

	return *this;
}

const TUint8 *TPtr8::Ptr() const
{
	return iPtr;
}

const TUint8 &TPtr8::operator[](TInt anIndex) const
{
	if(anIndex < 0 || anIndex >= Length())
		{
		dbg << Log::Indent() << "TPtrC8 bounds check failure" << Log::Endl();
		FatalError();
		}
	return iPtr[anIndex];
}


void TPtr8::Append(TChar aChar)
{
	if(iCurrentLength+1 > iMaxLength)
		{
		dbg << Log::Indent() << "TPtr8::Append range check failure" << Log::Endl();
		FatalError();
		}
	iPtr[iCurrentLength++] = aChar;
}





TPtrC8::TPtrC8(const TPtrC8 &aRef)
	: TDesC8(aRef), iPtr(aRef.iPtr)
{
}

TPtrC8 &TPtrC8::operator=(const TPtrC8 &aRhs)
{
	if(this == &aRhs) return *this; // handle self assignment
	if(aRhs.Length() > Length()) FatalError();
	memcpy(const_cast<TUint8 *>(iPtr), aRhs.Ptr(), aRhs.Length());

	return *this;
}

void TPtrC8::Set(TUint8 *aBuf, TInt aLength)
{
	iPtr = aBuf,
	iCurrentLength = aLength;
}

const TUint8 *TPtrC8::Ptr() const
{
	return iPtr;
}

const TUint8 &TPtrC8::operator[](TInt anIndex) const
{
	if(anIndex < 0 || anIndex >= Length())
		{
		dbg << Log::Indent() << "TPtrC8 bounds check failure" << Log::Endl();
		FatalError();
		}
	return iPtr[anIndex];
}



TDesC16::TDesC16()
	: iCurrentLength(0)
{
}

TDesC16::TDesC16(const TDesC16 &aRef)
	: iCurrentLength(aRef.iCurrentLength)
{
}

TDesC16::TDesC16(TInt aLength)
	: iCurrentLength(aLength)
{
}


TDes16::TDes16()
	: TDesC16(0), iMaxLength(0)
{
}

TDes16::TDes16(const TDes16 &aRef)
	: TDesC16(aRef), iMaxLength(aRef.iMaxLength)
{
}



TDes16::TDes16(TInt aLength,TInt aMaxLength)
	: TDesC16(aLength), iMaxLength(aMaxLength)
{
}


void TDes16::SetLength(TInt aLength)
{
	if(aLength < 0 || aLength > iMaxLength) FatalError();
	iCurrentLength = aLength;
}

TBool TDesC16::operator==(const TDesC8 &aDes) const
{
	if(Length() != aDes.Length())
		{
		return EFalse;
		}
	
	if(memcmp(Ptr(), aDes.Ptr(), Length()*2) == 0)
		{
		return ETrue; // Identical
		}
	return EFalse;
}


TPtrC16::TPtrC16(const TPtrC16 &aRef)
	: TDesC16(aRef), iPtr(aRef.iPtr)
{
}

TPtrC16 &TPtrC16::operator=(const TPtrC16 &aRhs)
{
	if(this == &aRhs) return *this; // handle self assignment
	if(aRhs.Length() > Length()) FatalError();
	memcpy(const_cast<TUint16 *>(iPtr), aRhs.Ptr(), aRhs.Length()*2);

	return *this;
}

void TPtrC16::Set(TUint16 *aBuf, TInt aLength)
{
	iPtr = aBuf,
	iCurrentLength = aLength;
}


const TUint16 *TPtrC16::Ptr() const
{
	return iPtr;
}

const TUint16 &TPtrC16::operator[](TInt anIndex) const
{
	if(anIndex < 0 || anIndex >= Length())
		{
		dbg << Log::Indent() << "TPtrC16 bounds check failure" << Log::Endl();
		FatalError();
		}
	
	return iPtr[anIndex];
}




TPtr16::TPtr16(const TPtr16 &aRef)
	: TDes16(aRef), iPtr(aRef.iPtr)
{
}

TPtr16 &TPtr16::operator=(const TPtr16 &aRhs)
{
	if(this == &aRhs) return *this; // handle self assignment
	if(aRhs.Length() > MaxLength()) FatalError();
	memcpy(iPtr, aRhs.Ptr(), aRhs.Length()*sizeof(TUint16));
	SetLength(aRhs.Length());

	return *this;
}

void TPtr16::Copy(const TDesC8 &aDes)
{
	// This is not quite 100% compatible because it does a correct
	// UTF-8 to UCS-2 conversion, instead of just stuffing in zeros.
	TInt outLength = 0;
	TText *outBuf = utf16FromUtf8(aDes.Ptr(), aDes.Length(), outLength);

	if(outLength > MaxLength()) FatalError();
	memcpy(iPtr, outBuf, outLength*2);
	SetLength(outLength);
	delete [] outBuf;
}


const TUint16 *TPtr16::Ptr() const
{
	return iPtr;
}

const TUint16 &TPtr16::operator[](TInt anIndex) const
{
	if(anIndex < 0 || anIndex >= Length())
		{
		dbg << Log::Indent() << "TPtr16 bounds check failure" << Log::Endl();
		FatalError();
		}
	
	return iPtr[anIndex];
}

void TPtr16::Append(TChar aChar)
{
	if(iCurrentLength+1 > iMaxLength)
		{
		dbg << Log::Indent() << "TPtr16::Append range check failure" << Log::Endl();
		FatalError();
		}
	iPtr[iCurrentLength++] = aChar;
}






TLIT16::TLIT16(const char *aStr)
	: iDes(0,0)
{


	// Expand UTF-8 into internal UTF-16LE representation
	TInt outputWords = 0;
	TText *outputBuf = utf16FromUtf8((TUint8 *)aStr, strlen(aStr), outputWords);
	// Work out actual string length and save the buffer and ptr in out descriptor
	iDes.Set(outputBuf, outputWords);
}

TLIT16::TLIT16(const TLIT16 &aRef)
	: iDes(aRef.iDes.Ptr(), aRef.iDes.Length())
{
}


TLIT16::~TLIT16()
{
	// May leak here, but TLITs should be at global scope anyway...
}


const TDesC16* TLIT16::operator&() const
{
	return &iDes;
}

TLIT16::operator const TDesC16&() const
{
	return iDes;
}

const TDesC16& TLIT16::operator()() const
{
	return iDes;
}


TLIT8::TLIT8(const char *aStr)
	: TPtrC8((const TUint8 *)aStr, strlen(aStr))
{
}

TLIT8::TLIT8(const TLIT8 &aRef)
	: TPtrC8(aRef)
{
}



void CleanupStack::PopAndDestroy(RArrayBase *aRArray)
{
	aRArray->Close();
}


void CleanupStack::PopAndDestroy(RFs *aFs)
{
	aFs->Close();
}

void PushL(void *)
{
}


// End of file
