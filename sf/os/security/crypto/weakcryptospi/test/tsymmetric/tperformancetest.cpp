/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "tperformancetest.h"
#include "symmetric.h"
#include "t_output.h"

#ifndef _FOO
_LIT(KPerfEFormat, "\tPerformance(encryption)\t%f");
_LIT(KPerfDFormat, "\tPerformance(decryption)\t%f");
_LIT(KTotalFormat, "\tPerformance(total)\t%f");
_LIT(KPerfThroughput, "\tThroughput (total)\t%f");
_LIT(KPerfThroughputAfterSetup, "\tThroughput after setup\t%f");
_LIT(KPerfEncryptorCreate, "\tPerformance(encryptor create)\t%f");
_LIT(KPerfDecryptorCreate, "\tPerformance(decryptor create)\t%f");
//_LIT(KDataSize,"\tData input size : %i B\r\n");
#endif

CTestAction* CPerformanceTest::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CPerformanceTest::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CPerformanceTest::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CPerformanceTest* self = new(ELeave) CPerformanceTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPerformanceTest::~CPerformanceTest()
{
	delete iEncryptor;
	delete iDecryptor;
}

CPerformanceTest::CPerformanceTest(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CCryptoTestAction(aFs, aConsole, aOut)
{}

void CPerformanceTest::DoPerformPrerequisiteL()
{
#ifndef _FOO
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 vector = Input::ParseElement(*iBody, KVectorStart, KVectorEnd, pos, err);

	DoInputParseL(vector);

	CBlockTransformation* encryptor = 0;
	CBlockTransformation* decryptor = 0;

	switch (iCipherType)
	{
		case (EDESECB):
		{//	Time the length of time it takes to set up the key schedule,
		//	save it and add to encrypt time, to be consistent with old API
		//	for comparison purposes (old API does both set up and encrypt/decrypt
		//	in a single operation).
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);
			
			encryptor = CDESEncryptor::NewL(iKey->Des());
			iEncryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				encryptor->Reset();
				iEncryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			delete encryptor;
			
			decryptor = CDESDecryptor::NewL(iKey->Des());
			
			diff = 0;
			iDecryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			delete decryptor;

			encryptor = CDESEncryptor::NewLC(iKey->Des());
			decryptor = CDESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case(EDESCBC):
		{
			CBlockTransformation* desEncryptor = NULL;
			CBlockTransformation* desDecryptor = NULL;

			desEncryptor = CDESEncryptor::NewLC(iKey->Des());								
			encryptor = CModeCBCEncryptor::NewL(desEncryptor, iIV->Des());				
			
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			startTime.UniversalTime();
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				encryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			CleanupStack::Pop(1);
			delete encryptor;

			desDecryptor = CDESDecryptor::NewLC(iKey->Des());
			decryptor = CModeCBCDecryptor::NewL(desDecryptor, iIV->Des());
			
			diff = 0;
			iDecryptIterations = 0;
			startTime.UniversalTime();
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			CleanupStack::Pop(1);
			delete decryptor;				

			desEncryptor = CDESEncryptor::NewLC(iKey->Des());
			desDecryptor = CDESDecryptor::NewLC(iKey->Des());
			
			encryptor = CModeCBCEncryptor::NewL(desEncryptor, iIV->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CModeCBCDecryptor::NewL(desDecryptor, iIV->Des());					
			CleanupStack::Pop(3);
		}
		break;
		case (E3DESECB):
		{
			encryptor = C3DESEncryptor::NewL(iKey->Des());
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				encryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			delete encryptor;

			diff = 0;
			iDecryptIterations = 0;
			decryptor = C3DESDecryptor::NewL(iKey->Des());
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			delete decryptor;

			encryptor = C3DESEncryptor::NewLC(iKey->Des());
			decryptor = C3DESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case (E3DESCBC):
		{
			CBlockTransformation* the3DESencryptor = NULL;
			CBlockTransformation* the3DESdecryptor = NULL;

			the3DESencryptor = C3DESEncryptor::NewL(iKey->Des());
			CleanupStack::PushL(the3DESencryptor);
			encryptor = CModeCBCEncryptor::NewL(the3DESencryptor, iIV->Des());

			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				encryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			CleanupStack::Pop(1);
			delete encryptor;				

			the3DESdecryptor = C3DESDecryptor::NewLC(iKey->Des());
			decryptor = CModeCBCDecryptor::NewL(the3DESdecryptor, iIV->Des());		

			diff = 0;
			iDecryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			CleanupStack::Pop(1);
			delete decryptor;

			the3DESencryptor = C3DESEncryptor::NewLC(iKey->Des());
			the3DESdecryptor = C3DESDecryptor::NewLC(iKey->Des());
			
			encryptor = CModeCBCEncryptor::NewL(the3DESencryptor, iIV->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CModeCBCDecryptor::NewL(the3DESdecryptor, iIV->Des());		
			CleanupStack::Pop(3);
		}
		break;
		case (EAESECB):
		{
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			encryptor = CAESEncryptor::NewL(iKey->Des());
			startTime.UniversalTime();
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				encryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			delete encryptor;

			diff = 0;
			iDecryptIterations = 0;
			decryptor = CAESDecryptor::NewL(iKey->Des());
			startTime.UniversalTime();
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			delete decryptor;
			
			encryptor = CAESEncryptor::NewLC(iKey->Des());
			decryptor = CAESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;	
		case (ERC2ECB):
		{
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			encryptor = CRC2Encryptor::NewL(iKey->Des(), iEffectiveKeyLen);
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				encryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			delete encryptor;
			
			diff = 0;
			decryptor = CRC2Decryptor::NewL(iKey->Des(), iEffectiveKeyLen);
			iDecryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			delete decryptor;

			encryptor = CRC2Encryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			decryptor = CRC2Decryptor::NewL(iKey->Des(), iEffectiveKeyLen);
			CleanupStack::Pop(encryptor);
		}
		break;
		case (ERC2CBC):
		{
			CBlockTransformation* theRC2encryptor = NULL;
			CBlockTransformation* theRC2decryptor = NULL;

			theRC2encryptor = CRC2Encryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			encryptor = CModeCBCEncryptor::NewL(theRC2encryptor, iIV->Des());

			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			startTime.UniversalTime();
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				encryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			CleanupStack::Pop(1);
			delete encryptor;

			diff = 0;
			iDecryptIterations = 0;
			theRC2decryptor = CRC2Decryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			decryptor = CModeCBCDecryptor::NewL(theRC2decryptor, iIV->Des());		
			startTime.UniversalTime();
			while (diff < iterationTime)
				{
				decryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			CleanupStack::Pop(1);
			delete decryptor;

			theRC2encryptor = CRC2Encryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			theRC2decryptor = CRC2Decryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			
			encryptor = CModeCBCEncryptor::NewL(theRC2encryptor, iIV->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CModeCBCDecryptor::NewL(theRC2decryptor, iIV->Des());		
			CleanupStack::Pop(3);
		}
		break;
		case (ERC4):
		{
			iEncryptor = CARC4::NewL(*iKey);
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				iEncryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);

			diff = 0;			
			iDecryptIterations = 0;
			iDecryptor = CARC4::NewL(*iKey);
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iDecryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
		}
		break;
		case (ECipherNull):
		{
			iEncryptor = CNullCipher::NewL();
			TTime startTime;
			TTime endTime;
			TTimeIntervalSeconds diff(0);
			const TTimeIntervalSeconds iterationTime(iIterationTime);

			iEncryptIterations = 0;
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iEncryptIterations++;
				iEncryptor->Reset();
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iEncryptorCreateTime = endTime.MicroSecondsFrom(startTime);
			
			diff = 0;
			iDecryptIterations = 0;
			iDecryptor = CNullCipher::NewL();
			startTime.UniversalTime();	
			while (diff < iterationTime)
				{
				iDecryptor->Reset();
				iDecryptIterations++;
				endTime.UniversalTime();
				endTime.SecondsFrom(startTime, diff);
				}
			endTime.UniversalTime();
			iDecryptorCreateTime = endTime.MicroSecondsFrom(startTime);
		}
		break;
			
		default:
		{
			ASSERT(0);
			User::Leave(KErrNotSupported);
		}
	}
	
	if( encryptor && decryptor )
		{
		CleanupStack::PushL(encryptor);
		CleanupStack::PushL(decryptor);
		
		CPaddingSSLv3* ePadding = CPaddingSSLv3::NewLC(encryptor->BlockSize());
		iEncryptor = CBufferedEncryptor::NewL(encryptor, ePadding);	
		CleanupStack::Pop(ePadding);	
		
		CPaddingSSLv3* dPadding = CPaddingSSLv3::NewLC(decryptor->BlockSize());
		iDecryptor = CBufferedDecryptor::NewL(decryptor, dPadding);
		CleanupStack::Pop(dPadding);
		CleanupStack::Pop(decryptor);
		CleanupStack::Pop(encryptor);
		}

	// clear the default input and output fields and fill with random data
	// since for performance testing we do not care about validating
	// correct output
	delete iInput;	
	iInput = NULL;
	iInput = HBufC8::NewMaxL(iRandDataSize);
	TPtr8 tempPtr = iInput->Des();
	TRandom::Random(tempPtr);	
	TBuf<128> tempbuf;
	//tempbuf.Format(KDataSize, (iInput->Length()));
	iOut.writeString(tempbuf);	
	delete iOutput;
	iOutput = NULL;
	iOutput = HBufC8::NewMaxL(iRandDataSize);
	TPtr8 tempPtr2 = iOutput->Des();
	TRandom::Random(tempPtr2);	
		
	iEResult = HBufC8::NewMaxL(iEncryptor->MaxOutputLength(iInput->Length()));
	iDResult = HBufC8::NewMaxL(iDecryptor->MaxOutputLength(iEResult->Size()));
#endif
}

void CPerformanceTest::DoPerformActionL()
{
	iResult = ETrue;
#ifndef _FOO
	TBuf<128> buf;
	TReal rate = I64REAL(iEncryptorCreateTime.Int64()) / iEncryptIterations;
	buf.Format(KPerfEncryptorCreate, rate, iEncryptIterations, iEncryptorCreateTime.Int64());
	iOut.writeString(buf);

	TTime startTime;
	TTime endTime;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds iterationTime(iIterationTime);
	
	iEncryptIterations = 0;
	startTime.UniversalTime();
	while (diff < iterationTime)
		{
		TPtr8 iEResultTemp(iEResult->Des());
		iEResultTemp.Zero();

		iEncryptor->Process(*iInput, iEResultTemp);
		iEncryptIterations++;
		endTime.UniversalTime();
		endTime.SecondsFrom(startTime, diff);
		}
	endTime.UniversalTime();

	TTimeIntervalMicroSeconds time = endTime.MicroSecondsFrom(startTime);
	rate = I64REAL(time.Int64()) / iEncryptIterations;
	buf.Format(KPerfEFormat, rate, iEncryptIterations, time.Int64());
	iOut.writeString(buf);

	rate = (static_cast<TReal>(iEncryptIterations) * iInput->Size() * 1000 * 1000) / (I64REAL(time.Int64()) * 1024 * 1024);	// Throughput in MB/s
	buf.FillZ();
	buf.Zero();
	buf.Format(KPerfThroughputAfterSetup, rate);
	iOut.writeString(buf);

	TInt64 totalEncryptTime = time.Int64();
	totalEncryptTime+=iEncryptorCreateTime.Int64();
	rate = I64REAL(totalEncryptTime) / iEncryptIterations;
	buf.FillZ();
	buf.Zero();
	buf.Format(KTotalFormat, rate, iEncryptIterations, totalEncryptTime);
	iOut.writeString(buf);
	rate = (static_cast<TReal>(iEncryptIterations) * iInput->Size() * 1000 * 1000) / (I64REAL(totalEncryptTime) * 1024 * 1024);	// Throughput in MB/s
	buf.FillZ();
	buf.Zero();
	buf.Format(KPerfThroughput, rate);
	iOut.writeString(buf);

//	Test decryption
	rate = I64REAL(iDecryptorCreateTime.Int64()) / iDecryptIterations;
	buf.Format(KPerfDecryptorCreate, rate, iDecryptIterations, iDecryptorCreateTime.Int64());
	iOut.writeString(buf);
	
	diff = 0;
	iDecryptIterations = 0;
	startTime.UniversalTime();	
	while (diff < iterationTime)
		{
		TPtr8 iDResultTemp(iDResult->Des());
		iDResultTemp.Zero();
		iDecryptor->Process(*iOutput, iDResultTemp);
		iDecryptIterations++;
		endTime.UniversalTime();
		endTime.SecondsFrom(startTime, diff);
		}
	endTime.UniversalTime();

	time = endTime.MicroSecondsFrom(startTime);	
	rate = I64REAL(time.Int64()) / iDecryptIterations;
	buf.FillZ();
	buf.Zero();
	buf.Format(KPerfDFormat, rate, iDecryptIterations, time.Int64());
	iOut.writeString(buf);

	rate = (static_cast<TReal>(iDecryptIterations) * iInput->Size() * 1000 * 1000) / (I64REAL(time.Int64()) * 1024 * 1024);	// Throughput in MB/s
	buf.FillZ();
	buf.Zero();
	buf.Format(KPerfThroughputAfterSetup, rate);
	iOut.writeString(buf);

	TInt64 totalDecryptTime = time.Int64();
	totalDecryptTime+=iDecryptorCreateTime.Int64();
	rate = I64REAL(totalDecryptTime) / iDecryptIterations;
	buf.FillZ();
	buf.Zero();
	buf.Format(KTotalFormat, rate, iDecryptIterations, totalDecryptTime);
	iOut.writeString(buf);
	
	rate = (static_cast<TReal>(iDecryptIterations) * iInput->Size() * 1000 * 1000) / (I64REAL(totalDecryptTime) * 1024 * 1024);	// Throughput in MB/s
	buf.FillZ();
	buf.Zero();
	buf.Format(KPerfThroughput, rate);
	iOut.writeString(buf);
	
#endif
}

