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


#include "tactionincremental.h"
#include "symmetric.h"
#include "des.h"

CTestAction* CActionIncremental::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionIncremental::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionIncremental::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionIncremental* self = new(ELeave) CActionIncremental(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionIncremental::~CActionIncremental()
{
}

CActionIncremental::CActionIncremental(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)						 
: CCryptoTestAction(aFs, aConsole, aOut)
{}

void CActionIncremental::DoPerformPostrequisiteL()
{
	delete iEncryptor;
	delete iDecryptor;
}

void CActionIncremental::DoPerformPrerequisiteL()
{
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 incremental = Input::ParseElement(*iBody, KIncrementalStart, KIncrementalEnd, pos, err);

	DoInputParseL(incremental);

	CBlockTransformation* encryptor = NULL;
	CBlockTransformation* decryptor = NULL;

	iEncryptor =0;
	iDecryptor =0;

	switch (iCipherType)
	{
		case (EDESECB):
		{
			//If the test is weak key test
			if(iExpectedWeakResult == KErrWeakKey)
				{
				//we expect to leave with KErrWeakKey reason
				if(CDES::IsWeakKey(iKey->Des()))
					{
					User::Leave(KErrWeakKey);
					}
				else //test is unsuccessful, leave with a different reason
					{
					User::Leave(KErrGeneral);
					}	
				}
			encryptor = CDESEncryptor::NewLC(iKey->Des());
			decryptor = CDESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case (EDESCBC):
		{
			CBlockTransformation* desEncryptor = NULL;
			CBlockTransformation* desDecryptor = NULL;
			
			if(iExpectedWeakResult == KErrWeakKey)
				{
				if(CDES::IsWeakKey(iKey->Des()))
					{
					User::Leave(KErrWeakKey);
					}
				else
					{
					User::Leave(KErrGeneral);
					}	
				}
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
			encryptor = C3DESEncryptor::NewLC(iKey->Des());
			decryptor = C3DESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case (E3DESCBC):
		{
			CBlockTransformation* the3DESencryptor = NULL;
			CBlockTransformation* the3DESdecryptor = NULL;

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
			encryptor = CAESEncryptor::NewLC(iKey->Des());
			decryptor = CAESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case (ERC2ECB):
		{
			encryptor = CRC2Encryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			decryptor = CRC2Decryptor::NewL(iKey->Des(), iEffectiveKeyLen);
			CleanupStack::Pop(encryptor);
		}
		break;
		case (ERC2CBC):
		{
			encryptor = CRC2Encryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			decryptor = CRC2Decryptor::NewL(iKey->Des(), iEffectiveKeyLen);
			CleanupStack::Pop(encryptor);
		}
		break;
		case (ERC4):
		{
			iEncryptor = CARC4::NewL(*iKey, 0); 
			iDecryptor = CARC4::NewL(*iKey, 0); 
		}
		break;
		case (ECipherNull):
		{
			iEncryptor = CNullCipher::NewL(); 
			iDecryptor = CNullCipher::NewL(); 
		}
		break;	
		default:
			ASSERT(0);
			User::Leave(KErrNotSupported);
	}

	CleanupStack::PushL(encryptor);
	CleanupStack::PushL(decryptor);
	if(!iEncryptor && !iDecryptor)
		{
		CPaddingSSLv3* dPadding = CPaddingSSLv3::NewLC(decryptor->BlockSize());
		CPaddingSSLv3* ePadding = CPaddingSSLv3::NewLC(encryptor->BlockSize());
		iEncryptor = CBufferedEncryptor::NewL(encryptor, ePadding);
		CleanupStack::Pop(ePadding);
		iDecryptor = CBufferedDecryptor::NewL(decryptor, dPadding);
		CleanupStack::Pop(dPadding);
		}

	CleanupStack::Pop(2, encryptor);

	iEResult = HBufC8::NewMaxL(iEncryptor->MaxFinalOutputLength(iInput->Length()));
	iDResult = HBufC8::NewMaxL(iDecryptor->MaxFinalOutputLength(iEResult->Size()));
}

void CActionIncremental::DoPerformActionL()
	{
	TRAPD(res, DoDoPerformActionL())
	if(res == KErrNoMemory)
		{
		iEncryptor->Reset();
		iDecryptor->Reset();
		}
	}

void CActionIncremental::DoDoPerformActionL()
{
	__UHEAP_MARK;
	iResult = ETrue;
	TPtr8 eResultActual = iEResult->Des();
	eResultActual.FillZ(eResultActual.MaxLength());
	eResultActual.SetLength(0);

	TPtr8 dResultActual = iDResult->Des();
	dResultActual.FillZ(dResultActual.MaxLength());
	dResultActual.SetLength(0);

	TInt len = iInput->Length();
	for(TInt i=1; i<len; i++)
	{
		TInt j = 0;
		for(; j+i<len; j+=i)
		{//	encryption in blocks of size i
			iEncryptor->Process(iInput->Mid(j,i), eResultActual);
		}
	
		iEncryptor->ProcessFinalL(iInput->Mid(j), eResultActual);
		
		j=0;
		for(; (j+(len-i))<len; j+=(len-i))
		{//	Decryption in blocks of size (len - i)
			iDecryptor->Process(eResultActual.Mid(j, len-i), dResultActual);
		}
		
		iDecryptor->ProcessFinalL(eResultActual.Mid(j), dResultActual);
		
		if(dResultActual != *iInput)
		{
			iResult = EFalse;
		}
		
		eResultActual.FillZ(eResultActual.MaxLength());
		dResultActual.FillZ(dResultActual.MaxLength());
		eResultActual.SetLength(0);
		dResultActual.SetLength(0);
	}
	__UHEAP_MARKEND;

}

