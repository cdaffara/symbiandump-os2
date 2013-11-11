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
#include "t_input.h"
#include "performancetest.h"

enum TActionMode {EMode1 = 1, EMode2 = 2, EMode3 = 3, EMode4 = 4};

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
	delete iX;

	if (iActionMode != EMode2)
		{
		iX1.Close();
		iX2.Close();
		}
	
	if (iActionMode == EMode3)
		{
		delete iNd;
		delete iGd;
		iN2.Close();
		iG2.Close();
		}
	
	 if (iActionMode == EMode4)
		 {
		 iN1.Close();
		 iG1.Close();
		 iN2.Close();
		 iG2.Close();
		 }
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

	iActionMode = Input::ParseIntElement(aTestActionSpec.iActionBody, _L8("<actionmode>"), _L8("</actionmode>"));
	
    iN = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<n>"));
    iG = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<g>"));
    
    if(iActionMode == EMode2 || iActionMode == EMode4)
    	{
    	iX = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<x>"));
    	}
    else if(iActionMode == EMode3)
    	{
    	iNd = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<nd>"));
    	iGd = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<gd>"));
    	}
	}

void CDHVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iN1 = RInteger::NewL(*iN);
	iN2 = RInteger::NewL(*iN);
	iG1 = RInteger::NewL(*iG);
	iG2 = RInteger::NewL(*iG);
	
	if(iActionMode == EMode2 || iActionMode == EMode4)
		{
		iX1 = RInteger::NewL(*iX);
		iX2 = RInteger::NewL(*iX);
		}	
	else if(iActionMode == EMode3)
		{
		iN3 = RInteger::NewL(*iNd);
		iG3 = RInteger::NewL(*iGd);
		}
	
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
		_LIT(KEncryptTime, "\tKey Agreements\t%f");
		buf.Format(KEncryptTime, rate);
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

	switch (iActionMode)
		{
		// Test for successful keypair generation with N and G
		case EMode1:
			{
			CDHKeyPair* keyPair1 = CDHKeyPair::NewL(iN1, iG1);
			CleanupStack::PushL(keyPair1);
			CDHKeyPair* keyPair2 = CDHKeyPair::NewLC(iN2, iG2);
			CDH* dh1 = CDH::NewL(keyPair1->PrivateKey());
			CleanupStack::PushL(dh1);
			CDH* dh2 = CDH::NewLC(keyPair2->PrivateKey());
			const HBufC8* key1 = dh1->AgreeL(keyPair2->PublicKey());
			CleanupStack::PushL(const_cast<HBufC8*>(key1));
			const HBufC8* key2 = dh2->AgreeL(keyPair1->PublicKey());
			CleanupStack::PushL(const_cast<HBufC8*>(key2));
			if(*key1 != *key2)
				{
				iResult = EFalse;
				}			
			CleanupStack::PopAndDestroy(2, const_cast<HBufC8*>(key1));	
			CleanupStack::PopAndDestroy(4, keyPair1);
			}
		break;
		
		// Test for successful keypair generation with N , G and X
		case EMode2:
			{
			CDHKeyPair* keyPair1 = CDHKeyPair::NewL(iN1, iG1, iX1);
			CleanupStack::PushL(keyPair1);
			CDHKeyPair* keyPair2 = CDHKeyPair::NewLC(iN2, iG2, iX2);
			CDH* dh1 = CDH::NewL(keyPair1->PrivateKey());
			CleanupStack::PushL(dh1);
			CDH* dh2 = CDH::NewLC(keyPair2->PrivateKey());
			const HBufC8* key1 = dh1->AgreeL(keyPair2->PublicKey());
			CleanupStack::PushL(const_cast<HBufC8*>(key1));
			const HBufC8* key2 = dh2->AgreeL(keyPair1->PublicKey());
			CleanupStack::PushL(const_cast<HBufC8*>(key2));
			if(*key1 != *key2)
				{
				iResult = EFalse;
				}			
			CleanupStack::PopAndDestroy(2, const_cast<HBufC8*>(key1));	
			CleanupStack::PopAndDestroy(4, keyPair1);
			}
			break;
		
		// Test for mis matching N & G key pair. 
		case EMode3:
			{
			CDHKeyPair* keyPair1 = CDHKeyPair::NewLC(iN1, iG1);			
			CDHKeyPair* keyPair2 = CDHKeyPair::NewLC(iN3, iG3);
			CDH* dh1 = CDH::NewLC(keyPair1->PrivateKey());
			HBufC8* key1 = NULL;
			TRAPD(err, key1 = dh1->AgreeL(keyPair2->PublicKey()));
			if (err != KErrArgument)
				{
				iResult = EFalse;
				delete key1;
				}
			CleanupStack::PopAndDestroy(3, keyPair1);
			}
			break;

		// Test for wromg values of G and X
		case EMode4:
			{
			CDHKeyPair* keyPair1 = NULL;			
			TRAPD(err, keyPair1 = CDHKeyPair::NewLC(iN1, iG1, iX1));
			if (err != KErrArgument)
				{
				iResult = EFalse;
				delete keyPair1;
				}			
			}
			break;

		default:
			break;			
		}
	__UHEAP_MARKEND;
	}


