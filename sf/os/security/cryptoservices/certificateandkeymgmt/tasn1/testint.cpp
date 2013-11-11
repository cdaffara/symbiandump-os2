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
* Implementation for testing TInt encoding/decoding
*
*/


#include "testint.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32math.h>
#include <e32cons.h>


CTestInt* CTestInt::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestInt* test = new (ELeave) CTestInt(aASN1Action);
	return test;
	}

CTestInt::CTestInt(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};


void CTestInt::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test TInt"));
	}

void CTestInt::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EInt);
	}



TBool CTestInt::PerformTest(CConsoleBase& aConsole, const TInt &aTest, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	// Choose value to encode
	TInt encodedValue;

	encodedValue = aTest;

	// Get encoder
	CASN1EncInt* encoder = CASN1EncInt::NewLC(encodedValue);
	
	// Prepare a buffer
	TUint length = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(length);
	TPtr8 tBuf = buf->Des();

	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);
	
	// Read it out again
	TASN1DecInteger decoder;
	TInt readLength = 0;
	TInt decodedValue = decoder.DecodeDERShortL(tBuf, readLength);
	
	// Check lengths of reads + values
	if ((writeLength != STATIC_CAST(TUint, readLength)) || (decodedValue != encodedValue))
		{
		aConsole.Printf(_L("\nERROR!  Problem integer: %d\n"), encodedValue);
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

TBool CTestInt::PerformTestsL(CConsoleBase& aConsole)
	{
	TInt nLow = Math::Random();
	TInt64 nHigh((TInt)Math::Random());
	TInt64 seed((nHigh << 32) + nLow);
	CTestParameter* test;
	TInt totalTests, currentTest=0;

	if(!CountTests(totalTests)) return(EFalse);

	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		test = (*iValues)[pos];
		switch(test->GetType())
			{
			case CTestParameter::EInt :
				{
				CIntTestParameter *rangeInt = REINTERPRET_CAST(CIntTestParameter*, test);

				if(!PerformTest(aConsole, rangeInt->Value(), currentTest, totalTests))
					{
					return(EFalse);
					}
				currentTest++;
				break;
				}
			case CTestParameter::EIntRange :
				{
				CIntRangeTestParameter *rangeInt = REINTERPRET_CAST(CIntRangeTestParameter*, test);

				for(TInt test = rangeInt->Start(); test <= rangeInt->Finish(); test++)
					{
					if(!PerformTest(aConsole, test, currentTest, totalTests))
						{
						return(EFalse);
						}
					currentTest++;
					}
				break;
				}
			case CTestParameter::ERandom :
				{
				CRandomTestParameter *randomInt = REINTERPRET_CAST(CRandomTestParameter*, test);
				TInt encodedValue;

				for(TInt test = 0; test <= randomInt->Interations(); test++)
					{
					encodedValue = Math::Rand(seed) >> (8 * (test % 4));
					if(!PerformTest(aConsole, encodedValue, currentTest, totalTests))
						{
						return(EFalse);
						}
					currentTest++;
					}
				break;
				}
			default:
				{
				return EFalse;
				}
			}
		}
	iASN1Action.ReportProgressL(KErrNone, totalTests, totalTests);
	return(ETrue);
	}

