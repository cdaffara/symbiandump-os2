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


#include "tconstructionvector.h"
#include "t_input.h"
#include <bigint.h>

class RIntegerTest : public RInteger
	{
	public:
		RIntegerTest(void) {}
		RIntegerTest(const RInteger& aInteger) {iSize = *((TUint*)&aInteger); iPtr = *(((TUint*)(&aInteger)+1));	}
		TUint* Ptr(void) {return RInteger::Ptr();}
	};

CTestAction* CConstructionVector::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CConstructionVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CConstructionVector::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CConstructionVector* self = new(ELeave) CConstructionVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CConstructionVector::~CConstructionVector()
	{
	delete iBody;
	delete iString;
	delete iMemoryLayout;
	}

CConstructionVector::CConstructionVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CConstructionVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iString = Input::ParseElementHexL(*iBody, _L8("<input>"));
	iMemoryLayout = Input::ParseElementHexL(*iBody, _L8("<memorylayout>"));
	}

void CConstructionVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CConstructionVector::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CConstructionVector::DoReportAction(void)
	{
	}

void CConstructionVector::DoCheckResult(TInt)
	{
	}

void CConstructionVector::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = ETrue;

	//This is just a derived class to get around the fact that iReg is
	//protected in RInteger.  See top of file for definition
	RIntegerTest a;
	//a = (RIntegerTest)(RInteger::NewL(*iString));
	a = static_cast<RIntegerTest>(RIntegerTest::NewL(*iString));
	CleanupStack::PushL(a);
	
	//This test ensures that the internal memory layout of a number is correct
	TUint compare = Mem::Compare((TUint8*)(a.Ptr()), a.ByteCount(), 
		iMemoryLayout->Ptr(), iMemoryLayout->Size());
	if( compare != 0 )
		{
		iResult = EFalse;
		}

	HBufC8* out = a.BufferLC();
	if( *out != *iString )
		{
		iResult = EFalse;
		}
	CleanupStack::PopAndDestroy(out);
	CleanupStack::PopAndDestroy(&a);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

