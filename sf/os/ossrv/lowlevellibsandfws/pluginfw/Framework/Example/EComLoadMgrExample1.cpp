// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EComLoadMgrExample1.h
// The implementation of a plugin used for load manager validation tests
// 
//

#include "EComLoadMgrExample1.h"
#include <e32base.h>
// __________________________________________________________________________
// Implementation

CImplementationLoadMgrExample1* CImplementationLoadMgrExample1::NewL(TAny* aInitParams)
	{
	CImplementationLoadMgrExample1* self=new(ELeave) CImplementationLoadMgrExample1();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(aInitParams); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CImplementationLoadMgrExample1::~CImplementationLoadMgrExample1()
	{
	}

CImplementationLoadMgrExample1::CImplementationLoadMgrExample1()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CImplementationLoadMgrExample1::ConstructL(TAny*)
	{
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10282332,	CImplementationLoadMgrExample1::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x10282334,	CImplementationLoadMgrExample1::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

