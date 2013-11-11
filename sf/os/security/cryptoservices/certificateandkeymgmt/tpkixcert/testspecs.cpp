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


#include "testspecs.h"

//test chain superclass
CTestChain::CTestChain()
	{
	}

CTestChain::~CTestChain()
	{
	delete iServerCerts;
	delete iRootCerts;
	delete iExtraCerts;
	}

void CTestChain::ConstructL()
	{
	iServerCerts = new(ELeave) CDesCArrayFlat (1);
	iRootCerts = new(ELeave) CDesCArrayFlat (1);
	iExtraCerts = new(ELeave) CDesCArrayFlat (1);
	}

//test build class		
CTestChainBuild* CTestChainBuild::NewL()
	{
	CTestChainBuild* self = CTestChainBuild::NewLC();
	CleanupStack::Pop();
	return self;
	}

CTestChainBuild* CTestChainBuild::NewLC()
	{
	CTestChainBuild* self = new(ELeave) CTestChainBuild;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestChainBuild::~CTestChainBuild()
	{
	delete iCorrectChain;
	}

//test chain validate class
CTestChainValidate* CTestChainValidate::NewL()
	{
	CTestChainValidate* self = CTestChainValidate::NewLC();
	CleanupStack::Pop();
	return self;
	}

CTestChainValidate* CTestChainValidate::NewLC()
	{
	CTestChainValidate* self = new(ELeave) CTestChainValidate;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestChainValidate::ConstructL()
	{
	CTestChain::ConstructL();
	iWarnings = new(ELeave)CArrayFixFlat<TValidationStatus> (1);
	iPolicies = new(ELeave) CDesCArrayFlat (1);
	}

CTestChainValidate::~CTestChainValidate()
	{
	delete iError;
	delete iWarnings;
	delete iPolicies;
	};

//test specs
TestSpec::TestSpec()
	{
	}

CTestChainBuild* TestSpec::TestBuildSpec1()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA1);
	ret->iServerCerts->AppendL(KCA2);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec2()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA2);
	ret->iServerCerts->AppendL(KCA1);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec3()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA1);
	ret->iExtraCerts->AppendL(KCA2);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec4()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA2);
	ret->iExtraCerts->AppendL(KCA1);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec5()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA1);
	ret->iServerCerts->AppendL(KCA2);
	ret->iServerCerts->AppendL(KRoot);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec6()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KRoot);
	ret->iServerCerts->AppendL(KCA2);
	ret->iServerCerts->AppendL(KCA1);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec7()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA1);
	ret->iServerCerts->AppendL(KCA2SameDN);
	ret->iServerCerts->AppendL(KCA2);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec8()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA2SameDN);
	ret->iServerCerts->AppendL(KCA1);
	ret->iServerCerts->AppendL(KCA2);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec9()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA1);
	ret->iServerCerts->AppendL(KCA2SameDN);
	ret->iExtraCerts->AppendL(KCA2);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec10()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA1);
	ret->iServerCerts->AppendL(KCA2);
	ret->iExtraCerts->AppendL(KCA2SameDN);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec11()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA2);
	ret->iServerCerts->AppendL(KCA1);
	ret->iExtraCerts->AppendL(KCA1SameDN);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainBuild* TestSpec::TestBuildSpec12()
	{
	CTestChainBuild* ret = CTestChainBuild::NewLC();
	ret->iServerCerts->AppendL(KEE);
	ret->iServerCerts->AppendL(KCA2);
	ret->iServerCerts->AppendL(KCA1SameDN);
	ret->iExtraCerts->AppendL(KCA1);
	ret->iRootCerts->AppendL(KRoot);
	const TPtrC name(KCorrectFile1);
	ret->iCorrectChain = name.AllocL();
	CleanupStack::Pop(); 
	return ret;
	}

//validate tests
CTestChainValidate* TestSpec::TestValidateSpec1()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaCA1EE1);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec2()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaCA1EE2);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec3()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaExpired);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EDateOutOfRange, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec4()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaFuture);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EDateOutOfRange, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec5()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaInvalidSig);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(ESignatureInvalid, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec6()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaKeyCompromise);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec7()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaBadSubjName2);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaTestCA2);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(ENameIsExcluded, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec8()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaBadSubjName1);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaTestCA2);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(ENameNotPermitted, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

//
CTestChainValidate* TestSpec::TestValidateSpec9()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaPathLen);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaTestCA2);
	ret->iServerCerts->AppendL(KFortezzaTestSubCA2);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EPathTooLong, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec10()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaInvalidCAUser1);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaBadCA1);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(ENotCACert, 1);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec11()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaInvalidCAUser2);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iServerCerts->AppendL(KFortezzaBadCA2);
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	TValidationStatus* status = new(ELeave) TValidationStatus(EBadKeyUsage, 1);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec12()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaCA2EE1);
	ret->iServerCerts->AppendL(KFortezzaPCA2);
	ret->iServerCerts->AppendL(KFortezzaCA2);
	ret->iRootCerts->AppendL(KFortezzaPAA2);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec13()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFortezzaCA2BadAltName);
	ret->iServerCerts->AppendL(KFortezzaPCA2);
	ret->iServerCerts->AppendL(KFortezzaCA2);
	ret->iRootCerts->AppendL(KFortezzaPAA2);
	TValidationStatus* status = new(ELeave) TValidationStatus(ENameIsExcluded, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec14()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KDashnet);
	ret->iRootCerts->AppendL(KRSASSCA);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}


CTestChainValidate* TestSpec::TestValidateSpec15()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(K3Do);
	ret->iRootCerts->AppendL(KRSASSCA);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec16()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KDashnet);
	ret->iRootCerts->AppendL(KRSASSCA_OLD);
	TValidationStatus* status = new(ELeave) TValidationStatus(EDateOutOfRange, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec17()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(K3Do);
	ret->iRootCerts->AppendL(KRSASSCA_OLD);
	TValidationStatus* status = new(ELeave) TValidationStatus(EDateOutOfRange, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec18()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KLloyds);
	ret->iServerCerts->AppendL(KVSignServer3);
	ret->iRootCerts->AppendL(KVSignCA3);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec19()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KRoot);
	ret->iRootCerts->AppendL(KRoot);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec20()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KRoot);
	ret->iRootCerts->AppendL(KVSignCA3);
	TValidationStatus* status = new(ELeave) TValidationStatus(EChainHasNoRoot, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

//n.b. this test should leave with KErrArgument
CTestChainValidate* TestSpec::TestValidateSpec21()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	TValidationStatus* status = new(ELeave) TValidationStatus(EChainHasNoRoot, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec22()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KMedCA1EE1);
	ret->iServerCerts->AppendL(KMedCA1);
	ret->iRootCerts->AppendL(KMedRootCA);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec23()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KMedCA1EE2);
	ret->iServerCerts->AppendL(KMedCA1);
	ret->iRootCerts->AppendL(KMedRootCA);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

//FINEID test cases
CTestChainValidate* TestSpec::TestValidateSpec24()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFINEIDLipponenSign);
	ret->iRootCerts->AppendL(KFINEIDCitizenCA);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec25()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFINEIDLipponenEncrypt);
	ret->iRootCerts->AppendL(KFINEIDCitizenCA);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec26()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFINEIDHelsinkiSign);
	ret->iRootCerts->AppendL(KFINEIDTestCA3);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec27()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KFINEIDHelsinkiEncrypt);
	ret->iRootCerts->AppendL(KFINEIDTestCA3);
	TValidationStatus* status = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = status;
	CleanupStack::Pop(); 
	return ret;
	}

//test warning generated when cert not self signed
CTestChainValidate* TestSpec::TestValidateSpec28()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iServerCerts->AppendL(KLloyds);
	ret->iRootCerts->AppendL(KVSignServer3);//not self signed
	TValidationStatus* error = new(ELeave) TValidationStatus(EValidatedOK, 0);
	TValidationStatus warning(ERootCertNotSelfSigned, 0);
	ret->iError = error;
	ret->iWarnings->AppendL(warning);
	CleanupStack::Pop(); 
	return ret;
	}

//test case with OpenSSL default certs
CTestChainValidate* TestSpec::TestValidateSpec29()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KApacheRSARoot);
	ret->iServerCerts->AppendL(KApacheRSAServer);
	TValidationStatus* error = new(ELeave) TValidationStatus(EDateOutOfRange, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}

CTestChainValidate* TestSpec::TestValidateSpec30()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KApacheDSARoot);
	ret->iServerCerts->AppendL(KApacheDSAServer);
	TValidationStatus* error = new(ELeave) TValidationStatus(EDateOutOfRange, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}

//normal chain, supply policy in chain, should work
CTestChainValidate* TestSpec::TestValidateSpec31()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	ret->iServerCerts->AppendL(KFortezzaCA1EE1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iPolicies->AppendL(KX22Policy);
	TValidationStatus* error = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}

//normal chain, supply policy not in chain, should fail
CTestChainValidate* TestSpec::TestValidateSpec32()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	ret->iServerCerts->AppendL(KFortezzaCA1EE1);
	ret->iServerCerts->AppendL(KFortezzaCA1);
	ret->iServerCerts->AppendL(KFortezzaPCA1);
	ret->iPolicies->AppendL(KExtraBogusPolicy);
	TValidationStatus* error = new(ELeave) TValidationStatus(ERequiredPolicyNotFound, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}

//chain with policy mapping, supply first policy, should work
CTestChainValidate* TestSpec::TestValidateSpec33()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	ret->iServerCerts->AppendL(KFortezzaCA2EE1);
	ret->iServerCerts->AppendL(KFortezzaCA2);
	ret->iServerCerts->AppendL(KFortezzaPCA2);
	ret->iServerCerts->AppendL(KFortezzaX22RNCrossCert);
	ret->iPolicies->AppendL(KX22Policy);
	TValidationStatus* error = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}

//chain with policy mapping, supply second policy, should work
CTestChainValidate* TestSpec::TestValidateSpec34()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	ret->iServerCerts->AppendL(KFortezzaCA2EE1);
	ret->iServerCerts->AppendL(KFortezzaCA2);
	ret->iServerCerts->AppendL(KFortezzaPCA2);
	ret->iServerCerts->AppendL(KFortezzaX22RNCrossCert);
	ret->iPolicies->AppendL(KRNPolicy);
	TValidationStatus* error = new(ELeave) TValidationStatus(EValidatedOK, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}

//chain with policy mappng, supply different policy, should fail
CTestChainValidate* TestSpec::TestValidateSpec35()
	{
	CTestChainValidate* ret = CTestChainValidate::NewLC();
	ret->iRootCerts->AppendL(KFortezzaPAA1);
	ret->iServerCerts->AppendL(KFortezzaCA2EE1);
	ret->iServerCerts->AppendL(KFortezzaCA2);
	ret->iServerCerts->AppendL(KFortezzaPCA2);
	ret->iServerCerts->AppendL(KFortezzaX22RNCrossCert);
	ret->iPolicies->AppendL(KExtraBogusPolicy);
	TValidationStatus* error = new(ELeave) TValidationStatus(ERequiredPolicyNotFound, 0);
	ret->iError = error;
	CleanupStack::Pop(); 
	return ret;
	}
