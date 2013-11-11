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
* streamcipher.cpp
*
*/


#include "streamcipher.h"
#include <cryptopanic.h>

EXPORT_C void CStreamCipher::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	Process(aInput, aOutput);
	}

EXPORT_C void CStreamCipher::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	TInt outputIndex = aOutput.Size();

	// aOutput may already have outputIndex bytes of data in it
	// check there will still be enough space to process the result
	__ASSERT_DEBUG(aOutput.MaxLength() - outputIndex >= MaxOutputLength(aInput.Length()), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	aOutput.Append(aInput);

	TPtr8 transformBuf((TUint8*)(aOutput.Ptr()) + outputIndex, aInput.Size(),
		aInput.Size());
	DoProcess(transformBuf);
	}

EXPORT_C TInt CStreamCipher::BlockSize() const
	{
	return (1);
	}

EXPORT_C TInt CStreamCipher::MaxOutputLength(TInt aInputLength) const
	{
	return aInputLength;
	}

EXPORT_C TInt CStreamCipher::MaxFinalOutputLength(TInt aInputLength) const
	{
	return aInputLength;
	}

