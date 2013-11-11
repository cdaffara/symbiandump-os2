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


#include <asymmetric.h>
#include <bigint.h>

/* MCryptoSystem */

EXPORT_C MCryptoSystem::MCryptoSystem(void)
	{
	}

/* CEncryptor */

EXPORT_C CEncryptor::CEncryptor(void)
	{
	}

/* CDecryptor */

EXPORT_C CDecryptor::CDecryptor(void)
	{
	}

/* MSignatureSystem */

EXPORT_C MSignatureSystem::MSignatureSystem(void)
	{
	}

/* CRSASignature */ 

EXPORT_C CRSASignature* CRSASignature::NewL(RInteger& aS)
	{
	CRSASignature* self = new(ELeave)CRSASignature(aS);
	return self;
	}

EXPORT_C CRSASignature* CRSASignature::NewLC(RInteger& aS)
	{
	CRSASignature* self = NewL(aS);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const TInteger& CRSASignature::S(void) const
	{
	return iS;
	}

EXPORT_C TBool CRSASignature::operator==(const CRSASignature& aSig) const
	{
	return ( S() == aSig.S() );
	}

EXPORT_C CRSASignature::~CRSASignature(void)
	{
	iS.Close();
	}

EXPORT_C CRSASignature::CRSASignature(RInteger& aS) : iS(aS)
	{
	}

/* CDSASignature */

EXPORT_C CDSASignature* CDSASignature::NewL(RInteger& aR, RInteger& aS)
	{
	CDSASignature* self = new(ELeave)CDSASignature(aR, aS);
	return self;
	}

EXPORT_C CDSASignature* CDSASignature::NewLC(RInteger& aR, RInteger& aS)
	{
	CDSASignature* self = NewL(aR, aS);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const TInteger& CDSASignature::R(void) const
	{
	return iR;
	}

EXPORT_C const TInteger& CDSASignature::S(void) const
	{
	return iS;
	}

EXPORT_C TBool CDSASignature::operator==(const CDSASignature& aSig) const
	{
	return ( R() == aSig.R() && S() == aSig.S() );
	}

EXPORT_C CDSASignature::~CDSASignature(void)
	{
	iR.Close();
	iS.Close();
	}

EXPORT_C CDSASignature::CDSASignature()
	{
	}

EXPORT_C CDSASignature::CDSASignature(RInteger& aR, RInteger& aS) 
	: iR(aR), iS(aS)
	{
	}

