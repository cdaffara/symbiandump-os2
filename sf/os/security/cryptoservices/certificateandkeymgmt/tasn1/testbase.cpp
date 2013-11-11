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
* Defines methods common to all test classes
*
*/


#include <e32cons.h>
#include "testbase.h"
#include "t_input.h"

_LIT(KParametersStart, "<parameters>");
_LIT(KValuesStart, "<values>");
_LIT(KRandomStart, "<random>");
const TInt KMaxValuesSize = 1024;


CTestBase::CTestBase(CASN1NormalTest &aASN1Action)
: iASN1Action(aASN1Action)
	{
	}

CTestBase::~CTestBase()
	{
	iParameters->Close();
	delete iParameters;
	iValues->ResetAndDestroy();
	delete iValues;
	};

CTestAction::TScriptError CTestBase::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::TScriptError syntaxError;
	iParameters = new (ELeave)RArray<CTestParameter::TType>;
	iValues = new (ELeave)RPointerArray<CTestParameter>;
	FillParameterArray();

	HBufC* aBody = HBufC::NewLC(aTestActionSpec.iActionBody.Length());
	
	aBody->Des().Copy(aTestActionSpec.iActionBody);
	TPtrC parameters(Input::ParseElement(*aBody, KParametersStart));

	syntaxError = CheckValueParametersL(parameters);
	if(syntaxError==CTestAction::ENone)
		syntaxError = CheckRandomParametersL(parameters);

	CleanupStack::PopAndDestroy();
	return(syntaxError);
	}


CTestAction::TScriptError CTestBase::CheckValueParametersL(const TDesC& aParameters)
	{
	TInt dummyPos = 0, valuePos = 0, lastDummy = 0;
	TBuf<KMaxValueSize> singleValue;
	CTestParameter *testParameter=0;
	TBuf<KMaxValuesSize> values;

	// finds values to run test with
	do
		{
		lastDummy = dummyPos;
		values = Input::ParseElement(aParameters, KValuesStart, dummyPos);

		// finds the next group of value
		if(lastDummy != dummyPos)
			{
			// parses each value in group with its expeced type
			for(TInt parameter = 0; parameter < iParameters->Count(); parameter++)
				{
				testParameter = NULL;
				// checks if its last parameter
				if(parameter < iParameters->Count() - 1)
					{
					// finds it position
					valuePos = values.Find((_L(",")));
					if(valuePos > 0)
						{
						// fills singleValue with value
						if(valuePos > KMaxValueSize)
							{
							iSyntaxErrorDescription.Copy(_L("value too long"));
							return(CTestAction::ESyntax);
							}
						singleValue.Copy(values.Left(valuePos));
						values = values.Mid(valuePos + 1);
						}
					else
						{
						// couldnt find it there is a problem
						iSyntaxErrorDescription.Copy(_L("Invalid number of parameters"));
						return(CTestAction::ESyntax);
						};
					}
				else
					{
					// its the last value fill singleValue with it
					if(values.Length() > KMaxValueSize)
						{
						iSyntaxErrorDescription.Copy(_L("value too long"));
						return(CTestAction::ESyntax);
						}
					singleValue.Copy(values);
					};

				// trims any white space
				singleValue.Trim();
				// checks what type its supposed to be
				switch((*iParameters)[parameter])
					{
					case CTestParameter::EInt:
						{
							// its an int, check to see if its a range
							if(singleValue.Find(_L("..")) == KErrNotFound)
								testParameter = CIntTestParameter::NewL(singleValue);
							else
								testParameter = CIntRangeTestParameter::NewL(singleValue);
							break;
						}
					case CTestParameter::EString:
						{
							// its a string
							testParameter = CStringTestParameter::NewL(singleValue);
							break;
						}
					case CTestParameter::EIntRange:
					case CTestParameter::ERandom:
						break;	//	Nothing to do
					};
				// if testparamer is found add it to the list
				if(testParameter)
					{	
					if(testParameter->iValid)
						iValues->Append(testParameter);
					else
						{
						delete testParameter;
						iSyntaxErrorDescription.Copy(_L("Invalid value "));
						iSyntaxErrorDescription.Append(singleValue);
						return(CTestAction::ESyntax);
						}
					}
				};
			}
		}
	while(lastDummy != dummyPos);

	return(CTestAction::ENone);
	};

CTestAction::TScriptError CTestBase::CheckRandomParametersL(const TDesC& aParameters)
	{
	TInt dummyPos = 0, lastDummy = 0;
	TBuf<KMaxValueSize> singleValue;
	CTestParameter *testParameter=0;
	TBuf<KMaxValuesSize> values;

	// finds any random tests
	do
		{
		lastDummy = dummyPos;
		values = Input::ParseElement(aParameters, KRandomStart, dummyPos);

		if(lastDummy != dummyPos)
			{
			if(values.Length() > KMaxValueSize)
				{
				iSyntaxErrorDescription.Copy(_L("value too long"));
				return(CTestAction::ESyntax);
				}
			singleValue.Copy(values);
			// found one create a random parameter with its interators
			testParameter = CRandomTestParameter::NewL(singleValue);
			// adds it to list
			if(testParameter)
				{	
				if(testParameter->iValid)
					iValues->Append(testParameter);
				else
					{
					delete testParameter;
					iSyntaxErrorDescription.Copy(_L("Invalid value "));
					iSyntaxErrorDescription.Append(singleValue);
					return(CTestAction::ESyntax);
					}
				}
			}
		}
	while(lastDummy != dummyPos);

	return(CTestAction::ENone);
	};

void CTestBase::OutputEncodingL(CConsoleBase& aConsole, TDesC8& aData)
	{
	aConsole.Printf(_L("Encoding: Length = "));
	TBuf<20> bits;
	bits.AppendNum(aData.Length());
	bits.Append(_L(", data = "));
	aConsole.Printf(bits);
	
	TInt size = aData.Length();
	for (TInt i = 0; i < size; ++i)
		{
		TBuf<10> tbuf;
		tbuf.AppendNumFixedWidth(aData[i], EHex, 2);
		aConsole.Printf(tbuf);
		}

	aConsole.Printf(_L("\n"));
	}

TBool CTestBase::CountTests(TInt &totalTests)
	{
	CTestParameter* test;
	TInt totalRandomTests = 0;
	TInt totalParameters = 0;
	TInt totalRangeTests = 0;

	totalTests=0;

	// counts number of tests to do for type
	for(TInt pos = 0; pos < iValues->Count(); pos++)
		{
		test = (*iValues)[pos];
		switch(test->GetType())
			{
			case CTestParameter::EInt :
			case CTestParameter::EString :
				{
				totalTests++;
				totalParameters++;
				break;
				}
			case CTestParameter::EIntRange :
				{
				CIntRangeTestParameter *rangeInt = REINTERPRET_CAST(CIntRangeTestParameter*, test);
				totalRangeTests += rangeInt->Range();
				totalParameters++;
				break;
				}
			case CTestParameter::ERandom :
				{
				CRandomTestParameter *randomInt = REINTERPRET_CAST(CRandomTestParameter*, test);
				
				totalRandomTests+= randomInt->Interations();
				break;
				}
			};
		};

	// only count each test once, remember each test can only have one parameter field of 
	// range or random
	totalTests = totalRandomTests + totalRangeTests + (totalParameters / iParameters->Count());
	// checks if tests correct number of parameters for type
	return((totalParameters %  iParameters->Count()) == 0);
	};

