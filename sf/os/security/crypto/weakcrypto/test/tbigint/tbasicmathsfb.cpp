/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tbasicmathsfb.h"
#include "t_input.h"
#include "t_output.h"
#include <bigint.h>
#include <random.h>

CTestAction* CBasicMathsFB::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CBasicMathsFB::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CBasicMathsFB::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CBasicMathsFB* self = new(ELeave) CBasicMathsFB(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CBasicMathsFB::~CBasicMathsFB()
	{
	delete iBody;
	}

CBasicMathsFB::CBasicMathsFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CBasicMathsFB::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	//HBufC8* length = Input::ParseElementHexL(*iBody, _L8("<bits>"));
	TUint bits = Input::ParseIntElement(*iBody, _L8("<bits>"), _L8("</bits>"));
	// the final /7 gives the number of times we have to increment by 7 to get
	// to that number of bytes and hence bits.
	iIterations = ((bits+7)/8)/7 + 1;
	}

void CBasicMathsFB::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CBasicMathsFB::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CBasicMathsFB::DoReportAction(void)
	{
	}

void CBasicMathsFB::DoCheckResult(TInt)
	{
	}

void CBasicMathsFB::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = ETrue;

	//min max values for NewRandomLC call
	RInteger min = RInteger::NewL(10);
	CleanupStack::PushL(min);
	RInteger max = RInteger::NewL(100);
	CleanupStack::PushL(max);

	//Generate iIterations*7 byte random sequences we are using 7 as it's a generator
	//mod 8.  Thus we'll cycle through every value (0-7) every 8 iterations.
	//This gives us a better feeling that certain byte lengths (and thus bit
	//lengths as the byte is chosen randomly) don't have errors.
	for(TUint i=1; i<iIterations; i++)
		{ 
		HBufC8* buf = HBufC8::NewMaxLC(i*7);
		TPtr8 ptr = buf->Des();
		TRandom::RandomL(ptr);

		//This is this iteration's random number
		RInteger initial = RInteger::NewL(ptr);
		CleanupStack::PushL(initial);

		//get a number x | 10 < x < 100
		RInteger crange = RInteger::NewRandomL(min, max);
		CleanupStack::PushL(crange);
		TUint range = crange.ConvertToLongL();
		CleanupStack::PopAndDestroy(); //crange

		AddSub(initial, range);
		MulDiv(initial, range);

		//GCD
		CleanupStack::PopAndDestroy(); //initial
		CleanupStack::PopAndDestroy();//buf
		iConsole.Printf(_L("."));
		}
	
	//Test a single iteration where the initial random number is less than a
	//word so the division and modulo routines that take words rather than
	//TIntegers can run.
	//do
		{
		//This is this iteration's random number
		RInteger initial = RInteger::NewRandomL(31);
		CleanupStack::PushL(initial);
		//get a number x | 10 < x < 100
		RInteger crange = RInteger::NewRandomL(min, max);
		CleanupStack::PushL(crange);
		TUint range = crange.ConvertToLongL();
		CleanupStack::PopAndDestroy(&crange); //crange

		AddSub(initial, range);
		MulDiv(initial, range);
		CleanupStack::PopAndDestroy(&initial); //initial
		iConsole.Printf(_L("."));
		} //while (0);

	CleanupStack::PopAndDestroy();//max
	CleanupStack::PopAndDestroy(); //min

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARK;
	}

void CBasicMathsFB::AddSub(const TInteger& aInitial, TUint aRange)
	{
	__UHEAP_MARK;
	//This is the copy we are going to do stuff to
	RInteger a = RInteger::NewL(aInitial);
	CleanupStack::PushL(a);

	// compute a*aRange using doubling
	TUint j=1;
	for(; j<aRange; j++)
		{
		a += aInitial;
		}

	//b = a*aRange;
	RInteger b = RInteger::NewL(a);
	CleanupStack::PushL(b);
	//compute (a*aRange)/aRange using subtraction
	for(j=1; j<aRange; j++)
		{
		b -= aInitial;
		}
	// b should be the same as the initial value
	if( b != aInitial )
		{
		iResult = EFalse;
		iOut.writeString(_L("AddSub Failure:"));
		iOut.writeNewLine();
		}

	RInteger c = RInteger::NewL(aInitial);
	CleanupStack::PushL(c);
	// compute a*aRange using normal multiplication
	c *= aRange;
	
	// c and a should now be the same
	if( c != a )
		{ 
		iResult = EFalse;
		}

	RInteger d = RInteger::NewL(a);
	CleanupStack::PushL(d);
	//compute (a*aRange)/aRange using normal division
	d /= aRange;
	if( d != aInitial )
		{
		iResult = EFalse;
		}
	RInteger e = RInteger::NewL(a);
	CleanupStack::PushL(e);
	e %= aRange;
	// (a*aRange)%aRange == 0
	if( e != 0 )
		{
		iResult = EFalse;
		}
	CleanupStack::PopAndDestroy(5); //e,d,c,b,a
	__UHEAP_MARKEND;
	}

void CBasicMathsFB::MulDiv(const TInteger& aInitial, TUint aRange)
	{
	__UHEAP_MARK;
	//This is the copy we are going to do stuff to
	RInteger a = RInteger::NewL(aInitial);
	CleanupStack::PushL(a);

	//compute a = aInitial^aRange using repeated multiplication
	TUint j=1;
	for(; j<aRange; j++)
		{
		a *= aInitial;
		}

	//b = a
	RInteger b = RInteger::NewL(a);
	CleanupStack::PushL(b);
	//try to find aInitial by repeatedly dividing b by aInitial aRange times
	for(j=1; j<aRange; j++)
		{
		TRAPD(res, b /= aInitial);
		//the first time through aInitial is 0 so this is expected
		if(res == KErrDivideByZero && aInitial.IsZero())
			{
			break;
			}
		else if(res == KErrDivideByZero && aInitial.NotZero())
			{
			iResult = EFalse;
			}
		else if(res != KErrNone)
			{
			User::Leave(res);
			}
		}
	// b should be the same as the initial value
	if( b != aInitial )
		{
		iResult = EFalse;
		}

	//tests division by something smaller than a word
	if(aInitial.WordCount() <= 1)
		{
		RInteger dividend = RInteger::NewL(a);
		CleanupStack::PushL(dividend);
		for(j=1; j<aRange; j++)
			{
			RInteger quotient;
			//try to find aInitial by repeatedly dividing dividend by aInitial aRange times 
			TRAPD(res, quotient = dividend.DividedByL(aInitial.ConvertToLongL()));
			//the first time through aInitial is 0 so this is expected
			if(res == KErrDivideByZero && aInitial.IsZero())
				{
				break;
				}
			else if(res == KErrDivideByZero && aInitial.NotZero())
				{
				iResult = EFalse;
				}
			else if(res != KErrNone)
				{
				User::Leave(res);
				}
			dividend.Set(quotient);
			}
		if( dividend != aInitial )
			{
			iResult = EFalse;
			}

		TUint remainder=1;
		TRAPD(res, remainder = a.ModuloL(aInitial.ConvertToLongL()));
		//the first time through aInitial is 0
		if(res != KErrDivideByZero && res != KErrNone)
			{
			User::Leave(res);
			}
		else if(res == KErrDivideByZero && aInitial.NotZero())
			{
			iResult = EFalse;
			}
		//else we have an expected divide by zero, ignore it.
		if(remainder != 0)
			{
			iResult = EFalse;
			}

		CleanupStack::PopAndDestroy(&dividend);
		}

	RInteger c = RInteger::NewL(aRange);
	CleanupStack::PushL(c);
	RInteger d = aInitial.ExponentiateL(c);
	CleanupStack::PushL(d);
	// c and a should now be the same
	if( d != a )
		{ 
		iResult = EFalse;
		}

	RInteger e = RInteger::NewL(a);
	CleanupStack::PushL(e);
	TRAPD(res, e %= aInitial);
	//the first time through aInitial is 0
	if(res != KErrDivideByZero && res != KErrNone)
		{
		User::Leave(res);
		}
	else if(res == KErrDivideByZero && aInitial.NotZero())
		{
		iResult = EFalse;
		}
	//else we have an expected divide by zero, ignore it.

	// (aInitial^aRange)%aInitial == 0
	if( e != 0 )
		{
		iResult = EFalse;
		}
	CleanupStack::PopAndDestroy(5);//e,d,c,b,a
	__UHEAP_MARKEND;
	}
