// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include "ConvertStep.h"
#include "srvres.h"
#include "AccessShrepos.h"
#include "CentRepConvTool.h"
#include "CompareTwoFiles.h"

_LIT(KConversionCmd, "ConversionCmd");
_LIT(KConvertToTextCmd, "ConvertToTextCmd");
_LIT(KRepUid, "RepUid");

_LIT_SECURITY_POLICY_FAIL(ALWAYSFAILPOLICY);

_LIT(KMissingKey, "Missing %S in config file");
_LIT(KCorruptEntry, "Fail to parse %S = %S");

_LIT(KOwner, "owner");
_LIT(KDefaultMeta, "DefaultMeta");
_LIT(KNumRangeMeta, "NumRangeMeta");
_LIT(KRangeMeta_, "RangeMeta_");
_LIT(KDefaultPolicy, "DefaultPolicy");
_LIT(KNumRangePolicies, "NumRangePolicies");
_LIT(KRangePolicy_, "RangePolicy_");
_LIT(KNumSinglePolicies, "NumSinglePolicies");
_LIT(KSinglePolicy_, "SinglePolicy_");
_LIT(KNumSettings, "NumSettings");
_LIT(KSetting_, "Setting_");
_LIT(KNullString, "\"\"");
_LIT(KTypeInt, "int");
_LIT(KTypeReal, "real");
_LIT(KTypeBinary, "binary");
_LIT(KRepositoryUid, "RepositoryUid") ;

//=========================================================
// helper functions
//=========================================================
static TInt ReadNumber(TLex& aLex, TUint32& aVal)
	{
	aLex.SkipSpace();
	if (aLex.Eos())
		{
		return KErrNotFound;
		}

	// Handle negative number case
	if (aLex.Peek() == '-')
		{
		TInt i;
		if (aLex.Val(i) != KErrNone)
			{
			return KErrCorrupt;
			}
		aVal = i;
		return KErrNone;
		}

	TRadix radix = EDecimal;
	if(aLex.Peek()=='0')
		{
		aLex.Inc();
		if(aLex.Peek().GetLowerCase()=='x')
			{
			aLex.Inc();
			radix = EHex;
			}
		else
			{
			aLex.UnGet();
			}
		}

	return aLex.Val(aVal, radix);
	}

static TInt ReadRange(TLex& aLex, TUint32& aLowKey,
					  TUint32& aHighKey, TUint32& aMask,
					  TUint32& aMeta, TBool aIncludeMeta)
{
	if (::ReadNumber(aLex, aLowKey) != KErrNone ||
		::ReadNumber(aLex, aHighKey) != KErrNone ||
		::ReadNumber(aLex, aMask) != KErrNone)
		{
		return KErrCorrupt;
		}

	if (aIncludeMeta &&
		::ReadNumber(aLex, aMeta) != KErrNone)
		{
		return KErrCorrupt;
		}

	return KErrNone;
}


HBufC8* HexToBinLC(const TDesC& aHexString)
	{
	TInt len = aHexString.Length();
	if (len == 0)
		{
		return NULL;
		}

	if (len % 2)
		{
		User::Leave(KErrCorrupt);
		}
		
	len /= 2;
	HBufC8* s = HBufC8::NewLC(len);

	TPtr8 ptr8 = s->Des();
	ptr8.SetLength(len);

	TInt i, j;
	for(i = 0, j = 0; i < len; i++, j += 2)
		{
		TPtrC oneByte = aHexString.Mid(j, 2); 
		TLex lex(oneByte);
		User::LeaveIfError(lex.Val(ptr8[i], EHex));
		}

	return s;
	}

//====================================================
// class CGenerateCreStep
//====================================================
CGenerateCreStep::CGenerateCreStep()
	{
	SetTestStepName(KGenerateCreStep);
	}


//===========================================================
TVerdict CGenerateCreStep::doTestStepL()
{
	TInt repositoryUidValue ;
	
	if (!GetHexFromConfig(ConfigSection(), KRepositoryUid, repositoryUidValue))
		{
		ERR_PRINTF2(KMissingKey, KRepositoryUid);
		return EFail;
		}
				
	CRepository* repository;
	repository = CRepository::NewLC(TUid::Uid(repositoryUidValue));
	
	TUint32 errorKey ;	
	TInt result = repository->Delete(0, 0, errorKey) ;
		if (result != KErrNone)
		{
		SetTestStepResult(EFail);		
		}
		
	result = repository->Reset() ;
	if (result != KErrNone)
		{
		SetTestStepResult(EFail);		
		}
	
	CleanupStack::PopAndDestroy(repository) ;
	return TestStepResult();
 }

//====================================================
// class CRoundTripStep
//====================================================
CRoundTripStep::CRoundTripStep()
	{
	SetTestStepName(KRoundTripStep);
	}

void CRoundTripStep::InitialiseServerResourcesL(TInt& aNumPushed)
	{
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	aNumPushed++;
	CActiveScheduler::Install(sched); // but no Start()

	TServerResources::InitialiseL();
	CleanupStack::PushL(TCleanupItem(TCleanupOperation(&TServerResources::Close), NULL));
	aNumPushed++;
	}

//===========================================================
TVerdict CRoundTripStep::doTestStepL()
{
	_LIT(KFirstPair, "FirstPair");
	_LIT(KSecondPair, "SecondPair");

	_LIT(KRoundTripStepStart, "CRoundTripStep::doTestStepL begin\r\n");
	Logger().WriteFormat(KRoundTripStepStart);

	TInt nPushed = 0;
	InitialiseServerResourcesL(nPushed);

	TPtrC data, file1, file2;
	for (TInt i = 0; i < 2; i++)
		{
		const TDesC& iniKey = (i == 0) ? KFirstPair() : KSecondPair();
		if (!GetStringFromConfig(ConfigSection(), iniKey, data))
			{
			ERR_PRINTF2(KMissingKey, &iniKey);
			return EFail;
			}

		TLex lex(data);
		file1.Set(lex.NextToken());
		file2.Set(lex.NextToken());
		_LIT(KShowTheFiles, "Comparing %S with %S");
		INFO_PRINTF3(KShowTheFiles, &file1, &file2);

		if (!FilesAreIdenticalL(TServerResources::iFs, file1, file2))
			{
			SetTestStepResult(EFail);
			}
		} // for

	_LIT(KRoundTripStepEnd, "CRoundTripStep::doTestStepL complete\r\n");
	Logger().WriteFormat(KRoundTripStepEnd);

	CleanupStack::PopAndDestroy(nPushed); // CActiveScheduler and
	                                      // TServerResources
	return TestStepResult();
 }

//====================================================
// class CConvertToBinaryStep
//====================================================
CConvertToBinaryStep::CConvertToBinaryStep()
	{
	SetTestStepName(KConvertToBinStep);
	}

TVerdict CConvertToBinaryStep::doTestStepL()
	{
	_LIT(KBinConvStepStart, "CConvertToBinaryStep::doTestStepL start\r\n");
	Logger().WriteFormat(KBinConvStepStart);

	TInt nPushed = 0;
	InitialiseServerResourcesL(nPushed);

	// 
	// Load the original text initialization file. Compare the
	// loaded values against the expected values in the TEF ini file.
	// This confirms the expected values in the test INI file make
	// sense before we use them on the converted output.
	if (EPass != VerifyTextFileL(iInFileName))
		{
		_LIT(KBadExpectedValues, "Expected values in TEF INI file fail validation");
		Logger().WriteFormat(KBadExpectedValues);
		}

	// Do the conversion
	CCentRepConvTool* tool = CCentRepConvTool::NewL(iCmdLine,
		TServerResources::iFs, EFalse);
	CleanupStack::PushL(tool);
	nPushed++;
	tool->ProcessCmdL();

	VerifyCreFileL(iOutFileName);

	// CActiveScheduler, TServerResources & tool
	CleanupStack::PopAndDestroy(nPushed);

	_LIT(KBinConvStepEnd, "CConvertToBinaryStep::doTestStepL complete\r\n\r\n");
	Logger().WriteFormat(KBinConvStepEnd);
	return TestStepResult();
	}

//====================================================
// class CConvertToTextStep
//====================================================
CConvertToTextStep::CConvertToTextStep()
	{
	SetTestStepName(KConvertToTextStep);
	}

TVerdict CConvertToTextStep::doTestStepL()
	{
	_LIT(KTextConvStepStart, "CConvertToTextStep::doTestStepL start\r\n");
	Logger().WriteFormat(KTextConvStepStart);

	TInt nPushed = 0;
	InitialiseServerResourcesL(nPushed);

	// This teststep use the .CRE output generated by 
	// CConvertToBinaryStep previously.

	// Do the conversion
	CCentRepConvTool* tool = CCentRepConvTool::NewL(iCmdLine,
		TServerResources::iFs, EFalse);
	CleanupStack::PushL(tool);
	nPushed++;

	tool->ProcessCmdL();

	VerifyTextFileL(iOutFileName);

	// CActiveScheduler, TServerResources & tool
	CleanupStack::PopAndDestroy(nPushed);

	_LIT(KTextConvStepEnd, "CConvertToTextStep::doTestStepL complete\r\n\r\n");
	Logger().WriteFormat(KTextConvStepEnd);
	return TestStepResult();
	}

//====================================================
// class CConvertStepBase
//====================================================
CConvertStepBase::~CConvertStepBase()
	{
	}

TVerdict CConvertStepBase::doTestStepPreambleL()
	{
	TPtrC data;
	TPtrC cmdLineKey;
	if (TestStepName() == KConvertToBinStep)
		{
		cmdLineKey.Set(KConversionCmd());
		}
	else
		{
		cmdLineKey.Set(KConvertToTextCmd());
		}

	if (!GetStringFromConfig(ConfigSection(), cmdLineKey, data))
		{
		ERR_PRINTF2(KMissingKey, &cmdLineKey);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	iCmdLine.Set(data);

	TLex lex(data);

	TBool pass = ETrue;
	// Skip "-o"
	_LIT(KMinusOh, "-o");
	if (lex.NextToken().Compare(KMinusOh) != 0)
		{
		pass = EFalse;
		}
	else
		{
		iOutFileName.Set(lex.NextToken());
		iInFileName.Set(lex.NextToken());
		if (iInFileName.Length() == 0)
			{
			pass = EFalse;
			}
		}

	if (!pass)
		{
		ERR_PRINTF3(KCorruptEntry, &cmdLineKey, &data);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (!GetStringFromConfig(ConfigSection(), KRepUid, data))
		{
		pass = EFalse;
		}
	else
		{
		TLex lex2(data);
		TUint32 i;
		if (KErrNone != ::ReadNumber(lex2, i))
			{
			pass = EFalse;
			}
		else
			{
			iRepUid.iUid = i;
			}	
		}

	if (! pass)
		{
		ERR_PRINTF3(KCorruptEntry, &KRepUid, &data);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	return EPass;
	}

void CConvertStepBase::InitialiseServerResourcesL(TInt& aNumPushed)
	{
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	aNumPushed++;
	CActiveScheduler::Install(sched); // but no Start()

	TServerResources::InitialiseL();
	CleanupStack::PushL(TCleanupItem(TCleanupOperation(&TServerResources::Close), NULL));
	aNumPushed++;
	}

TVerdict CConvertStepBase::VerifyTextFileL(const TDesC& aTextFile)
	{
	CSharedRepository* rep = LoadTextIniFileLC(aTextFile);
	CheckRepositoryL(*rep);
	CleanupStack::PopAndDestroy();  // rep

	return TestStepResult();
	}

CSharedRepository*
CConvertStepBase::LoadTextIniFileLC(const TDesC& aTextFile)
	{
	CSharedRepository* rep = CSharedRepository::NewL(iRepUid);
	CleanupStack::PushL(rep);

	HBufC* tempIniFileName = aTextFile.AllocLC();
	CIniFileIn* iniFile = NULL;
	User::LeaveIfError(CIniFileIn::NewLC(TServerResources::iFs,iniFile, *tempIniFileName));
	rep->ReloadContentL(*iniFile);
	CleanupStack::PopAndDestroy(); // iniFile
	CleanupStack::PopAndDestroy(); // tempIniFileName

	return rep;
	}

TVerdict CConvertStepBase::VerifyCreFileL(const TDesC& aCreFile)
	{
	CSharedRepository* rep = LoadCreFileLC(aCreFile);
	CheckRepositoryL(*rep);
	CleanupStack::PopAndDestroy();  // rep

	return TestStepResult();
	}

CSharedRepository*
CConvertStepBase::LoadCreFileLC(const TDesC& aCreFile)
	{
	CSharedRepository* rep = CSharedRepository::NewL(iRepUid);
	CleanupStack::PushL(rep);

	CDirectFileStore* store = CDirectFileStore::OpenLC(
		TServerResources::iFs, aCreFile, EFileRead|EFileShareReadersOnly);

	// Get the root stream and attempt to read the index from it
	TStreamId rootStreamId = store->Root() ;
	RStoreReadStream rootStream ;
	rootStream.OpenLC(*store, rootStreamId);
	// Internalize the repository
	rep->InternalizeCreL(rootStream);
	CleanupStack::PopAndDestroy(&rootStream);
	CleanupStack::PopAndDestroy(store);

	return rep;
	}


TVerdict CConvertStepBase::CheckRepositoryL(CSharedRepository& aSharedRep)
	{
	TConvToolTester shRepGetter(aSharedRep);
    VerifyOwnerL(aSharedRep);
    VerifyDefMetaL(shRepGetter);
    VerifyDefPoliciesL(shRepGetter);
    VerifyRangePoliciesL(shRepGetter);
	VerifySinglePoliciesL(shRepGetter);
	VerifySettingsL(shRepGetter);
	return TestStepResult();
	}

TVerdict CConvertStepBase::VerifyOwnerL(CSharedRepository& aSharedRep)
	{
	TUid owner = KNullUid;
	TPtrC data;
	if (GetStringFromConfig(ConfigSection(), KOwner, data))
		{
		TLex lex(data);
		TUint32 i;
		if (KErrNone != ::ReadNumber(lex, i))
			{
			ERR_PRINTF3(KCorruptEntry, &KOwner, &data);
			SetTestStepResult(EFail);
			User::Leave(KErrCorrupt);
			}
		owner.iUid = i;
		}

	if (owner != aSharedRep.Owner())
		{
		_LIT(KOwnerMism, "Owner mismatch, expect 0x%x, actual 0x%x");
		ERR_PRINTF3(KOwnerMism, owner.iUid, aSharedRep.Owner().iUid);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

TVerdict CConvertStepBase::VerifyDefPoliciesL(TConvToolTester& aShRepGetter)
	{
	TInt numPushed = 0;

	// verify default read/write policies
	const TSecurityPolicy& defRdPolicy = aShRepGetter.DefaultReadPolicy();
	const TSecurityPolicy& defWrPolicy = aShRepGetter.DefaultWritePolicy();

	TPtrC hexRdPolicy, hexWrPolicy;
	TPtrC data(KNullDesC);

	if (GetStringFromConfig(ConfigSection(), KDefaultPolicy, data))
		{
		TLex lex(data);
		ExtractRdWrPolicyStrings(hexRdPolicy, hexWrPolicy, lex);
		}

	HBufC8* expectedDefRd = ConstructTSecurityBufLC(aShRepGetter,
								hexRdPolicy, EGlobalDefault);
	numPushed++;
	HBufC8* expectedDefWr = ConstructTSecurityBufLC(aShRepGetter,
								hexWrPolicy, EGlobalDefault);
	numPushed++;

	_LIT(KDefPolicyMism, "Default policy mismatch, expected val %S"); 
	if (expectedDefWr->Compare(defWrPolicy.Package()) != 0 ||
		expectedDefRd->Compare(defRdPolicy.Package()) != 0)
		{
		Logger().WriteFormat(KDefPolicyMism, &hexWrPolicy);
		ShowSecuPolicies(defRdPolicy.Package(),
						 *expectedDefRd,
						 defWrPolicy.Package(),
						 *expectedDefWr);
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(numPushed);
	return TestStepResult();
	}

TVerdict CConvertStepBase::VerifyDefMetaL(TConvToolTester& aShRepGetter)
	{
	const TInt KMaxVarNameLen = 32;
	TBuf<KMaxVarNameLen> varName;
	TInt count, i;
	TUint32 lowKey, highKey, keyMask, meta;
	TPtrC data;

	// Verify DefaultMeta
	meta = 0;
	if (GetStringFromConfig(ConfigSection(), KDefaultMeta, data))
		{
		TLex lex(data);
		::ReadNumber(lex, meta);
		}

	TUint32 defaultMeta = aShRepGetter.DefaultMeta();
	if (defaultMeta != meta)
		{
		SetTestStepResult(EFail);
		_LIT(KMetaMismatch, "Expected meta %X, actual %x");
		ERR_PRINTF3(KMetaMismatch, meta, defaultMeta);
		}

	// Verify RangeMeta array size
	const RDefaultMetaArray& rangeMetaArray = aShRepGetter.DefaultMetaArray();
	count = 0;
	if (!GetIntFromConfig(ConfigSection(), KNumRangeMeta, count))
		{
		ERR_PRINTF2(KMissingKey, &KNumRangeMeta);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (rangeMetaArray.Count() !=  count)
		{
		_LIT(KRangeMetaArrayMism, "Expected RangeMeta array sz %d, actual %d");
		ERR_PRINTF3(KRangeMetaArrayMism, count, rangeMetaArray.Count());
		SetTestStepResult(EFail);
		}
	else
		{
		for (i = 0; i < count; i++)
			{
			varName.Copy(KRangeMeta_);
			varName.AppendNum(i);
			if (!GetStringFromConfig(ConfigSection(), varName, data))
				{
			 	ERR_PRINTF2(KMissingKey, &varName);
			 	SetTestStepResult(EFail);
			 	User::Leave(KErrCorrupt);
			 	}

			TLex lex(data);
			if (KErrNone != ::ReadRange(lex,lowKey,highKey,keyMask,meta,ETrue))
				{
				ERR_PRINTF3(KCorruptEntry, &varName,&data);
				SetTestStepResult(EFail);
				User::Leave(KErrCorrupt);
				}

			const TSettingsDefaultMeta& settingsMeta = rangeMetaArray[i];
			CompareRangeMeta(settingsMeta, lowKey, highKey, keyMask, meta);
			} // for
		} // if rangeMetaArray.Size == count

	return TestStepResult();
	}

void CConvertStepBase::CompareRangeMeta(const TSettingsDefaultMeta& aRangeMeta,
	TUint32 aLowKey, TUint32 aHighKey, TUint32 aKeyMask, TUint32 aMeta)
	{
	TUint32 actualLow = aRangeMeta.LowKey();
	TUint32 actualHigh = aRangeMeta.HighKey();
	TUint32 actualMask = aRangeMeta.KeyMask();
	TUint32 actualMeta = aRangeMeta.GetDefaultMetadata();

	if (aLowKey == actualLow &&
		aHighKey == actualHigh &&
		aKeyMask == actualMask &&
		aMeta == actualMeta)
		{
		return;
		}

	_LIT(KRangeMetaMism, "RangeMeta mismatch (expected, actual):");
	ERR_PRINTF1(KRangeMetaMism);

	_LIT(KRangeFormat, "Low=0x%X Hi=0x%X mask=0x%X value 0x%X");
	Logger().WriteFormat(KRangeFormat, aLowKey, aHighKey, aKeyMask,
		aMeta);
	Logger().WriteFormat(KRangeFormat, actualLow, actualHigh,
		actualMask, actualMeta);
	SetTestStepResult(EFail);
	}

void CConvertStepBase::ExtractRdWrPolicyStrings(TPtrC& aDefRdPolicy,
													TPtrC& aDefWrPolicy,
													TLex&  aLex)
{
	aDefRdPolicy.Set(KNullString);
	aDefWrPolicy.Set(KNullString);

	TPtrC p = aLex.NextToken();
	if (p.Length() > 0)
		{
		aDefRdPolicy.Set(p);
		}

	p.Set(aLex.NextToken());
	if (p.Length() > 0)
		{
		aDefWrPolicy.Set(p);
		}
}

//
// This method search for fallback defaults for Single, Range,
// and Global Read/Write TSecurityPolicy.
// If a single access policy is missing a read or write half,
// fill in the blank with a range default or global. If a range
// default is missing a read or write half, fill in the blank
// with global. If global def is missing read/write part, the
// fallback is AlwaysFail.
//
// Parameter aKey is needed if the security policy to construct
// is a single access policy.
HBufC8* CConvertStepBase::GetFallBackDefaultPolicyLC(
			TConvToolTester& aShRepGetter,
			TPolicyToConstruct aMode,
			TUint32 aKey)
	{
	const TSecurityPolicy* defFound = NULL;

	if (aMode == ESinglePolicyRead || aMode == ESinglePolicyWrite)
		{
		TSettingsAccessPolicy* rangePolicy = 
			aShRepGetter.RangePolicies().Find(aKey);
		if (rangePolicy)
			{
			defFound = (aMode == ESinglePolicyRead) ?
						rangePolicy->GetReadAccessPolicy() :
						rangePolicy->GetWriteAccessPolicy();
			}
		}

	if (defFound == NULL)
		{
		if (aMode == ERangePolicyRead || aMode == ESinglePolicyRead)
			{
			defFound = &(aShRepGetter.DefaultReadPolicy());
			}
		else if (aMode == ERangePolicyWrite || aMode == ESinglePolicyWrite)
			{
			defFound = &(aShRepGetter.DefaultWritePolicy());
			}
		}

	// The fall back for global default is AlwaysFail.
	if (aMode == EGlobalDefault)
		{
		defFound = &ALWAYSFAILPOLICY;
		}
	TPtrC8 des8 = defFound->Package();
	return des8.AllocLC();
	}

// 
HBufC8* CConvertStepBase::ConstructTSecurityBufLC(
			TConvToolTester& aShRepGetter,
			const TDesC& aHexString,
			TPolicyToConstruct aMode,
			TUint32 aKey)
	{
	HBufC8* p = NULL;

	if (aHexString.Length() == 16)
		{
		p = HexToBinLC(aHexString);
		}
	else if (aHexString.Length() == 0 || aHexString == KNullString)
		{
		p = GetFallBackDefaultPolicyLC(aShRepGetter, aMode, aKey);
		}
	else
		{
		_LIT(KBadSecurityStr, "Cannot convert %S to security policy");
		ERR_PRINTF2(KBadSecurityStr, &aHexString);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	return p;
	}

//
TVerdict CConvertStepBase::VerifyRangePoliciesL(TConvToolTester& aShRepGetter)
	{
	const TInt KMaxVarNameLen = 32;
	TBuf<KMaxVarNameLen> varName;
	TInt count, i;
	TUint32 lowKey, highKey, keyMask, dummy;
	TPtrC data;

	TInt numPushed = 0;

	// Verify RangePolicies array size
	const RRangePolicyArray& rangePolicies = aShRepGetter.RangePolicies();
	count = 0;
	if (!GetIntFromConfig(ConfigSection(), KNumRangePolicies, count))
		{
		ERR_PRINTF2(KMissingKey, &KNumRangePolicies);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (rangePolicies.Count() !=  count)
		{
		_LIT(KRangePolicyArrayMism, "Expected RangePolicy array sz %d, actual %d");
		ERR_PRINTF3(KRangePolicyArrayMism, count, rangePolicies.Count());
		SetTestStepResult(EFail);
		}
	else
		{
		for (i = 0; i < count; i++)
			{
			varName.Copy(KRangePolicy_);
			varName.AppendNum(i);
			if (!GetStringFromConfig(ConfigSection(), varName, data))
				{
			 	ERR_PRINTF2(KMissingKey, &varName);
			 	SetTestStepResult(EFail);
			 	User::Leave(KErrCorrupt);
			 	}

			TLex lex(data);
			if (KErrNone != ::ReadRange(lex,lowKey,highKey,keyMask,dummy,EFalse))
				{
				ERR_PRINTF3(KCorruptEntry, &varName, &data);
				SetTestStepResult(EFail);
				User::Leave(KErrCorrupt);
				}

			TPtrC hexRdPolicy, hexWrPolicy;
			ExtractRdWrPolicyStrings(hexRdPolicy, hexWrPolicy, lex);
			HBufC8* expectedRd = ConstructTSecurityBufLC(aShRepGetter,
								hexRdPolicy, ERangePolicyRead);
			numPushed++;
			HBufC8* expectedWr = ConstructTSecurityBufLC(aShRepGetter,
								hexWrPolicy, ERangePolicyWrite);
			numPushed++;			

			// Now compare
			CompareRangePolicy(rangePolicies[i], *expectedRd, *expectedWr,
				lowKey, highKey, keyMask);
			CleanupStack::PopAndDestroy(numPushed);
			numPushed = 0;	
			} // for
		} // if rangePolicies.Count == count

	return TestStepResult();
	}

TVerdict CConvertStepBase::VerifySinglePoliciesL(TConvToolTester& aShRepGetter)
	{
	const TInt KMaxVarNameLen = 32;
	TBuf<KMaxVarNameLen> varName;
	TInt count, i;
	TUint32 lowKey, highKey, keyMask, dummy;
	TPtrC data;

	TInt numPushed = 0;

	// Verify RangePolicies array size
	const RPointerArray<TSettingsAccessPolicy>& singlePolicies =
		aShRepGetter.SinglePolicies();
	count = 0;
	if (!GetIntFromConfig(ConfigSection(), KNumSinglePolicies, count))
		{
		ERR_PRINTF2(KMissingKey, &KNumSinglePolicies);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (singlePolicies.Count() !=  count)
		{
		_LIT(KsinglePolicyArrayMism, "Expected singlePolicy array sz %d, actual %d");
		ERR_PRINTF3(KsinglePolicyArrayMism, count, singlePolicies.Count());
		SetTestStepResult(EFail);
		}
	else
		{
		for (i = 0; i < count; i++)
			{
			varName.Copy(KSinglePolicy_);
			varName.AppendNum(i);
			if (!GetStringFromConfig(ConfigSection(), varName, data))
				{
			 	ERR_PRINTF2(KMissingKey, &varName);
			 	SetTestStepResult(EFail);
			 	User::Leave(KErrCorrupt);
			 	}

			TLex lex(data);
			if (KErrNone != ::ReadRange(lex,lowKey,highKey,keyMask,dummy,EFalse))
				{
				ERR_PRINTF3(KCorruptEntry, &varName, &data);
				SetTestStepResult(EFail);
				User::Leave(KErrCorrupt);
				}

			TPtrC hexRdPolicy, hexWrPolicy;
			ExtractRdWrPolicyStrings(hexRdPolicy, hexWrPolicy, lex);
			HBufC8* expectedRd = ConstructTSecurityBufLC(aShRepGetter,
								hexRdPolicy, ESinglePolicyRead);
			numPushed++;
			HBufC8* expectedWr = ConstructTSecurityBufLC(aShRepGetter,
								hexWrPolicy, ESinglePolicyWrite);
			numPushed++;			

			// Now compare. NB: CompareRangePolicy works fine on
			// SinglePolicies too.
			CompareRangePolicy(*singlePolicies[i], *expectedRd, *expectedWr,
				lowKey, highKey, keyMask);
			CleanupStack::PopAndDestroy(numPushed);
			numPushed = 0;	
			} // for
		} // if

	return TestStepResult();
	}

void CConvertStepBase::CompareRangePolicy(
							const TSettingsAccessPolicy& aActualPolicy,
							const TDesC8& aExpectedRd,
							const TDesC8& aExpectedWr,
							TUint32 aExpectedLowKey,
							TUint32 aExpectedHighKey,
							TUint32 aExpectedKeyMask)
	{
	TBool match = ETrue;
	if (aExpectedLowKey != aActualPolicy.LowKey() ||
		aExpectedHighKey != aActualPolicy.HighKey() ||
		aExpectedKeyMask != aActualPolicy.KeyMask())
		{
		match = EFalse;
		}

	const TSecurityPolicy* actualRd = aActualPolicy.GetReadAccessPolicy();
	if (aExpectedRd.Compare(actualRd->Package()) != 0)
		{
		match = EFalse;
		}

	const TSecurityPolicy* actualWr = aActualPolicy.GetWriteAccessPolicy();
	if (aExpectedWr.Compare(actualWr->Package()) != 0)
		{
		match = EFalse;
		}
	if (! match)
		{
		SetTestStepResult(EFail);

    	_LIT(KRangePolicyMism, "AccessPolicy mismatch (expected, actual):");
		_LIT(KRangePolicyFormat, "Low=0x%X Hi=0x%X mask=0x%X");
		ERR_PRINTF1(KRangePolicyMism);
		Logger().WriteFormat(KRangePolicyFormat, aExpectedLowKey,
			aExpectedHighKey, aExpectedKeyMask);
		Logger().WriteFormat(KRangePolicyFormat, aActualPolicy.LowKey(),
			aActualPolicy.HighKey(), aActualPolicy.KeyMask());
		ShowSecuPolicies(actualRd->Package(),
						 aExpectedRd,
						 actualWr->Package(),
						 aExpectedWr);
		}
	}


TInt CConvertStepBase::ExtractSettingL(TLex& aLex,
										   TServerSetting& aSetting,
										   TBool& aMetaFound)
	{
	aSetting.ResetValue();
	aSetting.SetMeta(0);
	aMetaFound = EFalse;
	
	TUint32 intval;
	TInt ret = ::ReadNumber(aLex, intval);
	if (KErrNone != ret)
		{
		return ret;
		}
	aSetting.SetKey(intval);

	// Next do the type and value
	TPtrC type = aLex.NextToken();

	if (aLex.Eos()) // make sure we have value
		{
		return KErrCorrupt;
		}


	if (type == KTypeInt)
		{
		ret = ::ReadNumber(aLex, intval);
		aSetting.SetIntValue(intval);
		}
	else if (type == KTypeReal)
		{
		TReal r;
		aLex.SkipSpace(); // normally done by ::ReadNumber and NextToken
		ret = aLex.Val(r, '.');
		if (KErrNone != ret)
			{
			return ret;
			}
		TReal* temp = new(ELeave)TReal(r);
		aSetting.SetRealValue(temp);
		}
	else if (type == KTypeBinary)
		{
		TPtrC hexStr = aLex.NextToken();
		HBufC8* s = NULL;
		if (hexStr != KNullString)
			{
			s = HexToBinLC(hexStr);
			CleanupStack::Pop(s);
			}
		aSetting.SetStrValue(s);
		}
	else
		{
		return KErrCorrupt;
		}

	if (!aLex.Eos())
		{
		ret = ::ReadNumber(aLex, intval);
		if (ret == KErrNone)
			{
			aMetaFound = ETrue;
			aSetting.SetMeta(intval);
			}
		else
			{
			return KErrCorrupt;
			}
		}

	return KErrNone;
	}

TVerdict CConvertStepBase::VerifySettingsL(TConvToolTester& aShRepGetter)
	{
	TInt count;

	// Verify array size
	const RSettingsArray& Settings = aShRepGetter.Settings();
	count = 0;
	if (!GetIntFromConfig(ConfigSection(), KNumSettings, count))
		{
		ERR_PRINTF2(KMissingKey, &KNumSettings);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (Settings.Count() !=  count)
		{
		_LIT(KSettingsArrayMism, "Expected number of settings %d, actual %d");
		ERR_PRINTF3(KSettingsArrayMism, count, Settings.Count());
		SetTestStepResult(EFail);
		return EFail;
		}

	const TInt KMaxVarNameLen = 32;
	TBuf<KMaxVarNameLen> varName;
	TInt i;
	TPtrC data;

	for (i = 0; i < count; i++)
		{
		varName.Copy(KSetting_);
		varName.AppendNum(i);
		if (!GetStringFromConfig(ConfigSection(), varName, data))
			{
			ERR_PRINTF2(KMissingKey, &varName);
			SetTestStepResult(EFail);
			User::Leave(KErrCorrupt);
			}

		TLex lex(data);
		TServerSetting expected;
		TBool metaFound;
		if (KErrNone != ExtractSettingL(lex,expected,metaFound))
			{
			ERR_PRINTF3(KCorruptEntry, &varName, &data);
			SetTestStepResult(EFail);
			User::Leave(KErrCorrupt);
			}

		expected.PushL();

		if (!metaFound)
			{
			aShRepGetter.SetMetaDataOnCreate(expected);
			}

		const TServerSetting& actual = Settings[i];
		TBool match = ( actual == expected &&
						actual.Meta() == expected.Meta() &&
						actual.Key() == expected.Key());
		if (!match)
			{
			SetTestStepResult(EFail);

			_LIT(KSettingMism, "Setting expected value %S does not match");
			ERR_PRINTF2(KSettingMism, &data);
			TBuf<256> buf;
			TPtrC actualType;

			if (actual.IsInt())
				{
				buf.Num(actual.GetIntValue());
				actualType.Set(KTypeInt());
				}
			else if (actual.IsReal())
				{
				TRealFormat defaultRealFormat;
				buf.Num(actual.GetRealValue(), defaultRealFormat);
				actualType.Set(KTypeReal());
				}
			else
				{
				HBufC8* s = const_cast<HBufC8*>(actual.GetStrValue());
				if (s)
					{
					for (TInt j = 0; j < s->Length() & j < 128; j++)
						{
						buf.AppendNumFixedWidth((*s)[j], EHex, 2);
						}
					}
				actualType.Set(KTypeBinary());
				}
			_LIT(KActualSetting, "actual: [%d] [%S] [%S] [0x%x]");
			Logger().WriteFormat(KActualSetting, actual.Key(),
				&actualType, &buf, actual.Meta());
			}

		expected.Pop();
		expected.ResetValue();
		} // for

	return TestStepResult();
	}

void CConvertStepBase::ShowSecuPolicies(const TDesC8& aActualRd,
											const TDesC8& aExpectedRd, 
											const TDesC8& aActualWr,
											const TDesC8& aExpectedWr)
	{
	_LIT(KSecuPolicyFormat, "Expected %S policy %S, actual %S");
	_LIT(KRead, "read");
	_LIT(KWrite, "write");
	TBuf<16> actualBuf, expectedBuf;
	TInt i;
	for (i = 0; i < 8; i++)
		{
		expectedBuf.AppendNumFixedWidth(aExpectedRd[i], EHex, 2);
		actualBuf.AppendNumFixedWidth(aActualRd[i], EHex, 2);
		}
	Logger().WriteFormat(KSecuPolicyFormat, &KRead, &expectedBuf, &actualBuf);

	expectedBuf.Zero();
	actualBuf.Zero();
	for (i = 0; i < 8; i++)
		{
		expectedBuf.AppendNumFixedWidth(aExpectedWr[i], EHex, 2);
		actualBuf.AppendNumFixedWidth(aActualWr[i], EHex, 2);
		}
	Logger().WriteFormat(KSecuPolicyFormat, &KWrite, &expectedBuf, &actualBuf);
	}
