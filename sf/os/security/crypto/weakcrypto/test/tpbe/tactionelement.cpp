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
#include "tactionelement.h"
#include "t_input.h"

_LIT8(KElementStart, "<element>");
_LIT8(KElementEnd, "</element>");

CTestAction* CActionElement::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionElement::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionElement::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionElement* self = new(ELeave) CActionElement(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionElement::~CActionElement()
	{
	delete iBody;
	}

CActionElement::CActionElement(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionElement::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	
	}

void CActionElement::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 encryptElement = Input::ParseElement(*iBody, KElementStart,
		KElementEnd, pos, err);

	pos = 0;
	TPtrC8 kdf = Input::ParseElement(*iBody, KKdfStart, KKdfEnd, pos, err);
	if (err == KErrNone)
		iKdf = kdf.AllocL();
	
	pos = 0;
	TPtrC8 saltLenBytes = Input::ParseElement(*iBody, KSaltLenBytesStart, KSaltLenBytesEnd, pos, err);
	if (err == KErrNone)
		iSaltLenBytes = saltLenBytes.AllocL();
	
	pos = 0;
	TPtrC8 iterCount = Input::ParseElement(*iBody, KIterCountStart, KIterCountEnd, pos, err);
	if (err == KErrNone)
		iIterCount = iterCount.AllocL();
	
	pos = 0;
	TPtrC8 passwdTemp = Input::ParseElement(encryptElement, KPasswdStart, 
		KPasswdEnd, pos, err);
	iPasswd = HBufC::NewL(passwdTemp.Length());
	TPtr16 passwdTemp3( iPasswd->Des());
	passwdTemp3.Copy(passwdTemp);

	pos = 0;
	TPtrC8 inputTemp = Input::ParseElement(encryptElement, KInputStart, 
		KInputEnd, pos, err);
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
	else if(cipher.Compare(KECipherARC4_128) == 0)
	{
		iCipher = ECipherARC4_128;
	}
	else if(cipher.Compare(KECipherARC4_40) == 0)
	{
		iCipher = ECipherARC4_40;
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
	   iCipher = ECipherAES_CBC_128;
	}
	
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CActionElement::DoPerformPostrequisite(TRequestStatus& aStatus)
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

void CActionElement::DoReportAction(void)
	{
	}

void CActionElement::DoCheckResult(TInt)
	{

	}

void CActionElement::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;
	HBufC8* pkcs12Pwd = 0;
	
	// default value is NULL to avoid RVCT warning
	// C2874W: encryption may be used before being set
	CPBEncryptElement* encryption = 0;
	if (iKdf == 0)
		{
		CleanupStack::PushL(pkcs12Pwd);
 		encryption = CPBEncryptElement::NewLC(*iPasswd, iCipher);
 		}
	else
		{
		// if supply KDF, must also supply salt len and iteration count
		ASSERT(iSaltLenBytes != 0 && iIterCount != 0);
		
		CPBEncryptParms* ep = CPBEncryptParms::NewLC();
		
		ep->SetCipher(iCipher);
		
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
			encryption = CPBEncryptElement::NewL(*iPasswd, *ep);
			}
		else if (*iKdf == _L8("PKCS#12"))
			{			
			pkcs12Pwd = PKCS12KDF::GeneratePasswordLC(*iPasswd);
			ep->SetKdf(CPBEncryptParms::EKdfPkcs12);
			encryption = CPBEncryptElement::NewL(*pkcs12Pwd, *ep);
			CleanupStack::Pop(pkcs12Pwd);
			}
		else
			User::Panic(_L("Unrec KDF"), 0);
		
		CleanupStack::PopAndDestroy(ep);
		// encryption could leak here, but for reservation above
		CleanupStack::PushL(pkcs12Pwd);
		CleanupStack::PushL(encryption);
		}

	CPBEncryptor* encryptor = encryption->NewEncryptLC();

	HBufC8* ciphertextTemp = HBufC8::NewLC(encryptor->MaxFinalOutputLength(iInput->Length())); 
	TPtr8 ciphertext = ciphertextTemp->Des();	
	encryptor->ProcessFinalL(*iInput, ciphertext);

	//create a mem buffer store
	CBufStore* store = CBufStore::NewLC(100);
	RStoreWriteStream write;

	//write the encryption data to a stream
	TStreamId dataStreamId = write.CreateLC(*store);
	encryption->EncryptionData().ExternalizeL(write);
	write.CommitL();
	CleanupStack::PopAndDestroy(); //CreateLC()

	//prepare to read the stream back in
	RStoreReadStream read;
	read.OpenLC(*store, dataStreamId);

	//read in Encryption Data
	CPBEncryptionData* data = CPBEncryptionData::NewL(read);
	CleanupStack::PopAndDestroy(); //OpenLC()
	CleanupStack::PushL(data);

	CPBEncryptElement* encryption1 =
			(pkcs12Pwd == 0)
	 	?	CPBEncryptElement::NewLC(*data, *iPasswd)
	 	:	CPBEncryptElement::NewLC(*data, *pkcs12Pwd);

	CPBDecryptor* decryptor = encryption1->NewDecryptLC();
	HBufC8* plaintextTemp = HBufC8::NewLC(decryptor->MaxOutputLength(ciphertext.Size())); 
	TPtr8 plaintext = plaintextTemp->Des();	
	decryptor->Process(ciphertext, plaintext);

	//this Mid call is due to get rid of the decrypted padding at the end
	if(plaintext.Mid(0,iInput->Length()) == *iInput)
		{
		iResult = ETrue;			
		}

	CleanupStack::PopAndDestroy(5); //plaintextTemp,decryptor,encryption1,data,store
	
	CPBEncryptionData* data1 = CPBEncryptionData::NewLC(encryption->EncryptionData());
	
	CPBEncryptElement* encryption2 =
			(pkcs12Pwd == 0)
	 	?	CPBEncryptElement::NewLC(*data1, *iPasswd)
	 	:	CPBEncryptElement::NewLC(*data1, *pkcs12Pwd);

	CPBDecryptor* decryptor1 = encryption2->NewDecryptLC();
	HBufC8* plaintextTemp2 = HBufC8::NewLC(decryptor1->MaxOutputLength(ciphertext.Size())); 
	TPtr8 plaintext2 = plaintextTemp2->Des();	
	decryptor1->Process(ciphertext, plaintext2);

	//this Mid call is due to get rid of the decrypted padding at the end
	if(!(plaintext2.Mid(0,iInput->Length()) == *iInput))
		{
		iResult = EFalse;			
		}
	
	CleanupStack::PopAndDestroy(plaintextTemp2);
	CleanupStack::PopAndDestroy(decryptor1);
	CleanupStack::PopAndDestroy(encryption2);
	CleanupStack::PopAndDestroy(data1);
	CleanupStack::PopAndDestroy(ciphertextTemp);
	CleanupStack::PopAndDestroy(encryptor);
	CleanupStack::PopAndDestroy(encryption);
	CleanupStack::PopAndDestroy(pkcs12Pwd);
	
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

void CActionElement::Hex(HBufC8& aString)
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
