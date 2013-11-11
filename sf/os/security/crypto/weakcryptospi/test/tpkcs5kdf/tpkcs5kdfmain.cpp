/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* tpkcs5main.cpp
*
*/


#include <pkcs5kdf.h>
#include <pkcs12kdf.h>
#include "t_testhandler.h"
#include "t_testsetup.h"
#include "tScriptTests.h"
#include "tactionderivekey.h"

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
    START_SCRIPT_LIST
	SCRIPT_ITEM(CActionDeriveKey,_L8("DeriveKey"))
	END_SCRIPT_LIST
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<64> scriptFile (sysDriveName);
	scriptFile.Append(_L("\\tpkcs5kdf\\tpkcs12kdftests.txt"));
	
	TBuf<64> logFile (sysDriveName);
	logFile.Append(_L("\\tpkcs5kdf\\tpkcs12kdftests.log"));
	
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);
    }


GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	TRAPD(error, callExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(_L("tpkcs5"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }

// The following code was used to generate test data.  It is left in
// the source file because it provides an easy way to generate a large
// number of PKCS#12 derived keys if required.

#if 0

static void TestPkcs12KdfL();
static void TestPasswordL(
	TInt aKeyLenInBits, TInt aIterCount, const TDesC8& aSalt,
	const TDesC8& aPassword, const TDesC& aPasswordText);
static void PrintKey(const TDesC8& aKey);

static void TestPkcs12KdfL()
	{
	__UHEAP_MARK;
	
	const TInt KKeyLenCount = 4;
	const TInt KKeyLens[KKeyLenCount] = {40, 128, 168, 368};
	
	const TInt KIterCount = 8;
	const TInt KIterCounts[KIterCount] = {1, 2, 4, 8, 128, 1024, 1536, 2048};
	
	const TInt KSaltCount = 3;
	TPtrC8 salts[KSaltCount];
	
	const TUint8 KSalt4[4] = {0x53, 0x41, 0x4c, 0x54};
	salts[0].Set(KSalt4, 4);
	
	const TUint8 KSalt20[20] =
		{
		0x1d, 0x56, 0x50, 0x78,		0xc3, 0x50, 0x6f, 0x89,
		0xbd, 0xa7, 0x3b, 0xb6,		0xe3, 0xe5, 0xb8, 0xa3,
		0x68, 0x3d, 0xd3, 0x62
		};
	salts[1].Set(KSalt20, 20);
	
	const TUint8 KSalt25[25] =
		{
		0xe2, 0x2c, 0x7b, 0x03,		0x16, 0x3a, 0xe5, 0x47,
		0xf8, 0x23, 0x9d, 0xa4,		0x0d, 0x6f, 0x46, 0xd7,
		0x9e, 0xa3, 0xc6, 0xff,		0xb3, 0xf0, 0x4e, 0xbe,
		0x61
		};
	salts[2].Set(KSalt25, 25);
	
	const TInt KPasswordCount = 5;
	HBufC8* passwords[KPasswordCount];
	TPtrC passwords0[KPasswordCount] =
		{
		_L("0000"), _L("0001"), _L("PSWD"),
		_L("password"), _L("abcdefghijklmnopqrstuvwxyz")
		};
	for (TInt i = 0; i < KPasswordCount; ++i)
		passwords[i] = PKCS12KDF::GeneratePasswordLC(passwords0[i]);

	for (TInt klenIdx = 0; klenIdx < KKeyLenCount; ++klenIdx)
		{
		for (TInt iterIdx = 0; iterIdx < KIterCount; ++iterIdx)
			{
			for (TInt saltIdx = 0; saltIdx < KSaltCount; ++saltIdx)
				{
				for (TInt pwdIdx = 0; pwdIdx < KPasswordCount; ++pwdIdx)
					{
					TestPasswordL(KKeyLens[klenIdx], KIterCounts[iterIdx], salts[saltIdx], *passwords[pwdIdx], passwords0[pwdIdx]);
					}
				}	// for (saltIdx = 0; saltIdx < KSaltCount; ++saltIdx)
			}	// for (int iterIdx = 0; iterIdx < KIterCount; ++iterIdx)
		}	// for (TInt klenIdx = 0; klenIdx < KKeyLenCount; ++klenIdx)
	
	CleanupStack::PopAndDestroy(KPasswordCount, passwords[0]);
	
	__UHEAP_MARKEND;
	}

static void TestPasswordL(
	TInt aKeyLenInBits, TInt aIterCount, const TDesC8& aSalt,
	const TDesC8& aPassword, const TDesC& aPasswordText)
	{
	__UHEAP_MARK;
	
	TBuf8<512> key;
	key.SetLength(aKeyLenInBits / 8);

	RDebug::Print(
		_L("\nkey len = %d, iter count = %d, password = \"%S\", salt len = %d"),
		aKeyLenInBits, aIterCount, &aPasswordText, aSalt.Length());
	PKCS12KDF::DeriveKeyL(key, PKCS12KDF::EIDByteEncryptKey, aPassword, aSalt, aIterCount);
	PrintKey(key);
	
	__UHEAP_MARKEND;
	}

static void PrintKey(const TDesC8& aKey)
/**
	Print the supplied key in hex byte format, with
	16 bytes on each row.
	
	@param	aKey			Key to print.
 */
	{
	const TInt KBlockSize = 16;
	TInt keyLen = aKey.Length();
	TInt rowCount = keyLen / KBlockSize;
	if ((keyLen % KBlockSize) != 0)
		++rowCount;

	for (TInt row = 0; row < rowCount; ++row)
		{
		TInt start = row * KBlockSize;
		TInt end = Min(start + KBlockSize, keyLen);

		TBuf<64> line;
		line.Format(_L("[%04x]"), start);
		for (TInt i = start; i < end; ++i)
			line.AppendFormat(_L(" %02x"), aKey[i]);
		line.Append('\n');
		RDebug::Print(line);
		}
	}

#endif	// #if 0

