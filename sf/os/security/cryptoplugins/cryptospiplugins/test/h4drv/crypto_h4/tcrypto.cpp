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
#include <e32def_private.h>

_LIT(KTxtEPOC32EX,"tcrypto: mainL failed");
_LIT(KTxtPressAnyKey," [press any key]");

#define RNG_TESTS
//#define RNG_USE_SVR
//#define KEYLEN 16
#define KEYLEN 24
//#define KEYLEN 32

#define USE_CBCMODE
//#define DISABLE_AES_CHECKS

//#define PADDING_PKCS7

//#define BUFLEN 256
#define BUFLEN (256*16)
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

LOCAL_D RTest test(_L("tcrypto"));

_LIT(KLddFileName,"cryptoldd.ldd");
_LIT(KPddFileName,"crypto.h4.pdd");

LOCAL_C void mainL() // initialize and call example code under cleanup stack
    {
#if 0
	test.Printf(KTxtPressAnyKey);
	test.Getch(); // get and ignore character
	test.Printf(_L("\n"));
#endif

	test.Title();

	RDebug::Printf("Hello from user side\n");
	

    TInt r;

    test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-CRYPTO-0001 Loading Physical Device "));
    r=User::LoadPhysicalDevice(KPddFileName);
    test(r==KErrNone || r==KErrAlreadyExists);

	test.Next(_L("Re-Loading Physical Device"));
	r=User::LoadPhysicalDevice(KPddFileName);
	test(r==KErrNone || r==KErrAlreadyExists);

    test.Next(_L("Loading Logical Device"));
    r=User::LoadLogicalDevice(KLddFileName);
    test(r==KErrNone || r==KErrAlreadyExists);

	test.Next(_L("Re-Loading Logical Device"));
	r=User::LoadLogicalDevice(KLddFileName);
	test(r==KErrNone || r==KErrAlreadyExists);
	
    test.Next(_L("Open Device"));
    RDevice device;
    r=device.Open(RCryptoDriver::Name());
    test(r==KErrNone);

    test.Next(_L("Get Device Capabilities"));
    RCryptoDriver::TCaps caps;
    TPckg<RCryptoDriver::TCaps>capsPckg(caps);
    capsPckg.FillZ(); // Zero 'caps' so we can tell if GetCaps has really filled it
    device.GetCaps(capsPckg);
    TVersion expectedVer(RCryptoDriver::VersionRequired());
    test(caps.iVersion.iMajor==expectedVer.iMajor);
    test(caps.iVersion.iMinor==expectedVer.iMinor);
    test(caps.iVersion.iBuild==expectedVer.iBuild);

	test.Next(_L("Close Device"));
    device.Close();

    test.Next(_L("Open Logical Channel"));
    RCryptoDriver ldd;
    r=ldd.Open();
    test(r==KErrNone);

	test.Next(_L("GetHwVersions"));
    RCryptoDriver::THwVersionsBuf hwVersionsBuf;
    hwVersionsBuf.FillZ(); // Zero 'config' so we can tell if GetConfig has really filled it
    r=ldd.GetHwVersions(hwVersionsBuf);
    test(r==KErrNone);
	test.Printf(_L("RNG h/w version 0x%x\n"), hwVersionsBuf().iRngHwVersion);
	test.Printf(_L("DES/3DES h/w version 0x%x\n"), hwVersionsBuf().iDes3DesHwVersion);
	test.Printf(_L("SHA1/MD5 h/w version 0x%x\n"), hwVersionsBuf().iSha1Md5HwVersion);
	test.Printf(_L("AES h/w version 0x%x\n"), hwVersionsBuf().iAesHwVersion);
	test.Printf(_L("PKA h/w version 0x%x\n"), hwVersionsBuf().iPkaHwVersion);

	test.Next(_L("GetConfig"));
    RCryptoDriver::TConfigBuf configBuf;
    configBuf.FillZ(); // Zero 'config' so we can tell if GetConfig has really filled it
    r=ldd.GetConfig(configBuf);
    test(r==KErrNone);
	
	{
	test.Next(_L("Open Logical Channel 2nd time"));
	RCryptoDriver lddB;
	r=lddB.Open();
	test(r==KErrNone);
	
	test.Next(_L("GetConfig"));
	RCryptoDriver::TConfigBuf configBuf;
	configBuf.FillZ(); // Zero 'config' so we can tell if GetConfig has really filled it
	r=lddB.GetConfig(configBuf);
	test(r==KErrNone);
	lddB.Close();
	}
	
    RCryptoDriver::TConfig& config=configBuf();

    test.Next(_L("SetConfig"));
    TInt setting = configBuf().iFakeDriverSetting+1;
    configBuf().iFakeDriverSetting = setting;
    r=ldd.SetConfig(configBuf); // Use SetConfig to change setting
    test(r==KErrNone);

    configBuf.FillZ();
    r=ldd.GetConfig(configBuf);
    test(r==KErrNone);
    test(configBuf().iFakeDriverSetting==setting);

    test.Next(_L("Check access by wrong client"));
    RCryptoDriver ldd2=ldd;
    r=ldd2.Duplicate(RThread(),EOwnerProcess);
    test(r==KErrAccessDenied);

    test.Next(_L("Check handle duplication"));
    ldd2=ldd;
    r=ldd2.Duplicate(RThread(),EOwnerThread);
    test(r==KErrNone);
    ldd2.Close();

	
    TRequestStatus status;
    TRequestStatus status2;
	TInt startCount;
	TInt endCount;


	{
    test.Next(_L("Random h/w use 2nd chan"));
	HBufC8 *hbuf = HBufC8::NewLC(20);
	TPtr8 buf = hbuf->Des();
	buf.SetLength(buf.MaxLength());
	ldd.Random(status,buf);
	User::WaitForRequest(status);

	test.Printf(_L("    Try 2nd chan"));
	RCryptoDriver lddB;
    r=lddB.Open();
    test(r==KErrNone);
	buf.SetLength(buf.MaxLength());
	lddB.Random(status,buf);
	User::WaitForRequest(status);

	test.Printf(_L("    Try re-using 1st chan\n"));
	buf.SetLength(buf.MaxLength());
	ldd.Random(status,buf);
	User::WaitForRequest(status);

	lddB.Close();
	CleanupStack::PopAndDestroy(hbuf);
	}
	



#ifdef RNG_TESTS
    test.Next(_L("Random (h/w)"));
#ifdef __MARM__
	HBufC8 *hbuf = HBufC8::NewLC(32768);
#else
	HBufC8 *hbuf = HBufC8::NewLC(1024);
#endif
	TPtr8 buf = hbuf->Des();
	buf.SetLength(buf.MaxLength());
	startCount = User::NTickCount();
	for(TInt i=0; i<100; ++i)
		{
		// Should discard existing contents of buf
		ldd.Random(status,buf);
		//	User::After(20);
		//	return;
		User::WaitForRequest(status);
		}
	endCount = User::NTickCount();
	test.Printf(_L("Duration in ticks (h/w) = %d\n"), endCount - startCount);
	
#ifndef __MARM__
	{
	TUint32 *p = (TUint32 *) &buf[0];
	test.Printf(_L("buf =\n    "));
	for(TInt i = 0 ; i<256; ++i)
		{
		if(i && (i%16 == 0))
			{
			test.Printf(_L("\n    "));
			}
		test.Printf(_L("%08x "), p[i]);
		}
	test.Printf(_L("\n"));
	}
	
#endif
	
	{
    test.Next(_L("Random (s/w)"));
#ifdef RNG_USE_SVR
 	RRandomSession rs;
	TRAPD(ret,rs.ConnectL());
	User::LeaveIfError(ret);
	CleanupClosePushL(rs);
#endif

	TInt startCount = User::NTickCount();
	for(TInt i=0; i<100; ++i)
		{
		// Should discard existing contents of buf
#ifdef RNG_USE_SVR
		User::LeaveIfError(rs.GetRandom(buf));
#else
		TRandom::RandomL(buf);
#endif
		}
	TInt endCount = User::NTickCount();
	test.Printf(_L("Duration in ticks (s/w) = %d\n"), endCount - startCount);
	
#ifdef RNG_USE_SVR
	CleanupStack::PopAndDestroy(&rs);
#endif
	}
	

#ifdef __MARM__
	const TInt end = 10;
#else
	const TInt end = 1;
#endif

	for(TInt i = 0 ; i<end; ++i)
		{
		test.Printf(_L("buf[%d] = 0x%x\n"), i, buf[i]);
		}
    r=status.Int();
    test(r==KErrNone);

	CleanupStack::PopAndDestroy(hbuf);

#endif

    test.Next(_L("Random - Generating key & IV for AES tests"));
	test.Printf(_L("Generating random key\n"));
	// Generate random 16 byte key
	TBuf8<KEYLEN> key;
	key.SetLength(key.MaxLength());
	ldd.Random(status, key);
	User::WaitForRequest(status);
	key[0] = 'K';
	key[1] = 'E';
	key[2] = 'Y';
	key[3] = '*';
	for(int z=4; z<KEYLEN; ++z) key[z] = z;

	test.Printf(_L("Generating random IV\n"));
	// Generate random 16 byte IV
	TBuf8<16> iv;
	iv.SetLength(iv.MaxLength());
	ldd.Random(status, iv);
	User::WaitForRequest(status);
	iv[0] = 'I';
	iv[1] = 'V';
	iv[2] = '*';
	iv[3] = '*';

	TBuf8<BUFLEN> plaintext;
	plaintext.SetLength(BUFLEN);
	plaintext[0] = 'P';
	plaintext[1] = 'L';
	plaintext[2] = 'A';
	plaintext[3] = 'I';
	plaintext[4] = 'N';
	plaintext.SetLength(BUFLEN);
	for(int i=0; i<BUFLEN; ++i)
		{
		plaintext[i] = i;
		}
	


    test.Next(_L("AES - S/W"));

#ifdef PADDING_PKCS7
	CPadding *pad = CPaddingPKCS7::NewLC(16);
#else
	CPadding *pad = CPaddingNone::NewLC(16);
#endif

#ifdef USE_CBCMODE
	// CBC
	test.Printf(_L("    CBC\n"));
	CAESEncryptor *rawaes = CAESEncryptor::NewLC(key); // pad, rawaes
	CModeCBCEncryptor *cbc = CModeCBCEncryptor::NewL(rawaes, iv);
	CleanupStack::Pop(rawaes); // pad
	CleanupStack::PushL(cbc);  // pad, cbc

	CBufferedEncryptor *aes = CBufferedEncryptor::NewL(cbc, pad);
	CleanupStack::Pop(cbc); // pad
	CleanupStack::Pop(pad); //
	CleanupStack::PushL(aes); //aes
#else
	// ECB
	test.Printf(_L("    ECB\n"));
	CAESEncryptor *rawaes = CAESEncryptor::NewLC(key); // pad, rawaes
	CBufferedEncryptor *aes = CBufferedEncryptor::NewL(rawaes, pad);
	CleanupStack::Pop(rawaes); // pad
	CleanupStack::Pop(pad); // 
	CleanupStack::PushL(aes); // aes
#endif	
	
	{
	TBuf8<10> b1;
	b1 = _L8("0123456789");
	TBuf8<6> b2;
	b2 = _L8("012345");
	TBuf8<64> b3;
	
	test.Printf(_L("Test of Process/ProcessFinal\n"));
	aes->Reset();
	aes->Process(b1,b3);
	aes->ProcessFinalL(b2, b3);
	aes->Reset();
	}
	
	test.Printf(_L("About to s/w encrypt\n"));
	TBuf8<BUFLEN> sw;
	sw.SetLength(0);
	aes->ProcessFinalL(plaintext, sw);

	test.Printf(_L("sw =\n    "));
	for(TInt i = 0 ; i<256; ++i)
		{
		if(i && (i%16 == 0))
			{
			test.Printf(_L("\n    "));
			}
		test.Printf(_L("%02x "), sw[i]);
		}
	test.Printf(_L("\n"));

	aes->Reset();
	test.Printf(_L("S/W - Encrypt %d bytes %d times\n"), BUFLEN, LOOPCOUNT);
	startCount = User::NTickCount();
	for(int z=0; z < LOOPCOUNT; ++z)
		{
		//aes->Reset();
		sw.SetLength(0);
		//aes->ProcessFinalL(plaintext, sw);
		aes->Process(plaintext, sw);
		}
	endCount = User::NTickCount();
	test.Printf(_L("Duration in ticks (s/w) = %d\n"), endCount - startCount);


	CleanupStack::PopAndDestroy(aes);

    test.Next(_L("AES - H/W"));

	
	test.Printf(_L("Setting config\n"));
#ifdef USE_CBCMODE
	r = ldd.SetAesConfig(ETrue, RCryptoDriver::ECbcMode, key, iv);
	test.Printf(_L("    CBC\n"));
#else
	r = ldd.SetAesConfig(ETrue, RCryptoDriver::EEcbMode, key, iv);
	test.Printf(_L("    ECB\n"));
#endif
	test(r==KErrNone);
	

	test.Printf(_L("H/W - Encrypt %d bytes %d times\n"), BUFLEN, LOOPCOUNT);
	TBuf8<BUFLEN> tmp2;
	startCount = User::NTickCount();
	for(int z=0; z < LOOPCOUNT; ++z)
		{
		tmp2.SetLength(0);
		ldd.AesRead(status2, tmp2, tmp2.MaxLength());
		ldd.AesWrite(status, plaintext);
		User::WaitForRequest(status);	
		User::WaitForRequest(status2);
#ifdef DISABLE_AES_CHECKS
		tmp2.SetLength(BUFLEN);
#endif
		}
	endCount = User::NTickCount();
	test.Printf(_L("Duration in ticks (h/w) = %d\n"), endCount - startCount);

	
#ifndef DISABLE_AES_CHECKS
#ifdef __MARM__
	test.Printf(_L("Compare tmp2 and sw\n"));
	test(tmp2==sw);
#else
	test.Printf(_L("Compare tmp2 and plaintext\n"));
    test(tmp2==plaintext);
#endif
#endif





//	User::Panic(_L("Fake app crash"), 42);

    test.Next(_L("Close Logical Channel"));
    ldd.Close();

    __KHEAP_MARKEND;

    test.Next(_L("Unload Logical Device"));
    r=User::FreeLogicalDevice(RCryptoDriver::Name());
    test(r==KErrNone);

    test.Next(_L("Unload Physical Device"));
    TName pddName(RCryptoDriver::Name());
    _LIT(KVariantExtension,".h4");
    pddName.Append(KVariantExtension);
    r=User::FreePhysicalDevice(pddName);
    test(r==KErrNone);

    test.End();

	test.Printf(KTxtPressAnyKey);
	test.Getch(); // get and ignore character
	test.Close();
    }


// End of file
