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
// The definition of the MPolicyObserver class
// 
//

#ifndef __POLICYOBSERVER_H__
#define __POLICYOBSERVER_H__

#include "UnloadPolicy.h"
#include "EComEntry.h"

/**
	@internalComponent
	@since 7.0
	Used by CRegistryData to inform the load manager when a change to the registry
	occurs.
 */

class MPolicyObserver
	{
public:
/**
	@fn				DllDeletedL(CUnloadPolicy*& aUnloadPolicy) = 0
	Intended Usage	: Called to indicate that a dll has been removed from the registry
	Error Condition	: Leaves with an error code depending on implementation
	@since			7.0
	@param			aUnloadPolicy The CUnloadPolicy of the dll which has been removed
*/
	
	virtual void DllDeletedL(CUnloadPolicy*& aUnloadPolicy) = 0;

/**
	@fn				DllAdded(const CEComEntry& aDllInfo) = 0
	Intended Usage	: Called to indicate that a dll has been added to the registry
	Error Condition	: None
	@since			7.0
	@param			aDllInfo Information on the dll which has been added to the registry
	@return			The unload policy for the dll if one exists, NULL otherwise
*/
	
	virtual CUnloadPolicy* DllAdded(const CEComEntry& aDllInfo) = 0;

/**
	@fn				UpdatePolicyIndexL() = 0
	Intended Usage	: Called to indicate the index of unload policies will be out of date 
	Error Condition	: Leaves with an error code depending on implementation
	@since			7.0
	@param			aDllInfo Information on the dll which has been added to the registry
*/
	
	virtual void UpdatePolicyIndexL() = 0;
	};

#endif // __POLICYOBSERVER_H__
