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
* CTestStep derived class implementation to print OS build version to log file output
*
*/



/**
 @file TEFBuildInfoLogStep.cpp
*/

#include "tefbuildinfologstep.h"
#include <test/wrapperutilsplugin.h>
#include <test/tefutils.h>

CTEFBuildInfoLogStep::CTEFBuildInfoLogStep()
	{
	SetTestStepName(KPrintBuildInfo);
	}

/**
 * Run preamble scripts for the test
 * Make the connection with the File Server and creates a session object
 * @return - TVerdict code
 * Override of base class pure virtual
 */
TVerdict CTEFBuildInfoLogStep::doTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	iSessionConnected=ETrue;
	return TestStepResult();
	}

/**
 * Run postample scripts for the test
 * @return - TVerdict code
 * Override of base class pure virtual
 */
TVerdict CTEFBuildInfoLogStep::doTestStepPostambleL()
	{
	iFs.Close();
	iSessionConnected=EFalse;
	return TestStepResult();
	}

/**
 * Overrides base class virtual
 * @return - TVerdict codes
 * Parses buildinfo.txt from \epoc32\... and prints it to TEF log file outout
 */
TVerdict CTEFBuildInfoLogStep::doTestStepL()
	{
	const TInt KBuildVersionMaxLength = 128;
	const TInt KBuildInfoFileMaxLength = 256;

	RFs fileServer;
	TVersionName version(fileServer.Version().Name());
	TDriveName defaultSysDrive(_L("C:"));	
	RLibrary pluginLibrary;
	CWrapperUtilsPlugin* plugin = TEFUtils::WrapperPluginNew(pluginLibrary);
	
	if (plugin!=NULL)
		{
		TDriveUnit driveUnit(plugin->GetSystemDrive());
		defaultSysDrive.Copy(driveUnit.Name());
		delete plugin;
		pluginLibrary.Close();
		}

	TBuf<KBuildVersionMaxLength> text;
	TInt startOfData;
	TBuf8<KBuildInfoFileMaxLength> buf;

	TFileName buildInfoFilePath(KBuildInfoFilePath);
	buildInfoFilePath.Replace(0, 2, defaultSysDrive);

	SetTestStepResult(EFail);
	if(BaflUtils::FileExists(iFs,buildInfoFilePath))
		{
		if (!(iFs.IsFileOpen(buildInfoFilePath,iSessionConnected)))
			{													
			RFile file;			
			User::LeaveIfError(file.Open(iFs, buildInfoFilePath, EFileShareAny));							
			User::LeaveIfError(file.Read(buf));
			startOfData = buf.Find(KBuildNumberFlag);
			if(!(startOfData==KErrNotFound))  // if build number flag present
				{
				startOfData += KOffsetToBuildNumber;
				if((KLengthOfBuildNumber+startOfData)<=buf.Length())  // if build number present
					{
					text.Copy(buf.Mid(startOfData));
					INFO_PRINTF2(_L("Build version is %S"),&text);
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("Build number is not specified along with ManufacturerSoftwareBuild flag"));
					}
				}
			else
				{
				INFO_PRINTF1(_L("ManufacturerSoftwareBuild flag is not found in the buildinfo.txt file"));
				}
			file.Close();
			}	
		}
	else 
		{
		INFO_PRINTF2(_L("%S -  file not found"), &buildInfoFilePath);
		}
	return TestStepResult();
	}
