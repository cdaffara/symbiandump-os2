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
// Complements ExampleResolver.cpp to form a custom resolver DLL.
// The Implementation UID is defined in the mmp file.
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include "ExampleResolver.h"

#if defined (CUSTOMRESOLVERIMPLUID_200126CD)
	#define MYIMPLUID 0x200126CD
#elif defined (CUSTOMRESOLVERIMPLUID_A0001346)
	#define MYIMPLUID 0xA0001346
#elif defined (CUSTOMRESOLVERIMPLUID_A0001347)
	#define MYIMPLUID 0xA0001347
#endif

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(MYIMPLUID, CExampleResolver::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
