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
* Implementation for testing encoded object output
*
*/


#include "testoutput.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <e32cons.h>
#include <f32file.h>


#include <bigint.h>

CTestOutput* CTestOutput::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestOutput* test = new (ELeave) CTestOutput(aASN1Action);
	return test;
	}

CTestOutput::CTestOutput(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action)
	{
	};


void CTestOutput::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Output (to file)"));
	}


TBool CTestOutput::PerformTestsL(CConsoleBase& aConsole)
	{
	// Get object to output
	CASN1EncBase* enc = MakeEncoderLC();

	// Prepare a buffer
	TUint length = enc->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(length);
	TPtr8 tBuf = buf->Des();
	
	// Write into the buffer
	TUint writeLength = 0;
	enc->WriteDERL(tBuf, writeLength);

	// Copy to file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TDriveUnit sysDrive (fs.GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<64> fileName(driveName);
	fileName.Append(_L("\\tasn1\\TASN1_test_output"));

	RFile file;
	User::LeaveIfError(file.Replace(fs, fileName, EFileWrite));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(tBuf));

	// Tidy up
	CleanupStack::PopAndDestroy(4); // Close file, close fs, buf, enc
	iASN1Action.ReportProgressL(KErrNone, 1, 1);

	aConsole.Write(_L("Now use DumpASN1 on "));
	aConsole.Write(fileName);
	aConsole.Write(_L("\n"));
	return(ETrue);
	}


CASN1EncBase* CTestOutput::MakeEncoderLC(const TBool aNest) const
	{
	// Sequence we'll be using
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();

	// Add objects
	TUint index = aNest ? 0 : 1;
	while (CASN1EncBase* enc = MakeEncObjLC(index++))
		{
		seq->AddChildL(enc);
		CleanupStack::Pop(); // enc
		}

	return seq;
	}


CASN1EncBase* CTestOutput::MakeEncObjLC(const TUint aIndex) const
	{
	switch (aIndex)
		{
		case 0:
			// Case 0 is always the nested sequence encoder
			return MakeEncoderLC(EFalse);
		case 1:
			return CASN1EncNull::NewLC();
		case 2:
			return CASN1EncBoolean::NewLC(ETrue);
		case 3:
			return CASN1EncBoolean::NewLC(EFalse);
		case 4:
			return CASN1EncInt::NewLC(12345);
		case 5:
			return CASN1EncInt::NewLC(-4354);
		case 6:
			{
			RInteger i = RInteger::NewRandomL(345, TInteger::EAllBitsRandom);
			CleanupStack::PushL(i);
			CASN1EncBigInt* enc = CASN1EncBigInt::NewLC(i);
			CleanupStack::Pop();
			CleanupStack::PopAndDestroy(&i);
			CleanupStack::PushL(enc);
			return enc;
			}
		case 7: // -ve this time
			{
			RInteger i = RInteger::NewRandomL(345, TInteger::EAllBitsRandom);
			CleanupStack::PushL(i);
			i *= -1;
			CASN1EncBigInt* enc = CASN1EncBigInt::NewLC(i);
			CleanupStack::Pop();
			CleanupStack::PopAndDestroy(&i);
			CleanupStack::PushL(enc);
			return enc;
			}
		case 8:
			{
			// INFOSEC policy UID
			_LIT(KTestOID, "2.16.840.1.101.2.1.3.10");
			return CASN1EncObjectIdentifier::NewLC(KTestOID);
			}
		case 9:
			{
			HBufC8* str = HBufC8::NewLC(256);
			TPtr8 des = str->Des();
			for (TUint i = 0; i < 256; ++i)
				{
				des.Append(STATIC_CAST(TChar, i));
				}
			CASN1EncBase* enc = CASN1EncOctetString::NewLC(des);
			CleanupStack::Pop();
			CleanupStack::PopAndDestroy(); // str
			CleanupStack::PushL(enc);
			return enc;
			}
		case 10: // without seconds
			{
			// DateTime month and day are 0-based
			TDateTime dateTime(1972, EDecember, 19, 11, 35, 0, 0);
			return CASN1EncGeneralizedTime::NewLC(dateTime); // Implicit conversion to TTime
			}
		case 11: // with seconds
			{
			// DateTime month and day are 0-based
			TDateTime dateTime(1972, EDecember, 19, 11, 35, 23, 0);
			return CASN1EncGeneralizedTime::NewLC(dateTime); // Implicit conversion to TTime
			}
		case 12:
			{
			CASN1EncBase* null = CASN1EncNull::NewLC();
			CleanupStack::Pop(); // null
			return CASN1EncExplicitTag::NewLC(null, 100);
			}
		default:
			return 0;
		}
	}
