/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkixCertChainHelper.h"
#include <pkixcertchain.h>
#include <pkixvalidationresult.h>
#include <unifiedcertstore.h>

_LIT(KPanicCat, "CPKIXCertChainHelper");

CPKIXCertChainHelper* CPKIXCertChainHelper::NewL(RFs& aFs)
	{
	CPKIXCertChainHelper* self = new (ELeave) CPKIXCertChainHelper();
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);
	return self;
	}
	
CPKIXCertChainHelper::CPKIXCertChainHelper() :
	CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);
	}

void CPKIXCertChainHelper::ConstructL(RFs& aFs)
	{
	iCertStore = CUnifiedCertStore::NewL(aFs, EFalse);
	}

CPKIXCertChainHelper::~CPKIXCertChainHelper()
	{
	Cancel();
	delete iCertStore;
	}

CUnifiedCertStore& CPKIXCertChainHelper::CertStore()
	{
	return *iCertStore;
	}

void CPKIXCertChainHelper::Validate(CPKIXCertChainBase& aCertChain,
									CPKIXValidationResult& aValidationResult, 
									const TTime& aValidationTime,
									TRequestStatus& aStatus)
	{
	StartValidate(aCertChain, aValidationResult, aValidationTime, NULL,
				  aStatus);
	}
	
void CPKIXCertChainHelper::Validate(CPKIXCertChainBase& aCertChain,
									CPKIXValidationResult& aValidationResult, 
									const TTime& aValidationTime,
									const CArrayPtr<HBufC>& aInitialPolicies, 
									TRequestStatus& aStatus)
	{
	StartValidate(aCertChain, aValidationResult, aValidationTime, &aInitialPolicies,
				  aStatus);
	}

void CPKIXCertChainHelper::CancelValidate()
	{
	if (iState == EInitializeCertStore ||
		iState == EValidateChain)
		{
		Cancel();
		}
	}

void CPKIXCertChainHelper::StartValidate(CPKIXCertChainBase& aCertChain,
										 CPKIXValidationResult& aValidationResult, 
										 const TTime& aValidationTime,
										 const CArrayPtr<HBufC>* aInitialPolicies, 
										 TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iState == EIdle, User::Panic(KPanicCat, 1));
	iCertChain = &aCertChain;
	iValidationResult = &aValidationResult;
	iValidationTime = aValidationTime;
	iInitialPolicies = aInitialPolicies;
	iClientStatus = &aStatus;
	aStatus = KRequestPending;
	InitializeCertStore();
	}

void CPKIXCertChainHelper::InitializeCertStore()
	{
	iState = EInitializeCertStore;
	if (iCertStoreInitialized)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		iCertStore->Initialize(iStatus);
		}
	SetActive();
	}

void CPKIXCertChainHelper::ValidateChainL()
	{
	iState = EValidateChain;
	if (iInitialPolicies)
		{
		iCertChain->ValidateL(*iValidationResult,
							  iValidationTime,
							  *iInitialPolicies,
							  iStatus);
		}
	else
		{
		iCertChain->ValidateL(*iValidationResult,
							  iValidationTime,
							  iStatus);
		}
	SetActive();
	}

void CPKIXCertChainHelper::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EInitializeCertStore:
			iCertStoreInitialized = ETrue;
			ValidateChainL();
			break;

		case EValidateChain:
			Complete(KErrNone);
			break;

		default:
			User::Invariant();
		}
	}

TInt CPKIXCertChainHelper::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void CPKIXCertChainHelper::DoCancel()
	{
	TInt result = KErrCancel;
	
	switch (iState)
		{
		case EInitializeCertStore:
			iCertStore->CancelInitialize();
			break;

		case EValidateChain:
			if (iStatus == KRequestPending)
				{
				iCertChain->CancelValidate();
				}
			else
				{
				result = iStatus.Int();
				}
			break;

		default:
			// Do nothing
			break;
		}

	Complete(result);
	}

void CPKIXCertChainHelper::Complete(TInt aError)
	{
	iState = EIdle;
	iCertChain = NULL;
	iValidationResult = NULL;
	iInitialPolicies = NULL;
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	}
