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


#include "tactionderivekey.h"
#include "t_input.h"
#include <pkcs5kdf.h>
#include <pkcs12kdf.h>
#include <stdlib.h>

_LIT8(KKdfStart, "<kdf>");
_LIT8(KKdfEnd, "</kdf>");
_LIT8(KPKCS12Kdf, "pkcs#12");
_LIT8(KDeriveKeyStart, "<derivekey>");
_LIT8(KDeriveKeyEnd, "</derivekey>");
_LIT8(KKeyStart, "<key>");
_LIT8(KKeyEnd, "</key>");
_LIT8(KPasswdStart, "<passwd>");
_LIT8(KPasswdEnd, "</passwd>");
_LIT8(KSaltStart, "<salt>");
_LIT8(KSaltEnd, "</salt>");
_LIT8(KIterationsStart, "<iterations>");
_LIT8(KIterationsEnd, "</iterations>");
_LIT8(KLeaveInPerformAction, "<leaveinperformaction>");
_LIT8(KLeaveInPerformActionEnd, "</leaveinperformaction>");

CTestAction* CActionDeriveKey::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionDeriveKey::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionDeriveKey::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionDeriveKey* self = new(ELeave) CActionDeriveKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionDeriveKey::~CActionDeriveKey()
	{
	delete iBody;
	}

CActionDeriveKey::CActionDeriveKey(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionDeriveKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	}

void CActionDeriveKey::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 deriveKey = Input::ParseElement(*iBody, KDeriveKeyStart, KDeriveKeyEnd, pos, err);

	// KDF is only explicitly specified for PKCS#12 derived keys
	pos = 0;
	TPtrC8 kdfTemp = Input::ParseElement(deriveKey, KKdfStart, KKdfEnd, pos, err);
	if (err == KErrNone)
		iKdf = kdfTemp.AllocL();
	
	pos = 0;
	TPtrC8 passwdTemp = Input::ParseElement(deriveKey, KPasswdStart, KPasswdEnd, pos, err);
	iPasswd = HBufC8::NewL(passwdTemp.Length());
	*iPasswd = passwdTemp;

	pos = 0;
	TPtrC8 iterationsTemp = Input::ParseElement(deriveKey, KIterationsStart, KIterationsEnd, pos, err);
	iIterations = HBufC8::NewL(iterationsTemp.Length() + 1); //added 1 for the null zero used later
	*iIterations = iterationsTemp;

	pos = 0;
	TPtrC8 saltTemp = Input::ParseElement(deriveKey, KSaltStart, KSaltEnd, pos, err);
	iSalt = HBufC8::NewL(saltTemp.Length());
	*iSalt = saltTemp;
	Hex(*iSalt);

	pos = 0;
	TPtrC8 keyTemp = Input::ParseElement(deriveKey, KKeyStart, KKeyEnd, pos, err);
	iKey = HBufC8::NewL(keyTemp.Length());
	*iKey = keyTemp;
	Hex(*iKey);

	iOutput = HBufC8::NewL(iKey->Length());

	pos = 0;
	iLeaveInPerformAction = Input::ParseIntElement(deriveKey, 
		KLeaveInPerformAction, KLeaveInPerformActionEnd, pos, err);
	if (err)
		{
		iLeaveInPerformAction = 0;
		}
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CActionDeriveKey::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	delete iKey;
	delete iSalt;
	delete iIterations;
	delete iPasswd;
	delete iOutput;
	delete iKdf;
	iKdf = 0;
	
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CActionDeriveKey::DoReportAction(void)
	{
	}

void CActionDeriveKey::DoCheckResult(TInt)
	{

	}

void CActionDeriveKey::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iResult = EFalse;
	
	if (iLeaveInPerformAction)
		{
		User::Leave(KErrArgument);
		}
	iOutput->Des().SetLength(iKey->Length());

	TUint8* nptr= (TUint8*)(iIterations->Des().PtrZ()); 
	TUint32 i = strtoul((char*)nptr, 0, 10); 
	
	iConsole.Printf(_L("."));
	TPtr8 outputActual = iOutput->Des();
	TPtr8 passwdActual = iPasswd->Des();
	if (iKdf != 0 && *iKdf == KPKCS12Kdf)
		{
		// convert the password to PKCS#12 password format
		HBufC* pwdNative = HBufC::NewLC(iPasswd->Length());
		pwdNative->Des().Copy(*iPasswd);
		HBufC8* pwdPKCS12 = PKCS12KDF::GeneratePasswordLC(*pwdNative);
		PKCS12KDF::DeriveKeyL(outputActual, PKCS12KDF::EIDByteEncryptKey, *pwdPKCS12, *iSalt, i);
		CleanupStack::PopAndDestroy(2, pwdNative);
		}
	else	// PKCS#5
		{
		TPtr8 saltActual = iSalt->Des();
		TPKCS5KDF::DeriveKeyL(outputActual, passwdActual, saltActual,i);
		}
		
	if(*iOutput == *iKey)
		{
		iResult = ETrue;
		}
		
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}

void CActionDeriveKey::Hex(HBufC8& aString)
/**
	Convert the supplied hex string into the binary equivalent.
	
	@param	aString			Hex string.  On entry this contains
							a sequence of hexadecimal characters,
							e.g., "3037AFC8EA".  On exit it is
							half the original length and each two-digit
							hex number is reduced to the matching
							byte value.
 */
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

