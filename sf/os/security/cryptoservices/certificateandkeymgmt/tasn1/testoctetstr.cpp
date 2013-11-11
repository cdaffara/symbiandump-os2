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
* Implementation for testing octet string encoding/decoding
*
*/


#include "testoctetstr.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32math.h>
#include <e32cons.h>

const TInt KMaxStringLength = 1000;

CTestOctetString* CTestOctetString::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestOctetString* test = new (ELeave) CTestOctetString(aASN1Action);
	test->ConstructL();
	return test;
	}

CTestOctetString::CTestOctetString(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};

void CTestOctetString::ConstructL()	
	{
	toEncodeBuf = HBufC8::NewMaxLC(KMaxStringLength);
	encodedBuf = HBufC8::NewMaxLC(KMaxStringLength + 10); // extra 10 for tag/length
	CleanupStack::Pop(2);
	};

void CTestOctetString::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EString);
	}


CTestOctetString::~CTestOctetString()	
	{
	delete toEncodeBuf;
	delete encodedBuf;
	}

void CTestOctetString::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Octet String"));
	}


TBool CTestOctetString::PerformTest(CConsoleBase& aConsole, const TPtr8 &toEncodePtr, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	TPtr8 encodedPtr = encodedBuf->Des();

	// Make the encoder
	CASN1EncOctetString* encoder = CASN1EncOctetString::NewLC(toEncodePtr);

	// Do the encoding
	TUint writeLength = 0;
	encoder->WriteDERL(encodedPtr, writeLength);
	
	// Read it out again
	TASN1DecOctetString decoder;
	TInt readLength = 0;
	HBufC8* readBuf = decoder.DecodeDERL(encodedPtr, readLength);
	CleanupStack::PushL(readBuf);
	
	// Check lengths of reads + values
	if ((writeLength != STATIC_CAST(TUint, readLength)) || (*readBuf != toEncodePtr))
		{
		aConsole.Printf(_L("\nERROR!\n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(2); // encoder, readBuf
		return(ETrue);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, aTestNumber, aTotalTests);
		CleanupStack::PopAndDestroy(2); // encoder, readBuf
		return(ETrue);
		}
	}


TBool CTestOctetString::PerformTestsL(CConsoleBase& aConsole)
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
			case CTestParameter::EString :
				{
				CStringTestParameter *testString = REINTERPRET_CAST(CStringTestParameter*, test);
				HBufC* value = HBufC::NewLC(KMaxStringLength);
				TPtr tBuf = value->Des();

				testString->GetValue(tBuf);

				if((tBuf.Length() % 2) != 0)
					User::Leave(KErrArgument);

				TPtr8 toEncodePtr(toEncodeBuf->Des());
				TUint theOctet;
				TInt octetPos=0;

				for(TInt octet = 0; octet < tBuf.Length(); octet+=2)
					{
					TLex lex(tBuf.Mid(octet, 2));
					
					if(lex.Val(theOctet, EHex)!=KErrNone)
						User::Leave(KErrArgument);
					toEncodePtr[octetPos++] = STATIC_CAST(TUint8, theOctet);
					};
				if(PerformTest(aConsole, toEncodePtr, currentTest, totalTests))
					{
					CleanupStack::PopAndDestroy();
					currentTest++;
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
				CRandomTestParameter *randomInt = REINTERPRET_CAST(CRandomTestParameter*, test);

				for (TInt count = 0; count <= randomInt->Interations(); ++count)
					{
					// Descriptor for part of buffer we'll use; fill with noise
					TInt toEncodeLength = count % KMaxStringLength;
					TPtr8 toEncodePtr(toEncodeBuf->Des());
					for (TInt i = 0; i < toEncodeLength; ++i)
						{
						toEncodePtr[i] = STATIC_CAST(TUint8, Math::Rand(seed));
						}
					if(!PerformTest(aConsole, toEncodePtr, currentTest, totalTests))
						return(EFalse);
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

