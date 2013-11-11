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

#ifndef __T_BUR_TEST_STEP_BACKUP_H__
#define __T_BUR_TEST_STEP_BACKUP_H__

#include "t_burteststepbase.h"
#include "t_asyncbackuptransferhandler.h"

_LIT(KBURTestBackup, "TestBackup");

namespace bur_ts
	{
	// Forward declaration
	class CAsyncBackupTransferHandler;
	
	class CBURTestStepBackup : public CBURTestStepBase
		/**
		Implements the backup operation. 
		
		@released
		@internalComponent
		*/
		{
	public:
		static CBURTestStepBackup* NewL(CBURTestServer& aTestServer);
		~CBURTestStepBackup();
		
		// Inherited from CTestStep
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepPostambleL();
		virtual TVerdict doTestStepL();
		
	private:
		CBURTestStepBackup(CBURTestServer& aTestServer);
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
#endif // __T_BUR_TEST_STEP_BACKUP_H__
