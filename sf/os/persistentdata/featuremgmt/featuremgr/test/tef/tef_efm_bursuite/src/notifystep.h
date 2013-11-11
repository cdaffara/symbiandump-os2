// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#if (!defined __NOTIFY_STEP_H__)
#define __NOTIFY_RESTORE_H__

#include "tef_efm_bursuitedefs.h"
#include <test/testexecutestepbase.h>
#include "tef_efm_bursuitestepbase.h"
#include <bautils.h>
#include <featmgr/featurecmn.h>
#include <featmgr/featurenotifier.h>

/**
 * Notify test step: specifically during a backup operation.
 * Inherits from CTefEfmBURSuiteStepBase and MFeatureObserver. The MFeatureObserver inheritance
 * is required because of the HandleNotifyChange() requisite.
 * This class will check a single notification request made during a backup operation is queued
 * correctly and then implemented after the BUR operation has completed.
 */ 
class CNotifyBackupStep : public CTefEfmBURSuiteStepBase, public MFeatureObserver
	{
	public:
		CNotifyBackupStep();
		~CNotifyBackupStep();
		
		/**
		 * Perform the thread's actions. 
		 * @leave KErrNone Will leave if connection to RFeatureControl returns something other than KErrNone .
		 */ 
		void DoThreadActionL();
		
		/**
		 * This function makes the call to DoThreadActionL.
		 * @see DoThreadActionL is called to execute the action content.
		 * @param aPtrThis a pointer to the object owning the thread.
		 * @return TInt returns 1 if the thread is correctly launched, and 0 otherwise.
		 */
		static TInt ThreadEntryPoint( TAny* aPtrThis );
		
		
		/**
		 * Overrides the HandleNotifyChange in MFeatureObserver
		 * @see MFeatureObserver::HandleNotifyChange();
		 */
		void HandleNotifyChange( TFeatureChangeType aType  , TFeatureEntry  aFeature  );
		
		/**
		 * Overrides the HandleNotifyChange in MFeatureObserver
		 * @see MFeatureObserver::HandleNotifyError();
		 */
		void HandleNotifyError( TInt aErr);
	
		/**
		 * Run the test for BUR operation backup 
		 */
		virtual TVerdict doTestStepL();
	};

_LIT( KNotifyBackupStep, "NotifyBackupStep" );



// ----------


/**
 * Notify test step: specifically during a restore operation.
 * Inherits from CTefEfmBURSuiteStepBase and MFeatureObserver. The MFeatureObserver inheritance
 * is required because of the HandleNotifyChange() requisite.
 * This class will check that three notification requests made during a restore operation are queued
 * correctly and then implemented after the BUR operation has completed. The three requests are
 * for adding a feature, changing an existing feature and deleting a feature.
 */ 
class CNotifyRestoreStep : public CTefEfmBURSuiteStepBase, public MFeatureObserver
	{
	public:
		CNotifyRestoreStep();
		~CNotifyRestoreStep();
		
		/**
		 * Overrides the HandleNotifyChange in MFeatureObserver
		 * @see MFeatureObserver::HandleNotifyChange();
		 */
		void HandleNotifyChange( TFeatureChangeType aType  , TFeatureEntry  aFeature  );
		
		/**
		 * Overrides the HandleNotifyChange in MFeatureObserver
		 * @see MFeatureObserver::HandleNotifyError();
		 */
		void HandleNotifyError( TInt aErr);
	
		/**
		 * Run the test for BUR operation backup 
		 */
		virtual TVerdict doTestStepL();
	};

_LIT( KNotifyRestoreStep, "NotifyRestoreStep" );



// ----------

_LIT(KADD, "Add");
_LIT(KDELETE, "Delete");
_LIT(KCHANGE, "Change");
_LIT(KBUR, "Operation");


#endif
