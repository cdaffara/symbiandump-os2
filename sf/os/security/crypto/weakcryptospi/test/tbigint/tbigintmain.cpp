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
#include "t_testsetup.h"
#include "t_input.h"
#include "t_output.h"
#include "tbigint.h"
#include "tconstructionfb.h"
#include "tconstructionvector.h"
#include "tbasicmathsvector.h"
#include "tbasicmathsfb.h"
#include "tmontgomeryvector.h"
#include "tmontgomeryfb.h"
#include "tprimevector.h"
#include "tmontgomeryperformance.h"
#include "tprimegenperformance.h"
#include "tprimevectorperformance.h"
#include "tprimegen.h"
#include "tprimegenvector.h"
#include <random.h>

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
    START_SCRIPT_LIST
	SCRIPT_ITEM(CMontgomeryPerformance,_L8("MontgomeryPerformance")),
	SCRIPT_ITEM(CPrimeVector,_L8("PrimeVector")),
	SCRIPT_ITEM(CConstructionFB,_L8("ConstructionFB")),
	SCRIPT_ITEM(CConstructionVector,_L8("ConstructionVector")),
	SCRIPT_ITEM(CBasicMathsVector,_L8("BasicMathsVector")),
	SCRIPT_ITEM(CBasicMathsFB,_L8("BasicMathsFB")),
	SCRIPT_ITEM(CMontgomeryVector,_L8("MontgomeryVector")),
	SCRIPT_ITEM(CMontgomeryFB,_L8("MontgomeryFB")),
	SCRIPT_ITEM(CPrimeGenPerformance,_L8("PrimeGenPerformance")),
	SCRIPT_ITEM(CPrimeVectorPerformance,_L8("PrimeVectorPerformance")),
	SCRIPT_ITEM(CPrimeGen,_L8("PrimeGen")),
	SCRIPT_ITEM(CPrimeGenVector,_L8("PrimeGenVector"))
	END_SCRIPT_LIST

//	Either run the top level build and test script or, if running this test specifically, 
//	uncomment the following and it'll run them all one after the other

    CSystemRandom *rng = CSystemRandom::NewL();
    SetThreadRandomLC(rng);

	TDriveUnit sysDrive (static_cast <TInt> (RFs::GetSystemDrive()));
	TBuf<64> scriptFile = sysDrive.Name();
	scriptFile.Append(_L("\\tbigint\\tbasicmathstests.txt"));
	TBuf<64> logFile = sysDrive.Name();
	logFile.Append(_L("\\tbigint\\tbasicmaths.log"));
	
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);
	
/*	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tconstructiontests.txt"),
		_L("c:\\tbigint\\tconstruction.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tprimegen.txt"),
		_L("c:\\tbigint\\tprimegen.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tprimegenperformance.txt"),
		_L("c:\\tbigint\\tprimegenperformance.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tmontgomerytests.txt"),
		_L("c:\\tbigint\\tmontgomery.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tperformancetests.txt"),
		_L("c:\\tbigint\\tperformance.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tprimetests.txt"),
		_L("c:\\tbigint\\tprime.log"));
	CTestSetup::CreateAndRunTestsL(theTestTypes, 
		_L("c:\\tbigint\\tprimetests2.txt"),
		_L("c:\\tbigint\\tprime2.log"));
*/
	
	DestroyThreadRandom();
	CleanupStack::Pop(); // Pop the cleanup item which would have reset/delete the threadrandom
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, callExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(_L("bigint"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
