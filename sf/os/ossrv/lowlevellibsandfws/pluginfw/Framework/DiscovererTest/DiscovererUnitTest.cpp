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

#include "DiscovererUnitTest.h"

// ______________________________________________________________________________
//

CDiscoverer_CreateAndDestroy_UnitTest* CDiscoverer_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CDiscoverer_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	// Chain to the base which calls the ConstructL
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_CreateAndDestroy_UnitTest::ConstructL()
	{
	UnitTestConstructL();
	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	
	// and a validators
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	//	Examples of C'tor and D'tor transitions on CDiscoverer class. 
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CDiscoverer_CreateAndDestroy_UnitTest::PrepareUnitTestL()
	{
	}

// ______________________________________________________________________________
//

CDiscoverer_MountDrive_UnitTest* CDiscoverer_MountDrive_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_MountDrive_UnitTest* self = 
					new(ELeave) CDiscoverer_MountDrive_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_MountDrive_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iMountValidator = new(ELeave) TDiscoverer_MountDriveL_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_MountDriveL_Transition(*iUTContext,*iMountValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_UnmountDrive_UnitTest* CDiscoverer_UnmountDrive_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_UnmountDrive_UnitTest* self = 
					new(ELeave) CDiscoverer_UnmountDrive_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_UnmountDrive_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iUnmountValidator = new(ELeave) TDiscoverer_UnmountDrive_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_UnmountDrive_Transition(*iUTContext,*iUnmountValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_IsValidEntryL_UnitTest* CDiscoverer_IsValidEntryL_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_IsValidEntryL_UnitTest* self = 
					new(ELeave) CDiscoverer_IsValidEntryL_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_IsValidEntryL_UnitTest::ConstructL()
	{
	_LIT(KDllFileSearchPath,"\\System\\Libs\\Plugins\\*");

	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// and the path

	// Set up the drive letter for the test (Z:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveZ);
	TDriveName driveName(iUTContext->iDriveUnit.Name());
	iUTContext->iPath.Set(KDllFileSearchPath(), NULL, &driveName);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iIsValidEntryValidator = new(ELeave) TDiscoverer_IsValidEntry_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_IsValidEntryL_Transition(*iUTContext,*iIsValidEntryValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

inline void CDiscoverer_IsValidEntryL_UnitTest::PrepareUnitTestL()
	{
	_LIT(KPathForDiscovery,"Discovery Path = %S");
	iDataLogger.LogInformationWithParameters(KPathForDiscovery,&(iUTContext->iPath.FullName()));
	User::LeaveIfError(iFs.GetDir(iUTContext->iPath.FullName(), KEntryAttMatchExclude | KEntryAttDir, ESortByUid, iUTContext->iDir));
	}

// ______________________________________________________________________________
//

CDiscoverer_ProcessEntryL_UnitTest* CDiscoverer_ProcessEntryL_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_ProcessEntryL_UnitTest* self = 
					new(ELeave) CDiscoverer_ProcessEntryL_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_ProcessEntryL_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iProcessEntryValidator = new(ELeave) TDiscoverer_ProcessEntryL_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_ProcessEntryL_Transition(*iUTContext,*iProcessEntryValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_CompleteNotificationProcessing_UnitTest* CDiscoverer_CompleteNotificationProcessing_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_CompleteNotificationProcessing_UnitTest* self = 
					new(ELeave) CDiscoverer_CompleteNotificationProcessing_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_CompleteNotificationProcessing_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));

	iCompleteNotificationProcessingValidator = new(ELeave) TDiscoverer_CompleteNotificationProcessing_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_CompleteNotificationProcessing_Transition(*iUTContext,*iCompleteNotificationProcessingValidator));
	
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_Notification_UnitTest* CDiscoverer_Notification_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_Notification_UnitTest* self = 
					new(ELeave) CDiscoverer_Notification_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_Notification_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));

	iNotificationValidator = new(ELeave) TDiscoverer_Notification_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Notification_Transition(*iUTContext,*iNotificationValidator));
	
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddBlockingTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}
// ______________________________________________________________________________
//

CDiscoverer_SuspendResume_UnitTest* CDiscoverer_SuspendResume_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_SuspendResume_UnitTest* self = 
					new(ELeave) CDiscoverer_SuspendResume_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_SuspendResume_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	
	iNotificationValidator = new(ELeave) TDiscoverer_Notification_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Notification_Transition(*iUTContext,*iNotificationValidator));

	iSuspendValidator = new(ELeave) TDiscoverer_Suspend_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Suspend_Transition(*iUTContext,*iSuspendValidator));
	
	iResumeValidator = new(ELeave) TDiscoverer_Resume_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Resume_Transition(*iUTContext,*iResumeValidator));

	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddBlockingTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_ScanDirectory_UnitTest* CDiscoverer_ScanDirectory_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_ScanDirectory_UnitTest* self = 
					new(ELeave) CDiscoverer_ScanDirectory_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_ScanDirectory_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iScanDirectoryValidator = new(ELeave) TDiscoverer_ScanDirectory_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_ScanDirectoryL_Transition(*iUTContext,*iScanDirectoryValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_ScanDirectoryCancel_UnitTest* CDiscoverer_ScanDirectoryCancel_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_ScanDirectoryCancel_UnitTest* self = 
					new(ELeave) CDiscoverer_ScanDirectoryCancel_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_ScanDirectoryCancel_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iScanDirectoryCancelValidator = new(ELeave) TDiscoverer_ScanDirectoryCancel_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_ScanDirectoryCancel_Transition(*iUTContext,*iScanDirectoryCancelValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//

CDiscoverer_ScanDirectoryIncrement_UnitTest* CDiscoverer_ScanDirectoryIncrement_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDiscoverer_ScanDirectoryIncrement_UnitTest* self = 
					new(ELeave) CDiscoverer_ScanDirectoryIncrement_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CDiscoverer_ScanDirectoryIncrement_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDiscoverer_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDiscoverer_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// Set up the drive letter for the test (C:)
	iUTContext->iDriveUnit = TDriveUnit(EDriveC);
	// Construct, and store the observer stub
	iUTContext->iDiscovererObserver = CObserverStub::NewL(*this,*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDiscoverer_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_NewL_Transition(*iUTContext,*iCtorValidator));
	iScanDirectoryIncrementValidator = new(ELeave) TDiscoverer_ScanDirectoryIncrement_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_ScanDirectoryIncrementL_Transition(*iUTContext,*iScanDirectoryIncrementValidator));
	iDtorValidator = new(ELeave) TDiscoverer_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CDiscoverer_Dtor_Transition(*iUTContext,*iDtorValidator));
	}
