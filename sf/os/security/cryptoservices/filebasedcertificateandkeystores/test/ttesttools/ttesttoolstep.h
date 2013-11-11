/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* test step declaration
*
*/


/**
 @file
*/

#ifndef __TTESTTOOLSTEP_H__
#define __TTESTTOOLSTEP_H__

#include <test/testexecutestepbase.h>
#include "ttesttoolserver.h"
#include "e32base.h"
#include "ttesttoolengine.h"

_LIT(KTestToolListCertStep, "ListCertStep"); 
_LIT(KTestToolGetTrustAppsStep, "GetTrustAppsStep");
_LIT(KTestToolListKeyStep, "ListKeyStep"); 
_LIT(KTestToolGetPolicyStep, "GetPolicyStep");
_LIT(KTestToolParseFileStep, "ParseFileStep");
_LIT(KTestToolGetTrustStep, "GetTrustStep");
_LIT(KTestToolCheckFileStep, "CheckFile");

_LIT(KExpectedLabel, "expectedlabel");
_LIT(KExpectedUser, "expecteduser");
_LIT(KExpectedLabel1, "expectedlabel1");
_LIT(KExpectedError, "expectederror");
_LIT(KExpectedLabel2, "expectedlabel2");
_LIT(KExpectedNumLabel, "numtimes");
_LIT(KExpectedUserExist, "user");
_LIT(KStore, "store");
_LIT(KExpectedOwner,"owner");
_LIT(KExpectedListStore,"liststore");
_LIT(KActualOutput, "actualoutput");
_LIT(KExpectedTrust, "trust");
_LIT(KFileName, "file");
_LIT(KCheckType, "check");


class CTestToolListCertStep : public CTestStep
	{
public:
	CTestToolListCertStep();
	~CTestToolListCertStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TVerdict iVerdict;
	TPtrC iExpectedLabel1;
	TPtrC iExpectedLabel2;
	TPtrC iExpectedOwner;
	TPtrC iActualOutput;
	TInt iExpectedStore;
	TInt iListStoreExist;
	TInt iExpectedNumLabel;
	TBool iLabel2Exist;
	TBool iLabel1Exist;
	TBool iOwnerExist;
	};


class CTestToolGetTrustAppsStep : public CTestStep
	{
public:
	CTestToolGetTrustAppsStep();
	~CTestToolGetTrustAppsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TVerdict iVerdict;
	RPointerArray<HBufC> iApps;
	TPtrC iExpectedLabel;
	};
	
class CTestToolGetTrustStep : public CTestStep
	{
public:
	CTestToolGetTrustStep();
	~CTestToolGetTrustStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TVerdict iVerdict;
	TPtrC iExpectedLabel;
	TBool iExpectedTrust;
	};	


class CTestToolListKeyStep : public CTestStep
	{
public:
	CTestToolListKeyStep();
	~CTestToolListKeyStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TVerdict iVerdict;
	TPtrC iExpectedLabel1;
	TPtrC iExpectedLabel2;
	TPtrC iActualOutput;
	TInt iExpectedStore;
	TInt iListStoreExist;
	TInt iExpectedNumLabel;
	TBool iLabel2Exist;
	TBool iLabel1Exist;
	};
	

class CTestToolGetPolicyStep : public CTestStep
	{
public:
	CTestToolGetPolicyStep();
	~CTestToolGetPolicyStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TVerdict iVerdict;
	TPtrC iExpectedLabel;
	TPtrC iExpectedUser;
	TBool iLabelExist;
	TInt iExpectedUserExist;
	};

class CTestToolParseFileStep : public CTestStep
	{
public:
	CTestToolParseFileStep();
	~CTestToolParseFileStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	HBufC8* GetErrorFromOutputFileLC(const TDesC8& aBuffer);
private:
	RPointerArray<HBufC> iArgs;
	TPtrC iExpectedError;
	TPtrC iActualOutput;
	};

class CTestToolCheckFileStep : public CTestStep
	{
public:
	CTestToolCheckFileStep();
	~CTestToolCheckFileStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	TPtrC iFileName;
	TPtrC iCheckType;
	};
#endif // __TTESTTOOLSTEP_H__
