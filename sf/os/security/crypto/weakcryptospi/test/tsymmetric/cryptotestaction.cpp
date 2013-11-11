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


#include "cryptotestaction.h"
#include <securityerr.h>
#include <cryptostrength.h>
#include "../common/inlines.h"
#include "t_output.h"

CCryptoTestAction::CCryptoTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CTestAction(aConsole, aOut), iFs(aFs), iEffectiveKeyLen(128)
{}

CCryptoTestAction::~CCryptoTestAction()
{
	delete iBody;
}

void CCryptoTestAction::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);	
}

void CCryptoTestAction::DoPerformPrerequisite(TRequestStatus& aStatus)
{
	TRequestStatus* status = &aStatus;
	
	TRAPD(r, DoPerformPrerequisiteL());
	if ((r != iExpectedWeakResult) && (TCrypto::Strength() == TCrypto::EWeak))
		{
		r = KErrUnknown;
		}
	User::RequestComplete(status, r);
	if (KErrNone==r)
		{
		iActionState = CTestAction::EAction;
		}
	else if (r==KErrKeyNotWeakEnough)
		{
		iResult = ETrue;
		iConsole.Printf(_L("Crypto libraries returned KErrKeyNotWeakEnough!  Passing test automatically.\n\r"));
		iOut.writeString(_L("Crypto libraries returned KErrKeyNotWeakEnough!  Passing test automatically.\r\n"));
		iActionState = CTestAction::EPostrequisite;
		}
	else if (r==KErrWeakKey)
 		{
 		iResult = ETrue;
 		iConsole.Printf(_L("Crypto libraries returned KErrWeakKey!  Passing test automatically.\n\r"));
 		iOut.writeString(_L("Crypto libraries returned KErrWeakKey!  Passing test automatically.\r\n"));
 		iActionState = CTestAction::EPostrequisite;
 		}
	else
		{
		iActionState = CTestAction::EPostrequisite;
		}
}

void CCryptoTestAction::DoPerformPostrequisite(TRequestStatus& aStatus)
{
	TRequestStatus* status = &aStatus;
	TRAPD(r, DoPerformPostrequisiteL());
	delete iKey;
	delete iInput;
	delete iOutput;
	delete iIV;
	delete iEResult;
	delete iDResult;

	iFinished = ETrue;
	User::RequestComplete(status, r);
}

void CCryptoTestAction::DoReportAction(void)
{}

void CCryptoTestAction::DoCheckResult(TInt)
{
	if (iResult==EFalse)
		iConsole.Printf(_L("X"));
	else
		iConsole.Printf(_L("."));
}

void CCryptoTestAction::PerformAction(TRequestStatus& aStatus)
{
	TRequestStatus* status = &aStatus;

	TRAPD(res, DoPerformActionL());
	if (res==KErrNoMemory)
		{
		User::Leave(res);	//	For OOM testing
		}
	iActionState = CTestAction::EPostrequisite;
	User::RequestComplete(status, res);
}


void CCryptoTestAction::DoInputParseL(TDesC8& aScriptBuffer)
{
	TInt err = KErrNone;
	TInt pos = 0;

	TPtrC8 keyTemp = Input::ParseElement(aScriptBuffer, KKeyStart, KKeyEnd, pos=0, err);
	iKey = HBufC8::NewL(keyTemp.Length());
	*iKey = keyTemp;
	Hex(*iKey);

	TPtrC8 inputTemp = Input::ParseElement(aScriptBuffer, KInputStart, KInputEnd, pos=0, err);
	iInput = HBufC8::NewL(inputTemp.Length());
	*iInput = inputTemp;
	Hex(*iInput);

	TPtrC8 ivTemp = Input::ParseElement(aScriptBuffer, KIVStart, KIVEnd, pos=0, err);
	iIV = HBufC8::NewL(ivTemp.Length());
	*iIV = ivTemp;
	Hex(*iIV);

	TPtrC8 outputTemp = Input::ParseElement(aScriptBuffer, KOutputStart, KOutputEnd, pos=0, err);
	iOutput = HBufC8::NewL(outputTemp.Length());
	*iOutput = outputTemp;
	Hex(*iOutput);

	TPtrC8 cipherType = Input::ParseElement(aScriptBuffer, KCipherTypeStart, KCipherTypeEnd, pos=0, err);
	iExpectedWeakResult = KErrNone;
	if( cipherType == _L8("DESECB") )
	{
		iCipherType = EDESECB;
	}
	else if( cipherType == _L8("DESCBC") )
	{
		iCipherType = EDESCBC;
	}
	else if( cipherType == _L8("3DESECB") )
	{
		iCipherType = E3DESECB;
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if( cipherType == _L8("3DESCBC") )
	{
		iCipherType = E3DESCBC;
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if( cipherType == _L8("AESECB") )
	{
		iCipherType = EAESECB;
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if (cipherType==_L8("AESENC_ECB") )
	{
		iCipherType = EAESMonteCarloEncryptECB;	
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if (cipherType==_L8("AESDEC_ECB") )
	{
		iCipherType = EAESMonteCarloDecryptECB;	
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if (cipherType==_L8("AESENC_CBC") )
	{
		iCipherType = EAESMonteCarloEncryptCBC;	
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if (cipherType==_L8("AESDEC_CBC") )
	{
		iCipherType = EAESMonteCarloDecryptCBC;		
		iExpectedWeakResult = KErrKeyNotWeakEnough;
	}
	else if( cipherType == _L8("RC2ECB") )
	{
		iCipherType = ERC2ECB;
		// weak enough if either aKey or aEffectiveKeyLen is weak
		TInt minKeySize = Min(iEffectiveKeyLen, BytesToBits(iKey->Size()));
		TRAP(iExpectedWeakResult, TCrypto::IsSymmetricWeakEnoughL(minKeySize));
	}
	else if( cipherType == _L8("RC2CBC") )
	{
		iCipherType = ERC2CBC;
		// weak enough if either aKey or aEffectiveKeyLen is weak
		TInt minKeySize = Min(iEffectiveKeyLen, BytesToBits(iKey->Size()));
		TRAP(iExpectedWeakResult, TCrypto::IsSymmetricWeakEnoughL(minKeySize));
	}
	else if( cipherType == _L8("RC4") )
	{
		iCipherType = ERC4;
		TRAP(iExpectedWeakResult, TCrypto::IsSymmetricWeakEnoughL(BytesToBits(iKey->Size())));
	}
	else if( cipherType == _L8("NULL") )
	{
		iCipherType = ECipherNull;
	}
 	else if( cipherType == _L8("DESECB_WKT"))
 	{
 		iCipherType = EDESECB;	
 		iExpectedWeakResult = KErrWeakKey;
 	}
 	else if( cipherType == _L8("DESCBC_WKT"))
 	{
 		iCipherType = EDESCBC;	
 		iExpectedWeakResult = KErrWeakKey;
 	}
	else 
	{
		TBuf<64> cipherName(0);
		cipherName.Copy(cipherType);
		TBuf<256> formattable;
		formattable.Format(_L("\nBad Cipher Type: %S"), &cipherName);
		iConsole.Printf(formattable);

		iActionState = CTestAction::EPostrequisite;
		User::Leave(KErrNotSupported);
	}

	if ( (iCipherType==ERC2ECB) || (iCipherType==ERC2CBC) )
	{
		TPtrC8 effKeyLenTemp = Input::ParseElement(aScriptBuffer, KEffKeyLenStart, KEffKeyLenEnd, pos=0, err);

		if (effKeyLenTemp.Length() != 0)
		{
			TLex8 lex;
			lex.Assign(effKeyLenTemp);
			User::LeaveIfError(lex.Val(iEffectiveKeyLen));
		}
	}

	TPtrC8 iterationsTemp = Input::ParseElement(aScriptBuffer, KIterationsStart, KIterationsEnd, pos=0, err);	
	TLex8 lex;
	lex.Assign(iterationsTemp);
	err = lex.Val(iIterationTime);

	TPtrC8 randTemp = Input::ParseElement(aScriptBuffer, KRandDataSizeStart, KRandDataSizeEnd, pos=0, err);	
	TLex8 randlex;
	randlex.Assign(randTemp);
	err = randlex.Val(iRandDataSize);	

	delete iBody;
	iBody = NULL;
}

void CCryptoTestAction::Hex(HBufC8& aString)
    {
    TPtr8 ptr=aString.Des();
    if (aString.Length()%2)
        {
        ptr.SetLength(0);
        return;
        }
    TInt i;
    for (i=0;i<aString.Length();i+=2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
        ptr[i/2]=tmp;
        }
    ptr.SetLength(aString.Length()/2);
    }

