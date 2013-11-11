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
// This file contains the definition for the Extended Interface TImplementationProxy structure.
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __EXTENDED_INTERFACE_IMPLEMENTATIONPROXY_H__
#define __EXTENDED_INTERFACE_IMPLEMENTATIONPROXY_H__

// ____________________________________________________________________________
// INCLUDES

#include <e32std.h>
#include <ecom/implementationproxy.h>

// ____________________________________________________________________________
// TYPES - TImplementationProxy3

/**
Pre-defined function pointer type used in TImplementationProxy3 to fetch the extended interface
@param      aObject A pointer to the instantiation interface implementation returned by a call to one of the 
            CreateImplementationL() methods
@param      aExtendedInterfaceUid Identifies the interface to fetch from the instantiation interface implementation.
@param      aBitFlags Output parameter used for communication between plugin's and ECOM. Currently used by plugin implementors 
            to signal if an extended interface implementation requires release.
@param      aReleaseObject Optional output parameter. If the plugin is allocating a new extension object then the address 
            returned by new must be returned in this argument for ECOM to supply it back to the plugin implementation for 
            later deletion when release is requried, aBitFlags must have the KReleaseRequiredMask flag set in such cases. 
            If a simple re-cast extension pattern is used in this proxy function then this argument should be left unchanged.
@leave		KErrNoMemory
@leave      Or any of the System Wide error codes
@return     TAny* A pointer to the extended interface implementation if the instantiation interface implementation
            supports it, or NULL if the instantiation interface implementation does not support it.
@publishedPartner
@released
*/
typedef TAny* (*TFuncPtrGetL)(TAny* /* aObject */, const TUid& /* aExtendedInterfaceUid */, TUint32& /* aBitFlags */, TAny*& /* aReleaseObject */);

/**
Pre-defined function pointer type used in TImplementationProxy3 to release the 
extended interface. Implementations of this function must never leave.
@param      aReleaseObject The release object address, previously supplied by a function of type TProxyExtendedInterfaceGetPtrL,
            that is to be deleted by the instantiation interface implementation.
@param      aExtendedInterfaceUid Identifies the extended interface implementation that is to be released. If the supplied
            uid is not recognised then this function should be implemented in such a way that it returns without performing
            any cleanup.  
@publishedPartner
@released
*/
typedef void (*TFuncPtrRelease)(TAny* /* aReleaseObject */, const TUid& /* aExtendedInterfaceUid */);

/**
Type definition for function pointer to fetch the extended interface.
@publishedPartner
@released
*/
typedef TFuncPtrGetL TProxyExtendedInterfaceGetPtrL;

/**
Type definition for function pointer to release the extended interface.
@publishedPartner
@released
*/
typedef TFuncPtrRelease TProxyExtendedInterfaceReleasePtr;

/**
Extended interface bit masks.
@publishedPartner
@released
*/
const TUint32 KReleaseRequiredMask = 0x00000001;


/**
The structure that defines the pairing required by the ECOM framework
to correctly identify the instantiation method pointer to provide
to a client's resolution request. This is for PLUGIN3 dll's and
implementations.

@publishedPartner
@released
*/
typedef struct
	{
	/** The unique UID for an Interface Implementation */
	TUid	iImplementationUid;
	/** The function pointer to the instantiation method */
	TProxyNewLPtr iNewLFuncPtr;
	/** The function pointer to the method used to fetch extended interfaces*/
	TProxyExtendedInterfaceGetPtrL iFuncPtrInterfaceGetL;
	/** The function pointer used to release the extended interfaces. This MUST be a non-leaving method.*/
	TProxyExtendedInterfaceReleasePtr iFuncPtrInterfaceRelease;
	/** reserved for future proofing*/
	TInt32 iReserved1;
	TInt32 iReserved2;
	TInt32 iReserved3;
	TInt32 iReserved4;
	} TImplementationProxy3;
	
/**
Macro to initialise an element of a static array of TImplementationProxy objects with PLUGIN3 type.
This macro deals with casting the function pointer to the type used in the structure.
@publishedPartner
@released
*/
#define IMPLEMENTATION_PROXY_ENTRY3(aUid, aFuncPtr, aGetExtFuncPtr, aReleaseExtFuncPtr)	{{aUid},(TProxyNewLPtr)(aFuncPtr),(aGetExtFuncPtr),(aReleaseExtFuncPtr),0,0,0,0}

#endif	// __EXTENDED_INTERFACE_IMPLEMENTATIONPROXY_H__
