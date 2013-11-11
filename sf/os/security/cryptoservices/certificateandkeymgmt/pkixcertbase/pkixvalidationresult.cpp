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


#include "pkixvalidationresult.h"

//validationresult
EXPORT_C CPKIXValidationResultBase* CPKIXValidationResultBase::NewL()
	{
	CPKIXValidationResultBase* s = CPKIXValidationResultBase::NewLC();
	CleanupStack::Pop();//s
	return s;
	}

EXPORT_C CPKIXValidationResultBase* CPKIXValidationResultBase::NewLC()
	{
	CPKIXValidationResultBase* s = new(ELeave) CPKIXValidationResultBase;
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

EXPORT_C CPKIXValidationResultBase::~CPKIXValidationResultBase()
	{
	delete iWarnings;
	if (iPolicies != NULL)
		{
		iPolicies->ResetAndDestroy();
		}
	delete iPolicies;
	iCertWarnings.ResetAndDestroy();
	}

EXPORT_C CPKIXValidationResultBase::CPKIXValidationResultBase()
	:iError(EChainHasNoRoot, 0)
	{
	}

EXPORT_C void CPKIXValidationResultBase::ConstructL()
	{
#ifdef _DEBUG
	// In OOM we don't want the size of the arrays to change because this will
	// cause a change in the numbers of allocated cells
	iWarnings = new(ELeave) CArrayFixFlat<TValidationStatus> (100);
	iPolicies = new(ELeave) CArrayPtrFlat<CX509CertPolicyInfo> (100);
	iPolicies->AppendL(0);
	iPolicies->ResetAndDestroy();
#else
	iWarnings = new(ELeave) CArrayFixFlat<TValidationStatus> (1);
	iPolicies = new(ELeave) CArrayPtrFlat<CX509CertPolicyInfo> (1);
#endif
	}

EXPORT_C const TValidationStatus CPKIXValidationResultBase::Error() const
	{
	return iError;
	}

EXPORT_C const CArrayFixFlat<TValidationStatus>& CPKIXValidationResultBase::Warnings() const
	{
	return *iWarnings;
	}

EXPORT_C const CArrayPtrFlat<CX509CertPolicyInfo>& CPKIXValidationResultBase::Policies() const
	{
	return *iPolicies;
	}

void CPKIXValidationResultBase::Reset()
	{
	iError = TValidationStatus(EChainHasNoRoot, 0);
	iWarnings->Reset();
	RemovePolicies();
	}

void CPKIXValidationResultBase::SetErrorAndLeaveL(const TValidationError aError, const TInt aCert)
	{
	SetError(aError, aCert);
	User::Leave(KErrGeneral);
	}

void CPKIXValidationResultBase::SetError(const TValidationError aError, const TInt aCert)
	{
	iError.iReason = aError;
	iError.iCert = aCert;
	}

void CPKIXValidationResultBase::AppendWarningL(TValidationStatus aWarning)
	{
	iWarnings->AppendL(aWarning);

	if (aWarning.iReason != ECriticalExtendedKeyUsage && aWarning.iReason != ECriticalPolicyMapping && 
		aWarning.iReason != ECriticalDeviceId && aWarning.iReason != ECriticalSid &&
		aWarning.iReason != ECriticalVid && aWarning.iReason != ECriticalCapabilities)
		{
		(*iCertWarnings[iCertWarnings.Count()-1]).AppendWarningL(aWarning);
		}
	}

void CPKIXValidationResultBase::AppendPolicyL(CX509CertPolicyInfo& aPolicy)
	{
	iPolicies->AppendL(&aPolicy);
	}

void CPKIXValidationResultBase::RemovePolicies()
	{
	iPolicies->ResetAndDestroy();
	}

EXPORT_C void CPKIXValidationResultBase::ExternalizeL(RWriteStream& aStream) const
	{
	// iError
	aStream.WriteL(TPckgC<TValidationStatus>(iError));

	// iWarnings
	aStream.WriteInt32L(iWarnings->Count());
	TInt32 i;
	for (i = 0;i < iWarnings->Count(); ++i)
		{
		aStream.WriteL(TPckgC<TValidationStatus>((*iWarnings)[i]));
		}

	// iPolicies
	aStream.WriteInt32L(iPolicies->Count());
	for (i = 0;i < iPolicies->Count(); ++i)
		{
		(*iPolicies)[i]->ExternalizeL(aStream);
		}
	
	// iCertWarnings
	aStream.WriteInt32L(iCertWarnings.Count());
	for (i = 0;i< iCertWarnings.Count(); ++i)
		{
		(*iCertWarnings[i]).ExternalizeL(aStream);
		}
	}

EXPORT_C void CPKIXValidationResultBase::InternalizeL(RReadStream& aStream)
	{
	// iError
	TPckg<TValidationStatus> pckg(iError);
	aStream.ReadL(pckg);

	// iWarnings
	iWarnings->Reset();
	
	TInt32 count=aStream.ReadInt32L();
	TInt32 i;
	for (i = 0;i < count; ++i)
		{
		TValidationStatus warning(EValidatedOK,0);
		TPckg<TValidationStatus> pckg(warning);
		aStream.ReadL(pckg);
		iWarnings->AppendL(warning);
		}

	// iPolicies
	iPolicies->ResetAndDestroy();

	count=aStream.ReadInt32L();
	for (i = 0;i < count; ++i)
		{
		CX509CertPolicyInfo* certPolicyInfo=CX509CertPolicyInfo::NewLC(aStream); 
		iPolicies->AppendL(certPolicyInfo);
		CleanupStack::Pop(certPolicyInfo);
		}
	
	// iCertWarnings
	iCertWarnings.Reset();

	count=aStream.ReadInt32L();
	for (i = 0;i < count; ++i)
		{
		CCertificateValidationWarnings* certWarning=CCertificateValidationWarnings::InternalizeL(aStream);
		CleanupStack::PushL(certWarning);
		iCertWarnings.AppendL(certWarning);
		CleanupStack::Pop(certWarning);
		}
	}

EXPORT_C const RPointerArray<CCertificateValidationWarnings>& CPKIXValidationResultBase::ValidationWarnings() const
	{
	return iCertWarnings;
	}

EXPORT_C void CPKIXValidationResultBase::AppendCertificateValidationObjectL(const CCertificateValidationWarnings& aCertWarning)
	{
	iCertWarnings.AppendL(&aCertWarning);
	}

EXPORT_C void CPKIXValidationResultBase::AppendCriticalExtensionWarningL(TDesC& aCriticalExt)
	{
	(*iCertWarnings[iCertWarnings.Count()-1]).AppendCriticalExtensionWarningL(aCriticalExt);
	}
