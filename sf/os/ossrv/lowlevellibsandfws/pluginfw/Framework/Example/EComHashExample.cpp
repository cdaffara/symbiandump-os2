// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of a plugin used for hash file validation tests
// 
//

#include "EComHashExample.h"
#include "TestUtilities.h"	
// __________________________________________________________________________
// Implementation

CImplementationHashExample* CImplementationHashExample::NewL(TAny* aInitParams)
	{
	CImplementationHashExample* self=new(ELeave) CImplementationHashExample();  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(aInitParams); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

CImplementationHashExample::~CImplementationHashExample()
	{
	}

CImplementationHashExample::CImplementationHashExample()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

void CImplementationHashExample::ConstructL(TAny*)
	{
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009E35,	CImplementationHashExample::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

