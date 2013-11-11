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


#ifndef __T_SCRIPTSETUP_H__
#define __T_SCRIPTSETUP_H__

#include "t_testsetup.h"
#include "tScriptTests.h"

//test setup base classes
_LIT8(KActionStart, "<action>");
_LIT8(KActionEnd, "</action>");
_LIT8(KActionNameStart, "<actionname>");
_LIT8(KActionNameEnd, "</actionname>");
_LIT8(KActionTypeStart, "<actiontype>");
_LIT8(KActionTypeEnd, "</actiontype>");
_LIT8(KActionBodyStart, "<actionbody>");
_LIT8(KActionBodyEnd, "</actionbody>");
_LIT8(KActionResultStart, "<actionresult>");
_LIT8(KActionResultEnd, "</actionresult>");
_LIT8(KActionGroupingStart, "<actiongrouping>");
_LIT8(KActionGroupingEnd, "</actiongrouping>");
_LIT8(KKnownDefectStart, "<knowndefect>");
_LIT8(KKnownDefectEnd, "</knowndefect>");
_LIT8(KRunTestStep, "RUN_TEST_STEP");
_LIT8(KStartTestStep, "START_TESTCASE");
_LIT8(KEndTestStep, "END_TESTCASE");

class CTestAction;
class TTestActionSpec;
class CTestSetup;
class CTestSpec;


class CScriptSetup : public CTestSetup
	{
public:
	IMPORT_C static CScriptSetup* NewLC(CConsoleBase* aConsole);

public:
	//aTestSpec is the class that holds the array of test actions, and has the GetNextTest function
	//theTestTypes is the static array of test types 
	//aCommandLineSettings contains the command line parameters
	IMPORT_C virtual void SetupTestsL(RFs& aFs,	CTestSpec& aTestSpec, TScriptTests theTestTypes[],
			const CTestHandlerSettings& aCommandLineSettings);
	IMPORT_C ~CScriptSetup();

	IMPORT_C TBool InitialiseL(RFs &aFs, const TDesC& aDefaultScript = KNullDesC, const TDesC& aDefaultLog = KNullDesC, TBool aUseCommandLine = ETrue);
	HBufC* iScriptPath;
protected:
	CScriptSetup(CConsoleBase* aConsole);

protected:
	HBufC8* iTestInput;
	HBufC8* scriptResult;

	TBool	iTefScript;

protected:
	IMPORT_C virtual CTestAction* CreateActionL(RFs& aFs, const TTestActionSpec& aTestActionSpec, 
		const TScriptTests theTestArray[]) const;
	};

#endif
