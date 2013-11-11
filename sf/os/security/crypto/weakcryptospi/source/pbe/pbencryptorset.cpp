/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <random.h>

#include <pbencryptor.h>
#include "pbesymmetricfactory.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/** The maximum block size supported (in bytes) */
const TUint KMaxBlockSizeSupported = 32;
#endif

EXPORT_C CPBEncryptorSet* CPBEncryptorSet::NewL(
	const TPBECipher aCipher, const TDesC8& aKey) 
	{
	CPBEncryptorSet* self = NewLC(aCipher, aKey);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptorSet* CPBEncryptorSet::NewLC(
	const TPBECipher aCipher, const TDesC8& aKey)
	{
	CPBEncryptorSet* self = new(ELeave) CPBEncryptorSet;
	CleanupStack::PushL(self);

	self->ConstructL(aCipher, aKey);
	
	return self;
	}

void CPBEncryptorSet::ConstructL(const TPBECipher aCipher,const TDesC8& aKey)
	{
	TInt blocksize = PBE::GetBlockBytes(aCipher);
	iIV = HBufC8::NewMaxL(blocksize); 
	TPtr8 iv = iIV->Des();
	TRandom::RandomL(iv);

	iCipher = PBE::MakeEncryptorL(aCipher, aKey, iv);
	}

void CPBEncryptorSet::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	if(!iIVSent)
		{
		aOutput.Append(*iIV);
		iIVSent = ETrue;
		}
	iCipher->Process(aInput, aOutput);
	}

void CPBEncryptorSet::ProcessFinalL(const TDesC8& aInput, 
	TDes8& aOutput)
	{
	if(!iIVSent)
		{
		aOutput.Append(*iIV);
		iIVSent = ETrue;
		}
	
	iCipher->ProcessFinalL(aInput, aOutput);
	}


CPBEncryptorSet::CPBEncryptorSet() : iIVSent(EFalse)
	{
	}

CPBEncryptorSet::~CPBEncryptorSet()
	{
	delete iIV;
	delete iCipher;	//	Don't delete iCBCDecryptor, this belongs to iCipher
	}

TInt CPBEncryptorSet::MaxOutputLength(TUint aMaxInputLength) const
    {
	if(iIVSent)
		{
		return (iCipher->MaxOutputLength(aMaxInputLength));
		}
	else 
		{//	If we've not sent the iv yet then its the length 
		//	from the cipher plus a blocksize for the iv
		return (iCipher->MaxOutputLength(aMaxInputLength + iCipher->BlockSize()));
		}
    }

TInt CPBEncryptorSet::MaxFinalOutputLength(TUint aMaxInputLength) const
    {
	if(iIVSent)
		{
		return (iCipher->MaxFinalOutputLength(aMaxInputLength));
		}
	else 
		{//	If we've not sent the iv yet then its the length 
		//	from the cipher plus a blocksize for the iv
		return (iCipher->MaxFinalOutputLength(aMaxInputLength + iCipher->BlockSize()));
		}
    }

EXPORT_C CPBDecryptorSet* CPBDecryptorSet::NewL(const TPBECipher aCipher, 
	const TDesC8& aKey)
	{
	CPBDecryptorSet* self = NewLC(aCipher, aKey);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBDecryptorSet* CPBDecryptorSet::NewLC(const TPBECipher aCipher,
	const TDesC8& aKey)
	{
	CPBDecryptorSet* self = new(ELeave) CPBDecryptorSet;
	CleanupStack::PushL(self);
	TBuf8<KMaxBlockSizeSupported> fakeIV;
	fakeIV.SetLength(PBE::GetBlockBytes(aCipher));
	self->ConstructL(aCipher, aKey, fakeIV);
	return self;
	}

void CPBDecryptorSet::ConstructL(const TPBECipher aCipher, 
	const TDesC8& aKey, const TDesC8& aIV)
	{
	iCipher = PBE::MakeDecryptorL(aCipher, aKey, aIV);
	iIVBuf = HBufC8::NewL(PBE::GetBlockBytes(aCipher));
	}

void CPBDecryptorSet::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	TPtrC8 input = ProcessIV(aInput);
	iCipher->Process(input, aOutput);
	}

void CPBDecryptorSet::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	TPtrC8 input = ProcessIV(aInput);
	iCipher->ProcessFinalL(input, aOutput);
	}

TPtrC8 CPBDecryptorSet::ProcessIV(const TDesC8& aInput)
	{
	if(!iIVSent)
		{
		TPtr8 iIVBufPtr(iIVBuf->Des());
		if(aInput.Length() + iIVBufPtr.Length() < iCipher->BlockSize())
			{
			iIVBufPtr.Append(aInput);
			return TPtrC8();
			}
		else 
			{
			TInt rem = iCipher->BlockSize() - iIVBufPtr.Length();
			iIVBufPtr.Append(aInput.Mid(0, rem));
			CBufferedDecryptor* blockDecryptor = static_cast<CBufferedDecryptor*>(iCipher);
			static_cast<CModeCBCDecryptor*>(blockDecryptor->BlockTransformer())->SetIV(iIVBufPtr);
			iIVSent = ETrue;
			return aInput.Mid(rem);
			}
		}
	else 
		{
		return aInput;
		}
	}

CPBDecryptorSet::CPBDecryptorSet() : iIVSent(EFalse)
	{
	}

CPBDecryptorSet::~CPBDecryptorSet()
	{
	delete iCipher;
	//iCipher owns iCBCDecryptor we _don't_ need to delete it
	delete iIVBuf;
	}

TInt CPBDecryptorSet::MaxOutputLength(TUint aMaxInputLength) const
    {
	if(iIVSent)
		{
		return (iCipher->MaxOutputLength(aMaxInputLength));
		}
	else	//	if we haven't sent the iv yet, then the length the max length of the
		{	//	cipher minus a BlockSize where the IV was sitting (as the iv would be part
			//	of the length they've sent us)
		return (iCipher->MaxOutputLength(aMaxInputLength - iCipher->BlockSize()));
		}
    }

TInt CPBDecryptorSet::MaxFinalOutputLength(TUint aMaxInputLength) const
    {
	if(iIVSent)
		{
		return (iCipher->MaxFinalOutputLength(aMaxInputLength));
		}
	else	//	if we haven't sent the iv yet, then the length the max length of the
		{	//	cipher minus a BlockSize where the IV was sitting (as the iv would be part
			//	of the length they've sent us)
		return (iCipher->MaxFinalOutputLength(aMaxInputLength - iCipher->BlockSize()));
		}
    }
