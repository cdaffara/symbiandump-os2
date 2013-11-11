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
// The implementation of a simple ECOM interface for testing purpose.
// 
//

/**
 @file
 @internalComponent
*/

#include "exampleNine.h"
#include "ImplementationProxy.h"

/**
Factory instantiation method.
*/
CRoguePlugin* CRoguePlugin::NewL(TAny*)
	{
	CRoguePlugin* self=new(ELeave) CRoguePlugin();
	return self;
	}

/** virtual destructor */
CRoguePlugin::~CRoguePlugin()
	{
	}

/** constructor */
CRoguePlugin::CRoguePlugin()
: CBase()
	{
	}

/** a test function */
TUid CRoguePlugin::InterfaceUid()
	{
	return KRogueInterfaceUid;
	}

// ___________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	// implementation UID 10009DC3 is in-used by many example ECOM
	// magic plugins
	IMPLEMENTATION_PROXY_ENTRY(KRogueImplUidValue,	CRoguePlugin::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
