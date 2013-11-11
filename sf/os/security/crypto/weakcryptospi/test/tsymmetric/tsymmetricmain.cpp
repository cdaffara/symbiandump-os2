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
*
*/


#include <e32base.h>
#include "t_testhandler.h"
#include "t_testsetup.h"
#include "tScriptTests.h"
#include "tactionvector.h"
#include "tperformancetest.h"
#include "tactionincremental.h"
#include "tactionmontecarlo.h"
#include "tactionincrementallegacy.h"

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
{
    START_SCRIPT_LIST
	SCRIPT_ITEM(CActionVector,_L8("Vector")),
	SCRIPT_ITEM(CPerformanceTest,_L8("Performance")),
	SCRIPT_ITEM(CActionIncremental,_L8("Incremental")),	
	SCRIPT_ITEM(CActionMonteCarlo, _L8("MonteCarlo")),
	SCRIPT_ITEM(CActionIncrementalLegacy,_L8("IncrementalLegacy"))
	END_SCRIPT_LIST
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<64> scriptFile (sysDriveName);
	scriptFile.Append(_L("\\tsymmetric\\tsymmetrictests.txt"));
	
	TBuf<64> logFile (sysDriveName);
	logFile.Append(_L("\\tsymmetric\\tsymmetrictests.log"));
	
	
//	RC2, RC4, DES, 3DES (ECB and CBC modes)
/*	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\symmetricOOMtests.txt"),
		_L("c:\\tsymmetric\\symmetricOOMtests.log"));*/
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);

//	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\tsymmetricperformancetests.txt"),
//		_L("c:\\tsymmetric\\tsymmetricperformancetests.log"));

//	There are so many other test scripts because it is impossible to put all the test
//	data in one big script.  The test framework just falls over with out of memory because
//	it attempts to create all test objects at the start of the test.  All these tests
//	are (of course) necessary hence they've been split.  Either run the top level build
//	and test script or, if running this test specifically, uncomment the following and it'll run them
//	all one after the other	
//	AES (ECB) using Rijndael vectors for all table values	
/*	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESECBVectorsScript.txt"),
		_L("c:\\tsymmetric\\aesecb_vectors.log"));

//	AES (ECB) KAT using variable text, known key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESECB_KAT_VT.txt"),
		_L("c:\\tsymmetric\\aesecb_kat_vt.log"));
 
//	AES (ECB) KAT using variable key, known text
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESECB_KAT_VK.txt"),
		_L("c:\\tsymmetric\\aesecb_kat_vk.log"));

//	AES Monte Carlo tests (ECB encrypt) with 128 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloEncryptECB128.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_encryptECB128.log"));

//	AES Monte Carlo tests (ECB encrypt) with 192 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloEncryptECB192.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_encryptECB192.log"));

//	AES Monte Carlo tests (ECB encrypt) with 256 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloEncryptECB256.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_encryptECB256.log"));

//	AES Monte Carlo tests (ECB decrypt) with 128 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloDecryptECB128.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_decryptECB128.log"));

//	AES Monte Carlo tests (ECB decrypt) with 192 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloDecryptECB192.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_decryptECB192.log"));

//	AES Monte Carlo tests (ECB decrypt) with 256 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloDecryptECB256.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_decryptECB256.log"));

//	AES Monte Carlo tests (CBC encrypt) with 128 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloEncryptCBC128.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_encryptCBC128.log"));

//	AES Monte Carlo tests (CBC encrypt) with 192 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloEncryptCBC192.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_encryptCBC192.log"));

//	AES Monte Carlo tests (CBC encrypt) with 256 bit key	
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloEncryptCBC256.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_encryptCBC256.log"));

//	AES Monte Carlo tests (CBC decrypt) with 128 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloDecryptCBC128.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_decryptCBC128.log"));

//	AES Monte Carlo tests (CBC decrypt) with 192 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloDecryptCBC192.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_decryptCBC192.log"));

//	AES Monte Carlo tests (CBC decrypt) with 256 bit key
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\AESMonteCarloDecryptCBC256.txt"),
		_L("c:\\tsymmetric\\aesmontecarlo_decryptCBC256.log"));

	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tsymmetric\\symmetricOOMtests.txt"),
		_L("c:\\tsymmetric\\symmetricOOMtests.log"));
*/}

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, callExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(_L("tsymmetric"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
