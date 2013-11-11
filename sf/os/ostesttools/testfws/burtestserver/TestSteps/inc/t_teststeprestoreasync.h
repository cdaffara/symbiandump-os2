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
* T_TestStepRestore.h
* 
*
*/



/**
 @file
 @released
*/

#ifndef T_TESTSTEPRESTOREASYNC_H
#define T_TESTSTEPRESTOREASYNC_H

#include "t_burteststepbase.h"

_LIT(KBURTestRestoreAsync, "TestRestoreAsync");

namespace bur_ts
	{

	class CBURTestStepRestoreAsync : public CBURTestStepBase
		/**
		Implements the restore operation.
		
		@released
		@internalComponent
		*/
		{
	public:
		static CBURTestStepRestoreAsync* NewL(CBURTestServer& aTestServer);
		~CBURTestStepRestoreAsync();
		
		// Inherited from CTestStep
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepPostambleL();
		virtual TVerdict doTestStepL();
		
	private:
		CBURTestStepRestoreAsync(CBURTestServer& aTestServer);
		void ConstructL();
		
		void BaseRestoreL();
		void IncrementalRestoreL();
		
		// Registration data restore handler
		void SupplyRegistrationDataL();
		
		// Passive restore handlers
		void SupplyPassiveSnapshotDataL();
		void RestorePassiveBaseDataL();
		void RestorePassiveIncDataL();
		
		// Active restore handlers
		void SupplyActiveSnapshotDataL();
		void RestoreActiveBaseDataL();
		void RestoreActiveIncDataL();
		
		// Restore packages
		void SupplySystemSnapshotDataL();
		void RestoreSystemBaseDataL();
		
		// java
		void RestoreJavaMidletL();
		void RestoreJavaMidletDataL();
		};
	}

#endif // T_TESTSTEPRESTOREASYNC_H

