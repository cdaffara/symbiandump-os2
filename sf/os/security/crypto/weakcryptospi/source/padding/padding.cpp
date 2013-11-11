/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <random.h>
#include <padding.h>
#include <securityerr.h>
#include <cryptopanic.h>

#include "paddingshim.h"

/* CPadding */
CPadding::CPadding(void) : iBlockBytes(-1)
	{
	}

EXPORT_C CPadding::CPadding(TInt aBlockBytes) : iBlockBytes(aBlockBytes)
	{
	__ASSERT_ALWAYS(aBlockBytes > 0, User::Invariant());
	}

EXPORT_C void CPadding::SetBlockSize(TInt aBlockBytes)
	{
	__ASSERT_ALWAYS(aBlockBytes > 0, User::Invariant());
	iBlockBytes = aBlockBytes;
	}

EXPORT_C TInt CPadding::BlockSize(void) const
	{
	return iBlockBytes;
	}

EXPORT_C TInt CPadding::MaxPaddedLength(TInt /*aInputBytes*/) const
	{
	return BlockSize();
	}

EXPORT_C TInt CPadding::MaxUnPaddedLength(TInt aInputBytes) const
	{
	return aInputBytes - MinPaddingLength();
	}

EXPORT_C void CPadding::PadL(const TDesC8& aInput, TDes8& aOutput)
	{
	// Check that the input is small enough to fit inside one padded block
	// Won't leave if input text is equal to blocksize. Let DoPadL handle such situations
	if(aInput.Length() > BlockSize() - MinPaddingLength()
			&& aInput.Length() != BlockSize()) 	
		User::Leave(KErrArgument);
	
	// Check that the output descriptor supplied is large enough to store the result
	if(aOutput.MaxLength() < MaxPaddedLength(aInput.Length())) 	
		User::Leave(KErrOverflow);

	// Call the virtual function, implemented by derived classes
	DoPadL(aInput, aOutput);
	}

TInt CPadding::GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return Extension_(aExtensionId, a0, a1);
	}

/* CPaddingNone */
EXPORT_C CPaddingNone* CPaddingNone::NewL(TInt aBlockBytes)
	{
	__ASSERT_ALWAYS(aBlockBytes > 0, User::Leave(KErrArgument));
	return CPaddingNoneShim::NewL(aBlockBytes);
	}

EXPORT_C CPaddingNone* CPaddingNone::NewLC(TInt aBlockBytes)
	{
	CPaddingNone* self = CPaddingNone::NewL(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CPaddingNone::CPaddingNone(TInt aBlockBytes):CPadding(aBlockBytes)
	{
	}

void CPaddingNone::DoPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	aOutput.Append(aInput);
	}

void CPaddingNone::UnPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= MaxPaddedLength(aInput.Length()), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));
	aOutput.Append(aInput);
	}

TInt CPaddingNone::MinPaddingLength(void) const
	{
	return 0;
	}

TInt CPaddingNone::MaxPaddedLength(TInt aInputSize) const
	{
	return aInputSize;
	}

/* CPaddingSSLv3 */
EXPORT_C CPaddingSSLv3* CPaddingSSLv3::NewL(TInt aBlockBytes)
	{
	__ASSERT_ALWAYS(aBlockBytes > 0, User::Leave(KErrArgument));
	return CPaddingSSLv3Shim::NewL(aBlockBytes);	
	}

EXPORT_C CPaddingSSLv3* CPaddingSSLv3::NewLC(TInt aBlockBytes)
	{
	CPaddingSSLv3* self = CPaddingSSLv3::NewL(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CPaddingSSLv3::CPaddingSSLv3(TInt aBlockBytes):CPadding(aBlockBytes)
	{
	}

void CPaddingSSLv3::DoPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	TInt paddingBytes=BlockSize()-(aInput.Length()%BlockSize());
	aOutput.Append(aInput);
	aOutput.SetLength(aOutput.Length()+paddingBytes);
	for (TInt i=1;i<=paddingBytes;i++)
		{
		aOutput[aOutput.Length()-i]=(TUint8)(paddingBytes-1);
		}
	}

void CPaddingSSLv3::UnPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	TInt paddingLen = aInput[aInput.Length()-1] + 1;

	if (paddingLen > aInput.Length())
		{
		User::Leave(KErrInvalidPadding);
		}

	TInt outlen = aInput.Length() - paddingLen;

	__ASSERT_DEBUG(aOutput.MaxLength() >= outlen, User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	aOutput.Append(aInput.Left(outlen));
	}

TInt CPaddingSSLv3::MinPaddingLength(void) const
	{
	//if aInputBytes is 1 less than the blocksize then we get 1 byte of padding
	return 1;
	}

TInt CPaddingSSLv3::MaxPaddedLength(TInt aInputBytes) const
	{
	TUint padBytes = BlockSize() - (aInputBytes % BlockSize());
	return padBytes + aInputBytes;
	}

/* CPaddingPKCS1Signature */
EXPORT_C CPaddingPKCS1Signature* CPaddingPKCS1Signature::NewL(TInt aBlockBytes)
	{
	return CPaddingPKCS1SignatureShim::NewL(aBlockBytes);
	}

EXPORT_C CPaddingPKCS1Signature* CPaddingPKCS1Signature::NewLC(TInt aBlockBytes)
	{
	CPaddingPKCS1Signature* self = CPaddingPKCS1Signature::NewL(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CPaddingPKCS1Signature::CPaddingPKCS1Signature(TInt aBlockBytes)
	: CPadding(aBlockBytes)
	{
	}

void CPaddingPKCS1Signature::DoPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	aOutput.SetLength(BlockSize());
	TInt i;
	TInt j;
	aOutput[0]=0;
	TInt startOfData=BlockSize()-aInput.Length();
	// PKCS1 also specifies a block type 0 for private key operations but
	// does not recommend its use. This block type (0) is compatible with 
	// unpadded data though so you can create PKCS1 type 0 blocks using 
	// CPaddingNone.
	aOutput[1]=1;				// Block type 1 (private key operation)
	for (i=2;i<(startOfData-1);i++)
		{
		aOutput[i]=0xff;
		}
	j=0;
	aOutput[startOfData-1]=0;				// separator
	for (i=startOfData;i<BlockSize();i++,j++)
		{
		aOutput[i]=aInput[j];
		}
	}
	
void CPaddingPKCS1Signature::UnPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	// erm, oops, this is not quite as simplistic as it first looks...
	// our integer class will strip any leading zeros so we might actually
	// get some real data that starts out looking like padding but isn't 
	// really
	
	TInt inputLen = aInput.Length();
	if (inputLen <=0 )				
		User::Leave(KErrInvalidPadding);	//	Invalid padding data

	// Leading zero may have been stripped off by integer class
	TInt dataStart=0;
	if (aInput[dataStart] == 0)
		{
		++dataStart;
		}

	if (dataStart < inputLen && aInput[dataStart])		//	might be mode one or mode zero,
		{
		++dataStart;
		while (dataStart < inputLen && aInput[dataStart] == 0xff)
			{
			++dataStart;
			}
		
		if (dataStart == inputLen || aInput[dataStart])	//	this would mean theres no zero between 0x01ff and data...so its not mode one
			dataStart=0;			//	mode zero, start from begining of data
		else
			++dataStart;
		}
	else							//	We've definitely got a mode zero 
		{							//	or broken data, assume mode zero
		dataStart=0;		
		}

	TInt len=inputLen-dataStart;

	__ASSERT_DEBUG(aOutput.MaxLength() >= len, User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	aOutput.SetLength(len);
	TInt i=0;
	while (dataStart<inputLen)
		{
		aOutput[i++]=aInput[dataStart++];
		}
	}

TInt CPaddingPKCS1Signature::MinPaddingLength(void) const
	{
	return 11; //0x00, 0x01, <MIN of 8 0xFF octets> , 0x00
	}

/* CPaddingPKCS1Encryption */
EXPORT_C CPaddingPKCS1Encryption* CPaddingPKCS1Encryption::NewL(
	TInt aBlockBytes)
	{
	return CPaddingPKCS1EncryptionShim::NewL(aBlockBytes);
	}

EXPORT_C CPaddingPKCS1Encryption* CPaddingPKCS1Encryption::NewLC(
	TInt aBlockBytes)
	{
	CPaddingPKCS1Encryption* self = CPaddingPKCS1Encryption::NewL(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CPaddingPKCS1Encryption::CPaddingPKCS1Encryption(TInt aBlockBytes)
	: CPadding(aBlockBytes)
	{
	}

void CPaddingPKCS1Encryption::DoPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	aOutput.SetLength(BlockSize());
	
	aOutput[0]=0;
	TInt startOfData=BlockSize()-aInput.Length();
	aOutput[1]=2;				// Block type 2 (public key operation)
	TBuf8<256> rnd(256);
	TRAPD(err, GenerateRandomBytesL(rnd));
	if((err != KErrNone) && (err != KErrNotSecure))
	    User::Leave(err);

	TInt i = 2;
	TInt j = 0;
	for (; i<(startOfData-1);)
		{
		if (rnd[j])
			{
			aOutput[i++]=rnd[j];
			}
		if (++j==256)
			{
			TRAP(err, GenerateRandomBytesL(rnd));
			if((err != KErrNone) && (err != KErrNotSecure))
				User::Leave(err);
			j=0;
			}
		}

	j=0;
	aOutput[startOfData-1]=0;				// separator
	for (i=startOfData;i<BlockSize();i++,j++)
		{
		aOutput[i]=aInput[j];
		}
	}
	
void CPaddingPKCS1Encryption::UnPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	TInt inputLen = aInput.Length();
	if (inputLen <= 0)				
		User::Leave(KErrInvalidPadding);	//	Invalid padding data

	// Leading zero may have been stripped off by integer class
	TInt dataStart=0;
	if (aInput[dataStart] == 0)
		{
		++dataStart;
		}
	
	// expecting mode 2 padding, otherwise broken
	if (dataStart == inputLen || aInput[dataStart] != 2)	
		{
		User::Leave(KErrInvalidPadding);
		}
	++dataStart;

	// skip random non zero bytes
	while (dataStart < inputLen && aInput[dataStart])
		{
		++dataStart;
		}

	// expecting zero separator
	if (dataStart == inputLen || aInput[dataStart] != 0)
		{
		User::Leave(KErrInvalidPadding);		
		}
	++dataStart;

	TInt len = inputLen - dataStart;
	__ASSERT_DEBUG(aOutput.MaxLength() >= len, User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	aOutput.SetLength(len);
	TInt i=0;
	while (dataStart<inputLen)
		{
		aOutput[i++]=aInput[dataStart++];
		}
	}

TInt CPaddingPKCS1Encryption::MinPaddingLength(void) const
	{
	return 11; //0x00, 0x02, <min of 8 random octets>, 0x00
	}
