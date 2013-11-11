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
 @file
 @released
*/

#ifndef __T_BUR_TEST_SERVER_H__
#define __T_BUR_TEST_SERVER_H__

#include <test/testexecuteserverbase.h>

namespace bur_ts
	{
	
	class CBURTestServer : public CTestServer
		/**
		CBURTestServer implements the Backup and Restore Test Server. The Test Server is based
		on the TestExecute Framework (TEF).
		
		When running the Test Server, the TestExecute scipts must specify:
			
			LOAD_SUITE BURTestServer
		
		This server handles two types of test steps only:\n
			- TestBackup\n
			- TestRestore
		
		Each test step must carry an ini file listing parameters:\n
			1. BackupDir=full path of the directory\n
				The directory to be used for
					- storing the backup data during a backup operation, or for
					- retrieving the backup data from during a restore.
			
			2. IsPartial=<[0|1]>\n
				Whether to carry out a partial backup and restore or a full one.
			
			3. IsIncremental=<[0|1]>\n
				Whether to carry out a base back up and restore or an incremental one.
			
			4. DriveList=a list of drive letters without spaces\n
				The drives to be backed up.
		
		This ini file must be specifed in the test script for each indiviudal test step, e.g.,
			
			RUN_TEST_STEP 100 BURTestServer TestBackup c:\\TestScripts\\mySettings.ini
		
		@released
		@internalComponent
		*/
		{
	
	public:
		static CBURTestServer* NewL();
		~CBURTestServer();
		
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		};
	}

#endif // __T_BUR_TEST_SERVER_H__
