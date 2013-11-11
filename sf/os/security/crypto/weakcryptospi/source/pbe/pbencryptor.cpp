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

EXPORT_C CPBEncryptorElement* CPBEncryptorElement::NewL(
	const TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV)
	{
	CPBEncryptorElement* self = NewLC(aCipher, aKey, aIV);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBEncryptorElement* CPBEncryptorElement::NewLC(
	const TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV)
	{
	CPBEncryptorElement* self = new(ELeave) CPBEncryptorElement;
	CleanupStack::PushL(self);
	self->ConstructL(aCipher, aKey, aIV);
	return self;
	}

void CPBEncryptorElement::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	iCipher->Process(aInput, aOutput);
	}

void CPBEncryptorElement::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	iCipher->ProcessFinalL(aInput, aOutput);
	}

void CPBEncryptorElement::ConstructL(const TPBECipher aCipher,
	const TDesC8& aKey, const TDesC8& aIV)
	{
	iCipher = PBE::MakeEncryptorL(aCipher, aKey, aIV);
	}

CPBEncryptorElement::CPBEncryptorElement()
	{
	}

CPBEncryptorElement::~CPBEncryptorElement()
	{
	delete iCipher;
	}

TInt CPBEncryptorElement::MaxOutputLength(TUint aMaxInputLength) const
	{
	return iCipher->MaxOutputLength(aMaxInputLength);
	}

TInt CPBEncryptorElement::MaxFinalOutputLength(TUint aMaxInputLength) const
	{
	return iCipher->MaxFinalOutputLength(aMaxInputLength);
	}

EXPORT_C CPBDecryptorElement* CPBDecryptorElement::NewL(
	const TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV)
	{
	CPBDecryptorElement* self = NewLC(aCipher, aKey, aIV);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPBDecryptorElement* CPBDecryptorElement::NewLC(
	const TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV)
	{
	CPBDecryptorElement* self = new(ELeave) CPBDecryptorElement;
	CleanupStack::PushL(self);
	self->ConstructL(aCipher, aKey, aIV);
	return self;
	}

void CPBDecryptorElement::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	iCipher->Process(aInput, aOutput);
	}

void CPBDecryptorElement::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	iCipher->ProcessFinalL(aInput, aOutput);
	}

void CPBDecryptorElement::ConstructL(const TPBECipher aCipher, 
	const TDesC8& aKey, const TDesC8& aIV)
	{
	iCipher = PBE::MakeDecryptorL(aCipher, aKey, aIV);
	}

CPBDecryptorElement::CPBDecryptorElement()
	{
	}

CPBDecryptorElement::~CPBDecryptorElement()
	{
	delete iCipher;
	}

TInt CPBDecryptorElement::MaxOutputLength(TUint aMaxInputLength) const
	{
	return iCipher->MaxOutputLength(aMaxInputLength);
	}

TInt CPBDecryptorElement::MaxFinalOutputLength(TUint aMaxInputLength) const
	{
	return iCipher->MaxFinalOutputLength(aMaxInputLength);
	}
