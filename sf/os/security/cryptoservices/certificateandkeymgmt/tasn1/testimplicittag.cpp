/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for testing implicit tagging
*
*/


#include "testimplicittag.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>

_LIT(KUniversalClass, "Universal");
_LIT(KApplicationSpecific, "ApplicationSpecific");
_LIT(KContextSpecific, "ContextSpecific");
_LIT(KPrivate, "Private");

CTestImplicitTag* CTestImplicitTag::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestImplicitTag* test = new (ELeave) CTestImplicitTag(aASN1Action);
	return test;
	}

CTestImplicitTag::CTestImplicitTag(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};


void CTestImplicitTag::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Implicit Tagging"));
	}

void CTestImplicitTag::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EString);
	iParameters->Append(CTestParameter::EInt);
	}


TBool CTestImplicitTag::PerformTest(CConsoleBase& aConsole, const TASN1Class& aClass, const TUint &aTest, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	CASN1EncBase* encoder = CASN1EncSequence::NewLC();
	encoder->SetTag(aTest, aClass);

	// Prepare a buffer
	TInt totalLength = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(totalLength);
	TPtr8 tBuf = buf->Des();

	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);

	// Read the buffer
	TASN1DecGeneric decoder(tBuf);
	decoder.InitL();

	TTagType readTag = decoder.Tag();
	TASN1Class readClass = decoder.Class();

	// Check tag + class read OK
	if (readTag != aTest || readClass != aClass)
		{
		aConsole.Write(_L("Implicitly tagged object mis-read"));
		OutputEncodingL(aConsole, tBuf);
		iASN1Action.ReportProgressL(KErrASN1EncodingError, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(2); // buf, encoder
		return(EFalse);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(2); // buf, encoder
		return(ETrue);
		}
	}

TBool CTestImplicitTag::PerformTestsL(CConsoleBase& aConsole)
	{
	CTestParameter* test;
	TInt totalTests, currentTest=0;
	TASN1Class classE;

	if(!CountTests(totalTests)) return(EFalse);

	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		test = (*iValues)[pos];
		if(test->GetType() == CTestParameter::EString)
		{
			CStringTestParameter *testString = REINTERPRET_CAST(CStringTestParameter*, test);
			TBuf<128> value;

			testString->GetValue(value);

			if(value == KUniversalClass)
				classE = EUniversal;
			else if(value == KApplicationSpecific)
				classE = EApplicationSpecific;
			else if(value == KPrivate)
				classE = EPrivate;
			else if(value == KContextSpecific)
				classE = EContextSpecific;
			else
				return(EFalse);
		}
		else
			return(EFalse);

		pos++;
		test = (*iValues)[pos];

		switch(test->GetType())
			{
			case CTestParameter::EInt :
				{
				CIntTestParameter *rangeInt = REINTERPRET_CAST(CIntTestParameter*, test);

				if(PerformTest(aConsole, classE, rangeInt->Value(), currentTest, totalTests))
					return(EFalse);
				currentTest++;
				break;
				}
			case CTestParameter::EIntRange :
				{
				CIntRangeTestParameter *rangeInt = REINTERPRET_CAST(CIntRangeTestParameter*, test);

				for(TInt test = rangeInt->Start(); test <= rangeInt->Finish(); test++)
					{
					if(!PerformTest(aConsole, classE, test, currentTest, totalTests))
						return(EFalse);
					currentTest++;
					}
				break;
				}
			default:
				return EFalse;
			}
		}
	iASN1Action.ReportProgressL(KErrNone, totalTests, totalTests);
	return(ETrue);
	}

