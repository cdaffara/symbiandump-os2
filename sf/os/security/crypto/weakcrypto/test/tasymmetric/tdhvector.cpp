/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tdhvector.h"
#include <t_input.h>
#include "performancetest.h"

CTestAction* CDHVector::NewL(RFs& aFs,
                               CConsoleBase& aConsole,
                               Output& aOut, 
                               const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDHVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CDHVector::NewLC(RFs& aFs,
                                CConsoleBase& aConsole,
                                Output& aOut, 
                                const TTestActionSpec& aTestActionSpec)
	{
	CDHVector* self = new(ELeave) CDHVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDHVector::~CDHVector()
	{
	delete iN;
	delete iG;
	}

CDHVector::CDHVector(RFs& /*aFs*/, 
                                     CConsoleBase& aConsole,
                                     Output& aOut)					 
: CVectorTest(aConsole, aOut)
	{
	}

void CDHVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iN = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<n>"));
    iG = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<g>"));

	}


void CDHVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iN1 = RInteger::NewL(*iN);
	iN2 = RInteger::NewL(*iN);
	iG1 = RInteger::NewL(*iG);
	iG2 = RInteger::NewL(*iG);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CDHVector::DoPerformanceTestActionL()
	{
	iResult = ETrue;
	__UHEAP_MARK;

	CDHKeyPair* keyPair1 = CDHKeyPair::NewLC(iN1, iG1);
	CDHKeyPair* keyPair2 = CDHKeyPair::NewLC(iN2, iG2);
	CDH* dh1 = CDH::NewLC(keyPair1->PrivateKey());
	
	TTimeIntervalMicroSeconds agreeTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

	TInt index = 0;

	start.UniversalTime();

	while (diff < KIterationTime)
		{
		delete const_cast<HBufC8*>(dh1->AgreeL(keyPair2->PublicKey()));
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		index++;
		}
	end.UniversalTime();
	
	agreeTime = end.MicroSecondsFrom(start);
	
    CleanupStack::PopAndDestroy(dh1);
    CleanupStack::PopAndDestroy(keyPair2);
    CleanupStack::PopAndDestroy(keyPair1);

	__UHEAP_MARKEND;

	if (iResult)
		{
		TReal rate = I64REAL(agreeTime.Int64()) / index;
		TReal agreetime = I64REAL(agreeTime.Int64());
		TBuf<256> buf;
		_LIT(KEncryptTime, "\tKey Agreements: %f us/agreement (%i in %f us)\r\n");
		buf.Format(KEncryptTime, rate, index, agreetime);
		iOut.writeString(buf);
		}
	else
		{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
		}
}

void CDHVector::DoPerformActionL()
	{
	iResult = ETrue;
	__UHEAP_MARK;

	CDHKeyPair* keyPair1 = CDHKeyPair::NewLC(iN1, iG1);
	CDHKeyPair* keyPair2 = CDHKeyPair::NewLC(iN2, iG2);
	CDH* dh1 = CDH::NewLC(keyPair1->PrivateKey());
	CDH* dh2 = CDH::NewLC(keyPair2->PrivateKey());
	const HBufC8* key1 = dh1->AgreeL(keyPair2->PublicKey());
	CleanupStack::PushL(const_cast<HBufC8*>(key1));
	const HBufC8* key2 = dh2->AgreeL(keyPair1->PublicKey());

	if(*key1 != *key2)
		{
		iResult = EFalse;
		}

	delete const_cast<HBufC8*>(key2);
	CleanupStack::PopAndDestroy(const_cast<HBufC8*>(key1));
	CleanupStack::PopAndDestroy(dh2);
	CleanupStack::PopAndDestroy(dh1);
	CleanupStack::PopAndDestroy(keyPair2);
	CleanupStack::PopAndDestroy(keyPair1);

	__UHEAP_MARKEND;
	}


