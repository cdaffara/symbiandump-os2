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
 @internalComponent
*/

#include <e32cmn.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>

#include "log.h"
#include <openmax/il/common/omxilconfigmanager.h>
#include <openmax/il/common/omxilspecversion.h>
#include <openmax/il/common/omxilutil.h>
#include "omxilportmanagerif.h"
#include "omxilconfigmanagerimpl.h"

EXPORT_C COmxILConfigManager*
COmxILConfigManager::NewL(
	const TDesC8& aComponentName,
	const OMX_VERSIONTYPE& aComponentVersion,
	const RPointerArray<TDesC8>& aComponentRoleList)
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::NewL"));
	__ASSERT_DEBUG(aComponentName.Length() && aComponentRoleList.Count(),
				   User::Panic(KOmxILConfigManagerPanicCategory, 1));

	COmxILConfigManager* self = new (ELeave)COmxILConfigManager();
	CleanupStack::PushL(self);
	self->ConstructL(aComponentName,
					 aComponentVersion,
					 aComponentRoleList);
	CleanupStack::Pop(self);
	return self;

	}

EXPORT_C void
COmxILConfigManager::ConstructL(const TDesC8& aComponentName,
								const OMX_VERSIONTYPE& aComponentVersion,
								const RPointerArray<TDesC8>& aComponentRoleList)
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::ConstructL"));
	ipConfigManagerImpl=COmxILConfigManagerImpl::NewL(aComponentName,
													aComponentVersion,
													aComponentRoleList);
	}

EXPORT_C
COmxILConfigManager::COmxILConfigManager()
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::COmxILConfigManager"));
	}

EXPORT_C
COmxILConfigManager::~COmxILConfigManager()
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::~COmxILConfigManager"));

	delete ipConfigManagerImpl;
	}

OMX_ERRORTYPE
COmxILConfigManager::GetComponentVersion(OMX_STRING aComponentName,
										 OMX_VERSIONTYPE* apComponentVersion,
										 OMX_VERSIONTYPE* apSpecVersion,
										 OMX_UUIDTYPE* apComponentUUID) const
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::GetComponentVersion"));
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->GetComponentVersion(aComponentName,
												apComponentVersion,
												apSpecVersion,
												apComponentUUID);
	}

EXPORT_C OMX_ERRORTYPE
COmxILConfigManager::GetParameter(OMX_INDEXTYPE aParamIndex,
								  TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::GetParameter"));
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->GetParameter(aParamIndex, apComponentParameterStructure);
	}

EXPORT_C OMX_ERRORTYPE
COmxILConfigManager::SetParameter(OMX_INDEXTYPE aParamIndex,
								  const TAny* apComponentParameterStructure,
								  OMX_BOOL aInitTime /*  = OMX_TRUE */)
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::SetParameter"));
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->SetParameter(aParamIndex, apComponentParameterStructure, aInitTime);
	}

EXPORT_C OMX_ERRORTYPE
COmxILConfigManager::GetConfig(OMX_INDEXTYPE aConfigIndex,
							   TAny* apComponentConfigStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::GetConfig"));
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->GetConfig(aConfigIndex, apComponentConfigStructure);
	}

EXPORT_C OMX_ERRORTYPE
COmxILConfigManager::SetConfig(OMX_INDEXTYPE aConfigIndex,
							   const TAny* apComponentConfigStructure)

	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::SetConfig"));
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->SetConfig(aConfigIndex, apComponentConfigStructure);
	}

EXPORT_C OMX_ERRORTYPE
COmxILConfigManager::GetExtensionIndex(
	OMX_STRING /*aParameterName*/,
	OMX_INDEXTYPE* /*apIndexType*/) const
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::GetExtensionIndex"));

	// No custom index here for now...
	return OMX_ErrorUnsupportedIndex;
	}

OMX_ERRORTYPE
COmxILConfigManager::ComponentRoleEnum(OMX_U8* aRole,
									   OMX_U32 aIndex) const
	{
    DEBUG_PRINTF(_L8("COmxILConfigManager::ComponentRoleEnum"));
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->ComponentRoleEnum(aRole, aIndex);
	}

void COmxILConfigManager::SetPortManager(MOmxILPortManagerIf* aPortManager)
	{
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
	return ipConfigManagerImpl->SetPortManager(aPortManager);
	}
EXPORT_C
void COmxILConfigManager::InsertParamIndexL(TUint aParamIndex)
    {
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
    return ipConfigManagerImpl->InsertParamIndexL(aParamIndex);
    }
EXPORT_C
TInt COmxILConfigManager::FindParamIndex(TUint aParamIndex) const
    {
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
    return ipConfigManagerImpl->FindParamIndex(aParamIndex);
    }
EXPORT_C
void COmxILConfigManager::InsertConfigIndexL(TUint aConfigIndex)
    {
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
    return ipConfigManagerImpl->InsertConfigIndexL(aConfigIndex);
    }
EXPORT_C
TInt COmxILConfigManager::FindConfigIndex(TUint aConfigIndex) const
    {
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
    return ipConfigManagerImpl->FindConfigIndex(aConfigIndex);
    }

RArray<TUint>& COmxILConfigManager::ManagedParamIndexes()
    {
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
    return ipConfigManagerImpl->ManagedParamIndexes();
    }
EXPORT_C
RArray<TUint>& COmxILConfigManager::ManagedConfigIndexes()
    {
    __ASSERT_ALWAYS(ipConfigManagerImpl, User::Panic(KOmxILConfigManagerPanicCategory, 1));
    return ipConfigManagerImpl->ManagedConfigIndexes();
    }
EXPORT_C
TInt COmxILConfigManager::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return CBase::Extension_(aExtensionId, a0, a1);
	}
