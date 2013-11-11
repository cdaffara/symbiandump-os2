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
* tconstruction.cpp
*
*/


#include "tconstructionfb.h"
#include "t_input.h"
#include <bigint.h>
#include <random.h>

CTestAction* CConstructionFB::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CConstructionFB::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CConstructionFB::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CConstructionFB* self = new(ELeave) CConstructionFB(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CConstructionFB::~CConstructionFB()
	{
	}

CConstructionFB::CConstructionFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CConstructionFB::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CConstructionFB::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CConstructionFB::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CConstructionFB::DoReportAction(void)
	{
	}

void CConstructionFB::DoCheckResult(TInt)
	{
	}

void CConstructionFB::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = ETrue;

	//Generate 100 i*5 byte random sequences
	//This tests:
	// - NewLC(const TDesC8&)
	// - NewLC(const RInteger&)
	// - BufferLC()
	// By comparing the output of BufferLC we can have (some) assurance
	// that the internal integer code is intrepreting strings in the correct
	// order and endianess
	for(TUint i=0; i<100; i++)
		{ 
		HBufC8* buf = HBufC8::NewMaxLC(i*5);
		TPtr8 ptr = buf->Des();
		TRandom::RandomL(ptr);

		RInteger a = RInteger::NewL(ptr);
		CleanupStack::PushL(a);
		HBufC8* out = a.BufferLC();
		if( *out != *buf )
			{
			iResult = EFalse;
			}
		CleanupStack::PopAndDestroy(out);
		
		RInteger b = RInteger::NewL(a);
		CleanupStack::PushL(b);
		if( a != b )
			{
			iResult = EFalse;
			}
		out = b.BufferLC();
		if( *out != *buf )
			{
			iResult = EFalse;
			}
		CleanupStack::PopAndDestroy(out);
		CleanupStack::PopAndDestroy(); //b
		CleanupStack::PopAndDestroy();//a
		CleanupStack::PopAndDestroy(buf);
		}

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

