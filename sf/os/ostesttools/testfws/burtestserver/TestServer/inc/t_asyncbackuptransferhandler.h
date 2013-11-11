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

#ifndef __ASYNC_BACKUP_TRANSFER_HANDLER_H__
#define __ASYNC_BACKUP_TRANSFER_HANDLER_H__

#include "t_teststepbackup.h"

using namespace conn;

namespace bur_ts
	{
	// Forward declaration
	class CBURTestStepBase;
	
	class CAsyncBackupTransferHandler : public CActive
		/**
		Implements a mechanism for asynchronous data transfers bewteen CTestStepBackup 
		and CSBEClient.
		
		@released
		@internalComponent
		*/
		{
	public:
		static CAsyncBackupTransferHandler* NewL(CBURTestStepBase* aTestStep);
		~CAsyncBackupTransferHandler();
		
		// CActive
		void StartL();
		TInt RunError(TInt aError);
		
		TBool Success();
		
	private:
		CAsyncBackupTransferHandler(CBURTestStepBase* aTestStep);
		virtual void ConstructL();
	
		// CActive
		void RunL();
		void DoCancel();
		
		void ContinueL();
		void MakeRequestDataCallL();
		void CurrentIDL();
		
	private:
		/** */
		CActiveSchedulerWait* iActiveScheduler;
		
		/** Current index into iTransferTypes */
		TInt iCurrentIndex;
		
		/** Successful if all transfers are completed without errors */
		TBool iSuccess;
		
		/** Parent test step for access to its logging */
		CBURTestStepBase* iTestStep;
		
		/** */
		HBufC* iID;
		};
	}

#endif // __ASYNC_BACKUP_TRANSFER_HANDLER_H__

