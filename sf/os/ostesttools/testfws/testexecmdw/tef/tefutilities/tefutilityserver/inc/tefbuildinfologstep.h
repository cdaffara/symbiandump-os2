/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CTestStep derived test step class to print OS build version to log file output
*
*/



/**
 @file TEFBuildInfoLogStep.h
*/

#if (!defined __TEF_BUILD_INFO_LOG_STEP_H__)
#define __TEF_BUILD_INFO_LOG_STEP_H__


#include <test/testexecutestepbase.h>
#include <e32std.h>
#include <f32file.h>
#if !(defined TEF_LITE)
#include <bautils.h>
#endif
_LIT(KPrintBuildInfo,"PrintBuildInfo");
_LIT(KBuildInfoFilePath,"?:\\TEFUtilityServer\\data\\BuildInfo.txt");
_LIT8(KBuildNumberFlag,"ManufacturerSoftwareBuild");

const TInt KOffsetToBuildNumber = 27; // Length of ManufacturerSoftwareBuild + 2 spaces 
const TInt KLengthOfBuildNumber = 5; // Length of current build number

class CTEFBuildInfoLogStep : public CTestStep
{
public:
	CTEFBuildInfoLogStep();
public:
	virtual	TVerdict	doTestStepL();
	virtual	TVerdict	doTestStepPreambleL();
	virtual	TVerdict	doTestStepPostambleL();

private:
	RFs		iFs;
	// Keeps the status of the file server session object
	TBool	iSessionConnected;	
};

#endif
