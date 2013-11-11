// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
#ifndef __ConvertStep_H__
#define __ConvertStep_H__

#include <e32std.h>
#include <test/testexecutestepbase.h>
#include "shrepos.h"
#include "CentRepConvTool.h"
#include "setting.h"

_LIT(KConvertToBinStep, "ConvertToBinaryStep");

enum TPolicyToConstruct
	{
	EGlobalDefault,
	ERangePolicyRead,
	ERangePolicyWrite,
	ESinglePolicyRead,
	ESinglePolicyWrite
	};

class CConvertStepBase : public CTestStep
	{
public:
	~CConvertStepBase();
	TVerdict doTestStepPreambleL();
	void InitialiseServerResourcesL(TInt& aNumPushed);

protected:
	CSharedRepository* LoadTextIniFileLC(const TDesC& aTextFile);
	CSharedRepository* LoadCreFileLC(const TDesC& aCreFile);

	void ExtractRdWrPolicyStrings(TPtrC& aDefRdPolicy,
							  	  TPtrC& aDefWrPolicy,
								  TLex& aLex);
	TVerdict VerifyTextFileL(const TDesC& aTextFile);
	TVerdict VerifyCreFileL(const TDesC& aCreFile);
	TVerdict CheckRepositoryL(CSharedRepository& aSharedRep);
	TVerdict VerifyOwnerL(CSharedRepository& aSharedRep);
	TVerdict VerifyDefMetaL(TConvToolTester& aShRepGetter);
	TVerdict VerifyDefPoliciesL(TConvToolTester& aShRepGetter);
	TVerdict VerifyRangePoliciesL(TConvToolTester& aShRepGetter);
	TVerdict VerifySinglePoliciesL(TConvToolTester& aShRepGetter);
	TVerdict VerifySettingsL(TConvToolTester& aShRepGetter);

	void CompareRangeMeta(const TSettingsDefaultMeta& aRangeMeta,
					  	  TUint32 aLowKey, TUint32 aHighKey, 
					  	  TUint32 aKeyMask, TUint32 aMeta);

	HBufC8* GetFallBackDefaultPolicyLC(TConvToolTester& aShRepGetter,
									   TPolicyToConstruct aMode,
									   TUint32 aKey);
	HBufC8* ConstructTSecurityBufLC(TConvToolTester& aShRepGetter,
									const TDesC& aHexString,
									TPolicyToConstruct aMode,
									TUint32 aKey = 0);
	void CompareRangePolicy(const TSettingsAccessPolicy& aActualPolicy,
							const TDesC8& aExpectedRd,
							const TDesC8& aExpectedWr,
							TUint32 aExpectedLowKey,
							TUint32 aExpectedHighKey,
							TUint32 aExpectedKeyMask);
	TInt ExtractSettingL(TLex& aLex,
						 TServerSetting& aSetting,
						 TBool& aMetaFound);

	void ShowSecuPolicies(const TDesC8& aActualRd,
						  const TDesC8& aExpectedRd,
						  const TDesC8& aActualWr,
						  const TDesC8& aExpectedWr);
protected:
	TPtrC iCmdLine;
	TPtrC iOutFileName;
	TPtrC iInFileName;
	TUid  iRepUid;
	};

//================================================
class CConvertToBinaryStep : public CConvertStepBase
	{
public:
	CConvertToBinaryStep();
	virtual TVerdict doTestStepL(void);
	~CConvertToBinaryStep(){}
	};

//================================================
_LIT(KConvertToTextStep, "ConvertToTextStep");

class CConvertToTextStep : public CConvertStepBase
	{
public:
	CConvertToTextStep();
	virtual TVerdict doTestStepL(void);
	~CConvertToTextStep() {}
	};

//================================================
_LIT(KRoundTripStep, "RoundTripStep");

class CRoundTripStep : public CTestStep
	{
public:
	CRoundTripStep();
	~CRoundTripStep() {}
	virtual TVerdict doTestStepL(void);
	void InitialiseServerResourcesL(TInt& aNumPushed);
	};
	
//================================================
_LIT(KGenerateCreStep, "GeneratePersistedCreStep");

class CGenerateCreStep : public CTestStep
	{
public:
	CGenerateCreStep();
	~CGenerateCreStep() {}
	virtual TVerdict doTestStepL(void);
//	void InitialiseServerResourcesL(TInt& aNumPushed);
	};

#endif
