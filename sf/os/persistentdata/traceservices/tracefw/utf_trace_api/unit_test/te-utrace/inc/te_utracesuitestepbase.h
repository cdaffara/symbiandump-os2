/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 @file Te_UTraceSuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_UTRACE_STEP_BASE__)
#define __TE_UTRACE_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <e32utrace.h>
#include "te_btracecmds.h"
// Please add your include here if you have

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables
* and members
* and this is the place to define these common variable and members.
*
****************************************************************************/
_LIT16(KName16,"A _LIT16 string with integer parameter %i !");
_LIT8(KName8,"A _LIT8 string with integer parameter %i !");
_LIT16(KAny16,"TAny16 buffer!");
_LIT8(KAny8,"TAny8 buffer!");
_LIT(KLogFileName, "c:\\Logs\\testexecute\\utrace.btl");
_LIT(KLogUBenchFileName, "c:\\Logs\\testexecute\\ubench.btl");
_LIT(KLogKBenchFileName, "c:\\Logs\\testexecute\\kbench.btl");


class CTe_UTraceSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_UTraceSuiteStepBase();
	CTe_UTraceSuiteStepBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void ReplaceFile(const TDesC& aFilename);
	void OpenFile(const TDesC& aFilename);
//Please add your class members which will be common to all individual test steps:
protected:
	TInt GetConfigs();
	TInt StartLogging();
	TInt StopLogging();

	HBufC8*		iReadData;
	HBufC8*		iWriteData;

	TPrimaryFilter iPF1;
	TPrimaryFilter iPF2;
	TSecondaryFilter iSF1;
	TBool iLoggingEnabled;
	TBool iTraceEnabled;

	TBuf16<0x100> iBuf16;
	TBuf8<0x100> iBuf8;
	TBuf16<0x100> iAny16;
	TBuf8<0x100> iAny8;
	TUint32 iA1;
	TUint32 iA2;
	TUint32 iA3;
	TUint32 iUid;
	TUint32 iTid;
	TUint32 iUidReal;

	Tracer iTracer;
	TPtrC iFilename;
	};

#endif
