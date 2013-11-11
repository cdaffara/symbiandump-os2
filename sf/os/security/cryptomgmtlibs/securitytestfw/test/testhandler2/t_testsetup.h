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


#ifndef __T_TESTSETUP_H__
#define __T_TESTSETUP_H__

//The test groupings. Defined here to save multiple declarations in tScriptTests.h and tHardcodedTests.h
#define EXOOM		1	//Do not run if doing OOM testing
#define INOOM		2	//Run only if doing OOM testing
#define EXCANCEL	4	//Do not run if doing cancellation testing
#define INCANCEL	8	//Run only if doing cancellation testing
#define SKIP		16	//Do not run unless the "-s" is specified
#define SMOKE		32	//Run only if "-e" is specified
#define INTER		64	//Run only if "-i" is specified
#define DEFAULTGROUPING		SMOKE

#include <e32base.h>
#include "t_output.h"

class CTestHandlerSettings;
struct TScriptTests;
struct THardcodedTests;

class CTestSetup : public CBase
	{
public:
	IMPORT_C ~CTestSetup();

	/**
	 * Run the tests described in a script.
	 *
	 * @param aUseCommandLine - if true (the default), use any script and log
	 * files specified on the commandline in preference to aScript and aLogFile.
	 * @param aResult - boolean which gets set to indicate the result
	 * @param aConsole - specifies a user defined console. NULL if we the 
	 * test setup creates one
	 */
	
	IMPORT_C static void CreateAndRunTestsL(TScriptTests theTestTypes[], const TDesC& aScript, const TDesC& aLogFile, 
		TBool aUseCommandline = ETrue, CConsoleBase* aConsole = NULL, TBool* aResult = NULL);
	IMPORT_C static void CreateAndRunTestsL(THardcodedTests theTestTypes[], const TDesC& aDefaultLog);

	inline CConsoleBase& Console(void) { return(*iConsole);}
	inline Output& LogFile(void) { return(*iLogFile);}

protected:
	CTestSetup(CConsoleBase* aConsole);
	void ConstructL();
	void OpenLogFileL(RFs &aFs, TInt nPos, const TDesC &aDefaultLog, TBool aUseCommandline = ETrue);

protected:
	/// A console that the test framework uses
	CConsoleBase* iConsole;
	/// Do we own iConsole?
	TBool iConsoleOwned;
	/// Interface for writing to the log file
	Output* iLogFile;
	/// A condole that the test actions write to
	CConsoleBase* iTestConsole;

protected:
	TBool CheckAllFlags(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings);
	TBool CheckExhaustiveandSmoke(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings);
	TBool CheckSkipped(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings);
	TBool CheckInteractive(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings);
	TBool CheckOOMandCancel(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings);

	IMPORT_C static HBufC* GetArgument(TInt nPos=0);
private:
	static void InitFileserverSessionLC(RFs& aFs);
	};

#endif
