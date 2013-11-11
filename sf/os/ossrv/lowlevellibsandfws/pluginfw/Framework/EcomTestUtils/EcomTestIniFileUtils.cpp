// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32base.h>
#include <bautils.h>
#include <f32file.h>
#include "../EcomTestUtils/EcomTestIniFileUtils.h"
#include "../EcomTestUtils/EcomTestUtils.h"

_LIT(KCDriveIniFileName, "c:\\private\\10009D8F\\EComSrvr.ini");
_LIT(KZDriveIniFileName, "z:\\private\\10009D8F\\EComSrvr.ini");
_LIT(KIniBackupFileName, "c:\\private\\10009D8F\\EComSrvr_backup.ini");
_LIT(KIniFileName,       "z:\\test\\data\\EComSrvr.ini");


//
//
//Helper functions
//
//

/**
Disable Ssa by using the EComSrvr_SsaDisabled.ini
The function will make a backup of the existing EComSrvr.ini if there is one.
@param aTest the RTest that this method is called from
@param	aFs A reference to an connected file server session.
*/
void DisableSsa(RTest& aTest, RFs& /*aFs*/)
	{
	TInt err = KErrNone;
	TParse iniFileName;
	TParse backupIniFileName;
	TParse tempFileName;

	//check if ecomsrvr.ini file exists. If so make a backup
	iniFileName.Set(KCDriveIniFileName, NULL, NULL);
	backupIniFileName.Set(KIniBackupFileName, NULL, NULL);
	TRAP(err, EComTestUtils::RfsReplaceFileL(
					iniFileName.FullName(),
					backupIniFileName.FullName()));
	if(err != KErrNotFound) //it is ok if the file does not exist
		{
		TESTC(aTest, err, KErrNone);
		}
					
	//copy ecomsrvr.ini to disable ssa
	tempFileName.Set(KIniFileName, NULL, NULL);
	TRAP(err, EComTestUtils::FileManCopyFileL(
					tempFileName.FullName(),
	                iniFileName.FullName()));
	TESTC(aTest, err, KErrNone);
	}
	
/**
Enable Ssa by removing EComSrvr.ini if there is one
The function will make a backup of the existing EComSrvr.ini if there is one.
@param aTest the RTest that this method is called from
@param	aFs A reference to an connected file server session.
*/
void EnableSsa(RTest& aTest, RFs& /*aFs*/)
	{
	TInt err = KErrNone;
	TParse iniFileName;
	TParse backupIniFileName;
	TParse tempFileName;

	//check if ecomsrvr.ini file exists. If so make a backup
	iniFileName.Set(KCDriveIniFileName, NULL, NULL);
	backupIniFileName.Set(KIniBackupFileName, NULL, NULL);
	TRAP(err, EComTestUtils::RfsReplaceFileL(
					iniFileName.FullName(),
					backupIniFileName.FullName()));
	if(err != KErrNotFound) //it is ok if the file does not exist
		{
		TESTC(aTest, err, KErrNone);
		}
	}

/**
Reset the ssa by replacing the ini file with the backup if
a backup was created in the first place
@param aTest the RTest that this method is called from
@param	aFs A reference to an connected file server session.
*/
void ResetSsa(RTest& aTest, RFs& /*aFs*/)
	{
	TInt err = KErrNone;
	TParse iniFileName;
	TParse backupIniFileName;

	iniFileName.Set(KCDriveIniFileName, NULL, NULL);
	backupIniFileName.Set(KIniBackupFileName, NULL, NULL);
	
	//remove ecomsrvr.ini
	TRAP(err, EComTestUtils::RfsDeleteFileL(iniFileName.FullName()));
	TESTC(aTest, err, KErrNone);

	//if the ecomsrvr.ini file initially existed put it back using 
	//the backup
	TRAP(err, EComTestUtils::RfsReplaceFileL(
					backupIniFileName.FullName(),
					iniFileName.FullName()));
	if(err != KErrNotFound) //it is ok if the file does not exist
		{
		TESTC(aTest, err, KErrNone);
		}
	}

/**
Cheks if z:\\private\\10009D8F\\EComSrvr.ini exists. If it does
then fails the test to ensure the following tests are not run.
@param aTest the RTest that this method is called from
@param	aFs A reference to an connected file server session.
*/
void TestEnableDisableSsaL(RTest& aTest, RFs& aFs)
	{
	TParse iniFileName;
	iniFileName.Set(KZDriveIniFileName, NULL, NULL);
	TBool isFileExisting = BaflUtils::FileExists(aFs, iniFileName.FullName());
	
	if(isFileExisting)
		{
		//If the file already exists on z: drive then the test that calls 
		//this method cannot be executed. Probably the test is trying to 
		//disable/enable SSA behaviour using the c: drive location. Since
		//z:drive has higher priority it will never be possible to enable
		//SSA behaviour.
		TESTC(aTest, isFileExisting, EFalse);
		RDebug::Print(_L("EcomSrvr.ini file exists in z: drive. This test cannot be executed.\n"));
		}
	}
