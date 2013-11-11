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

#ifndef OMXILCONFIGMANAGERIMPL_H
#define OMXILCONFIGMANAGERIMPL_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>

#include "omxilindexmanager.h"

// Forward declarations
class MOmxILPortManagerIf;

class COmxILConfigManagerImpl : public COmxILIndexManager
	{

public:

	static COmxILConfigManagerImpl* NewL(
		const TDesC8& aComponentName,
		const OMX_VERSIONTYPE& aComponentVersion,
		const RPointerArray<TDesC8>& aComponentRoleList);

	~COmxILConfigManagerImpl();

	virtual OMX_ERRORTYPE GetComponentVersion(
		OMX_STRING aComponentName,
		OMX_VERSIONTYPE* apComponentVersion,
		OMX_VERSIONTYPE* apSpecVersion,
		OMX_UUIDTYPE* apComponentUUID) const;

	OMX_ERRORTYPE GetParameter(
		OMX_INDEXTYPE aParamIndex,
		TAny* apComponentParameterStructure) const;

	OMX_ERRORTYPE SetParameter(
		OMX_INDEXTYPE aParamIndex,
		const TAny* apComponentParameterStructure,
		OMX_BOOL aInitTime = OMX_TRUE);

	OMX_ERRORTYPE GetConfig(
		OMX_INDEXTYPE aConfigIndex,
		TAny* apComponentConfigStructure) const;

	OMX_ERRORTYPE SetConfig(
		OMX_INDEXTYPE aConfigIndex,
		const TAny* apComponentConfigStructure);

	OMX_ERRORTYPE ComponentRoleEnum(
		OMX_U8* aRole,
		OMX_U32 aIndex) const;
		
	void SetPortManager(MOmxILPortManagerIf* aPortManager);
	
private:

	COmxILConfigManagerImpl();

	void ConstructL(const TDesC8& aComponentName,
					const OMX_VERSIONTYPE& aComponentVersion,
					const RPointerArray<TDesC8>& aComponentRoleList);

	static TBool CompareRoles(const HBufC8& aRole1, const HBufC8& aRole2);
	
private:

	// Reference to the component's port manager
	MOmxILPortManagerIf* ipPortManager;

	// Reference to the component's port manager
	RBuf8 iComponentName;

	// The list of OpenMAX IL roles supported by the component
	RPointerArray<HBufC8> iComponentRoleList;

	// The current OpenMAX IL role
	TUint iCurrentRoleIndex;

	// The current version of this component (this is different to the spec
	// version)
	OMX_VERSIONTYPE iComponentVersion;

	// OpenMAX IL resource concealment structure (only set/get, resource
	// concealment logic not implemented)
	OMX_RESOURCECONCEALMENTTYPE iParamDisableResourceConcealment;

	// OpenMAX IL component suspension policy structure (only set/get,
	// component suspension logic not implemented)
	OMX_PARAM_SUSPENSIONPOLICYTYPE iParamSuspensionPolicy;

	// OpenMAX IL component priority structure (only set/get, component
	// priority logic not implemented)
	OMX_PRIORITYMGMTTYPE iConfigPriorityMgmt;

	};

#endif // OMXILCONFIGMANAGERIMPL_H
