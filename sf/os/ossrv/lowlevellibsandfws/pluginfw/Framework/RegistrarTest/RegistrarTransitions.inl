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
// The implementation of the transition methods for the CRegistrar class.
// 
//

#include "RegistrarStateAccessor.h"

// ______________________________________________________________________________
//
_LIT(KRegistrarNewLTransition,"CRegistrar_NewL_Transition");

inline CRegistrar_NewL_Transition::CRegistrar_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_NewL_Transition::TransitMethodL()
	{
	_LIT(KRegistrarNewLTransitMethod, "CRegistrar::NewL transition");
	Context().DataLogger().LogInformation(KRegistrarNewLTransitMethod);
	Context().iRegistrar = CRegistrar::NewL(Context().iRegistryData, Context().iRegistrarObserver, Context().iFs);
	}

inline CRegistrar_UnitTestContext& CRegistrar_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarDtorTransition,"CRegistrar_Dtor_Transition");

inline CRegistrar_Dtor_Transition::CRegistrar_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_Dtor_Transition::TransitMethodL()
	{
	_LIT(KRegistrarDtorTransitMethod, "CRegistrar::Dtor transition");
	Context().DataLogger().LogInformation(KRegistrarDtorTransitMethod);
	delete Context().iRegistrar;
	Context().iRegistrar = NULL;
	}

inline CRegistrar_UnitTestContext& CRegistrar_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarDiscoveriesBeginTransition,"CRegistrar_DiscoveriesBegin_Transition");

inline CRegistrar_DiscoveriesBegin_Transition::CRegistrar_DiscoveriesBegin_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarDiscoveriesBeginTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_DiscoveriesBegin_Transition::TransitMethodL()
	{
	_LIT(KRegistrarDiscoveriesBeginTransitMethod, "CRegistrar::DiscoveriesBegin transition");
	Context().DataLogger().LogInformation(KRegistrarDiscoveriesBeginTransitMethod);
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.DiscoveriesBegin(*(Context().iRegistrar));
	}

inline CRegistrar_UnitTestContext& CRegistrar_DiscoveriesBegin_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarDiscoveriesCompleteTransition,"CRegistrar_DiscoveriesComplete_Transition");

inline CRegistrar_DiscoveriesComplete_Transition::CRegistrar_DiscoveriesComplete_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarDiscoveriesCompleteTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_DiscoveriesComplete_Transition::TransitMethodL()
	{
	_LIT(KRegistrarDiscoveriesCompleteTransitMethod, "CRegistrar::DiscoveriesComplete transition");
	Context().DataLogger().LogInformation(KRegistrarDiscoveriesCompleteTransitMethod);
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.DiscoveriesComplete(*(Context().iRegistrar), ETrue);
	}

inline CRegistrar_UnitTestContext& CRegistrar_DiscoveriesComplete_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarResumeTransition,"CRegistrar_Resume_Transition");

inline CRegistrar_Resume_Transition::CRegistrar_Resume_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarResumeTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_Resume_Transition::TransitMethodL()
	{
	_LIT(KRegistrarResumeTransitMethod, "CRegistrar::Resume transition");
	Context().DataLogger().LogInformation(KRegistrarResumeTransitMethod);
	Context().iReturnErrorCode = Context().iRegistrar->Resume();
	}

inline CRegistrar_UnitTestContext& CRegistrar_Resume_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarSuspendTransition,"CRegistrar_Suspend_Transition");

inline CRegistrar_Suspend_Transition::CRegistrar_Suspend_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarSuspendTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_Suspend_Transition::TransitMethodL()
	{
	_LIT(KRegistrarSuspendTransitMethod, "CRegistrar::Suspend transition");
	Context().DataLogger().LogInformation(KRegistrarSuspendTransitMethod);
	Context().iReturnErrorCode = Context().iRegistrar->Suspend();
	}

inline CRegistrar_UnitTestContext& CRegistrar_Suspend_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarParseLTransition,"CRegistrar_ParseL_Transition");

inline CRegistrar_ParseL_Transition::CRegistrar_ParseL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistrarParseLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_ParseL_Transition::TransitMethodL()
	{
	_LIT(KRegistrarParseLTransitMethod, "CRegistrar::ParseL transition");
	Context().DataLogger().LogInformation(KRegistrarParseLTransitMethod);
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.ParseL(*Context().iRegistrar, *Context().iDllData, KResourceFileName);
	}

inline CRegistrar_UnitTestContext& CRegistrar_ParseL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarDisableTransition,"CRegistrar_Disable_Transition");

inline CRegistrar_Disable_Transition::CRegistrar_Disable_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarDisableTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_Disable_Transition::TransitMethodL()
	{
	_LIT(KRegistrarDisableTransitMethod, "CRegistrar::Disable transition");
	Context().DataLogger().LogInformation(KRegistrarDisableTransitMethod);
	Context().iReturnErrorCode = Context().iRegistrar->Disable(Context().iImplementationUid);
	}

inline CRegistrar_UnitTestContext& CRegistrar_Disable_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarDriveReinstatedLTransition,"CRegistrar_DriveReinstatedL_Transition");

inline CRegistrar_DriveReinstatedL_Transition::CRegistrar_DriveReinstatedL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistrarDriveReinstatedLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_DriveReinstatedL_Transition::TransitMethodL()
	{
	_LIT(KRegistrarDriveReinstatedLTransitMethod, "CRegistrar::DriveReinstatedL transition");
	Context().DataLogger().LogInformation(KRegistrarDriveReinstatedLTransitMethod);
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.DriveReinstatedL(*(Context().iRegistrar), Context().iDriveUnit);
	}

inline CRegistrar_UnitTestContext& CRegistrar_DriveReinstatedL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarDriveRemovedTransition,"CRegistrar_DriveRemoved_Transition");

inline CRegistrar_DriveRemoved_Transition::CRegistrar_DriveRemoved_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarDriveRemovedTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_DriveRemoved_Transition::TransitMethodL()
	{
	_LIT(KRegistrarDriveRemovedTransitMethod, "CRegistrar::DriveRemoved transition");
	Context().DataLogger().LogInformation(KRegistrarDriveRemovedTransitMethod);
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.DriveRemoved(*(Context().iRegistrar), Context().iDriveUnit);
	}

inline CRegistrar_UnitTestContext& CRegistrar_DriveRemoved_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarEnableTransition,"CRegistrar_Enable_Transition");

inline CRegistrar_Enable_Transition::CRegistrar_Enable_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistrarEnableTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_Enable_Transition::TransitMethodL()
	{
	_LIT(KRegistrarEnableTransitMethod, "CRegistrar::Enable transition");
	Context().DataLogger().LogInformation(KRegistrarEnableTransitMethod);
	Context().iReturnErrorCode = Context().iRegistrar->Enable(Context().iImplementationUid);
	}

inline CRegistrar_UnitTestContext& CRegistrar_Enable_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarRegisterDiscoveryLTransition,"CRegistrar_RegisterDiscoveryL_Transition");

inline CRegistrar_RegisterDiscoveryL_Transition::CRegistrar_RegisterDiscoveryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistrarRegisterDiscoveryLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_RegisterDiscoveryL_Transition::TransitMethodL()
	{
	_LIT(KRegistrarRegisterDiscoveryLTransitMethod, "CRegistrar::RegisterDiscoveryL transition");
	Context().DataLogger().LogInformation(KRegistrarRegisterDiscoveryLTransitMethod);
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.RegisterDiscoveryL(*(Context().iRegistrar), Context().iDllEntry);
	}

inline CRegistrar_UnitTestContext& CRegistrar_RegisterDiscoveryL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarParseRegistrationDataLTransition,"CRegistrar_ParseRegistrationDataL_Transition");

inline CRegistrar_ParseRegistrationDataL_Transition::CRegistrar_ParseRegistrationDataL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistrarParseRegistrationDataLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistrar_ParseRegistrationDataL_Transition::TransitMethodL()
	{
	_LIT(KRegistrarParseRegistrationDataLTransitMethod, "CRegistrar::ParseRegistrationDataL transition");
	Context().DataLogger().LogInformation(KRegistrarParseRegistrationDataLTransitMethod);
	// Find the appropriate drive entry
	TChar driveLetter = User::UpperCase((Context().iDllEntry.iName)[0]) - 'A';
	TDriveUnit driveUnit = EDriveA + driveLetter;
	TRegistrar_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistrar_StateAccessor&, Context().StateAccessor());
	accessor.ParseRegistrationDataL(*Context().iRegistrar, 
									 Context().iDllEntry, 
									 KResourceFileName, 
									 driveUnit,
									 Context().iUpdate,
									 Context().iIndex);
	}

inline CRegistrar_UnitTestContext& CRegistrar_ParseRegistrationDataL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext);
	}

