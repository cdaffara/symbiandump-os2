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


#ifndef CENTREPNOTIFYHANDLERTEST_H
#define CENTREPNOTIFYHANDLERTEST_H

// INCLUDES

#include <e32test.h>
#include <e32debug.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include "testscheduler.h"
#include "t_centrep_notifier_handler.h"


/**
*  This a CenRepNotifyHandlerTest class.
*/
NONSHARABLE_CLASS(CCenRepNotifyHandlerTest) : public CBase, MCenRepNotifyHandlerCallback 
	{
	public:
		// Constructors and destructor
		static CCenRepNotifyHandlerTest* NewL();
		static CCenRepNotifyHandlerTest* NewLC();
		virtual ~CCenRepNotifyHandlerTest();

	public:
		// Actual test case functions
		TInt ListenToKeyChangesL(TTestCaseInfo& aTestInfo);
		TInt ListenToAllKeysL(TTestCaseInfo& aTestInfo);
		TInt NoMultipleNotificationsL(TTestCaseInfo& aTestInfo);
		TInt NoNotificationAfterStopListeningL(TTestCaseInfo& aTestInfo);
		TInt NonExistentKeyL(TTestCaseInfo& aTestInfo);
		TInt TwoNotificationHandlersL(TTestCaseInfo& aTestInfo);
		TInt IncorrectKeyTypeL(TTestCaseInfo& aTestInfo);
		TInt RepeatingCallsL(TTestCaseInfo& aTestInfo);
		TInt CancelRequestL(TTestCaseInfo& aTestInfo);
		TInt ExplicitExercise(TTestCaseInfo& aTestInfo);
		TInt ExplicitExerciseRunErrorL(TTestCaseInfo& aTestInfo);

	public:
		// Notification handlers
		void HandleNotifyInt(TUint32 aId, TInt aNewValue);
		void HandleNotifyReal(TUint32 aId, TReal aNewValue);
		void HandleNotifyString(TUint32 aId, const TDesC16& aNewValue);
		void HandleNotifyBinary(TUint32 aId, const TDesC8& aNewValue);
		void HandleNotifyGeneric(TUint32 aId);
		void HandleNotifyErrorL(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler);

	private:
		// Internal functions
		CCenRepNotifyHandlerTest();
		void ConstructL();
		void InitializeNotifyData();
		TBool CheckResult(TTestCaseInfo& aTestInfo);
		void TestStartLC(TTestCaseInfo& aTestInfo);
		void TestFinish(TTestCaseInfo& aTestInfo);
		void StartSchedulerL();

	private:
		// Data
		
		//Pointer to CentRep notify handler
		CCenRepNotifyHandler* iNotifyHandler;

		//Pointer to CentRep notify handler (needed in test case ListeningForTwoNotificationHandler)
		CCenRepNotifyHandler* iNotifyHandler2;

		//Pointer to central repository session
		CRepository* iSession;
		CActiveScheduler* iMyScheduler;
		
		TInt iNotifyInt;
		TReal iNotifyReal;
		TBuf16<NCentralRepositoryConstants::KMaxUnicodeStringLength> iNotifyString;
		TBuf8<NCentralRepositoryConstants::KMaxBinaryLength> iNotifyBinary;

		TBool iUseSecondHandler;

		TInt iNotifyInt2;
		TReal iNotifyReal2;
		TBuf16<NCentralRepositoryConstants::KMaxUnicodeStringLength> iNotifyString2;
		TBuf8<NCentralRepositoryConstants::KMaxBinaryLength> iNotifyBinary2;

		TUint32 iNotificationId;
		TUint32 iNotificationId2;

		TInt iNotifyError;
	};

#endif      // CENTREPNOTIFYHANDLERTEST_H

// End of File
