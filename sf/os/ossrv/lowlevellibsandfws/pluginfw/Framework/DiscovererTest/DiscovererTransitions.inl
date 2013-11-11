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
// Implementation of the CDiscoverer transition class methods.
// 
//

#include "DiscovererObserverStub.h"

// Public methods first
// ______________________________________________________________________________
//
_LIT(KDiscovererNewLTransition,"CDiscoverer_NewL_Transition");

inline CDiscoverer_NewL_Transition::CDiscoverer_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDiscovererNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_NewL_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::Ctor transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	Context().iDiscoverer = CDiscoverer::NewL(Context().DiscovererObserver(), Context().iFs);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererDtorTransition,"CDiscoverer_Dtor_Transition");

inline CDiscoverer_Dtor_Transition::CDiscoverer_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_Dtor_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::Dtor transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	delete Context().iDiscoverer;
	Context().iDiscoverer = NULL;
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererResumeTransition,"CDiscoverer_Resume_Transition");

inline CDiscoverer_Resume_Transition::CDiscoverer_Resume_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererResumeTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_Resume_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::Resume transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	Context().iDiscoverer->Resume();
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_Resume_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererSuspendTransition,"CDiscoverer_Suspend_Transition");

inline CDiscoverer_Suspend_Transition::CDiscoverer_Suspend_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererSuspendTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_Suspend_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::Suspend transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	Context().iDiscoverer->Suspend();
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_Suspend_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// Private method transitions
// ______________________________________________________________________________
//
_LIT(KDiscovererNotificationTransition,"CDiscoverer_Notification_Transition");

inline CDiscoverer_Notification_Transition::CDiscoverer_Notification_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererNotificationTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_Notification_Transition::TransitMethodL()
	{
	// Simulate the RunL of the notifier firing to run the
	// notification transition.
	_LIT(KTransitMethod,"CDiscoverer::Notification transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,Context().StateAccessor());
	// Notification processing is asynchronous,
	// but it is a backgrounding task, not a true async
	// signally process so !???
	iStatus = KRequestPending;
	// give the flag to the stub so that we complete
	// when the stub's completion method is called
	Context().iNotificationStatus = &iStatus;
	// and kick off the notification
	accessor.Notification(Context().iDiscoverer);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_Notification_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererScanDirectoryLTransition,"CDiscoverer_ScanDirectoryL_Transition");

inline CDiscoverer_ScanDirectoryL_Transition::CDiscoverer_ScanDirectoryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDiscovererScanDirectoryLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_ScanDirectoryL_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::ScanDirectoryL transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).ScanDirectoryL(Context().iDiscoverer);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_ScanDirectoryL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererScanDirectoryIncrementLTransition,"CDiscoverer_ScanDirectoryIncrementL_Transition");

inline CDiscoverer_ScanDirectoryIncrementL_Transition::CDiscoverer_ScanDirectoryIncrementL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDiscovererScanDirectoryIncrementLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_ScanDirectoryIncrementL_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::ScanDirectoryIncrementL transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).ScanDirectoryIncrementL(Context().iDiscoverer);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_ScanDirectoryIncrementL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererScanDirectoryCancelTransition,"CDiscoverer_ScanDirectoryCancel_Transition");

inline CDiscoverer_ScanDirectoryCancel_Transition::CDiscoverer_ScanDirectoryCancel_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererScanDirectoryCancelTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_ScanDirectoryCancel_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::ScanDirectoryCancel transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).ScanDirectoryCancel(Context().iDiscoverer);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_ScanDirectoryCancel_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererCompleteNotificationProcessingTransition,"CDiscoverer_CompleteNotificationProcessing_Transition");

inline CDiscoverer_CompleteNotificationProcessing_Transition::CDiscoverer_CompleteNotificationProcessing_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererCompleteNotificationProcessingTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_CompleteNotificationProcessing_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::CompleteNotificationProcessing transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).CompleteNotificationProcessing(Context().iDiscoverer);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_CompleteNotificationProcessing_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererIsValidEntryLTransition,"CDiscoverer_IsValidEntryL_Transition");

inline CDiscoverer_IsValidEntryL_Transition::CDiscoverer_IsValidEntryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDiscovererIsValidEntryLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_IsValidEntryL_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::IsValidEntryL transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor());
	CDir* dir = Context().iDir;
	TInt count = dir->Count();
	if(count)
		accessor.ValidateEntryL(Context().iDiscoverer, (*dir)[0], Context().iPath, Context().iReturnedFileEntry);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_IsValidEntryL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererProcessEntryLTransition,"CDiscoverer_ProcessEntryL_Transition");

inline CDiscoverer_ProcessEntryL_Transition::CDiscoverer_ProcessEntryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDiscovererProcessEntryLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_ProcessEntryL_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::ProcessEntryL transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).ProcessEntryL(Context().iDiscoverer, *Context().iReturnedFileEntry);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_ProcessEntryL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererMountDriveLTransition,"CDiscoverer_MountDriveL_Transition");

inline CDiscoverer_MountDriveL_Transition::CDiscoverer_MountDriveL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDiscovererMountDriveLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_MountDriveL_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::MountDriveL transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).DriveMountedL(Context().iDiscoverer, Context().iDriveUnit);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_MountDriveL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDiscovererUnmountDriveTransition,"CDiscoverer_UnmountDrive_Transition");

inline CDiscoverer_UnmountDrive_Transition::CDiscoverer_UnmountDrive_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDiscovererUnmountDriveTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDiscoverer_UnmountDrive_Transition::TransitMethodL()
	{
	_LIT(KTransitMethod,"CDiscoverer::DriveUnmountedL transition");
	Context().DataLogger().LogInformation(KTransitMethod);
	REINTERPRET_CAST(TDiscoverer_StateAccessor&, Context().StateAccessor()).DriveUnmountedL(Context().iDiscoverer, Context().iDriveUnit);
	}

inline CDiscoverer_UnitTestContext& CDiscoverer_UnmountDrive_Transition::Context() const
	{
	return REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext);
	}
