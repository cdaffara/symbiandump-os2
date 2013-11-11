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

_LIT(KTxtEPOC32EX,"tasync: mainL failed");
_LIT(KTxtPressAnyKey," [press any key]");

//#define KEYLEN 16
#define KEYLEN 24
//#define KEYLEN 32

//#define BUFLEN 256
//#define BUFLEN (256*16)
#define BUFLEN 32
#define LOOPCOUNT 10000


// public
CConsoleBase* console; // write all your messages to this

// private
LOCAL_C void mainL();

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,mainL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }

LOCAL_D RTest test(_L("tasync"));

LOCAL_D void dumpBuffer(const TDesC &aName, const TDesC8 &aBuf);

LOCAL_C void encryptL(const TDesC8 &aKey, const TDesC8 &aIV, const TDesC8 &aPlainText, 
					  TDes8 &aCipherText);
LOCAL_C void decryptL(const TDesC8 &aKey, const TDesC8 &aIV, const TDesC8 &aCiphertext, 
					  TDes8 &aPlaintext);

LOCAL_C void mainL() // initialize and call example code under cleanup stack
    {
	test.Title();
	//
	// Generate key and IV
	//
    test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-ASYNC-0001 Generating key & IV for AES tests "));
	test.Next(_L("Generating key\n"));
	// Generate 16 byte key
	TBuf8<KEYLEN> key;
	key.SetLength(KEYLEN);
	key[0] = 'K';
	key[1] = 'E';
	key[2] = 'Y';
	key[3] = '*';
	for(int z=4; z<KEYLEN; ++z) key[z] = z;


	test.Next(_L("Generating IV\n"));
	TBuf8<16> iv;
	iv.SetLength(iv.MaxLength());
	iv[0] = 'I';
	iv[1] = 'V';
	iv[2] = '*';
	iv[3] = '*';

	TBuf8<BUFLEN> plaintext;
	plaintext.FillZ();
	plaintext.SetLength(plaintext.MaxLength());
	plaintext[0] = 'P';
	plaintext[1] = 'L';
	plaintext[2] = 'A';
	plaintext[3] = 'I';
	plaintext[4] = 'N';
	plaintext[5] = '1';
	for(int i=6; i<BUFLEN/2; ++i)
		{
		plaintext[i] = i;
		}
	plaintext[BUFLEN/2] = 'P';
	plaintext[BUFLEN/2+1] = 'L';
	plaintext[BUFLEN/2+2] = 'A';
	plaintext[BUFLEN/2+3] = 'I';
	plaintext[BUFLEN/2+4] = 'N';
	plaintext[BUFLEN/2+5] = '2';
	for(int i=BUFLEN/2+6; i<BUFLEN; ++i)
		{
		plaintext[i] = i;
		}
	test.End();

	TBuf8<BUFLEN+16> ciphertext;
	encryptL(key, iv, plaintext, ciphertext);

	TBuf8<BUFLEN> plaintext2;
	decryptL(key, iv, ciphertext, plaintext2);

	test.Printf(_L("Checking plaintext2 == plaintext\n"));
	test(plaintext2 == plaintext);

	test.Printf(KTxtPressAnyKey);
	test.Getch(); // get and ignore character
	test.Close();

	}

LOCAL_C void encryptL(const TDesC8 &aKey, const TDesC8 &aIV, const TDesC8 &aPlaintext, 
					  TDes8 &aCiphertext)
	{
	RDebug::Printf("Starting tasync encryption tests\n");
	
	TPtrC8 chunk1(aPlaintext.Left(16));
	TPtrC8 chunk2(aPlaintext.Mid(16, aPlaintext.Length()-16));
	
	
	//
	// Encrypt using legacy API - Reference
	//
	TBuf8<BUFLEN+16> sw;
	sw.FillZ();
	sw.SetLength(0);

	test.Start(_L("AES - S/W - Reference encrypt"));
	
	test.Printf(_L("    CBC\n"));
	CAESEncryptor *rawaes = CAESEncryptor::NewLC(aKey); // rawaes
	CModeCBCEncryptor *cbc = CModeCBCEncryptor::NewL(rawaes, aIV);
	CleanupStack::Pop(rawaes); //
	CleanupStack::PushL(cbc);  // cbc
	
	CPadding *pad = CPaddingPKCS7::NewLC(16); // cbc, pad
	CBufferedEncryptor *aes = CBufferedEncryptor::NewL(cbc, pad);
	CleanupStack::Pop(pad); // cbc
	CleanupStack::Pop(cbc);
	CleanupStack::PushL(aes); // aes
		
	test.Printf(_L("About to s/w encrypt (old api)\n"));
	aes->Process(chunk1, sw);
	aes->ProcessFinalL(chunk2, sw);
	dumpBuffer(_L("sw"), sw);
		
	CleanupStack::PopAndDestroy(aes);

	//
	// Encrypt using legacy API
	//
	test.Start(_L("AES - S/W - Legacy API - Parallel encrypt"));
	
	test.Printf(_L("    CBC2\n"));
	TBuf8<BUFLEN+16> sw2;
	sw2.FillZ();
	sw2.SetLength(0);
	CAESEncryptor *rawaes2 = CAESEncryptor::NewLC(aKey); // rawaes2
	CModeCBCEncryptor *cbc2 = CModeCBCEncryptor::NewL(rawaes2, aIV);
	CleanupStack::Pop(rawaes2); //
	CleanupStack::PushL(cbc2);  // cbc2
	
	CPadding *pad2 = CPaddingPKCS7::NewLC(16); // cbc2, pad2
	CBufferedEncryptor *aes2 = CBufferedEncryptor::NewL(cbc2, pad2);
	CleanupStack::Pop(pad2); // cbc2
	CleanupStack::Pop(cbc2);
	CleanupStack::PushL(aes2); // aes2

	test.Printf(_L("    CBC3\n"));
	TBuf8<BUFLEN+16> sw3;
	sw3.FillZ();
	sw3.SetLength(0);
	CAESEncryptor *rawaes3 = CAESEncryptor::NewLC(aKey); // rawaes3
	CModeCBCEncryptor *cbc3 = CModeCBCEncryptor::NewL(rawaes3, aIV);
	CleanupStack::Pop(rawaes3); //
	CleanupStack::PushL(cbc3);  // cbc3
	
	CPadding *pad3 = CPaddingPKCS7::NewLC(16); // cbc3, pad3
	CBufferedEncryptor *aes3 = CBufferedEncryptor::NewL(cbc3, pad3);
	CleanupStack::Pop(pad3); // cbc3
	CleanupStack::Pop(cbc3);
	CleanupStack::PushL(aes3); // aes3
	
	test.Printf(_L("About to parallel encrypt\n"));
#if 1
	// Parallel
	aes2->Process(chunk1, sw2);
	aes3->Process(chunk1, sw3);
	aes2->ProcessFinalL(chunk2, sw2);
	aes3->ProcessFinalL(chunk2, sw3);
#else
	// Sequential
	aes2->Process(chunk1, sw2);
	aes2->ProcessFinalL(chunk2, sw2);
	aes3->Process(chunk1, sw3);
	aes3->ProcessFinalL(chunk2, sw3);
#endif		

	dumpBuffer(_L("sw2"), sw2);
	dumpBuffer(_L("sw3"), sw3);


	CleanupStack::PopAndDestroy(aes3);
	CleanupStack::PopAndDestroy(aes2);

	test.Printf(_L("Checking sw2 == sw3\n"));
	test(sw2 == sw3);

	test.Printf(_L("Checking sw2 == sw\n"));
	test(sw2 == sw);

	aCiphertext = sw;

    test.End();
	}

LOCAL_C void decryptL(const TDesC8 &aKey, const TDesC8 &aIV, const TDesC8 &aCiphertext, 
					  TDes8 &aPlaintext)
	{
	RDebug::Printf("Starting tasync decryption tests\n");
	
	TPtrC8 chunk1(aCiphertext.Left(16));
	TPtrC8 chunk2(aCiphertext.Mid(16, aCiphertext.Length()-16));
	
	//
	// Decrypt using legacy API - Reference
	//
	TBuf8<BUFLEN+16> sw;
	sw.FillZ();
	sw.SetLength(0);

	test.Start(_L("AES - S/W - Reference decrypt"));
	
	test.Printf(_L("    CBC\n"));
	CAESDecryptor *rawaes = CAESDecryptor::NewLC(aKey); // rawaes
	CModeCBCDecryptor *cbc = CModeCBCDecryptor::NewL(rawaes, aIV);
	CleanupStack::Pop(rawaes); //
	CleanupStack::PushL(cbc);  // cbc
	
	CPadding *pad = CPaddingPKCS7::NewLC(16); // cbc, pad
	CBufferedDecryptor *aes = CBufferedDecryptor::NewL(cbc, pad);
	CleanupStack::Pop(pad); // cbc
	CleanupStack::Pop(cbc);
	CleanupStack::PushL(aes); // aes
		
	test.Printf(_L("About to s/w decrypt (old api)\n"));
	aes->Process(chunk1, sw);
	aes->ProcessFinalL(chunk2, sw);
	dumpBuffer(_L("sw"), sw);
		
	CleanupStack::PopAndDestroy(aes);

	//
	// Decrypt using legacy API
	//
	test.Start(_L("AES - S/W - Legacy API - Parallel decrypt"));
	
	test.Printf(_L("    CBC2\n"));
	TBuf8<BUFLEN+16> sw2;
	sw2.FillZ();
	sw2.SetLength(0);
	CAESDecryptor *rawaes2 = CAESDecryptor::NewLC(aKey); // rawaes2
	CModeCBCDecryptor *cbc2 = CModeCBCDecryptor::NewL(rawaes2, aIV);
	CleanupStack::Pop(rawaes2); //
	CleanupStack::PushL(cbc2);  // cbc2
	
	CPadding *pad2 = CPaddingPKCS7::NewLC(16); // cbc2, pad2
	CBufferedDecryptor *aes2 = CBufferedDecryptor::NewL(cbc2, pad2);
	CleanupStack::Pop(pad2); // cbc2
	CleanupStack::Pop(cbc2);
	CleanupStack::PushL(aes2); // aes2

	test.Printf(_L("    CBC3\n"));
	TBuf8<BUFLEN+16> sw3;
	sw3.FillZ();
	sw3.SetLength(0);
	CAESDecryptor *rawaes3 = CAESDecryptor::NewLC(aKey); // rawaes3
	CModeCBCDecryptor *cbc3 = CModeCBCDecryptor::NewL(rawaes3, aIV);
	CleanupStack::Pop(rawaes3); //
	CleanupStack::PushL(cbc3);  // cbc3
	
	CPadding *pad3 = CPaddingPKCS7::NewLC(16); // cbc3, pad3
	CBufferedDecryptor *aes3 = CBufferedDecryptor::NewL(cbc3, pad3);
	CleanupStack::Pop(pad3); // cbc3
	CleanupStack::Pop(cbc3);
	CleanupStack::PushL(aes3); // aes3
	
	test.Printf(_L("About to parallel decrypt\n"));

	// Parallel
	aes2->Process(chunk1, sw2);
	aes3->Process(chunk1, sw3);
	aes2->ProcessFinalL(chunk2, sw2);
	aes3->ProcessFinalL(chunk2, sw3);

	dumpBuffer(_L("sw2"), sw2);
	dumpBuffer(_L("sw3"), sw3);


	CleanupStack::PopAndDestroy(aes3);
	CleanupStack::PopAndDestroy(aes2);

	test.Printf(_L("Checking sw2 == sw3\n"));
	test(sw2 == sw3);

	test.Printf(_L("Checking sw2 == sw\n"));
	test(sw2 == sw);

	aPlaintext = sw;

    test.End();
    }



LOCAL_D void dumpBuffer(const TDesC &aName, const TDesC8 &aBuf)
	{
	test.Printf(_L("%S ="), &aName);
	TInt len = aBuf.Length();
	for(TInt i = 0 ; i < len; ++i)
		{
		if(i%16 == 0)
			{
			test.Printf(_L("\n    "));
			}
		test.Printf(_L("%02x "), aBuf[i]);
		}
	test.Printf(_L("\n"));
	}


// End of file
