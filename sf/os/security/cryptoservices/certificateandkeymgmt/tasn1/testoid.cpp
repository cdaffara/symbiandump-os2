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
* Implementation for testing object identifier encoding/decoding
*
*/


#include "testoid.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>
#include <e32math.h>

const TInt KMinOIDTerms = 2;
const TInt KMaxOIDTerms = KNumberOfIDs;
const TInt KMaxTermSize = 999999;
const TInt KMaxTermDigits = 6;
const TInt KMaxTermBits = 20;       // Bits in KMaxTermsSize
const TInt KMaxBufSize = (KMaxTermDigits * (KMaxOIDTerms - 2)) + 2 + KMaxOIDTerms;

CTestOID* CTestOID::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestOID* test = new (ELeave) CTestOID(aASN1Action);
	return test;
	}

CTestOID::CTestOID(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};


void CTestOID::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Object Identifier"));
	}


void CTestOID::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EString);
	}

TBool CTestOID::PerformTest(CConsoleBase& aConsole, HBufC &aTest, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	// Get data, place in encoder
	CASN1EncObjectIdentifier* encoder = CASN1EncObjectIdentifier::NewLC(aTest);

	// Prepare a buffer
	TInt totalLength = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(totalLength);
	TPtr8 tBuf = buf->Des();
	
	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);

	// Read it out again + check lengths
	TASN1DecObjectIdentifier decoder;
	TInt readLength = 0;
	HBufC* decodedData = decoder.DecodeDERL(tBuf, readLength);
	CleanupStack::PushL(decodedData);
	
	if (writeLength != STATIC_CAST(TUint, readLength) || aTest != *decodedData)
		{
		aConsole.Write(_L("ERROR!\nData to encode: "));
		aConsole.Write(aTest);
		aConsole.Write(_L("\nDecoded data: "));
		aConsole.Write(*decodedData);
		aConsole.Write(_L("\n"));
		OutputEncodingL(aConsole, tBuf);
		iASN1Action.ReportProgressL(KErrASN1EncodingError, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(3); // decodedData, buf, encoder
		return(EFalse);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(3); // decodedData, buf, encoder
		return(ETrue);
		}
	}


TBool CTestOID::PerformTestsL(CConsoleBase& aConsole)
	{
	CTestParameter* test;
	TInt totalTests, currentTest=0;

	if(!CountTests(totalTests)) return(EFalse);

	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		test = (*iValues)[pos];
		switch(test->GetType())
			{
			case CTestParameter::EString :
				{
				CStringTestParameter *testString = REINTERPRET_CAST(CStringTestParameter*, test);
				HBufC* value = HBufC::NewLC(KMaxBufSize);
				TPtr tBuf = value->Des();

				testString->GetValue(tBuf);
				if(PerformTest(aConsole, *value, currentTest, totalTests))
					{
					currentTest++;
					CleanupStack::PopAndDestroy();
					}
				else
					{
					CleanupStack::PopAndDestroy();
					return(EFalse);
					}
				break;
				}
			case CTestParameter::ERandom :
				{
				CRandomTestParameter *testRandom = REINTERPRET_CAST(CRandomTestParameter*, test);
				HBufC* value;
					
				for(TInt test = 0; test <= testRandom->Interations(); test++)
					{
					value = HBufC::NewLC(KMaxBufSize);

					TPtr tBuf = value->Des();
					tBuf.AppendNum(static_cast<TUint>(Math::Random() % 3), EDecimal);
					tBuf.Append('.');
					tBuf.AppendNum(static_cast<TUint>(Math::Random() % 40), EDecimal);

					TInt targetTerms = (test % (1 + KMaxOIDTerms - KMinOIDTerms)) + KMinOIDTerms;
					for (TInt terms = 2; terms < targetTerms; ++terms)
						{
						TInt term = Math::Random() % (KMaxTermSize >> (test % KMaxTermBits));
						tBuf.Append('.');
						tBuf.AppendNum(term, EDecimal);
						}
					if(PerformTest(aConsole, *value, currentTest, totalTests))
						{
						CleanupStack::PopAndDestroy();
						currentTest++;
						}
					else
						{
						CleanupStack::PopAndDestroy();
						return(EFalse);
						}
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

