// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:

// __EDIT_ME__ Create your own class definition based on this
#if (!defined __TE_comsdbg_SERVER_H__)
#define __TE_comsdbg_SERVER_H__
#include <test/testexecuteserverbase.h>

class CTE_comsdbgServer : public CTestServer
	{
public:
	static CTE_comsdbgServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	void InitDir();
	TPtrC GetVersion( void );
	};

//Filename constants
_LIT(KFloggerOutputFile,"c:\\logs\\log.txt"); // log file name and path

_LIT(KFloggerIniFile,"c:\\logs\\commsdbg.ini");
_LIT(KTempDuringCreationFloggerIniFile,"c:\\logs\\floggercreate.ini");
_LIT(KTempPreserveFloggerIniFile,"c:\\logs\\tempflogger.ini");
_LIT(KFloggerTestIniMediaSourceFile,"c:\\logs\\ts_flogger.ini");

_LIT(KFloggerOutputDir,"C:\\logs");
_LIT(KTestResultsDir,"c:\\logs\\TestResults\\");

#endif
