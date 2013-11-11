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


#include "tmontgomeryfb.h"
#include "t_input.h"
#include "t_output.h"
#include "tutils.h"
#include <bigint.h>
#include <random.h>
#include "../../source/bigint/mont.h"

CTestAction* CMontgomeryFB::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CMontgomeryFB::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CMontgomeryFB::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CMontgomeryFB* self = new(ELeave) CMontgomeryFB(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CMontgomeryFB::~CMontgomeryFB()
	{
	delete iBody;
	}

CMontgomeryFB::CMontgomeryFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CMontgomeryFB::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	HBufC8* length = Input::ParseElementHexL(*iBody, _L8("<bits>"));
	CleanupStack::PushL(length);
	RInteger clength = RInteger::NewL(*length);
	CleanupStack::PopAndDestroy(length);
	CleanupStack::PushL(clength);
	TUint bits = clength.ConvertToLongL();
	CleanupStack::PopAndDestroy();//clength
	// the final /7 gives the number of times we have to increment by 7 to get
	// to that number of bytes and hence bits.
	iIterations = ((bits+7)/8)/7 + 1;
	}

void CMontgomeryFB::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CMontgomeryFB::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CMontgomeryFB::DoReportAction(void)
	{
	}

void CMontgomeryFB::DoCheckResult(TInt)
	{
	}

void CMontgomeryFB::PerformAction(TRequestStatus& aStatus)
	{
	TRAPD(res, PerformActionL());
	__ASSERT_ALWAYS(!res, User::Panic(_L("CMontgomeryFB::PerformAction"), res));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}

void CMontgomeryFB::PerformActionL()
	{
	__UHEAP_MARK;
	
	iResult = ETrue;

	//Generate iIterations*7 byte random sequences we are using 7 as it's a
	//generator mod 8.  Thus we'll cycle through every value (0-7) every 8
	//iterations.  This gives us a better feeling that certain byte lengths
	//(and thus bit lengths as the byte is chosen randomly) don't have errors.
	for(TUint i=1; i<=iIterations; i++)
		{ 
		//generate a prime of roughly i*7*8 bits
		RInteger prime = RInteger::NewPrimeL(i*7*8);
		CleanupStack::PushL(prime);
		CMontgomeryStructure* mont = CMontgomeryStructure::NewLC(prime);
		
		//generate a random number of x | 2 < x < prime
		RInteger base = RInteger::NewRandomL(TInteger::Two(), prime);
		CleanupStack::PushL(base);
		
		//This is using Fermat's Little Theorem
		//  (base ^ prime) % prime == base or
		//  (base ^ prime-1) % prime == 1
		const TInteger& y = mont->ExponentiateL(base, prime);
		if( y != base )
			{
			iResult = EFalse;
			iConsole.Printf(_L("X"));
			iOut.writeString(_L("Failure exponentiating:"));
			iOut.writeNewLine();
			Utils::DumpInteger(iOut, _L("base: "), base);
			Utils::DumpInteger(iOut, _L("prime: "), prime);
			Utils::DumpInteger(iOut, _L("output: "), (const RInteger&)y);
			Utils::DumpInteger(iOut, _L("expected: "), base);
			}

		CleanupStack::PopAndDestroy(3, &prime);//base,mont,prime

		iConsole.Printf(_L("."));
		}

	__UHEAP_MARKEND;
	}

