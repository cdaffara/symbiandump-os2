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
* Implementation for testing explicit tagging
*
*/


#include "testexplicittag.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>


CTestExplicitTag* CTestExplicitTag::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestExplicitTag* test = new (ELeave) CTestExplicitTag(aASN1Action);
	return test;
	}

CTestExplicitTag::CTestExplicitTag(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};

void CTestExplicitTag::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Explicit Tagging"));
	}

void CTestExplicitTag::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EInt);
	}


TBool CTestExplicitTag::PerformTest(CConsoleBase& aConsole, const TInt &aTest, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	// Get the encoder (a sequence with a null)
	CASN1EncSequence* seqEnc = CASN1EncSequence::NewLC();
	CASN1EncBase* null = CASN1EncNull::NewLC();
	seqEnc->AddChildL(null);
	CleanupStack::Pop(); // null

	CASN1EncBase* encoder = seqEnc;
	// Put loads of tag wrappers on
	// Starting at 3 fudges count to have the total number
	// of objects encoded
	for (TInt tags = 3; tags < aTest; ++tags)
		{
		CleanupStack::Pop(); // New explicit tag will take ownership (even if NewLC Leaves)
		encoder = CASN1EncExplicitTag::NewLC(encoder, tags);
		}

	// Now increase the size of the inner sequence, to check that the size change
	// propagates upwards through the tag wrappers
	null = CASN1EncNull::NewLC();
	seqEnc->AddChildL(null);
	CleanupStack::Pop(); // null

	// Prepare a buffer
	TInt totalLength = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(totalLength);
	TPtr8 tBuf = buf->Des();

	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);
	
	// Check number of objects in buffer reads OK
	TInt objectsRead = ReadAndCountL(tBuf);
	if (objectsRead != aTest)
		{
		aConsole.Write(_L("Explicitly tagged object mis-read"));
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


TInt CTestExplicitTag::ReadAndCountL(const TDesC8& aBuf)
	{
	TInt result = 1;
	
	TASN1DecGeneric decoder(aBuf);
	decoder.InitL();

	TInt bufLength = aBuf.Length();
	TInt readLength = decoder.LengthDER();

	__ASSERT_ALWAYS(bufLength >= readLength, User::Leave(KErrCorrupt));
	if (bufLength > readLength)
		{
		// Read the remainder of this buffer too
		result += ReadAndCountL(aBuf.Mid(readLength));
		}

	if (decoder.Encoding()[0] & 0x20		// Tagged as constructed
		&& decoder.LengthDERContent() > 0) 
		{
		// Read objects from inside the sequence too
		result += ReadAndCountL(decoder.GetContentDER());
		}

	return result;
	}

TBool CTestExplicitTag::PerformTestsL(CConsoleBase& aConsole)
	{
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
					return(EFalse);
				currentTest++;
				break;
				}
			case CTestParameter::EIntRange :
				{
				CIntRangeTestParameter *rangeInt = REINTERPRET_CAST(CIntRangeTestParameter*, test);

				for(TInt test = rangeInt->Start(); test <= rangeInt->Finish(); test++)
					{
					if(!PerformTest(aConsole, test, currentTest, totalTests))
						return(EFalse);
					currentTest++;
					};
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
