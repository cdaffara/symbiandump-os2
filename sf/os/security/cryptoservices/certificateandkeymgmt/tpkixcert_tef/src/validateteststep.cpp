/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "validateteststep.h"


CValidateCertChainStep::CValidateCertChainStep()
	{
	SetTestStepName(KValidateCertChainStep);
	}

CValidateCertChainStep::~CValidateCertChainStep()
	{
	delete iActiveObject;
	}

TVerdict CValidateCertChainStep::doTestStepPreambleL()
	{
	CPkixCertStepBase::doTestStepPreambleL();
	GetIntFromConfig(ConfigSection(), KNumberOfCerts, iNumberOfCerts);
	return EPass;
	}

void CValidateCertChainStep::PerformTestL()
	{
	CPkixCertStepBase::PerformTestL();
	
	iCertChain->SetSupportedCriticalExtensionsL(iProcessedOids);
	
	TTime time;
	time.UniversalTime();
	
	ValidateL(time, ConfigSection());
	
	CleanupStack::PopAndDestroy(iCertChain);

	}


void CValidateCertChainStep::ValidateL(TTime aValidateTime, TDes& aConfigSection)
	{
	
	CPKIXValidationResult* result = CPKIXValidationResult::NewLC();
	
	iActiveObject = new (ELeave) CValidateCertChainHelper(Logger());
	CleanupStack::PushL(iActiveObject);

	User::LeaveIfError(iActiveObject->PerformTestL(iCertChain, *result, aValidateTime));

	CleanupStack::PopAndDestroy(iActiveObject);
	iActiveObject = NULL;

	TInt expectedError;
	GetIntFromConfig(aConfigSection, KError, expectedError);
		
	if ((result->Error()).iReason != expectedError)
		{
		ERR_PRINTF3(_L("ERROR: Error: %d.  Expected error: %d"), (result->Error()).iReason, expectedError);
		User::Leave(KErrGeneral);
		}
			
	if (expectedError != EValidatedOK)
		{
		//we are expected to error
		TInt certIndex;
		GetIntFromConfig(aConfigSection, KErrorIndex, certIndex);	
		if ((result->Error()).iCert != certIndex)
			{			
			ERR_PRINTF3(_L("ERROR: Error certificate index: %d.  Expected index: %d"), (result->Error()).iCert, certIndex);
			User::Leave(KErrGeneral);			
			}
		else{
			CleanupStack::PopAndDestroy(result);
			return; //we have passed
			}
		}
		
	
	const CArrayFixFlat<TValidationStatus>& warns = result->Warnings();
	RArray<TValidationStatus> resultOldWarnings;
	CleanupClosePushL(resultOldWarnings);	
	TInt i;
	for (i=0; i < warns.Count(); ++i)
		{
		resultOldWarnings.AppendL(warns[i]);
		}

	const RPointerArray<CCertificateValidationWarnings>& certificateWarningsArray = result->ValidationWarnings();	
	
	HBufC* key;

	RArray<TValidationStatus> processedPolicyQualifiers;
	CleanupClosePushL(processedPolicyQualifiers);		
	
	TInt qualifierWarning = 18;

	for (i=0; i < iNumberOfCerts; ++i)
		{
		key = HBufC::NewLC(KCert().Length() + KKeyFormat().Length() + KPolicyIndex().Length()); 
		TPtr ptr(key->Des());
		ptr = KCert();
		ptr.AppendFormat(KKeyFormat(), i);
		ptr.Append(KPolicyIndex());	
		
		TInt policyIndex = 0;
		GetIntFromConfig(aConfigSection, *key, policyIndex);
		
		if (policyIndex > 0)
			{
			TValidationError error = (TValidationError) qualifierWarning;
			processedPolicyQualifiers.AppendL(TValidationStatus(error, policyIndex));
			}

		CleanupStack::PopAndDestroy(key);
		}
	
	//handle 1st part old warnings first - check any warnings we expect are present (later we check no additionals) 
	CheckWarningsPresentAndRemoveL(processedPolicyQualifiers, resultOldWarnings);
	
	RArray<TInt> configOldWarnings;
	CleanupClosePushL(configOldWarnings);
	RArray<TValidationStatus> processedConfigOldWarnings;
	CleanupClosePushL(processedConfigOldWarnings);		
	
	RArray<TValidationStatus> configNewWarnings;
	CleanupClosePushL(configNewWarnings);
	RArray<TPtrC> configCritOids;
	CleanupClosePushL(configCritOids);	
	
	for (TInt cert_index=0; cert_index < iNumberOfCerts; ++cert_index)
		{
		INFO_PRINTF2(_L("Certificate index: %d"), cert_index);
		key = HBufC::NewLC(KCert().Length() + KKeyFormat().Length() + KOldWarnings().Length()); 
		TPtr ptr(key->Des());
		ptr = KCert();
		ptr.AppendFormat(KKeyFormat(), cert_index);
		ptr.Append(KOldWarnings());
		
		GetIntArrayFromConfigL(aConfigSection, *key, configOldWarnings);
		
		processedConfigOldWarnings.Reset();
		for (TInt i=0; i < configOldWarnings.Count(); ++i)
			{
			TValidationError error = (TValidationError) configOldWarnings[i];
			processedConfigOldWarnings.AppendL(TValidationStatus(error, cert_index));
			}
		
		TransferToNewWarningsL(configOldWarnings, configNewWarnings, cert_index);
		
		ptr.Zero();	
		ptr = KCert();
		ptr.AppendFormat(KKeyFormat(), cert_index);
		ptr.Append(KCritOid());
			
		GetStringArrayFromConfigL(aConfigSection, *key, configCritOids);
		
		CleanupStack::PopAndDestroy(key);
		
		//we now have all our arguments from the config file
		
		//handle 1st part old warnings first - check any warnings we expect are present (later we check no additionals) 
		CheckWarningsPresentAndRemoveL(processedConfigOldWarnings, resultOldWarnings);
		
		
		CCertificateValidationWarnings* certWarning = certificateWarningsArray[((iNumberOfCerts - cert_index) - 1)];	//not owned	//warnings returned in reverse order
		
		//check cert and certindex match up correctly		
		if (certWarning->CertIndex() != cert_index)
			{
			ERR_PRINTF3(_L("ERROR: Certificate indexes do not match.  Certificate index: %d.  Expected index: %d"), certWarning->CertIndex(), cert_index);
			User::Leave(KErrGeneral);
			}
		
		key = HBufC::NewLC(KCert().Length() + KKeyFormat().Length() + KPolicyIndex().Length()); 
		TPtr pt(key->Des());
		pt = KCert();
		pt.AppendFormat(KKeyFormat(), cert_index);
		pt.Append(KPolicyIndex());	
			
		TInt policyIndex = 0;
		GetIntFromConfig(aConfigSection, *key, policyIndex);
			
		if (policyIndex > 0)
			{
			TValidationError error = (TValidationError) qualifierWarning;
			configNewWarnings.AppendL(TValidationStatus(error, policyIndex));
			}

		CleanupStack::PopAndDestroy(key);
			
		//check new warnings behave correctly
		CheckWarningsPresentAndRemoveL(certWarning->Warnings(), configNewWarnings);
		
		if (configNewWarnings.Count() != 0)
			{
			for (TInt err=0; err < configNewWarnings.Count(); ++err)
				{
				ERR_PRINTF2(_L("ERROR: Warning(new API) not reported: %d"), (configNewWarnings[err]).iReason);
				}
			User::Leave(KErrGeneral);
			}
		
		
		//check critoids behave correctly
		CheckCriticalExtsPresentAndRemoveL(certWarning->CriticalExtensionsFound(), configCritOids);
		if (configCritOids.Count() != 0)
			{
			for (TInt err=0; err < configCritOids.Count(); ++err)
				{
				ERR_PRINTF2(_L("ERROR: Critical extension not reported: %S"), &configCritOids[err]);
				}			
			User::Leave(KErrGeneral);
			}
			
		configOldWarnings.Reset();
		configNewWarnings.Reset();
		configCritOids.Reset();		
		}
		
	CleanupStack::PopAndDestroy(5, &processedPolicyQualifiers);	
		
	//check if any warnings in old api we weren't expecting	
	if (resultOldWarnings.Count() != 0)
		{	
		for (TInt warn=0; warn < resultOldWarnings.Count(); ++warn)
			{
			ERR_PRINTF3(_L("ERROR: Warning(old API) not reported: %d.  Index: %d"), (resultOldWarnings[warn]).iReason, (resultOldWarnings[warn]).iCert);
			}		
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(2, result);
	}

void CValidateCertChainStep::TransferToNewWarningsL(RArray<TInt>& aOldWarnings, RArray<TValidationStatus>& aNewWarnings, TInt aCertIndex)
	{
	for (TInt i=0; i < aOldWarnings.Count(); ++i)
		{
		switch (aOldWarnings[i])
			{
			case ECriticalExtendedKeyUsage:
				break;
			case ECriticalPolicyMapping:
				break;
			case ECriticalDeviceId:
				break;
			case ECriticalSid:
				break;
			case ECriticalVid:
				break;
			case ECriticalCapabilities:
				break;
			case ECriticalCertPoliciesWithQualifiers:	//handled by ini parameter due to overloading by warning of iCert value
				break;
			default:
				TValidationError error = (TValidationError) aOldWarnings[i];
				aNewWarnings.AppendL(TValidationStatus(error, aCertIndex));							
			}
		}
	}





void CValidateCertChainStep::CheckWarningsPresentAndRemoveL(const RArray<TValidationStatus>& aWarningsToCheckFor, RArray<TValidationStatus>& aWarningsToCheckIn)
	{
	TBool found;
	for (TInt warnings_index=0; warnings_index < aWarningsToCheckFor.Count(); ++warnings_index)
		{
		found = EFalse;
		for (TInt k=0; k < aWarningsToCheckIn.Count(); ++k)
			{
			if (ValidationStatusEqual(aWarningsToCheckFor[warnings_index], aWarningsToCheckIn[k]))
				{
				found = ETrue;
				aWarningsToCheckIn.Remove(k);
				break;
				}
			}
				
		if (!found)	
			{	
			ERR_PRINTF2(_L("ERROR: Warning not present: %d"), aWarningsToCheckFor[warnings_index].iReason);
			User::Leave(KErrGeneral); 
			}
		}	
	}


TBool CValidateCertChainStep::ValidationStatusEqual(const TValidationStatus& aValidationStatus1, const TValidationStatus& aValidationStatus2)
	{
	if (aValidationStatus1.iReason != aValidationStatus2.iReason)
		{
		return EFalse;
		}
	if (aValidationStatus1.iCert != aValidationStatus2.iCert)
		{
		return EFalse;
		}
	return ETrue;	
	}


//takes away from aOidsToCheckAgainst
void CValidateCertChainStep::CheckCriticalExtsPresentAndRemoveL(const RPointerArray<TDesC>& aOidsToLookFor, RArray<TPtrC>& aOidsToCheckAgainst)
	{
	TBool found;
	for (TInt oid_index=0; oid_index < aOidsToLookFor.Count(); ++oid_index)
		{
		found = EFalse;
		for (TInt k=0; k < aOidsToCheckAgainst.Count(); ++k)
			{
			if ((*aOidsToLookFor[oid_index])==(aOidsToCheckAgainst[k]))
				{
				found = ETrue;
				aOidsToCheckAgainst.Remove(k);
				break;
				}
			}
				
		if (!found)	
			{
			ERR_PRINTF2(_L("ERROR: Critical extension OID not present: %S"), aOidsToLookFor[oid_index]);
			User::Leave(KErrGeneral);	
			}
		}		
	}



//////////////////////////************

CValidateCertChainHelper::CValidateCertChainHelper(CTestExecuteLogger& aLogger)
:  CActive(EPriorityStandard), iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}
	
	
CValidateCertChainHelper::~CValidateCertChainHelper()
	{
	Cancel();
	}
	

TInt CValidateCertChainHelper::PerformTestL(CPKIXCertChain* aCertChain, CPKIXValidationResult& aValidResult, TTime aValidateTime)
	{
	iCertChain = aCertChain;
	
	//call async
	aCertChain->ValidateL(aValidResult, aValidateTime, iStatus);
	
	SetActive();
   	CActiveScheduler::Start();
   	
    return iRunError;	
	}
	
	
void CValidateCertChainHelper::DoCancel()
	{
	iCertChain->CancelValidate();
	}
	
	
void CValidateCertChainHelper::RunL()
	{
	iRunError = KErrNone;
	User::LeaveIfError(iStatus.Int());
	
	//processing here

	
	CActiveScheduler::Stop();	
	}
	

TInt CValidateCertChainHelper::RunError(TInt aError)
	{
	iRunError = aError;
	//processing here

	
	CActiveScheduler::Stop();
	return KErrNone;	
	}

