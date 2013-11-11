/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include "msfdc.h"

const TImplementationProxy ImplementationTable[] =
	{
#ifndef __DUMMYFDF__
	IMPLEMENTATION_PROXY_ENTRY(0x10285B0D, CMsFdc::NewL),
#else
	IMPLEMENTATION_PROXY_ENTRY(0x20031DF7, CMsFdc::NewL),
#endif
	};
/**
  A factory method that returns the number of implementation of an interfaces
  and the implementation proxy entry, in the entry the pointer of the factory method 
  that constructs a CMsFdc is also returned.
 
  @param	aTableCount	A reference of TInt that holds the number of entries in the
  			implementation proxy entry table.
  @return	The implementation proxy entry table.
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
