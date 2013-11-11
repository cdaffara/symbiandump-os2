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

#ifndef __T_BUR_TEST_STEP_RESTORE_H__
#define __T_BUR_TEST_STEP_RESTORE_H__

#include "t_burteststepbase.h"

_LIT(KBURTestRestore, "TestRestore");

namespace bur_ts
	{

	class CBURTestStepRestore : public CBURTestStepBase
		/**
		Implements the restore operation.
		
		@released
		@internalComponent
		*/
		{
	public:
		static CBURTestStepRestore* NewL(CBURTestServer& aTestServer);
		~CBURTestStepRestore();
		
		// Inherited from CTestStep
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepPostambleL();
		virtual TVerdict doTestStepL();
		
	private:
		CBURTestStepRestore(CBURTestServer& aTestServer);
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

#endif // __T_BUR_TEST_STEP_RESTORE_H__

