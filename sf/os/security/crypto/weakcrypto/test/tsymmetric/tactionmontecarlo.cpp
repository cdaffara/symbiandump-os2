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


#include "tactionmontecarlo.h"
#include "bufferedtransformation.h"
#include "rijndael.h"
#include "cbcmode.h"
#include "padding.h"

const TInt KAESBlockSizeBytes = 16;	//	128 bits

CTestAction* CActionMonteCarlo::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionMonteCarlo::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionMonteCarlo::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionMonteCarlo* self = new(ELeave) CActionMonteCarlo(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionMonteCarlo::~CActionMonteCarlo()
{
	delete iEncrypt;
	delete iDecrypt;
}

CActionMonteCarlo::CActionMonteCarlo(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CCryptoTestAction(aFs, aConsole, aOut)
{}


void CActionMonteCarlo::DoPerformPrerequisiteL()
{
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 monteCarlo = Input::ParseElement(*iBody, KMonteCarloStart, KMonteCarloEnd, pos, err);

	DoInputParseL(monteCarlo);

	CBlockTransformation* encryptor = NULL;
	CBlockTransformation* decryptor = NULL;

	switch (iCipherType)
	{
		case (EAESMonteCarloEncryptECB):
		{
			encryptor = CAESEncryptor::NewLC(iKey->Des());
		}
		break;
		case (EAESMonteCarloDecryptECB):
		{
			decryptor = CAESDecryptor::NewLC(iKey->Des());
		}
		break;
		case (EAESMonteCarloEncryptCBC):
		{
			CBlockTransformation* aesEncryptor = NULL;		
			aesEncryptor = CAESEncryptor::NewLC(iKey->Des());
			
			encryptor = CModeCBCEncryptor::NewL(aesEncryptor, iIV->Des());
			CleanupStack::Pop(aesEncryptor);
			CleanupStack::PushL(encryptor);		
		}
		break;
		case (EAESMonteCarloDecryptCBC):
		{
			CBlockTransformation* aesDecryptor = NULL;		
			aesDecryptor = CAESDecryptor::NewLC(iKey->Des());
			
			decryptor = CModeCBCDecryptor::NewL(aesDecryptor, iIV->Des());
			CleanupStack::Pop(aesDecryptor);
			CleanupStack::PushL(decryptor);		
		}
		break;
		default:
		{
			ASSERT(0);
			User::Leave(KErrNotSupported);
		}
	}
	

	CPaddingSSLv3* padding = 0;
	if (encryptor)
		{
		padding = CPaddingSSLv3::NewLC(encryptor->BlockSize());
		iEncrypt = CBufferedEncryptor::NewL(encryptor, padding);	
		iEResult = HBufC8::NewMaxL(iEncrypt->MaxOutputLength(iInput->Length()));
		}
	else if (decryptor)
		{
		padding = CPaddingSSLv3::NewLC(decryptor->BlockSize());
		iDecrypt = CBufferedDecryptor::NewL(decryptor, padding);
		iDResult = HBufC8::NewMaxL(iDecrypt->MaxOutputLength(iInput->Size()));
		}

	CleanupStack::Pop(2);	//	padding, encryptor/decryptor

}


void CActionMonteCarlo::DoPerformActionL()
{
	iResult = EFalse;

	__ASSERT_DEBUG(iInput->Size()==KAESBlockSizeBytes, User::Panic(_L("tsymmetric"), KErrNotSupported));
	
	if (iCipherType==EAESMonteCarloEncryptECB)
		DoAESEncryptECB();
	else if (iCipherType==EAESMonteCarloDecryptECB)
		DoAESDecryptECB();	
	else if (iCipherType==EAESMonteCarloEncryptCBC)
		DoAESEncryptCBC();
	else if (iCipherType==EAESMonteCarloDecryptCBC)
		DoAESDecryptCBC();
	else
		User::Leave(KErrNotSupported);
}

void CActionMonteCarlo::DoAESEncryptECB()
{
	TPtr8 theEncryptResult(iEResult->Des());
	theEncryptResult.FillZ(theEncryptResult.MaxLength());
	theEncryptResult.SetLength(0);

	TInt index = 0;
	TPtr8 theInput(iInput->Des());
	for (; index < KMonteCarloIterations; index++)
	{
		iEncrypt->Process(theInput, theEncryptResult);	
		theInput.Copy(theEncryptResult);
		theEncryptResult.FillZ(theEncryptResult.MaxLength());
		theEncryptResult.SetLength(0);
	}
	
	if (*iOutput==*iEResult)
	{	
		iResult = ETrue;
	}
}

void CActionMonteCarlo::DoAESDecryptECB()
{
	TPtr8 theDecryptResult(iDResult->Des());
	theDecryptResult.FillZ(theDecryptResult.MaxLength());
	theDecryptResult.SetLength(0);

	TInt index = 0;
	TPtr8 theInput(iInput->Des());
	for (; index < KMonteCarloIterations; index++)
	{
		iDecrypt->Process(theInput, theDecryptResult);	
		theInput.Copy(theDecryptResult);
		theDecryptResult.FillZ(theDecryptResult.MaxLength());
		theDecryptResult.SetLength(0);
	}
	
	if (*iOutput==*iInput)
	{	
		iResult = ETrue;
	}
}

void CActionMonteCarlo::DoAESEncryptCBC()
    {	
	TPtr8 theEncryptResult(iEResult->Des());
	theEncryptResult.FillZ(theEncryptResult.MaxLength());
	theEncryptResult.SetLength(0);

	TInt index = 0;
	TPtr8 theInput(iInput->Des());

	TBuf8<KAESBlockSizeBytes> nextBuf;
	nextBuf.FillZ(KAESBlockSizeBytes);

    for (; index < KMonteCarloIterations-1; index++)
	    {
	    iEncrypt->Process(theInput, theEncryptResult);	

	    if (index==0)
		theInput.Copy(*iIV);	//	First loop, use the original IV as next PT block
		else	
		theInput.Copy(nextBuf);	//	Use previous CT block as next PT block
		
	    //	 Save CT block for next loop when it'll become the PT block	
		nextBuf.Copy(theEncryptResult);
	    //	 Reset for next encryption	
		theEncryptResult.FillZ(theEncryptResult.MaxLength());
		theEncryptResult.SetLength(0);
	    }
	
    iEncrypt->Process(theInput, theEncryptResult);	
	
	if  (theEncryptResult.Compare(*iOutput)==KErrNone)
	    {	
	    iResult = ETrue;
	    }

    }

void CActionMonteCarlo::DoAESDecryptCBC()
    {
	TPtr8 theDecryptResult(iDResult->Des());
	theDecryptResult.FillZ(theDecryptResult.MaxLength());
	theDecryptResult.SetLength(0);

	TInt index = 0;
    TPtr8 theInput(iInput->Des());

    for (; index < KMonteCarloIterations-1; index++)
	    {
	    iDecrypt->Process(theInput, theDecryptResult);	
		
		//	Use previous PT block as next CT block
	 	theInput.Copy(theDecryptResult);

	    //  Reset for next decryption	
		theDecryptResult.FillZ(theDecryptResult.MaxLength());
		theDecryptResult.SetLength(0);
	    }
	
    // Last loop	
    iDecrypt->Process(theInput, theDecryptResult);	

	if (theDecryptResult.Compare(*iOutput)==KErrNone)
 	   {	
	   iResult = ETrue;
	   }

    }
