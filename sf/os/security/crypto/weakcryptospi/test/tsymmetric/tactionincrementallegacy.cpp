/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tactionincrementallegacy.h"
#include "symmetric.h"
#include "des.h"
#include "securityerr.h"

CTestAction* CActionIncrementalLegacy::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionIncrementalLegacy::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionIncrementalLegacy::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionIncrementalLegacy* self = new(ELeave) CActionIncrementalLegacy(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionIncrementalLegacy::~CActionIncrementalLegacy()
{
}

CActionIncrementalLegacy::CActionIncrementalLegacy(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)						 
: CCryptoTestAction(aFs, aConsole, aOut)
{}

void CActionIncrementalLegacy::DoPerformPostrequisiteL()
{
if (iEncryptor)
	{
	iEncryptor->Reset();
	delete iEncryptor;
	}

if (iDecryptor)
	{
	iDecryptor->Reset();
	delete iDecryptor;
	}
}

void CActionIncrementalLegacy::DoPerformPrerequisiteL()
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
			encryptor = CDESEncryptor::NewL(iKey->Des());
			CleanupStack::PushL(encryptor);
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

			desEncryptor = CDESEncryptor::NewL(iKey->Des());
			CleanupStack::PushL(desEncryptor);
			desDecryptor = CDESDecryptor::NewL(iKey->Des());
			CleanupStack::PushL(desDecryptor);
			
			encryptor = CModeCBCEncryptor::NewL(desEncryptor, iIV->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CModeCBCDecryptor::NewLC(desDecryptor, iIV->Des());		
			CleanupStack::Pop(4);
		}
		break;
		case (E3DESECB):
		{
			encryptor = C3DESEncryptor::NewL(iKey->Des());
			CleanupStack::PushL(encryptor);
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
			decryptor = CModeCBCDecryptor::NewLC(the3DESdecryptor, iIV->Des());		
			CleanupStack::Pop(4);
		}
		break;
		case (EAESECB):
		{
			encryptor = CAESEncryptor::NewL(iKey->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CAESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case (ERC2ECB):
		{
			encryptor = CRC2Encryptor::NewL(iKey->Des(), iEffectiveKeyLen);
			CleanupStack::PushL(encryptor);
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
			iDecryptor = CARC4::NewLC(*iKey, 0); 
			CleanupStack::Pop(iDecryptor);
		}
		break;
		case (ECipherNull):
		{
			iEncryptor = CNullCipher::NewL(); 
			iDecryptor = CNullCipher::NewLC(); 
			CleanupStack::Pop(iDecryptor);		
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
				
		iEncryptor = CBufferedEncryptor::NewLC(encryptor, ePadding);
		CleanupStack::Pop(2, ePadding);	
		iDecryptor = CBufferedDecryptor::NewLC(decryptor, dPadding);
		CleanupStack::Pop(2, dPadding);
		
		TInt desEBlockSize = encryptor->BlockSize();
		TInt desDBlockSize = decryptor->BlockSize();
		TInt bufEBlockSize = iEncryptor->BlockSize();
		TInt bufDBlockSize = iDecryptor->BlockSize();
		ASSERT((desEBlockSize/desDBlockSize) == (bufEBlockSize/bufDBlockSize));
		
		TInt desEKeySize = encryptor->KeySize();
		TInt desDKeySize = decryptor->KeySize();
		ASSERT(desEKeySize == desDKeySize);
				
		encryptor->Reset();
		decryptor->Reset();
		}	
	
	TInt encryptorKeySize = iEncryptor->KeySize();
	TInt decryptorKeySize = iDecryptor->KeySize();
	ASSERT(encryptorKeySize == decryptorKeySize);
	 
	CleanupStack::Pop(2, encryptor);

	iEResult = HBufC8::NewMaxL(iEncryptor->MaxFinalOutputLength(iInput->Length()));
	iDResult = HBufC8::NewMaxL(iDecryptor->MaxFinalOutputLength(iEResult->Size()));
}

void CActionIncrementalLegacy::DoPerformActionL()
	{
	TRAPD(res, DoDoPerformActionL())
	if(res == KErrNoMemory)
		{
		iEncryptor->Reset();
		iDecryptor->Reset();
		}
	}

void CActionIncrementalLegacy::DoDoPerformActionL()
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

