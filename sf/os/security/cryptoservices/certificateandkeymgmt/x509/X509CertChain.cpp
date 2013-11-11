/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <x509certchain.h>

EXPORT_C TValidationStatus::TValidationStatus(const TValidationError aError, const TInt aCert)
	:iReason(aError), iCert(aCert)
	{
	}

//x509 cert chain
/**
* If the certificate has decoded the members from TeletexString then the return value 
* may be incorrect because TeletexString type is not fully supported by this library.
* Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
* null characters in the second byte of each character. This will work as expected 
* for cases where the string contains ASCII data.
*/
EXPORT_C CArrayPtrFlat<CX509Certificate>* CX509CertChain::DecodeCertsL(const TDesC8& aBinaryData)
	{
	CArrayPtrFlat<CX509Certificate>* temp = new(ELeave) CArrayPtrFlat<CX509Certificate> (1);
	TCleanupItem cleanupCerts(CleanupCertArray, temp);
	CleanupStack::PushL(cleanupCerts);
	TInt pos = 0;//start at the start
	while (pos < aBinaryData.Length())
		{
		CX509Certificate* cert = CX509Certificate::NewLC(aBinaryData, pos);
		temp->AppendL(cert);
		CleanupStack::Pop();
		}
	CleanupStack::Pop();//temp
	return temp;
	}

EXPORT_C CX509CertChain::~CX509CertChain()
	{
	if (iChain)
		{
		iChain->ResetAndDestroy();
		delete iChain;
		}
	}

EXPORT_C TInt CX509CertChain::Count() const
	{
	return iChain->Count();
	}

EXPORT_C const CX509Certificate& CX509CertChain::Cert(TInt aIndex) const
	{
	return *(iChain->At(aIndex));
	}


EXPORT_C TBool CX509CertChain::IsEqualL(const CX509CertChain& aOther) const
	{
	TInt num1 = Count();
	TInt num2 = aOther.Count();
	if (num1 != num2)
		{
		return EFalse;
		}
	
	for (TInt i = 0; i < num1; ++i)
		{
		const CX509Certificate& cert1 = Cert(i);
		const CX509Certificate& cert2 = aOther.Cert(i);
		
		if (!cert1.IsEqualL(cert2))
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}


void CX509CertChain::CleanupCertArray(TAny* aArray)
	{
	CArrayPtrFlat<CX509Certificate>* array = REINTERPRET_CAST(CArrayPtrFlat<CX509Certificate>*, aArray);
	array->ResetAndDestroy();
	delete array;
	}

// x509 certificate warning
EXPORT_C CCertificateValidationWarnings* CCertificateValidationWarnings::NewL(TInt aIndex)
	{
	CCertificateValidationWarnings* self = CCertificateValidationWarnings::NewLC(aIndex);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCertificateValidationWarnings* CCertificateValidationWarnings::NewLC(TInt aIndex)
	{
	CCertificateValidationWarnings* self = new(ELeave) CCertificateValidationWarnings(aIndex);
	CleanupStack::PushL(self);
	return self;
	}

CCertificateValidationWarnings::CCertificateValidationWarnings(TInt aIndex)
	: iCertIndex(aIndex)
	{
	}

EXPORT_C CCertificateValidationWarnings::~CCertificateValidationWarnings()
	{
	iWarnings.Reset();
	iCriticalExtsFound.ResetAndDestroy();
	}

EXPORT_C const RPointerArray<TDesC>& CCertificateValidationWarnings::CriticalExtensionsFound() const
	{
	return iCriticalExtsFound;
	}

EXPORT_C const RArray<TValidationStatus>& CCertificateValidationWarnings::Warnings() const
	{
	return iWarnings;
	}

EXPORT_C TInt CCertificateValidationWarnings::CertIndex() const
	{
	return iCertIndex;
	}

EXPORT_C CCertificateValidationWarnings* CCertificateValidationWarnings::InternalizeL(RReadStream& aStream)
	{
	// iCertIndex
	TInt certIndex = aStream.ReadInt32L();
			
	// create a cert warning using the cert index and ref cert
	CCertificateValidationWarnings* certWarning = CCertificateValidationWarnings::NewLC(certIndex);
	
	// iWarnings
	TInt32 count = aStream.ReadInt32L();
	for (TInt x=0; x<count; ++x)
		{
		TValidationStatus warning(EValidatedOK,0);
		TPckg<TValidationStatus> pckg(warning);
		aStream.ReadL(pckg);
		certWarning->AppendWarningL(warning);	
		}
	
	// iCriticalExtsFound
	count = aStream.ReadInt32L();
	for (TInt x=0; x<count; ++x)
		{
		TInt length = aStream.ReadInt32L();
		HBufC* oid = HBufC::NewLC(length);
		TPtr oidPtr = oid->Des();
		aStream.ReadL(oidPtr, length);
		certWarning->AppendCriticalExtensionWarningL(*oid);	
		CleanupStack::Pop(oid);
		}	

	CleanupStack::Pop(certWarning);
	return certWarning;
	}

EXPORT_C void CCertificateValidationWarnings::ExternalizeL(RWriteStream& aStream) const
	{
	// iCertIndex;
	aStream.WriteInt32L(iCertIndex);
	
	// iWarnings;
	TInt x;
	aStream.WriteInt32L(iWarnings.Count());
	for (x=0; x<iWarnings.Count(); ++x)
		{
		aStream.WriteL(TPckgC<TValidationStatus>(iWarnings[x]));
		}	
	
	// iCriticalExtsFound;
	aStream.WriteInt32L(iCriticalExtsFound.Count());
	for (x=0; x<iCriticalExtsFound.Count(); ++x)
		{
		aStream.WriteInt32L((*iCriticalExtsFound[x]).Length());
		aStream.WriteL(*iCriticalExtsFound[x]);
		}
	}

EXPORT_C void CCertificateValidationWarnings::AppendWarningL(TValidationStatus aWarning)
	{
	iWarnings.AppendL(aWarning);
	}

EXPORT_C void CCertificateValidationWarnings::AppendCriticalExtensionWarningL(TDesC& aCriticalExt)
	{
	iCriticalExtsFound.AppendL(&aCriticalExt);
	}
