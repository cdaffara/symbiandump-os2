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
// This file contains the definition for the TImplementationProxy structure.
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __IMPLEMENTATIONPROXY_H__
#define __IMPLEMENTATIONPROXY_H__

// ____________________________________________________________________________
// INCLUDES


#include <e32std.h>


// ____________________________________________________________________________
// TYPES - TImplementationProxy

/**
Pre-defined function pointer type used in TImplementationProxy
@leave		KErrNoMemory
@leave      Or any of the System Wide error codes
@return     TAny* A pointer to the instantiated interface implementation.
*/
typedef TAny* (*TFuncPtr)();

/**
Typedef used to cover the difference between EABI and older handling
of the instantiation method pointer.
*/
#if  defined(__EABI__) || defined(__GCCXML__) || defined(__X86GCC__) || defined(__GCCE__)
  typedef TFuncPtr TProxyNewLPtr;
#else
  typedef TAny* TProxyNewLPtr;
#endif


/**
The structure that defines the pairing required by the ECOM framework
to correctly identify the instantiation method pointer to provide
to a client's resolution request.
*/
typedef struct
	{
	/** The unique UID for an Interface Implementation */
	TUid	iImplementationUid;
	/** The pointer to the instantiation method for the Interface Implementation identified by the UID.*/
	TProxyNewLPtr iNewLFuncPtr;
	} TImplementationProxy;
	
/**
Macro to initialise an element of a static array of TImplementationProxy objects.
This macro deals with casting the function pointer to the type used in the structure.
*/
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(TProxyNewLPtr)(aFuncPtr)}

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecom/extendedinterfaceimplementationproxy.h>
#endif


#endif	// __IMPLEMENTATIONPROXY_H__
