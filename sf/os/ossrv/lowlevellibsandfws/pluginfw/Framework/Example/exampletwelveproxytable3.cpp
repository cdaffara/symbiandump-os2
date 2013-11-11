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
// ExampleTweleveProxytable3.cpp
// The proxy table of PlugIn EComExample12.
// 
//

/**
 @file
 @internalComponent
*/

#include "ExtendedInterfaceImplementationProxy.h"
#include "exampletwelve.h"

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy3 KImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY3(0x10009E39,CImplementationClassTwelve::NewL,CImplementationClassTwelve::GetExtendedInterfaceL,CImplementationClassTwelve::ReleaseExtendedInterface),
	IMPLEMENTATION_PROXY_ENTRY3(0x10009E3C,CImplementationClassTwelveBasic::NewL,0,0),
	IMPLEMENTATION_PROXY_ENTRY3(0x10009E3D,CImplementationClassTwelve2::NewL,CImplementationClassTwelve2::GetExtendedInterfaceL,0),
	};

EXPORT_C const TImplementationProxy3* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy3);
	return KImplementationTable;
	}
