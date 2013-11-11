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
* Implementation for testing generalized time object encoding/decoding
*
*/


#include "testgeneralizedtime.h"
#include "tasn1normaltest.h"
#include <asn1enc.h>
#include <asn1dec.h>

#include <e32cons.h>
#include <e32math.h>


CTestGeneralizedTime* CTestGeneralizedTime::NewL(CASN1NormalTest &aASN1Action)
	{
	CTestGeneralizedTime* test = new (ELeave) CTestGeneralizedTime(aASN1Action);
	return test;
	}

CTestGeneralizedTime::CTestGeneralizedTime(CASN1NormalTest &aASN1Action) : CTestBase(aASN1Action) 
	{
	};


void CTestGeneralizedTime::GetName(TDes& aBuf)
	{
	aBuf.Copy(_L("Test Generalized Time"));
	}


void CTestGeneralizedTime::FillParameterArray(void)
	{
	iParameters->Append(CTestParameter::EString);
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EInt);
	iParameters->Append(CTestParameter::EString);
	iParameters->Append(CTestParameter::EString);
	}

TBool CTestGeneralizedTime::ValidateOutcome(CConsoleBase& aConsole, const TTime& originalTime, const TTime& decodedTime, const TInt &aTestNumber, 
		const TInt &aTotalTests)
	{
	if (originalTime != decodedTime)
		{
		aConsole.Write(_L("ERROR!\nEncoded time = "));
		OutputTime(aConsole, originalTime);
		aConsole.Write(_L("Decoded time = "));
		OutputTime(aConsole, decodedTime);
		aConsole.Write(_L("\n"));
		iASN1Action.ReportProgressL(KErrASN1EncodingError, aTestNumber, aTotalTests);
		return(EFalse);
		}
	else
		{
		iASN1Action.ReportProgressL(KErrNone, aTestNumber, aTotalTests);
		return(ETrue);
		}
	}

TBool CTestGeneralizedTime::PerformTestWithEncoderL(CConsoleBase& aConsole, const TTime &aTest, const TInt &aTestNumber, 
		const TInt &aTotalTests)
	{
		TUint writeLength;
		TInt readLength;
		TTime decodedTime, originalTime;
	
		originalTime = aTest;
		CASN1EncGeneralizedTime* encoder = CASN1EncGeneralizedTime::NewLC(aTest);
			
		// Prepare a buffer
		TInt totalLength = encoder->LengthDER();
		HBufC8* buf = HBufC8::NewMaxLC(totalLength);
		TPtr8 tBuf = buf->Des();
			
		// Write into the buffer
		writeLength = 0;
		encoder->WriteDERL(tBuf, writeLength);
			
		// Read it out again + check lengths
		TASN1DecGeneralizedTime decoder;
		readLength = 0;
		decodedTime = decoder.DecodeDERL(tBuf, readLength);

		CleanupStack::PopAndDestroy(2); // buf, encoder

		if(writeLength != STATIC_CAST(TUint, readLength))
			return EFalse;
		
		return ValidateOutcome(aConsole, originalTime, decodedTime, aTestNumber, aTotalTests);
	}

TBool CTestGeneralizedTime::PerformTestWithoutEncoderL(CConsoleBase& aConsole, const TTime &aTest, const TInt &aTestNumber,
		const TInt &aTotalTests, const TDesC& fraction, const TDesC& offset)
	{
	
	TTime decodedTime, originalTime = aTest;
	
	
	//we construct the encoded time manually and pass it to decoder,
			
		_LIT(KTimeFormat, "%F%Y%M%D%H%T%S");
		TBuf<32> timeString;
		aTest.FormatL(timeString, KTimeFormat);
		TInt fracSec = 0, i = 0;
		if(fraction != KNullDesC)
			{
				timeString.Append('.');
				timeString.Append(fraction);
				TInt multiplier = 100000;
			
				// parse fraction of second
				while(i < fraction.Length())
					{
												
					fracSec += (fraction[i++]-'0') * multiplier;
					multiplier /= 10;
					
					}			
				TTimeIntervalMicroSeconds intervalMs = fracSec;
				originalTime += intervalMs;
			}
		if(offset != KNullDesC)
			{
				timeString.Append(offset);
				TTimeIntervalHours intervalHH = (offset[1]-'0') * 10 + (offset[2]-'0');
				TTimeIntervalMinutes intervalMM = (offset[3]-'0') * 10 + (offset[4]-'0');
				switch(offset[0])
				{
					case '+':
						originalTime -= intervalHH;
						originalTime -= intervalMM;
						break;
					case '-':
						originalTime += intervalHH;
						originalTime += intervalMM;
						break;
					default:
					// Malformed offset
						return EFalse;
				}
			}
				
		//convert to 8 bit
		TBuf8<32> timeString8bit;
		timeString8bit.Copy(timeString);
		
		//create a buf for storage of encoded time
		TInt totalLength = timeString8bit.Length() + 2;
		HBufC8* encodedTime = HBufC8::NewLC(totalLength);
		TPtr8 encodedTimePtr = encodedTime->Des();

		//construct an encoded time manually
		encodedTimePtr.Append(STATIC_CAST(TUint8, 0) | 0x1b); //add tag
		encodedTimePtr.Append(STATIC_CAST(TUint8, timeString8bit.Length())); //add length
		encodedTimePtr.Append(timeString8bit); // add content
			
		//Decode and read it out for later test on decoder
		TASN1DecGeneralizedTime decoder;
	
		TASN1DecGeneric gen(encodedTimePtr);
		gen.InitL();
		decodedTime = decoder.DecodeDERL(gen);

		CleanupStack::PopAndDestroy(encodedTime); // buf
						
		
		return ValidateOutcome(aConsole, originalTime, decodedTime, aTestNumber, aTotalTests);

	}


// Is there a built-in function to do this for us?
void CTestGeneralizedTime::OutputTime(CConsoleBase& aConsole, const TTime& aTime)
	{
	TDateTime dt(aTime.DateTime());
	aConsole.Printf(_L("Y = %d, M = %d, D = %d, H = %d, M = %d, S = %d, mS = %d\n"),
		dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
	}


TBool CTestGeneralizedTime::PerformTestsL(CConsoleBase& aConsole)
	{
	TInt nLow = Math::Random();
	TInt64 nHigh((TInt)Math::Random());
	TInt64 iSeed((nHigh << 32) + nLow);

	CTestParameter* test;
	TInt totalTests, currentTest=0;
	TInt year, month, day, hour, minute, second;
	TBuf<3> fracSecond;
	TBuf<5> offset;
	TBuf<15> typeOfTest;
	CStringTestParameter *testString;
	
	if(!CountTests(totalTests)) return(EFalse);

	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		typeOfTest.Zero();
		test = (*iValues)[pos];
				
		if(test->GetType() == CTestParameter::EString)
			{
				//get the type of generalizedTime we are testing
				testString = REINTERPRET_CAST(CStringTestParameter*, test);
				testString->GetValue(typeOfTest);
			
				// gets year
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt)
					return(EFalse);
				CIntTestParameter *testInt = REINTERPRET_CAST(CIntTestParameter*, test);
				year = testInt->Value();

				// gets month
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt) 
					return(EFalse);
				testInt = REINTERPRET_CAST(CIntTestParameter*, test);
				month = testInt->Value();

				// gets day
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt) 
					return(EFalse);
				testInt = REINTERPRET_CAST(CIntTestParameter*, test);
				day = testInt->Value();

				// gets hour
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt) 
					return(EFalse);
				testInt = REINTERPRET_CAST(CIntTestParameter*, test);
				hour = testInt->Value();

				// gets minute
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt) 
					return(EFalse);
				testInt = REINTERPRET_CAST(CIntTestParameter*, test);
				minute = testInt->Value();

				// gets second
				test = (*iValues)[++pos];
				if(test->GetType() != CTestParameter::EInt) 
					return(EFalse);
				testInt = REINTERPRET_CAST(CIntTestParameter*, test);
				second = testInt->Value();

				TDateTime dateTime(year, TMonth(month - 1), TMonth(day - 1), hour, minute, second, 0);
				TTime encodedTime = dateTime;
								
				if(typeOfTest == KEncoder)
				//we are testing with encoder as well as decoder
				//Since encoder only accepts TTime as input, we could only pass
				//in standard zulu time (asn.1 generalizedTime in 
				//second form without optional fraction of second and offset
				// hours and minutes)	
					{
						pos += 2; //ignore fraction of second and offset
						
						if(!PerformTestWithEncoderL(aConsole, encodedTime, currentTest, totalTests))
							return(EFalse);
							
					}
				else if(typeOfTest == KNoEncoder)	
				// Here we test decoding asn.1 generalized Time in any forms other than standard zulu,
				// with optional fraction of seconds and/or offset hours and minutes
					{
					// gets fraction of second
					
						test = (*iValues)[++pos];
						testString = REINTERPRET_CAST(CStringTestParameter*, test);
						testString->GetValue(fracSecond);
						
						// gets offset
						test = (*iValues)[++pos];
						testString = REINTERPRET_CAST(CStringTestParameter*, test);
						testString->GetValue(offset);
		
						if(!PerformTestWithoutEncoderL(aConsole, encodedTime, currentTest, totalTests, fracSecond, offset))
							return(EFalse);
							
					}
				else
					User::Leave(KErrNotSupported);
				currentTest++;
			}
		else if(test->GetType() == CTestParameter::ERandom)
			{
			CRandomTestParameter *randomInt = REINTERPRET_CAST(CRandomTestParameter*, test);

			for(TInt test = 0; test <= randomInt->Interations(); test++)
				{
				TInt nLow = Math::Rand(iSeed);
				TInt64 nHigh = Math::Rand(iSeed);
				TInt64 time64((nHigh << 32) + nLow);
				// Set to random time, but with 4-digit year and microseconds zero
				TTime encodedTime(time64);
				TDateTime dateTime(encodedTime.DateTime());
				dateTime.SetYear(dateTime.Year() % 10000);
				dateTime.SetMicroSecond(0);
				encodedTime = dateTime;
				if(!PerformTestWithEncoderL(aConsole, encodedTime, currentTest, totalTests))
					return(EFalse);
					;
				currentTest++;
				}
			}
		else
			return(EFalse);
		};

	

	iASN1Action.ReportProgressL(KErrNone, totalTests, totalTests);
	return(ETrue);
	};

