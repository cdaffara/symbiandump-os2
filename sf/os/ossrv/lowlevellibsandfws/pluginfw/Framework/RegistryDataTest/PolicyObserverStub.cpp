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
//

#include "PolicyObserverStub.h"
#include "RegistryDataTransitions.h"

TPolicyObserverStub::TPolicyObserverStub(CRegistryData_UnitTestContext& aUTContext) :
iUTContext(aUTContext)
	{
	// Do nothing here
	}

void TPolicyObserverStub::DllDeletedL(CUnloadPolicy*& /*aUnloadPolicy*/)
	{
	_LIT(KDllDeleted, "TPolicyObserverStub::DllDeletedL called");
	iUTContext.DataLogger().LogInformation(KDllDeleted);
	}

CUnloadPolicy* TPolicyObserverStub::DllAdded(const CEComEntry& /*aDllInfo*/)
	{
	_LIT(KDllAdded, "TPolicyObserverStub::DllAdded called");
	iUTContext.DataLogger().LogInformation(KDllAdded);
	return NULL;
	}

void TPolicyObserverStub::UpdatePolicyIndexL()
	{
	_LIT(KUpdatePolicy, "TPolicyObserverStub::UpdatePolicyIndexL called");
	iUTContext.DataLogger().LogInformation(KUpdatePolicy);
	}
