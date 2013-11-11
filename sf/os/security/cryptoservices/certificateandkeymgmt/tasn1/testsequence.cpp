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
* Implementation for testing sequence object encoding/decoding
*
*/


#include "testsequence.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <e32math.h>
#include <e32cons.h>

#include <bigint.h>

CTestSequence* CTestSequence::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestSequence* test = new (ELeave) CTestSequence(aASN1Action);
	return test;
	}

CTestSequence::CTestSequence(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};


void CTestSequence::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Sequence"));
	}

void CTestSequence::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EInt);
	}


TBool CTestSequence::PerformTest(CConsoleBase& aConsole, const TInt &aTestDepth, const TInt &aTestSize, const TInt &aTestNumber, const TInt &aTotalTests)
	{
	iObjectsEncoded = 1;
	CASN1EncBase* encoder = MakeSequenceEncoderLC(aTestDepth, 0, aTestSize);
	
	// Prepare a buffer
	TInt totalLength = encoder->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(totalLength);
	TPtr8 tBuf = buf->Des();
	
	// Write into the buffer
	TUint writeLength = 0;
	encoder->WriteDERL(tBuf, writeLength);

	// Check number of objects in buffer reads OK and matches what we hoped
	TInt objectsRead = ReadAndCountL(tBuf);
	if (objectsRead != iObjectsEncoded)
		{
		aConsole.Write(_L("ERROR! Encoded sequence object count mismatch\n"));
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


CASN1EncSequence* CTestSequence::MakeSequenceEncoderLC(TInt aMaxDepth, 
													   TInt aMinSize, 
													   TInt aMaxSize)
	{
	__ASSERT_ALWAYS(aMaxDepth > 0, User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aMinSize >= 0, User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aMinSize <= aMaxSize, User::Leave(KErrArgument));

	CASN1EncSequence* encoder = CASN1EncSequence::NewLC();

	TUint targetSize = aMinSize + (Math::Random() % (aMaxSize + 1 - aMinSize));
	TUint size = 0;
	while (size < targetSize)
		{
		const TInt KMaxChildType = 3;
		const TInt childTypes = aMaxDepth > 1 ? KMaxChildType + 1 : KMaxChildType;
		const TInt childType = (Math::Random() >> 5) % childTypes;
		CASN1EncBase* child = 0;
		CASN1EncSequence* seqEnc = 0;
		switch (childType)
			{
			case 0:
				child = CASN1EncInt::NewLC(Math::Random());
				break;
			case 1:
				{
				RInteger i = RInteger::NewRandomL(1234, TInteger::EAllBitsRandom);
				CleanupStack::PushL(i);
				CASN1EncBigInt* encInt = CASN1EncBigInt::NewLC(i);
				CleanupStack::Pop(); // encInt
				CleanupStack::PopAndDestroy(&i); // a deep copy of the info is taken
				CleanupStack::PushL(encInt);
				child = encInt;
				break;
				}
				case 2:
				child = CASN1EncNull::NewLC();
				break;
			case KMaxChildType:
				// last case must be for next-level sequence encoder
				seqEnc = MakeSequenceEncoderLC(aMaxDepth - 1, aMinSize, aMaxSize);
				child = seqEnc;
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
			}
		
		if (child)
			{
			encoder->AddChildL(child);
			CleanupStack::Pop(); // child
			++iObjectsEncoded;
			++size;

			// If we've just added a sequence, want to test ability to add things to a sequence
			// *that is itself a child*, to check size changes propagate upwards to parent
			if (seqEnc)
				{
				child = CASN1EncInt::NewLC(Math::Random());

				seqEnc->AddChildL(child);
				CleanupStack::Pop(); // child
				++iObjectsEncoded;
				seqEnc = 0;
				}
			}
		}
	
	return encoder;
	}


TInt CTestSequence::ReadAndCountL(const TDesC8& aBuf)
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

TBool CTestSequence::PerformTestsL(CConsoleBase& aConsole)
	{
	// Keep these two coprime to maximise combinations
	const TInt KMaxSize = 11;
	const TInt KMaxDepth = 10;
	CTestParameter* test;
	TInt totalTests, currentTest=0;
	TInt maxDepth;
	TInt maxSize;

	if(!CountTests(totalTests)) return(EFalse);

	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		test = (*iValues)[pos];
		switch(test->GetType())
			{
			case CTestParameter::EInt :
				{
				CIntTestParameter *intTest = REINTERPRET_CAST(CIntTestParameter*, test);

				// Get the encoder and decoder
				maxDepth = intTest->Value();
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt)
					{
					User::Leave(KErrNotSupported);
					}
				intTest = REINTERPRET_CAST(CIntTestParameter*, test);
				maxSize = intTest->Value();
				if(!PerformTest(aConsole, maxDepth, maxSize, currentTest, totalTests))
					{
					return(EFalse);
					}
				currentTest++;
				break;
				}
			case CTestParameter::ERandom :
				{
				CRandomTestParameter *randomTest = REINTERPRET_CAST(CRandomTestParameter*, test);
				// Get the encoder and decoder

				for(TInt test = 0; test <= randomTest->Interations(); test++)
					{

					// Get the encoder and decoder
					maxDepth = (test % KMaxDepth) + 1;
					maxSize = test % KMaxSize;

					currentTest++;
					// Don't do the too-big combinations (too slow)
					if (maxDepth +  maxSize > 14) continue;
					if(!PerformTest(aConsole, maxDepth, maxSize, currentTest, totalTests))
						{
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
