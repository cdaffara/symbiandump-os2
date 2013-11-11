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
// Implementation of the CDiscoverer transition validation classes.
// 
//

// ______________________________________________________________________________
//
inline TDiscoverer_Ctor_TransitionValidator::TDiscoverer_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_Ctor_TransitionValidator::ValidatePreConditions()
	{
	// Always true
	return ETrue;
	}

inline TBool TDiscoverer_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_Dtor_TransitionValidator::TDiscoverer_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_MountDriveL_TransitionValidator::TDiscoverer_MountDriveL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_MountDriveL_TransitionValidator::ValidatePreConditions()
	{
	// For mount drive all that is required is that the invariant test is done.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_MountDriveL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For mount drive.
	// mount has been signalled to the observer
	// all that is required is that the invariant test is done.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_UnmountDrive_TransitionValidator::TDiscoverer_UnmountDrive_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_UnmountDrive_TransitionValidator::ValidatePreConditions()
	{
	// For unmount drive all that is required is that the invariant test is done.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_UnmountDrive_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For unmount drive 
	// unmount has been signalled to the observer
	// all that is required is that the invariant test is done.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_IsValidEntry_TransitionValidator::TDiscoverer_IsValidEntry_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_IsValidEntry_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_IsValidEntry_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_ProcessEntryL_TransitionValidator::TDiscoverer_ProcessEntryL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_ProcessEntryL_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_ProcessEntryL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_CompleteNotificationProcessing_TransitionValidator::TDiscoverer_CompleteNotificationProcessing_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_CompleteNotificationProcessing_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_CompleteNotificationProcessing_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_Notification_TransitionValidator::TDiscoverer_Notification_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_Notification_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_Notification_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_Suspend_TransitionValidator::TDiscoverer_Suspend_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_Suspend_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_Suspend_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_Resume_TransitionValidator::TDiscoverer_Resume_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_Resume_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_Resume_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDiscoverer_ScanDirectory_TransitionValidator::TDiscoverer_ScanDirectory_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_ScanDirectory_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_ScanDirectory_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}


// ______________________________________________________________________________
//
inline TDiscoverer_ScanDirectoryCancel_TransitionValidator::TDiscoverer_ScanDirectoryCancel_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_ScanDirectoryCancel_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_ScanDirectoryCancel_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}


// ______________________________________________________________________________
//
inline TDiscoverer_ScanDirectoryIncrement_TransitionValidator::TDiscoverer_ScanDirectoryIncrement_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDiscoverer_ScanDirectoryIncrement_TransitionValidator::ValidatePreConditions()
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}

inline TBool TDiscoverer_ScanDirectoryIncrement_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	TDiscoverer_StateAccessor& accessor = REINTERPRET_CAST(TDiscoverer_StateAccessor&,iUTContext.StateAccessor());
	if(accessor.InvariantTest(REINTERPRET_CAST(CDiscoverer_UnitTestContext&,iUTContext).iDiscoverer))
		return EFalse;
	return ETrue;
	}


