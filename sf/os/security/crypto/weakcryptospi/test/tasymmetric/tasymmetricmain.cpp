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
#include "tScriptTests.h"
#include "trsaencryptfb.h"
#include "trsasignfb.h"
#include "tdsasignfb.h"
#include "trsavector.h"
#include "tdsavector.h"
#include "tdsasignfb.h"
#include "tdsaprimegen.h"
#include "performancetest.h"
#include "tdhvector.h"
#include "trsaparams.h"
#include "tasymmetricmisc.h"

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
	START_SCRIPT_LIST
	SCRIPT_ITEM(CRSAEncryptFB,_L8("RSAEncryptStandardAndCRTFB")),
	SCRIPT_ITEM(CRSASignFB,_L8("RSASignStandardAndCRTFB")),
	SCRIPT_ITEM(CDSASignFB,_L8("DSASignFB")),
	SCRIPT_ITEM(CRSAEncryptVector,_L8("RSAEncryptVector")),
	SCRIPT_ITEM(CRSADecryptVector,_L8("RSADecryptVector")),
	SCRIPT_ITEM(CRSADecryptVectorCRT,_L8("RSADecryptVectorCRT")),
	SCRIPT_ITEM(CRSASignVector,_L8("RSASignVector")),
	SCRIPT_ITEM(CRSASignVectorCRT,_L8("RSASignVectorCRT")),
	SCRIPT_ITEM(CRSAVerifyVector,_L8("RSAVerifyVector")),
	SCRIPT_ITEM(CDSASignVector,_L8("DSASignVector")),
	SCRIPT_ITEM(CDSAVerifyVector,_L8("DSAVerifyVector")),
	SCRIPT_ITEM(CDSASignFB,_L8("DSASignFB")),
	SCRIPT_ITEM(CDSAPrimeGen,_L8("DSAPrimeGen")),
	SCRIPT_ITEM(CPerformanceTest, _L8("PerformanceTestSettings")),
	SCRIPT_ITEM(CDHVector,_L8("DHVector")),
	SCRIPT_ITEM(CRSATestPublicKey,_L8("RSAPublicKeyParams")),
	SCRIPT_ITEM(CRSATestPrivateKey,_L8("RSAPrivateKeyParams")),
	SCRIPT_ITEM(CRSATestPrivateKeyCRT,_L8("RSAPrivateKeyCRTParams")),
	SCRIPT_ITEM(CAsymmetricMiscellaneous,_L8("AsymmetricMiscellaneous"))
	END_SCRIPT_LIST

//	Either run the top level build and test script or, if running this test specifically, 
//	uncomment the following and it'll run them all one after the other

	//get the system drive.	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> scriptFile = sysDrive.Name();
	scriptFile.Append(_L("\\tasymmetric\\tasymmetrictests.txt"));
	TBuf<64> logFile = sysDrive.Name();
	logFile.Append(_L("\\tasymmetric\\tasymmetrictests.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);
	
/* If you want to run all of the asymmetric tests from the command
 * line, uncomment this next section.
#ifndef _DEBUG
	CTestSetup::CreateAndRunTestsL(theTestTypes, _L("c:\\tasymmetric\\asymmetricPerformance.txt"),
		_L("c:\\tasymmetric\\asymmetricPerformance.log"));	
#endif
*/
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	CPerformance::NewL();
	TRAPD(error, callExampleL());
	CPerformance::ClosePerformanceTester();
	__ASSERT_ALWAYS(!error,User::Panic(_L("tasymmetricmain"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
