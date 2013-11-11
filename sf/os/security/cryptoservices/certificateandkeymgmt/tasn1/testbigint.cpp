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
* testinteger.cpp
* Implementation for testing TInteger encoding/decoding
*
*/


#include "testbigint.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>


CTestBigInt* CTestBigInt::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestBigInt* test = new (ELeave) CTestBigInt(aASN1Action);
	return test;
	}

CTestBigInt::CTestBigInt(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};

void CTestBigInt::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test BigInteger"));
	}

void CTestBigInt::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EInt);
	}


//	This function can leave
TBool CTestBigInt::PerformTest(CConsoleBase& aConsole, const RInteger& aTest, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	// Make the value to encode
	RInteger encodedValue = RInteger::NewL(0);
	CleanupStack::PushL(encodedValue);
	encodedValue.CopyL(aTest);
	
	// Make the encoder
	CASN1EncBigInt* encoder = CASN1EncBigInt::NewLC(encodedValue);

	// Prepare a buffer
	HBufC8* buf = HBufC8::NewMaxLC(encoder->LengthDER());
	TPtr8 tBuf = buf->Des();
	
	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);
	
	// Read it out again
	TASN1DecInteger decoder;
	TInt readLength = 0;
	RInteger decodedValue = decoder.DecodeDERLongL(tBuf, readLength);
	CleanupStack::PushL(decodedValue);

	//Check that a positive integer has not been encoded as negative - DEF038956
	TBool negEncodeErr = EFalse;
	RInteger null = RInteger::NewL(0);
	CleanupStack::PushL(null);
	if (aTest > null)
		{
		HBufC8* originalValue = aTest.BufferLC();
		TPtrC8 tOriginalValue = originalValue->Des();
		//determine which byte to check (should a leading zero byte have been added)
		if ((*originalValue)[0] & 0x80)
			{
			//for example 000000FF, aTest value FF, *buf 020200FF need to confirm (*buf)[2] is 00
			//encoder->LengthDER() = 4, tOriginalValue.Length() = 1
			if ((*buf)[encoder->LengthDER() - tOriginalValue.Length() - 1] != 0)
				{
				negEncodeErr = ETrue;
				}
			}
		else
			{
			//for example 0000007F, aTest value 7F, *buf 02027F need to confirm leading bit (*buf)[2] not set
			//encoder->LengthDER() = 3, tOriginalValue.Length() = 1
			if ((*buf)[encoder->LengthDER() - tOriginalValue.Length()] & 0x80)
				{
				negEncodeErr = ETrue;
				}
			}
		CleanupStack::PopAndDestroy(); //originalValue
		}
	CleanupStack::PopAndDestroy(&null); //null

	// Check lengths of reads + values
	if ((writeLength != STATIC_CAST(TUint, readLength)) || !(decodedValue == encodedValue) || (negEncodeErr))
	//if(1)
		{
		aConsole.Printf(_L("\nERROR!  Problem integer: \n"));
		OutputIntegerL(aConsole, encodedValue);
		OutputIntegerL(aConsole, decodedValue);
		OutputEncodingL(aConsole, tBuf);
		iASN1Action.ReportProgressL(KErrASN1EncodingError, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(4, &encodedValue); // decodedValue, buf,  encoder, encodedValue
		return(EFalse);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(4, &encodedValue); // decodedValue, buf,  encoder, encodedValue
		return(ETrue);
		}
	}


void CTestBigInt::OutputIntegerL(CConsoleBase& aConsole, RInteger& aInt)
	{
	aConsole.Printf(_L("Bytes() = "));
	TBuf<10> bytes;
	bytes.AppendNum(aInt.ByteCount());
	bytes.Append(_L(", "));
	aConsole.Printf(bytes);

	aConsole.Printf(_L("Bits() = "));
	TBuf<10> bits;
	bits.AppendNum(aInt.BitCount());
	bits.Append(_L(", "));
	aConsole.Printf(bits);

	HBufC8* buf = aInt.BufferLC();
	TPtr8 ptr = buf->Des();
	TInt size = ptr.Length();
	for (TInt i = 0; i < size; ++i)
		{
		TBuf<10> tbuf;
		tbuf.AppendNumFixedWidth(ptr[i], EHex, 2);
		aConsole.Printf(tbuf);
		}

	RInteger null = RInteger::NewL(0);
	if (aInt < null)
		{
		ASSERT(EFalse);	//	Shouldn't happen for new crypto api - no signed integers
		aConsole.Printf(_L(" (-ve)"));
		}
	else
		{
		aConsole.Printf(_L(" (+ve)"));
		}

	null.Close();

	aConsole.Printf(_L("\n"));
	CleanupStack::PopAndDestroy(); // buf;
	}


TBool CTestBigInt::PerformTestsL(CConsoleBase& aConsole)
	{
	const TInt KMaxBits = 2048;
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
				RInteger encodedValue;
				encodedValue = RInteger::NewL(rangeInt->Value());
				CleanupStack::PushL(encodedValue);
				if(PerformTest(aConsole, encodedValue, currentTest, totalTests))
					CleanupStack::PopAndDestroy(&encodedValue);
				else
					{
					CleanupStack::PopAndDestroy(&encodedValue);
					return(EFalse);
					};
				currentTest++;
				break;
				}
			case CTestParameter::EIntRange :
				{
				CIntRangeTestParameter *rangeInt = REINTERPRET_CAST(CIntRangeTestParameter*, test);
				for(TInt test = rangeInt->Start(); test <= rangeInt->Finish(); test++)
					{
					RInteger encodedValue;
					encodedValue = RInteger::NewL(test);						
					CleanupStack::PushL(encodedValue);
					if(PerformTest(aConsole, encodedValue, currentTest, totalTests))
						CleanupStack::PopAndDestroy(&encodedValue);
					else
						{
						CleanupStack::PopAndDestroy(&encodedValue);
						return(EFalse);
						}
					currentTest++;
					};
				break;
				}
			case CTestParameter::ERandom :
				{
				CRandomTestParameter *randomInt = REINTERPRET_CAST(CRandomTestParameter*, test);
				for(TInt test = 0; test <= randomInt->Interations(); test++)
					{
					RInteger encodedValue;
					encodedValue = RInteger::NewRandomL((test % KMaxBits) + 1, TInteger::EAllBitsRandom);
					CleanupStack::PushL(encodedValue);
					if(PerformTest(aConsole, encodedValue, currentTest, totalTests))
						CleanupStack::PopAndDestroy(&encodedValue);
					else
						{	
						CleanupStack::PopAndDestroy(&encodedValue);
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
	};

