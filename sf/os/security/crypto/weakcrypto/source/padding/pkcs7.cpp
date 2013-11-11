/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <padding.h>
#include <securityerr.h>
#include <cryptopanic.h>

/* CPaddingPKCS7 */
EXPORT_C CPaddingPKCS7* CPaddingPKCS7::NewL(TInt aBlockBytes)
	{
	__ASSERT_ALWAYS(aBlockBytes > 0, User::Leave(KErrArgument));
	return new(ELeave)CPaddingPKCS7(aBlockBytes);
	}

EXPORT_C CPaddingPKCS7* CPaddingPKCS7::NewLC(TInt aBlockBytes)
	{
	CPaddingPKCS7* self = CPaddingPKCS7::NewL(aBlockBytes);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CPaddingPKCS7::CPaddingPKCS7(TInt aBlockBytes):CPadding(aBlockBytes)
	{
	}

void CPaddingPKCS7::DoPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	TInt paddingBytes = BlockSize()-(aInput.Length()%BlockSize());		
	aOutput.Append(aInput);
	aOutput.SetLength(aOutput.Length()+paddingBytes);
	for (TInt i=1;i<=paddingBytes;i++)
		{
		aOutput[aOutput.Length()-i]=(TUint8)(paddingBytes);
		}
	}

void CPaddingPKCS7::UnPadL(const TDesC8& aInput,TDes8& aOutput)
	{
	TUint inputLen = aInput.Length();
 	TUint paddingLen = (TUint)(aInput[inputLen - 1]);

	if (paddingLen > inputLen)
 		{
 		User::Leave(KErrInvalidPadding);
 		}
   
	TInt outlen = aInput.Length() - paddingLen;

	__ASSERT_DEBUG(aOutput.MaxLength() >= outlen, User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	aOutput.Append(aInput.Left(outlen));
	for (TInt i=outlen;i<inputLen;i++)
		{
		//All padding bytes must equal the length of padding expected
		if (aInput[i]!=paddingLen)
			{
			User::Leave(KErrInvalidPadding);
			}
		}
	}

TInt CPaddingPKCS7::MinPaddingLength(void) const
	{
	//if aInputBytes is 1 less than the blocksize then we get 1 byte of padding
	return 1;
	}

TInt CPaddingPKCS7::MaxPaddedLength(TInt aInputBytes) const
	{
	TUint padBytes = BlockSize() - (aInputBytes % BlockSize());
	return padBytes + aInputBytes;
	}

