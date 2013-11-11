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
// Stub of the MPolicyObserver interface which is required by 
// CRegistryData.
// 
//

#ifndef __POLICYOBSERVERSTUB_H__
#define __POLICYOBSERVERSTUB_H__

#include "PolicyObserver.h"

class CRegistryData_UnitTestContext;

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Stub providing the MPolicyObserver functionality which is required by 
	CRegistryData.
 */
class TPolicyObserverStub : public MPolicyObserver
	{
public:
/**
	@fn				TPolicyObserverStub(CRegistryData_UnitTestContext& aUTContext)
	Intended Usage	: Constructor
	@leave  		KErrNoMemory
	@since			7.0
	@param			aUTContext The context within which this stub is being used.
*/
	
	TPolicyObserverStub(CRegistryData_UnitTestContext& aUTContext);

/**
	@fn				DllDeletedL(CUnloadPolicy*& aUnloadPolicy)
	Intended Usage	: Called to indicate that a dll has been removed from the registry
	Error Condition	: Leaves with an error code depending on implementation
	@since			7.0
	@param			aUnloadPolicy The CUnloadPolicy of the dll which has been removed
*/
	
	void DllDeletedL(CUnloadPolicy*& aUnloadPolicy);

/**
	@fn				DllAdded(const CEcomEntry& aDllInfo)
	Intended Usage	: Called to indicate that a dll has been added to the registry
	Error Condition	: None
	@since			7.0
	@param			aDllInfo Information on the dll which has been added to the registry
	@return			The unload policy for the dll if one exists, NULL otherwise
*/
	
	CUnloadPolicy* DllAdded(const CEComEntry& aDllInfo);

/**
	@fn				UpdatePolicyIndexL()
	Intended Usage	: Called to indicate the index of unload policies will be out of date 
	Error Condition	: Leaves with an error code depending on implementation
	@since			7.0
	@param			aDllInfo Information on the dll which has been added to the registry
*/
	
	void UpdatePolicyIndexL();

private:
	CRegistryData_UnitTestContext& iUTContext;
	};	// TPolicyObserverStub

#endif		// __POLICYOBSERVERSTUB_H__
