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
// The implementation of the transition classes for the CRegistryData tests.
// 
//

// ______________________________________________________________________________
//
_LIT(KRegistryDataNewLTransition,"CRegistryData_NewL_Transition");

inline CRegistryData_NewL_Transition::CRegistryData_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistryDataNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_NewL_Transition::TransitMethodL()
	{
	Context().iRegistryData = CRegistryData::NewL(Context().iFs);
	}

inline CRegistryData_UnitTestContext& CRegistryData_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataDtorTransition,"CRegistryData_Dtor_Transition");

inline CRegistryData_Dtor_Transition::CRegistryData_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistryDataDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_Dtor_Transition::TransitMethodL()
	{
	delete Context().iRegistryData;
	Context().iRegistryData = NULL;
	}

inline CRegistryData_UnitTestContext& CRegistryData_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataAddDllDataLTransition,"CRegistryData_AddDllDataL_Transition");

inline CRegistryData_AddDllDataL_Transition::CRegistryData_AddDllDataL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataAddDllDataLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_AddDllDataL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataAddDllDataLTransitMethod, "CRegistryData::AddDllDataL transition");
	Context().DataLogger().LogInformation(KRegistryDataAddDllDataLTransitMethod);
	Context().iRegistryData->AddDllDataL(Context().iDriveUnit, 
										 Context().iFoundDriveIndex,
										 Context().iDllData);

	Context().iDllData = NULL;
	}

inline CRegistryData_UnitTestContext& CRegistryData_AddDllDataL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataGetImplementationInformationLTransition,"CRegistryData_GetImplementationInformationL_Transition");

inline CRegistryData_GetImplementationInformationL_Transition::CRegistryData_GetImplementationInformationL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataGetImplementationInformationLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_GetImplementationInformationL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataGetImplementationInformationLTransitMethod, "CRegistryData::GetImplementationInformationL transition");
	Context().DataLogger().LogInformation(KRegistryDataGetImplementationInformationLTransitMethod);
	Context().iImplementationRef = &(Context().iRegistryData->GetImplementationDataL(Context().iImplementationUid));
	}

inline CRegistryData_UnitTestContext& CRegistryData_GetImplementationInformationL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataIsRegisteredWithDateLTransition,"CRegistryData_IsRegisteredWithDateL_Transition");

inline CRegistryData_IsRegisteredWithDateL_Transition::CRegistryData_IsRegisteredWithDateL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataIsRegisteredWithDateLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_IsRegisteredWithDateL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataIsRegisteredWithDateLTransitMethod, "CRegistryData::IsRegisteredWithDateL transition");
	Context().DataLogger().LogInformation(KRegistryDataIsRegisteredWithDateLTransitMethod);
	Context().iRegistryData->IsRegisteredWithDateL(Context().iDriveUnit,
												   Context().iDllEntry[2], 
												   Context().iDllEntry.iModified, 
												   Context().iUpdate,
												   Context().iFoundDriveIndex);
	}

inline CRegistryData_UnitTestContext& CRegistryData_IsRegisteredWithDateL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataListImplementationsLTransition,"CRegistryData_ListImplementationsL_Transition");

inline CRegistryData_ListImplementationsL_Transition::CRegistryData_ListImplementationsL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataListImplementationsLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_ListImplementationsL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataListImplementationsLTransitMethod, "CRegistryData::ListImplementationsL transition");
	Context().DataLogger().LogInformation(KRegistryDataListImplementationsLTransitMethod);
	Context().iUidList = &Context().iRegistryData->ListImplementationsL(Context().iInterfaceUid);
	}

inline CRegistryData_UnitTestContext& CRegistryData_ListImplementationsL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataSetEnabledStateTransition,"CRegistryData_SetEnabledState_Transition");

inline CRegistryData_SetEnabledState_Transition::CRegistryData_SetEnabledState_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataSetEnabledStateTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_SetEnabledState_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataSetEnabledStateTransitMethod, "CRegistryData::SetEnabledState transition");
	Context().DataLogger().LogInformation(KRegistryDataSetEnabledStateTransitMethod);
	Context().iMethodCompletionStatus = Context().iRegistryData->SetEnabledState(Context().iImplementationUid, Context().iEnabledState);
	}

inline CRegistryData_UnitTestContext& CRegistryData_SetEnabledState_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataPersistNowLTransition,"CRegistryData_PersistNowL_Transition");

inline CRegistryData_PersistNowL_Transition::CRegistryData_PersistNowL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataPersistNowLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_PersistNowL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataPersistNowLTransitMethod, "CRegistryData::PersistNowL transition");
	Context().DataLogger().LogInformation(KRegistryDataPersistNowLTransitMethod);
	Context().iRegistryData->PersistNowL();
	}

inline CRegistryData_UnitTestContext& CRegistryData_PersistNowL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataTemporaryUninstallLTransition,"CRegistryData_TemporaryUninstallL_Transition");

inline CRegistryData_TemporaryUninstallL_Transition::CRegistryData_TemporaryUninstallL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataTemporaryUninstallLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_TemporaryUninstallL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataTemporaryUninstallLTransitMethod, "CRegistryData::TemporaryUninstallL transition");
	Context().DataLogger().LogInformation(KRegistryDataTemporaryUninstallLTransitMethod);
	Context().iRegistryData->TemporaryUninstallL(Context().iDriveUnit);
	}

inline CRegistryData_UnitTestContext& CRegistryData_TemporaryUninstallL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataUndoTemporaryUninstallLTransition,"CRegistryData_UndoTemporaryUninstallL_Transition");

inline CRegistryData_UndoTemporaryUninstallL_Transition::CRegistryData_UndoTemporaryUninstallL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataUndoTemporaryUninstallLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_UndoTemporaryUninstallL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataUndoTemporaryUninstallLTransitMethod, "CRegistryData::UndoTemporaryUninstallL transition");
	Context().DataLogger().LogInformation(KRegistryDataUndoTemporaryUninstallLTransitMethod);
	Context().iRegistryData->UndoTemporaryUninstallL(Context().iDriveUnit);
	}

inline CRegistryData_UnitTestContext& CRegistryData_UndoTemporaryUninstallL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataUpdateDllDataLTransition,"CRegistryData_UpdateDllDataL_Transition");

inline CRegistryData_UpdateDllDataL_Transition::CRegistryData_UpdateDllDataL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataUpdateDllDataLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_UpdateDllDataL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataUpdateDllDataLTransitMethod, "CRegistryData::UpdateDllDataL transition");
	Context().DataLogger().LogInformation(KRegistryDataUpdateDllDataLTransitMethod);
	Context().iRegistryData->UpdateDllDataL(Context().iFoundDriveIndex, Context().iNewDllData);
	
	Context().iNewDllData = NULL;
	}

inline CRegistryData_UnitTestContext& CRegistryData_UpdateDllDataL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataBuildIndexesLTransition,"CRegistryData_BuildIndexesL_Transition");

inline CRegistryData_BuildIndexesL_Transition::CRegistryData_BuildIndexesL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataBuildIndexesLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_BuildIndexesL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataBuildIndexesLTransitMethod, "CRegistryData::BuildIndexesL transition");
	Context().DataLogger().LogInformation(KRegistryDataBuildIndexesLTransitMethod);
	TRegistryData_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistryData_StateAccessor&, Context().StateAccessor());
		accessor.BuildIndexesL(*Context().iRegistryData);
	}

inline CRegistryData_UnitTestContext& CRegistryData_BuildIndexesL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataIndexedFindTransition,"CRegistryData_IndexedFind_Transition");

inline CRegistryData_IndexedFind_Transition::CRegistryData_IndexedFind_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataIndexedFindTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_IndexedFind_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataIndexedFindTransitMethod, "CRegistryData::IndexedFind transition");
	Context().DataLogger().LogInformation(KRegistryDataIndexedFindTransitMethod);
	TRegistryData_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistryData_StateAccessor&, Context().StateAccessor());
		accessor.IndexedFind(*Context().iRegistryData, Context().iImplementationUid);
	}

inline CRegistryData_UnitTestContext& CRegistryData_IndexedFind_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataRestoreLTransition,"CRegistryData_RestoreL_Transition");

inline CRegistryData_RestoreL_Transition::CRegistryData_RestoreL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataRestoreLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_RestoreL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataRestoreLTransitMethod, "CRegistryData::RestoreL transition");
	Context().DataLogger().LogInformation(KRegistryDataRestoreLTransitMethod);
	TRegistryData_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistryData_StateAccessor&, Context().StateAccessor());
		accessor.RestoreL(*Context().iRegistryData, Context().iReadStream);
	}

inline CRegistryData_UnitTestContext& CRegistryData_RestoreL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataStoreLTransition,"CRegistryData_StoreL_Transition");

inline CRegistryData_StoreL_Transition::CRegistryData_StoreL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataStoreLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_StoreL_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataStoreLTransitMethod, "CRegistryData::StoreL transition");
	Context().DataLogger().LogInformation(KRegistryDataStoreLTransitMethod);
	TRegistryData_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistryData_StateAccessor&, Context().StateAccessor());
	accessor.StoreL(*Context().iRegistryData, Context().iWriteStream);
	}

inline CRegistryData_UnitTestContext& CRegistryData_StoreL_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataMatchOnUidTransition,"CRegistryData_MatchOnUid_Transition");

inline CRegistryData_MatchOnUid_Transition::CRegistryData_MatchOnUid_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRegistryDataMatchOnUidTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_MatchOnUid_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataMatchOnUidTransitMethod, "CRegistryData::MatchOnUid transition");
	Context().DataLogger().LogInformation(KRegistryDataMatchOnUidTransitMethod);
	TRegistryData_StateAccessor& accessor = 
		REINTERPRET_CAST(TRegistryData_StateAccessor&, Context().StateAccessor());
		accessor.MatchOnUid();
	}

inline CRegistryData_UnitTestContext& CRegistryData_MatchOnUid_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataDiscoveriesBeginningTransition,"CRegistryData_DiscoveriesBeginning_Transition");

inline CRegistryData_DiscoveriesBeginning_Transition::CRegistryData_DiscoveriesBeginning_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KRegistryDataDiscoveriesBeginningTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CRegistryData_DiscoveriesBeginning_Transition::TransitMethodL()
	{
	_LIT(KRegistryDataDiscoveriesBeginningTransitMethod, "CRegistryData::DiscoveriesBeginning transition");
	Context().DataLogger().LogInformation(KRegistryDataDiscoveriesBeginningTransitMethod);

	Context().iRegistryData->DiscoveriesBeginning();
	}

inline CRegistryData_UnitTestContext& CRegistryData_DiscoveriesBeginning_Transition::Context() const
	{
	return REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	}

