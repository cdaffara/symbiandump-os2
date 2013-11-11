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
* T_TestStepBackup.h
* 
*
*/



/**
 @file
 @released
*/

#ifndef T_TESTSTEPBACKUPASYNC_H
#define T_TESTSTEPBACKUPASYNC_H

#include "t_burteststepbase.h"
#include "t_asyncbackuptransferhandler.h"

_LIT(KBURTestBackupAsync, "TestBackupAsync");

namespace bur_ts
	{
	// Forward declaration
	class CAsyncBackupTransferHandler;
	
	class CBURTestStepBackupAsync : public CBURTestStepBase
		/**
		Implements the backup operation. 
		
		@released
		@internalComponent
		*/
		{
	public:
		static CBURTestStepBackupAsync* NewL(CBURTestServer& aTestServer);
		~CBURTestStepBackupAsync();
		
		// Inherited from CTestStep
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepPostambleL();
		virtual TVerdict doTestStepL();
		
	private:
		CBURTestStepBackupAsync(CBURTestServer& aTestServer);
		void ConstructL();
		
		// Core backup function
		void DoBackupL();
		void DoActiveBackupL(TTransferDataType aTransferDataType);
		
		// Following 2 methods implement sequences of backup operations during 
		// base and incremental backups respectively
		void BaseBackupL();
		void IncrementalBackupL();
		
		// Registration data backup handler
		void BackupRegistrationDataL();
		
		// Passive backup handlers
		void BackupPassiveSnapshotDataL();
		void SupplyPassiveSnapshotDataL();
		void BackupPassiveBaseDataL();
		void BackupPassiveIncDataL();
		
		// Active backup handlers
		void BackupActiveSnapshotDataL();
		void SupplyActiveSnapshotDataL();
		void BackupActiveBaseDataL();
		void BackupActiveIncDataL();
		
		// Packages backup
		void SupplySystemSnapshotDataL();
		void BackupSystemSnapshotDataL();
		void BackupSystemBaseDataL();
		
		// java
		void BackupJavaMidletsL();
		void BackupJavaMidletsDataL();
		
		// public
		void BackupPublicDataL();
		void DoPublicBackupL(RPointerArray<CSBGenericDataType>& aTransferTypes);
	
	private:
		/** Handler for async data requests */
		CAsyncBackupTransferHandler* iDataTransferHandler;
		};
	}
#endif // T_TESTSTEPBACKUPASYNC_H
