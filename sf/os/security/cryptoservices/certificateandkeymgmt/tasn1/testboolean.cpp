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
* Implementation for testing boolean object encoding/decoding
*
*/


#include "testboolean.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>

CTestBoolean* CTestBoolean::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestBoolean* test = new (ELeave) CTestBoolean(aASN1Action);
	return test;
	}

CTestBoolean::CTestBoolean(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};

void CTestBoolean::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Boolean"));
	}

void CTestBoolean::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EInt);
	}

 
TBool CTestBoolean::PerformTest(CConsoleBase& aConsole, const TBool& aTest)
	{
	// Make the encoder
	CASN1EncBoolean* encoder = CASN1EncBoolean::NewLC(aTest);

	// Prepare a buffer
	TInt length = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(length);
	TPtr8 tBuf = buf->Des();

	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);
	
	// Read it out again + check lengths
	TASN1DecBoolean decoder;
	TInt readLength = 0;
	TBool decodedValue = decoder.DecodeDERL(tBuf, readLength);
	
	if (writeLength != STATIC_CAST(TUint, readLength) || decodedValue != aTest)
		{
		aConsole.Write(_L("ERROR!\n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, 1, 1);
		CleanupStack::PopAndDestroy(2); // buf, encoder
		return(EFalse);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, 1, 1);
		CleanupStack::PopAndDestroy(2); // buf, encoder
		return(ETrue);
		}
	}


TBool CTestBoolean::PerformTestsL(CConsoleBase& aConsole)
	{
	CTestParameter* test;
	TInt totalTests;

	if(!CountTests(totalTests)) return(EFalse);

	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		test = (*iValues)[pos];

		switch(test->GetType())
			{
			case CTestParameter::EInt :
				{
				CIntTestParameter *rangeInt = REINTERPRET_CAST(CIntTestParameter*, test);

				if(!PerformTest(aConsole, rangeInt->Value()))
					return(EFalse);
				break;
				}
			case CTestParameter::EIntRange :
				{
				CIntRangeTestParameter *rangeInt = REINTERPRET_CAST(CIntRangeTestParameter*, test);

				for(TInt test = rangeInt->Start(); test <= rangeInt->Finish(); test++)
					{
					if(!PerformTest(aConsole, test))
						return(EFalse);
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

