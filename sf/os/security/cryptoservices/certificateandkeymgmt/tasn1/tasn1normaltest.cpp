/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ASN1TestAction.cpp: implementation of the CASN1NormalTest class.
*
*/


/**
 @file
*/

#include "tasn1normaltest.h"
#include "testnull.h"
#include "testboolean.h"
#include "testint.h"
#include "testbigint.h"
#include "testoctetstr.h"
#include "testoid.h"
#include "testinvalidoid.h"
#include "testgeneralizedtime.h"
#include "testsequence.h"
#include "testexplicittag.h"
#include "testimplicittag.h"
#include "testoutput.h"
#include "testbitstr.h"
#include "t_input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TInt CASN1NormalTest::iReportCount = 1;

_LIT(KASN1TestStart, "<asn1test>");
_LIT(KTypeStart, "<type>");
_LIT(KIntType, "int");
_LIT(KBigIntType, "bigint");
_LIT(KBooleanType, "boolean");
_LIT(KNullType, "null");
_LIT(KExplicitTagType, "explicittag");
_LIT(KImplicitTagType, "implicittag");
_LIT(KGeneralizedTimeType, "generalizedtime");
_LIT(KOctetStringType, "octetstring");
_LIT(KObjectIDType, "objectid");
_LIT(KInvaidObjectIDType, "invalidobjectid");
_LIT(KSequenceType, "sequence");
_LIT(KOutputType, "output");
_LIT(KBitStrType, "bitstring");

CASN1NormalTest::~CASN1NormalTest()
	{
		delete iTestType;
	}

CASN1NormalTest::CASN1NormalTest(RFs& /*aFs*/, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iTestType(0), iPercentReported(-1), iReportNumber(iReportCount++) 
	{
	}

CTestAction* CASN1NormalTest::NewL(RFs& aFs,
										 CConsoleBase& aConsole,
										 Output& aOut, 
										 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CASN1NormalTest::NewLC(aFs, aConsole,
		aOut,  aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CASN1NormalTest::NewLC(RFs& aFs,
										  CConsoleBase& aConsole,
										  Output& aOut, 
										  const TTestActionSpec& aTestActionSpec)
	{
	CASN1NormalTest* self = new(ELeave) CASN1NormalTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}


void CASN1NormalTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	HBufC* body = HBufC::NewLC(aTestActionSpec.iActionBody.Length());

	body->Des().Copy(aTestActionSpec.iActionBody);
	TPtrC rootCert = Input::ParseElement(*body, KASN1TestStart);
	TPtrC iType = Input::ParseElement(rootCert, KTypeStart);
	
	// constucts correct class depending on string
	if(iType.CompareF(KIntType)==0)
		iTestType = CTestInt::NewL(*this);
	else if(iType.CompareF(KBigIntType)==0)
		iTestType = CTestBigInt::NewL(*this);
	else if(iType.CompareF(KBooleanType)==0)
		iTestType = CTestBoolean::NewL(*this);
	else if(iType.CompareF(KNullType)==0)
		iTestType = CTestNull::NewL(*this);
	else if(iType.CompareF(KExplicitTagType)==0)
		iTestType = CTestExplicitTag::NewL(*this);
	else if(iType.CompareF(KImplicitTagType)==0)
		iTestType = CTestImplicitTag::NewL(*this);
	else if(iType.CompareF(KGeneralizedTimeType)==0)
		iTestType = CTestGeneralizedTime::NewL(*this);
	else if(iType.CompareF(KOctetStringType)==0)
		iTestType = CTestOctetString::NewL(*this);
	else if(iType.CompareF(KObjectIDType)==0)
		iTestType = CTestOID::NewL(*this);
	else if(iType.CompareF(KSequenceType)==0)
		iTestType = CTestSequence::NewL(*this);
	else if(iType.CompareF(KOutputType)==0)
		iTestType = CTestOutput::NewL(*this);
	else if(iType.CompareF(KBitStrType)==0)
		iTestType = CTestBitStr::NewL(*this);
	else if(iType.CompareF(KInvaidObjectIDType)==0)
		iTestType = CTestInvalidOID::NewL(*this);

	if(iTestType)
		SetScriptError(iTestType->ConstructL(aTestActionSpec), iTestType->iSyntaxErrorDescription);

	CleanupStack::PopAndDestroy(); // body
	}

void CASN1NormalTest::PerformAction(TRequestStatus& aStatus)
	{
	TRAPD(err, iResult = iTestType->PerformTestsL(iConsole));
	if (err==KErrNoMemory)
		User::Leave(err);
	else
		{
		iActionState = EPostrequisite;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}


void CASN1NormalTest::ReportProgressL(const TInt aError, const TInt aCount, const TInt aMax)
	{
	if(aMax == 0) return;
	TInt newPercentage = aCount * 100 / aMax;
	
	if (newPercentage > iPercentReported && iResult == 0)
		{
		iPercentReported = newPercentage;
		
		iConsole.Printf(_L(" %d"), iPercentReported);

		if (iPercentReported == 100)
			{
			iConsole.Write(_L("%"));
			iConsole.Write(_L(" Completed:\n"));
			CheckResult(aError);
			iPercentReported = -1;
			}
		}
	if(aError != 0)
		{
		iFinished = ETrue;
		iResult = EFalse;
		}
	}

void CASN1NormalTest::DoCheckResult(TInt aError)
	{
	if(aError == 0 && !iFinished)
		{
		iFinished = ETrue;
		iResult = ETrue;
		}
	}


void CASN1NormalTest::DoReportAction()
	{
	HBufC *tmp = HBufC::NewMaxLC(iNameInfo->Length());

	// converts name to unicode (if necessary) and dumps it out to screen
 	tmp->Des().Copy(*iNameInfo);
	iConsole.Printf(_L("\n"));
	iConsole.Printf(_L("Report %d - "), iReportNumber);
	iConsole.Write(*tmp);
	iConsole.Printf(_L("\n"));

	CleanupStack::PopAndDestroy(); // tmp
	}

TBool CASN1NormalTest::TestResult(TInt /*aError*/)
	{
	return 0;
	}
	
void CASN1NormalTest::PerformCancel()
	{
	}
	
void CASN1NormalTest::Reset()
	{
	}

