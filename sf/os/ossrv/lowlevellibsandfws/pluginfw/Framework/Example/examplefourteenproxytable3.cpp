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
// The proxy table of PlugIn EComExample14Upgraded
// 
//

/**
 @file
 @internalComponent
*/

#include "ecom/extendedinterfaceimplementationproxy.h"
#include "examplefourteen.h"

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy3 ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY3(0x10009E48,CImplementationClassFourteen::NewL, 0, 0)
	};

EXPORT_C const TImplementationProxy3* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy3);

	return ImplementationTable;
	}

