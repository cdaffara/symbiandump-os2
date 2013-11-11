// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation for plugin used for resolver performance tests
// 
//

#include "EComPerfTestDllTwo.h"
// __________________________________________________________________________
// Implementation

CImplementationClassExampleTwo* CImplementationClassExampleTwo::NewL(TAny* aInitParams)
	{
	CImplementationClassExampleTwo* self=new(ELeave) CImplementationClassExampleTwo();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(aInitParams); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CImplementationClassExampleTwo::~CImplementationClassExampleTwo()
	{
	}

CImplementationClassExampleTwo::CImplementationClassExampleTwo()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CImplementationClassExampleTwo::ConstructL(TAny*)
	{
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009DF6,	CImplementationClassExampleTwo::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

