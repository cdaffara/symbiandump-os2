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


#include "bufferedtransformation.h"
#include "blocktransformation.h"
#include "padding.h"
#include "../common/inlines.h"
#include <cryptopanic.h>

EXPORT_C CBufferedTransformation::~CBufferedTransformation()
	{
	delete iBT;
	delete iPadding;
	delete iInputStoreBuf;
	}

void CBufferedTransformation::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= MaxOutputLength(aInput.Length()), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	TInt blockSize = iBT->BlockSize();

	if ( (aInput.Size() + iInputStore.Size()) < blockSize )
		{
		iInputStore.Append(aInput);
		}
	else
		{
		TInt outputIndex = aOutput.Size();
		aOutput.Append(iInputStore);

		TInt inputIndex = blockSize - iInputStore.Size();
		aOutput.Append(aInput.Mid(0, inputIndex));

		TPtr8 transformBuf((TUint8*)(aOutput.Ptr()) + outputIndex, blockSize,
			blockSize);
		//This should read: 
		//TPtr8 transformBuf(aOutput.Mid(outputIndex, blockSize));
		//but in the wonderful world of descriptors, Mid returns a TPtrC8 even
		//when called on a TPtr8.  Fantastic eh?
		iBT->Transform(transformBuf);

		outputIndex += blockSize;

		TInt len = aInput.Size() - blockSize;

		for (; inputIndex<=len; inputIndex+=blockSize)
			{
			aOutput.Append(aInput.Mid(inputIndex, blockSize));			
			transformBuf.Set((TUint8*)(aOutput.Ptr()) + outputIndex, blockSize,
				blockSize);
			iBT->Transform(transformBuf);
			outputIndex += blockSize;
			}

		iInputStore.Zero();
		if (inputIndex < aInput.Size())
			iInputStore.Append(aInput.Mid(inputIndex));
		}
	}

TInt CBufferedTransformation::MaxOutputLength(TInt aInputLength) const
	{
	TInt rem = (aInputLength + iInputStore.Size()) % (iBT->BlockSize());
	return ((aInputLength + iInputStore.Size()) - rem);
	}

void CBufferedTransformation::Reset()
	{
	iBT->Reset();
	iInputStore.Zero();
	}

TInt CBufferedTransformation::BlockSize() const
	{
	return (iBT->BlockSize());
	}

TInt CBufferedTransformation::KeySize() const
	{
	return (iBT->KeySize());
	}

EXPORT_C CBlockTransformation* CBufferedTransformation::BlockTransformer() const
{
	return (iBT);
}

CBufferedTransformation::CBufferedTransformation()
	: iInputStore(0,0,0)
	{
	}

void CBufferedTransformation::ConstructL(CBlockTransformation* aBT, CPadding* aPadding)
	{
	iInputStoreBuf = HBufC8::NewL(aBT->BlockSize());
	iInputStore.Set(iInputStoreBuf->Des());

	// Take ownership last - doesn't take ownership if we leave
	iBT = aBT;
	iPadding = aPadding;
	}


// CBufferedEncryptor

EXPORT_C CBufferedEncryptor* CBufferedEncryptor::NewL(
	CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedEncryptor* self = NewLC(aBT,aPadding);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBufferedEncryptor* CBufferedEncryptor::NewLC(
	CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedEncryptor* self = new (ELeave) CBufferedEncryptor();
	CleanupStack::PushL(self);
	self->ConstructL(aBT, aPadding);
	return self;
	}

CBufferedEncryptor::CBufferedEncryptor()
	{
	}

void CBufferedEncryptor::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= MaxFinalOutputLength(aInput.Length()), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));
	Process(aInput, aOutput);
	
	TInt outputIndex = aOutput.Size();
	iPadding->PadL(iInputStore, aOutput);
	assert(aOutput.Size() % iBT->BlockSize() == 0);

	TUint blockSize = iBT->BlockSize();
	TInt len = aOutput.Size() - outputIndex;
	
	for(TInt i=len; i>0; i-=blockSize)
		{
		TPtr8 transformBuf((TUint8*)(aOutput.Ptr()) + outputIndex, blockSize,
			blockSize);
		iBT->Transform(transformBuf);
		outputIndex+=blockSize;
		}

	iInputStore.Zero();
	}

TInt CBufferedEncryptor::MaxFinalOutputLength(TInt aInputLength) const
	{
    return iPadding->MaxPaddedLength(iInputStore.Size() + aInputLength);
	}

// CBufferedDecryptor

EXPORT_C CBufferedDecryptor* CBufferedDecryptor::NewL(
	CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedDecryptor* self = NewLC(aBT,aPadding);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBufferedDecryptor* CBufferedDecryptor::NewLC(
	CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedDecryptor* self = new (ELeave) CBufferedDecryptor();
	CleanupStack::PushL(self);
	self->ConstructL(aBT, aPadding);
	return self;
	}

CBufferedDecryptor::CBufferedDecryptor()
	{
	}

void CBufferedDecryptor::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= MaxFinalOutputLength(aInput.Length()), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	assert((aInput.Size() + iInputStore.Size()) % iPadding->BlockSize()==0);
	assert(aInput.Size() + iInputStore.Size() !=0 );
	assert(iPadding->BlockSize() % BlockSize() == 0);
	//1) Decrypt into aOutput up till the last full _padding_ blocksize
	//If this panics with descriptor problems, you've probably called
	//ProcessFinalL with a non-_padding_ blocksized aligned amount of data.
	TInt lenToDecrypt = aInput.Size() - iPadding->BlockSize();
	if(lenToDecrypt > 0)
		{
		Process(aInput.Left(lenToDecrypt), aOutput);
		assert(iInputStore.Size()==0);
		}
	else
		{
		lenToDecrypt = 0;
		}
	
	//2) Decrypt the last _padding_ blocksize into a new buffer
	HBufC8* padBuf = HBufC8::NewLC(iPadding->BlockSize());
	TPtr8 padPtr = padBuf->Des(); 
	Process(aInput.Mid(lenToDecrypt), padPtr);
	assert(iInputStore.Size()==0);
		
	//3) Unpad that last _padding_ blocksize into aOutput
	// Note that padding systems must always, like everything else in crypto,
	// _append_ data.
	iPadding->UnPadL(padPtr, aOutput);

	CleanupStack::PopAndDestroy(padBuf);
	}

TInt CBufferedDecryptor::MaxFinalOutputLength(TInt aInputLength) const
	{
	return iPadding->MaxUnPaddedLength(aInputLength + iInputStore.Size());
	}
