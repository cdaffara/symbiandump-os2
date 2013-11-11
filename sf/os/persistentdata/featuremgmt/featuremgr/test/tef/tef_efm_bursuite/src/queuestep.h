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

#if (!defined __QUEUE_STEP_H__)
#define __QUEUE_RESTORE_H__

#include "tef_efm_bursuitedefs.h"
#include <test/testexecutestepbase.h>
#include "tef_efm_bursuitestepbase.h"
#include <bautils.h>
#include <featmgr/featurecmn.h>


/**
 * Queue test class: for testing the queuing mechanism during a backup operation.
 * This class will check that SetFeature will be queued correctly if a request is
 * made during a backup operation. It will be implemented after the BUR operation 
 * has completed.
 */
class CQueueBackupStep : public CTefEfmBURSuiteStepBase
	{
public:
	CQueueBackupStep();
	~CQueueBackupStep();
	virtual TVerdict doTestStepL();
	
	/**
	 * Perform the thread's actions. 
	 * @leave KErrNone Will leave if connection to RFeatureControl returns something other than KErrNone .
	 */ 
	void DoThreadActionL();
	
	/**
	 * This function makes the call to DoThreadActionL.
	 * @see DoThreadActionL is called to execute the action content.
	 * @param aPtrThis a pointer to CQueueBackupStep.
	 * @return TInt returns 1 if the thread is correctly launched, and 0 otherwise.
	 */
	static TInt ThreadEntryPoint( TAny* aPtrThis );
	
	};

_LIT( KQUEUEBackupStep, "QueueBackupStep" );
	
	

/**
 * Queue test class: for testing the queuing mechanism during a restore operation.
 * This class will check that delete, set and addfeature will be queued correctly if 
 * a request is made during a restore operation. It will be implemented after 
 * the BUR operation has completed.
 */
class CQueueRestoreStep : public CTefEfmBURSuiteStepBase
	{
public:
	CQueueRestoreStep();
	~CQueueRestoreStep();
	virtual TVerdict doTestStepL();
	
	/**
	 * Perform the thread's actions: SetFeature on KNewUid1. 
	 * @leave KErrNone Will leave if connection to RFeatureControl returns something other than KErrNone.
	 */ 
	void DoThreadActionUid1L();
	
	/**
	 * Perform the thread's actions: AddFeature on KNewUid2. 
	 * @leave KErrNone Will leave if connection to RFeatureControl returns something other than KErrNone.
	 */ 
	void DoThreadActionUid2L();
	
	/**
	 * This function makes the call to DoThreadActionUid1L.
	 * @see DoThreadActionUid1L is called to execute SetFeature on KNewUid1.
	 * @param aPtrThis a pointer to CQueueRestoreStep.
	 * @return TInt returns 1 if the thread is correctly launched, and 0 otherwise.
	 */
	static TInt ThreadEntryPointUID1( TAny* aPtrThis );	
	
	/**
	 * This function makes the call to DoThreadActionUid2L.
	 * @see DoThreadActionUid2L is called to execute AddFeature on KNewUid2. 
	 * @param aPtrThis a pointer to CQueueRestoreStep.
	 * @return TInt returns 1 if the thread is correctly launched, and 0 otherwise.
	 */
	static TInt ThreadEntryPointUID2( TAny* aPtrThis );	
	
	
	};

_LIT( KQUEUERestoreStep, "QueueRestoreStep" );


#endif
