/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/

#ifndef VALIDATETESTSTEP_H
#define VALIDATETESTSTEP_H

#include "pkixcertstepbase.h"
#include <pkixcertchain.h>
#include <securitydefs.h>
#include <x509certchain.h>
#include <e32std.h>


_LIT(KValidateCertChainStep, "ValidateCertChain");

_LIT(KError, "error");
_LIT(KErrorIndex, "errorindex");
_LIT(KNumberOfCerts, "numberofcerts");
_LIT(KCert, "cert");
_LIT(KOldWarnings, "oldwarning");
_LIT(KCritOid, "critoid");
_LIT(KPolicyIndex, "policyindex");

class CValidateCertChainStep;

class CValidateCertChainHelper : public CActive
	{
public:
	CValidateCertChainHelper(CTestExecuteLogger& aLogger);
	~CValidateCertChainHelper();
	
	TInt PerformTestL(CPKIXCertChain* aCertChain, CPKIXValidationResult& aValidResult, TTime aValidateTime);
	void DoCancel();
	void RunL();
	virtual TInt RunError(TInt aError);

	CTestExecuteLogger& Logger(){return iLogger;}

private:
	CTestExecuteLogger& iLogger;
	TInt iRunError;
	CPKIXCertChain* iCertChain;	//not owned
	
	};
	
	
	
class CValidateCertChainStep : public CPkixCertStepBase
	{
public:
	CValidateCertChainStep();
	~CValidateCertChainStep();
	
	virtual TVerdict doTestStepPreambleL();
	
	virtual void PerformTestL();

	void ValidateL(TTime aValidateTime, TDes& aConfigSection);
	
	void TransferToNewWarningsL(RArray<TInt>& aOldWarnings, RArray<TValidationStatus>& aNewWarnings, TInt aCertIndex);
	//takes away from aWarningsToCheckIn
	void CheckWarningsPresentAndRemoveL(const RArray<TValidationStatus>& aWarningsToCheckFor, RArray<TValidationStatus>& aWarningsToCheckIn);
	TBool ValidationStatusEqual(const TValidationStatus& aValidationStatus1, const TValidationStatus& aValidationStatus2);
	//takes away from aOidsToCheckAgainst
	void CheckCriticalExtsPresentAndRemoveL(const RPointerArray<TDesC>& aOidsToLookFor, RArray<TPtrC>& aOidsToCheckAgainst);
	
private:
	CValidateCertChainHelper*	iActiveObject;
	TInt iNumberOfCerts;

		
	};	

#endif // VALIDATETESTSTEP_H
