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


#include <pbedata.h>
#include <stdlib.h>
#include <s32mem.h>
#include <s32std.h>
#include "tpbe.h"
#include "tactionset.h"
#include "t_input.h"

_LIT8(KSetStart, "<set>");
_LIT8(KSetEnd, "</set>");

CTestAction* CActionSet::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionSet::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionSet::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionSet* self = new(ELeave) CActionSet(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionSet::~CActionSet()
	{
	delete iBody;
	}

CActionSet::CActionSet(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionSet::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	
	}

void CActionSet::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 encryptElement = Input::ParseElement(*iBody, KSetStart,
		KSetEnd, pos, err);
	TPtrC8 kdf = Input::ParseElement(*iBody, KKdfStart, KKdfEnd, pos=0, err);
	if (err == KErrNone)
		iKdf = kdf.AllocL();
	
	TPtrC8 saltLenBytes = Input::ParseElement(*iBody, KSaltLenBytesStart, KSaltLenBytesEnd, pos=0, err);
	if (err == KErrNone)
		iSaltLenBytes = saltLenBytes.AllocL();
	
	TPtrC8 iterCount = Input::ParseElement(*iBody, KIterCountStart, KIterCountEnd, pos=0, err);
	if (err == KErrNone)
		iIterCount = iterCount.AllocL();
	
	TPtrC8 passwdTemp = Input::ParseElement(encryptElement, KPasswdStart, 
		KPasswdEnd, pos=0, err);
	iPasswd = HBufC::NewL(passwdTemp.Length());
	TPtr16 passwdTemp3( iPasswd->Des());
	passwdTemp3.Copy(passwdTemp);

	TPtrC8 inputTemp = Input::ParseElement(encryptElement, KInputStart, 
		KInputEnd, pos=0, err);
	iInput = HBufC8::NewL(inputTemp.Length());
	*iInput = inputTemp;

	TPtrC8 cipher = Input::ParseElement(*iBody, KCipherStart, KCipherEnd);
	if (cipher.Compare(KECipherAES_CBC_128) == 0)
	{
	   iCipher = ECipherAES_CBC_128;
	}
	else if (cipher.Compare(KECipherAES_CBC_192) == 0)
	{
	   iCipher = ECipherAES_CBC_192;
	}
	else if (cipher.Compare(KECipherAES_CBC_256) == 0)
	{
	   iCipher = ECipherAES_CBC_256;
	}
	else if (cipher.Compare(KECipherDES_CBC) == 0)
	{
	   iCipher = ECipherDES_CBC;
	}
	else if (cipher.Compare(KECipher3DES_CBC) == 0)
	{
	   iCipher = ECipher3DES_CBC;
	}
	else if (cipher.Compare(KECipherRC2_CBC_40) == 0)
	{
	   iCipher = ECipherRC2_CBC_40;
	}
	else if (cipher.Compare(KECipherRC2_CBC_128) == 0)
	{
	   iCipher = ECipherRC2_CBC_128;
	}
	else if (cipher.Compare(KECipherRC2_CBC_40_16) == 0)
	{
	   iCipher = ECipherRC2_CBC_40_16;
	}
	else if (cipher.Compare(KECipherRC2_CBC_128_16) == 0)
	{
	   iCipher = ECipherRC2_CBC_128_16;
	}
	else if(cipher.Compare(KECipher2Key3DES_CBC) == 0)
	{
		iCipher = ECipher2Key3DES_CBC;
	}
	else if(cipher.Compare(KECipherRC2_CBC_40_5) == 0)
	{
		iCipher = ECipherRC2_CBC_40_5;
	}	
	else
	{
	   iCipher = ECipherAES_CBC_128; // Default value if the <cipher> tag is missing
	}

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CActionSet::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	delete iPasswd;
	delete iInput;
	delete iKdf;
	iKdf = 0;
	delete iSaltLenBytes;
	iSaltLenBytes = 0;
	delete iIterCount;
	iIterCount = 0;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CActionSet::DoReportAction(void)
	{
	}

void CActionSet::DoCheckResult(TInt)
	{

	}

void CActionSet::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;
	HBufC8* pkcs12Pwd = 0;
	
	// default value is NULL to avoid RVCT warning
	// C2874W: set may be used before being set
	CPBEncryptSet* set = 0;
	if (iKdf == 0)
		{
		CleanupStack::PushL(pkcs12Pwd);
 		set = CPBEncryptSet::NewLC(*iPasswd, iCipher);
 		}
	else
		{
		// if supply KDF, must also supply salt len and iteration count
		ASSERT(iSaltLenBytes != 0 && iIterCount != 0);
		
		CPBEncryptParms* ep = CPBEncryptParms::NewLC();
		
		ep->SetCipherL(iCipher);
		
		TInt saltLenBytes;
		TInt r = TLex8(*iSaltLenBytes).Val(saltLenBytes);
		ASSERT(r == KErrNone);
		ep->ResizeSaltL(saltLenBytes);
		
		TInt iterCount;
		r = TLex8(*iIterCount).Val(iterCount);
		ASSERT(r == KErrNone);
		ep->SetIterations(iterCount);
		
		CleanupStack::PushL((CBase*)0);
		CleanupStack::Pop((CBase*)0);

		if (*iKdf == _L8("PKCS#5"))
			{
			ep->SetKdf(CPBEncryptParms::EKdfPkcs5);
			set = CPBEncryptSet::NewL(*iPasswd, *ep);
			}
		else if (*iKdf == _L8("PKCS#12"))
			{
			pkcs12Pwd = PKCS12KDF::GeneratePasswordLC(*iPasswd);
			ep->SetKdf(CPBEncryptParms::EKdfPkcs12);
			set = CPBEncryptSet::NewL(*pkcs12Pwd, *ep);
			CleanupStack::Pop(pkcs12Pwd);
			}
		else
			User::Panic(_L("Unrec KDF"), 0);
		
		CleanupStack::PopAndDestroy(ep);
		// encryption could leak here, but for reservation above
		CleanupStack::PushL(pkcs12Pwd);
		CleanupStack::PushL(set);
		}
	CPBEncryptor* encryptor = set->NewEncryptLC();
	HBufC8* ciphertextTemp = HBufC8::NewLC(encryptor->MaxFinalOutputLength(iInput->Length())); 

	TPtr8 ciphertext = ciphertextTemp->Des();	
	encryptor->ProcessFinalL(*iInput, ciphertext);
	TBuf<128> newPwdTemp(*iPasswd);
	newPwdTemp.Append('a');

	TBuf8<128> newPwdTemp8;
	
	TPBPassword newPassword(KNullDesC);
	if (pkcs12Pwd == 0)
		new(&newPassword) TPBPassword(newPwdTemp);
	else
		{
		HBufC8* newPwd = PKCS12KDF::GeneratePasswordLC(newPwdTemp);
		newPwdTemp8.Copy(*newPwd);
		new(&newPassword) TPBPassword(newPwdTemp8);
		CleanupStack::PopAndDestroy(newPwd);
		}

	set->ChangePasswordL(newPassword);
	
	//create a mem  buffer store
	CBufStore* store = CBufStore::NewLC(100);
	RStoreWriteStream write;

	//write the encrypted master key to a stream
	TStreamId keyStreamId = write.CreateLC(*store);
	write << set->EncryptedMasterKey();
	write.CommitL();
	CleanupStack::PopAndDestroy(); //CreateLC()

	//write the encryption data to another stream
	TStreamId dataStreamId = write.CreateLC(*store);
	set->EncryptionData().ExternalizeL(write);
	write.CommitL();
	CleanupStack::PopAndDestroy(); //CreateLC()

	//prepare to read the streams back in, creating a new TPBEncryptionData
	RStoreReadStream read;
	read.OpenLC(*store, dataStreamId);

	//read in Encryption data
	CPBEncryptionData* data = CPBEncryptionData::NewL(read);
	CleanupStack::PopAndDestroy(); //OpenLC()
	CleanupStack::PushL(data);

	//read in encrypted master key
	read.OpenLC(*store, keyStreamId);
	HBufC8* encryptedMasterKey = HBufC8::NewLC(read, 10000); //some large number

	//create a new set encryption class
	CPBEncryptSet* set2 = CPBEncryptSet::NewLC(*data, *encryptedMasterKey, newPassword);

	HBufC8* plaintextTemp = HBufC8::NewLC(ciphertext.Length());
	TPtr8 plaintext = plaintextTemp->Des();	

	CPBDecryptor* decryptor = set2->NewDecryptLC();
	decryptor->Process(ciphertext, plaintext);

	//this Mid call is due to get rid of the decrypted padding at the end
	if(plaintext.Mid(0,iInput->Length()) == *iInput)
		{
		iResult = ETrue;			
		}
	
	CleanupStack::PopAndDestroy(decryptor);
	CleanupStack::PopAndDestroy(plaintextTemp);
	CleanupStack::PopAndDestroy(set2);
	CleanupStack::PopAndDestroy(encryptedMasterKey);
	CleanupStack::PopAndDestroy(1); //OpenLC
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(store);
	CleanupStack::PopAndDestroy(ciphertextTemp);
	CleanupStack::PopAndDestroy(encryptor);
	CleanupStack::PopAndDestroy(set);
	CleanupStack::PopAndDestroy(pkcs12Pwd);
	
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

void CActionSet::Hex(HBufC8& aString)
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
