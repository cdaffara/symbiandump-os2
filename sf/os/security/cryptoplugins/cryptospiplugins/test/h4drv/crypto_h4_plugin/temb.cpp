/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/
#include "cryptodriver.h"
#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32debug.h>
#include <rijndael.h>
#include <cbcmode.h>
#include <padding.h>
#include <bufferedtransformation.h>
#include <random.h>
#include "keys.h"
#include <cryptospi/cryptospidef.h>
#include "cryptosymmetriccipherapi.h"
#include "kmsclient.h"
#include <f32file.h>
//#include "kmsservercommon.h"
using namespace CryptoSpi;

_LIT(KTxtEPOC32EX,"temb: mainL failed");
//_LIT(KTxtPressAnyKey," [press any key]");

//#define KEYLEN 16
#define KEYLEN 24
//#define KEYLEN 32

#define PKCS7PAD


//#define BUFLEN 256
#define BUFLEN (256*16)
#define LOOPCOUNT 10000

LOCAL_D RTest test(_L("Embedded Key Tests"));


class CTestConsole:public CConsoleBase

	{
	public:
		static CTestConsole* NewL(CConsoleBase* aCon);
		TInt Create(const TDesC16& aTitle,TSize aSize) {return iCon->Create(aTitle,aSize);};
		void Read(TRequestStatus& aStatus) {iCon->Read(aStatus);};
		void ReadCancel(void) {iCon->ReadCancel();};
		void Write(const TDesC16& aString);
		TPoint CursorPos(void) const {return iCon->CursorPos();};
		void SetCursorPosAbs(const TPoint& aPos) {iCon->SetCursorPosAbs(aPos);};
		void SetCursorPosRel(const TPoint& aPos) {iCon->SetCursorPosRel(aPos);};
		void SetCursorHeight(TInt aHeight) {iCon->SetCursorHeight(aHeight);};
		void SetTitle(const TDesC16& aTitle) {iCon->SetTitle(aTitle);};
		void ClearScreen(void) {iCon->ClearScreen();};
		void ClearToEndOfLine(void) {iCon->ClearToEndOfLine();};
		TSize ScreenSize(void) const {return iCon->ScreenSize();};
		TKeyCode KeyCode(void) const {return iCon->KeyCode();};
		TUint KeyModifiers(void) const {return iCon->KeyModifiers();};
		~CTestConsole(void);
		void SetLogFile(RFile &aFile);
	private:
		CTestConsole(void);
		CConsoleBase* iCon;
		RFile* iFile;
	};

CTestConsole* CTestConsole::NewL(CConsoleBase* aCon)
	{
	CTestConsole* self;
	self=new (ELeave) CTestConsole;
	self->iCon=aCon;
	self->iFile=NULL;
	return self;
	}

CTestConsole::CTestConsole(void):CConsoleBase()

	{
	}

CTestConsole::~CTestConsole(void)

	{
	delete iCon;
	if (iFile)
		{
		iFile->Close();
		}
	}

void CTestConsole::Write(const TDesC16& aString)

	{
	iCon->Write(aString);
	if (iFile)
		{
		TUint8 space[200];
		TPtr8 ptr(space,200);
		ptr.Copy(aString);
		iFile->Write(ptr);
		}
	}

void CTestConsole::SetLogFile(RFile &aFile)

	{
	iFile = &aFile;
	}

// private
LOCAL_C void mainL();

GLDEF_C TInt E32Main() // main function called by E32
    {
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	
	//
	// Run the tests
	//
	__UHEAP_MARK;
	TRAPD(error,mainL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	__UHEAP_MARKEND;

	delete cleanup; // destroy clean-up stack
	return 0; // and return
    }


_LIT(KLddFileName,"cryptoldd.ldd");
_LIT(KPddFileName,"crypto.h4.pdd");

LOCAL_C void mainL() // initialize and call example code under cleanup stack
    {
	test.Title();
	CTestConsole *con = CTestConsole::NewL(test.Console());

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TDriveUnit sysDrive (fs.GetSystemDrive());
	TBuf<24> logFile (sysDrive.Name());
	logFile.Append(_L("\\temblog.txt"));

	RFile file;
	User::LeaveIfError(file.Replace(fs, logFile, EFileShareAny|EFileWrite));
	CleanupClosePushL(file);

	con->SetLogFile(file);
	test.SetConsole(con);

    TInt r;
	RDebug::Printf("Hello from user side\n");
	
    test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-TEMB-0001 Load driver "));

    test.Next(_L("Loading Physical Device"));
    r=User::LoadPhysicalDevice(KPddFileName);
    test(r==KErrNone || r==KErrAlreadyExists);

    test.Next(_L("Loading Logical Device"));
    r=User::LoadLogicalDevice(KLddFileName);
    test(r==KErrNone || r==KErrAlreadyExists);

	//
	// Generate key and IV
	//
    test.Start(_L("Random - Generating key & IV for AES tests"));
	test.Printf(_L("\tGenerating random key\n"));
	// Generate random 16 byte key
	TBuf8<KEYLEN> key;
	key.SetLength(key.MaxLength());
	TRandom::RandomL(key);
	key[0] = 'K';
	key[1] = 'E';
	key[2] = 'Y';
	key[3] = '*';
	for(int z=4; z<KEYLEN; ++z) key[z] = z;

	test.Printf(_L("\tGenerating random IV\n"));
	// Generate random 16 byte IV
	TBuf8<16> iv;
	iv.SetLength(iv.MaxLength());
	TRandom::RandomL(iv);
	iv[0] = 'I';
	iv[1] = 'V';
	iv[2] = '*';
	iv[3] = '*';

	TBuf8<BUFLEN> plaintext;
	plaintext.FillZ();
	plaintext.SetLength(BUFLEN);
	plaintext[0] = 'P';
	plaintext[1] = 'L';
	plaintext[2] = 'A';
	plaintext[3] = 'I';
	plaintext[4] = 'N';
	for(int i=0; i<BUFLEN; ++i)
		{
		plaintext[i] = i;
		}


	//
	// KMS tests
	//
    test.Next(_L("KMS - Store key"));
	TBuf8<BUFLEN+16> kmsData;
	kmsData.FillZ();
	kmsData.SetLength(0);
	do
		{
		RKeyMgmtSession kms;
		User::LeaveIfError(kms.Connect());
		CleanupClosePushL(kms);
		
		TKeyHandle keyHandle;
		User::LeaveIfError(kms.StoreKey(key, keyHandle));
		_LIT_SECURITY_POLICY_PASS(KAlwaysPass);
		User::LeaveIfError(kms.AddUsage(keyHandle, 0 /* operation */, KAlwaysPass));
		
		test.Next(_L("KMS - Attempt to use key via embedded key handle"));
		TPckgBuf<TKeyHandle> keyHandlePkg;
		keyHandlePkg() = keyHandle;

		TKeyProperty keyProperty = {KAesUid, KNullUid, KSymmetricKeyUid, KNonExtractableKey};
		CCryptoParams* keyParam =CCryptoParams::NewLC();
		keyParam->AddL(keyHandlePkg, KSymmetricKeyParameterUid);
		CKey *ckey=CKey::NewL(keyProperty, *keyParam);
		CleanupStack::PopAndDestroy(keyParam);
		CleanupStack::PushL(ckey);
		CryptoSpi::CSymmetricCipher *aes = 0;
		CSymmetricCipherFactory::CreateSymmetricCipherL(aes,
														KAesUid,
														*ckey,
														KCryptoModeEncryptUid,
														KOperationModeCBCUid,
														KPaddingModePKCS7Uid,
														NULL);
		CleanupStack::PopAndDestroy(ckey);
		CleanupStack::PushL(aes);

		aes->SetOperationModeL(CryptoSpi::KOperationModeCBCUid);
		aes->SetIvL(iv);		

		aes->ProcessFinalL(plaintext, kmsData);

		CleanupStack::PopAndDestroy(aes);
		CleanupStack::PopAndDestroy(&kms);
		} while(false);


	//
	// Encrypt using legacy API
	//
	TBuf8<BUFLEN+16> sw;
	sw.FillZ();
	sw.SetLength(0);
	do 
		{ 
		test.Next(_L("Encrypt using key directly (non-KMS)"));
		
		// ECB
		test.Printf(_L("    CBC\n"));
		CAESEncryptor *rawaes = CAESEncryptor::NewLC(key); // rawaes
		CModeCBCEncryptor *cbc = CModeCBCEncryptor::NewL(rawaes, iv);
		CleanupStack::Pop(rawaes); //
		CleanupStack::PushL(cbc);  // cbc
		
#ifdef PKCS7PAD
		CPadding *pad = CPaddingPKCS7::NewLC(16); // cbc, pad
#else
		CPadding *pad = CPaddingNone::NewLC(16); // cbc, pad
#endif
		CBufferedEncryptor *aes = CBufferedEncryptor::NewL(cbc, pad);
		CleanupStack::Pop(pad); // cbc
		CleanupStack::Pop(cbc);
		CleanupStack::PushL(aes); // aes
		
		test.Printf(_L("About to s/w encrypt (old api)\n"));
		aes->ProcessFinalL(plaintext, sw);
		
		CleanupStack::PopAndDestroy(aes);
		} while(false);

	test.Printf(_L("Checking KMS encrypt and direct encrypt had the same result\n"));
	test(kmsData == sw);
    test.End();
	
	test.Printf(_L("\r\n0 tests failed out of 1\r\n"));
		
	// test.Printf(KTxtPressAnyKey);
	// test.Getch(); // get and ignore character
	test.Close();

	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);
    }


// End of file
