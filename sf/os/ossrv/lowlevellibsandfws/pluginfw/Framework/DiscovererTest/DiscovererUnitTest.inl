// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the CDiscoverer Unit Test class methods.
// 
//

_LIT(KDiscovererCreateAndDestroyUnitTest,"CDiscoverer_CreateAndDestroy_UnitTest");

inline TInt CDiscoverer_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_CreateAndDestroy_UnitTest::~CDiscoverer_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instances
	// and close any open handles
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_CreateAndDestroy_UnitTest::CDiscoverer_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererMountDriveUnitTest,"CDiscoverer_MountDrive_UnitTest");

inline TInt CDiscoverer_MountDrive_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iMountValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_MountDrive_UnitTest::~CDiscoverer_MountDrive_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iMountValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_MountDrive_UnitTest::CDiscoverer_MountDrive_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererMountDriveUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererUnmountDriveUnitTest,"CDiscoverer_UnmountDrive_UnitTest");

inline TInt CDiscoverer_UnmountDrive_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iUnmountValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_UnmountDrive_UnitTest::~CDiscoverer_UnmountDrive_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iUnmountValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_UnmountDrive_UnitTest::CDiscoverer_UnmountDrive_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererUnmountDriveUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererIsValidEntryLUnitTest,"CDiscoverer_IsValidEntryL_UnitTest");

inline TInt CDiscoverer_IsValidEntryL_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iIsValidEntryValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_IsValidEntryL_UnitTest::~CDiscoverer_IsValidEntryL_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iIsValidEntryValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_IsValidEntryL_UnitTest::CDiscoverer_IsValidEntryL_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererIsValidEntryLUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}
// ______________________________________________________________________________
//
_LIT(KDiscovererProcessEntryLUnitTest,"CDiscoverer_ProcessEntryL_UnitTest");

inline TInt CDiscoverer_ProcessEntryL_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iProcessEntryValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_ProcessEntryL_UnitTest::~CDiscoverer_ProcessEntryL_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iProcessEntryValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_ProcessEntryL_UnitTest::CDiscoverer_ProcessEntryL_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererProcessEntryLUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererCompleteNotificationProcessingUnitTest,"CDiscoverer_CompleteNotificationProcessing_UnitTest");

inline TInt CDiscoverer_CompleteNotificationProcessing_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iCompleteNotificationProcessingValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_CompleteNotificationProcessing_UnitTest::~CDiscoverer_CompleteNotificationProcessing_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iCompleteNotificationProcessingValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_CompleteNotificationProcessing_UnitTest::CDiscoverer_CompleteNotificationProcessing_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererCompleteNotificationProcessingUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}


// ______________________________________________________________________________
//
_LIT(KDiscovererNotificationUnitTest,"CDiscoverer_Notification_UnitTest");

inline TInt CDiscoverer_Notification_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iNotificationValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_Notification_UnitTest::~CDiscoverer_Notification_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iNotificationValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_Notification_UnitTest::CDiscoverer_Notification_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererNotificationUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererResumeUnitTest,"CDiscoverer_SuspendResume_UnitTest");

inline TInt CDiscoverer_SuspendResume_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iNotificationValidator;
	delete iSuspendValidator;
	delete iResumeValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_SuspendResume_UnitTest::~CDiscoverer_SuspendResume_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iNotificationValidator;
	delete iSuspendValidator;
	delete iResumeValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_SuspendResume_UnitTest::CDiscoverer_SuspendResume_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererResumeUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererScanDirectoryUnitTest,"CDiscoverer_ScanDirectory_UnitTest");

inline TInt CDiscoverer_ScanDirectory_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iScanDirectoryValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_ScanDirectory_UnitTest::~CDiscoverer_ScanDirectory_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iScanDirectoryValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_ScanDirectory_UnitTest::CDiscoverer_ScanDirectory_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererScanDirectoryUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererScanDirectoryCancelUnitTest,"CDiscoverer_ScanDirectoryCancel_UnitTest");

inline TInt CDiscoverer_ScanDirectoryCancel_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iScanDirectoryCancelValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_ScanDirectoryCancel_UnitTest::~CDiscoverer_ScanDirectoryCancel_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iScanDirectoryCancelValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_ScanDirectoryCancel_UnitTest::CDiscoverer_ScanDirectoryCancel_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererScanDirectoryCancelUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererScanDirectoryIncrementUnitTest,"CDiscoverer_ScanDirectoryIncrement_UnitTest");

inline TInt CDiscoverer_ScanDirectoryIncrement_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iScanDirectoryIncrementValidator;
	delete iDtorValidator;
	return error;
	}

inline CDiscoverer_ScanDirectoryIncrement_UnitTest::~CDiscoverer_ScanDirectoryIncrement_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iScanDirectoryIncrementValidator;
	delete iDtorValidator;
	}

inline CDiscoverer_ScanDirectoryIncrement_UnitTest::CDiscoverer_ScanDirectoryIncrement_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDiscovererScanDirectoryIncrementUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}
