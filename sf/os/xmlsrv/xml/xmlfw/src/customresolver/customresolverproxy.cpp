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
// Proxy methods implementation for ECOM 
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "customresolver.h"


GLDEF_C TInt E32Dll()
	{
	return KErrNone;
	}

// Exported proxy for instantiation method resolution

// Define the Implementation UIDs for XmlCustomResolver
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10273862, Xml::CCustomResolver::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

