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


#include "tactionvector.h"
#include "symmetric.h"

CTestAction* CActionVector::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CActionVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
}

CTestAction* CActionVector::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
{
	CActionVector* self = new(ELeave) CActionVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionVector::~CActionVector()
{
	delete iEncryptor;
	delete iDecryptor;
}

CActionVector::CActionVector(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
:	CCryptoTestAction(aFs, aConsole, aOut)
{}


void CActionVector::DoPerformPrerequisiteL()
{
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 vector = Input::ParseElement(*iBody, KVectorStart, KVectorEnd, pos, err);

	DoInputParseL(vector);

	CBlockTransformation* encryptor = NULL;
	CBlockTransformation* decryptor = NULL;
	iEncryptor = 0;
	iDecryptor = 0;

	switch (iCipherType)
	{
		case (EDESECB):
		{
			encryptor = CDESEncryptor::NewLC(iKey->Des());
			decryptor = CDESDecryptor::NewL(iKey->Des());
			CleanupStack::Pop(encryptor);
		}
		break;
		case(EDESCBC):
		{
			CBlockTransformation* desEncryptor = NULL;
			CBlockTransformation* desDecryptor = NULL;

			desEncryptor = CDESEncryptor::NewLC(iKey->Des());
			desDecryptor = CDESDecryptor::NewLC(iKey->Des());
			
			encryptor = CModeCBCEncryptor::NewL(desEncryptor, iIV->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CModeCBCDecryptor::NewL(desDecryptor, iIV->Des());		

			CleanupStack::Pop(3, desEncryptor);
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

			CleanupStack::Pop(3, the3DESencryptor);
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
			CBlockTransformation* theRC2encryptor = NULL;
			CBlockTransformation* theRC2decryptor = NULL;

			theRC2encryptor = CRC2Encryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			theRC2decryptor = CRC2Decryptor::NewLC(iKey->Des(), iEffectiveKeyLen);
			
			encryptor = CModeCBCEncryptor::NewL(theRC2encryptor, iIV->Des());
			CleanupStack::PushL(encryptor);
			decryptor = CModeCBCDecryptor::NewL(theRC2decryptor, iIV->Des());		

			CleanupStack::Pop(3, theRC2encryptor);
		}
		break;
		case (ERC4):
		{
			iEncryptor = CARC4::NewL(*iKey,0);
			iDecryptor = CARC4::NewL(*iKey,0);
		}
		break;
		case (ECipherNull):
		{
			iEncryptor = CNullCipher::NewL();
			iDecryptor = CNullCipher::NewL();
		}
		break;
			
		default:
		{
			ASSERT(0);
			User::Leave(KErrNotSupported);
		}
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

	iEResult = HBufC8::NewMaxL(iEncryptor->MaxOutputLength(iInput->Length()));
	iDResult = HBufC8::NewMaxL(iDecryptor->MaxOutputLength(iEResult->Size()));

	CleanupStack::Pop(2, encryptor);
}

void CActionVector::DoPerformActionL()
{
//	First perform the test blockwise (ie passing in one block at a time)	
	TUint blockSize = iEncryptor->BlockSize();
	
	if((iInput->Size() % blockSize) != 0)
		{
		// input was not a multiple of the blocksize
		iResult = EFalse;
		return;
		}

	TUint index = 0;
	while (index <= (iInput->Size() - blockSize)) 
	{
		TPtr8 theEncryptResult((TUint8*)&(iEResult->operator[](index)), blockSize, blockSize);
		theEncryptResult.FillZ(theEncryptResult.MaxLength());
		theEncryptResult.SetLength(0);

		TPtrC8 theEncryptInput(iInput->Mid(index, blockSize));	
		iEncryptor->Process(theEncryptInput, theEncryptResult);

		if (iOutput->Mid(index, blockSize) == theEncryptResult)
			iResult = ETrue;
		else
			break;	//	No point doing any more
	
		index+=blockSize;
	}
	
	
	if (*iOutput==*iEResult)
	{
		iResult = ETrue;

		index = 0;
		while (index <= (iEResult->Size()- blockSize))
		{
			TPtr8 theDecryptResult((TUint8*)&(iDResult->operator[](index)), blockSize, blockSize);
			theDecryptResult.FillZ(theDecryptResult.MaxLength());
			theDecryptResult.SetLength(0);

			TPtrC8 theDecryptInput(iEResult->Mid(index, blockSize));
			iDecryptor->Process(theDecryptInput, theDecryptResult);
			
			if(iInput->Mid(index, blockSize) != theDecryptResult)
			{
				iResult = EFalse;
				break;	//	No point doing any more
			}
		
			index+=blockSize;
		}
	
		if (*iInput!=*iDResult)
		iResult = EFalse;	
	}
	
	
	iEncryptor->Reset();
	iDecryptor->Reset();
//	Now, if input is longer than a single block, repeat passing all the data
	if ((TUint)iInput->Size() > blockSize)
	{
		TPtr8 theEncryptResult(iEResult->Des());
		theEncryptResult.FillZ(theEncryptResult.MaxLength());
		theEncryptResult.SetLength(0);

		TPtrC8 theInput(*iInput);
		iEncryptor->Process(theInput, theEncryptResult);

		if (*iOutput!=*iEResult)
		{
			iResult = EFalse;
		}
		else
		{
			TPtr8 theDecryptResult(iDResult->Des());
			theDecryptResult.FillZ(theDecryptResult.MaxLength());
			theDecryptResult.SetLength(0);

			iDecryptor->Process(*iEResult, theDecryptResult);
			if (*iInput!=*iDResult)
				iResult = EFalse;
		}
	}
}

