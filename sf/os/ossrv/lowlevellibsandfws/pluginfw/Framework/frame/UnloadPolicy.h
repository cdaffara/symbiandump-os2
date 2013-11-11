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
// This file contains the definition of the CUnloadPolicy class.
// 
//

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_UNLOADPOLICY_3A1D309500CE_INCLUDED
#define _INC_UNLOADPOLICY_3A1D309500CE_INCLUDED

#include <e32base.h>
#include <f32file.h>
#include <e32uid.h>
#include <ecom/extendedinterfaceimplementationproxy.h>
#include "EComEntry.h"

/** 
Type of the function pointer which is the proxy into the interface implementation collection 
@internalComponent
*/
typedef TImplementationProxy* (*TInstantiationL)(TInt&);

/** 
Type of the function pointer which is the proxy into the interface implementation collection 3
@internalComponent
*/
typedef TImplementationProxy3* (*TInstantiation3L)(TInt&);
/**
@enum TUnloadPolicyStatus
@since 7.0
Signifies if an UnloadPolicy can be deleted after its reference count has been decreased.
This will be true if the reference count is zero and the policy is on the garbage list.
@internalComponent
*/
enum TUnloadPolicyStatus {EDeleteMe, EDontDeleteMe};

/**
	@internalComponent
	@since 7.0
	Controls the unloading of implementation groupings
 */

NONSHARABLE_CLASS(CUnloadPolicy) : public CBase
{
public:
/**
	@fn				NewLC(const TEntry& aDllEntry)
	Intended Usage	: Standardized safe construction which leaves the object on the cleanup stack.
	Error Condition	: Cannot fully construct because of memory limitations.
	@leave  		KErrNoMemory
	@since			7.0
	@param			aDllEntry Information on the dll which this policy refers to
	@return			A pointer to the new class
	@post			CUnloadPolicy is fully constructed and initialized.
 */
	
	static CUnloadPolicy* NewLC(const TEntry& aDllEntry);

/**
	@fn				~CUnloadPolicy()
	Intended Usage	: Standard default d'tor
	Error Condition	: None
	@since			7.0
 */
	
	~CUnloadPolicy();

/**
	@fn				DecreaseReference()
	Intended Usage	: Decrease the reference count for the specified grouping and therefore 
					check whether it should be unloaded.
	Error Condition	: None.
	@since			7.0
	@return			EDeleteMe if this unload policy can be deleted because its reference 
					count is zero and it is on the garbage list.
	@pre			This object is fully constructed and initialized
	@post			If the decreased reference count has reached zero  and the policy is not
					on the garbage list then the timer is activated.
 */
	
	TUnloadPolicyStatus DecreaseReference();

/**
	@fn				IncreaseReference()
	Intended Usage	: Increase the reference count for the specified grouping.
	Error Condition	: None.
	@since			7.0
	@pre			This object is fully constructed and initialized
	@post			The reference count has been increased and if there was an outstanding timer 
					request it has been cancelled.
 */
	
	void IncreaseReference();

/**
	@fn				ReferenceCount() const
	Intended Usage	: Returns the current reference count for this grouping
	Error Condition	: None.
	@since			7.0
	@return			The current library reference count for this unload policy
	@pre			This object is fully constructed and initialized
 */
	
	TInt ReferenceCount() const;

/**
	@fn				DllEntryInformation() const
	Intended Usage	: Returns information on the dll which this unload policy refers to.
	Error Condition	: None.
	@since			7.0
	@pre			This object is fully constructed and initialized
 */
	
	const CEComEntry& DllEntryInformation() const;

/**
	@fn				LoadDllAndReturnProxyL()
	Intended Usage	: Loads the dll which this unload policy refers to and returns a function pointer
					to the proxy into the dll.
	Error Condition	: Leaves with an error code.
	@leave  		KErrInvalidDll, @see RLibrary::Load
	@since			7.0
	@return			The proxy function into the dll to which this unload policy refers.
	@pre			This object is fully constructed and initialized
	@post			iLibrary is unloaded
 */
	
	TLibraryFunction LoadDllAndReturnProxyL();

private:
/**
	@internalComponent
	@fn				CUnloadPolicy()
	Intended Usage	: Standardized default c'tor
	Error Condition	: None
	@since			7.0
	@post			CUnloadPolicy is fully constructed
 */
	
	CUnloadPolicy();

/**
	@fn				ConstructL(const TEntry& aDllEntry)
	Intended Usage	: Standardized 2nd, (Initialization) phase of two phase construction.
	Error Condition	: None
	@leave  		None
	@since			7.0
	@param			aDllEntry Information on the dll to which this unload policy refers
	@pre 			CUnloadPolicy is fully constructed.
	@post			CUnloadPolicy is fully initialized.
 */
	
	void ConstructL(const TEntry& aDllEntry);
	
//Attributes
private:
	/** The controlled implementation grouping */
	
	RLibrary iLibrary;
	/** Information on the Dll which this is the unload policy for */
	
	const CEComEntry* iDllEntry;
	/** 
		A reference count indicating that an implementation from
		this implementation grouping has been instantiated / removed
	*/	
	TInt iReferencesInUseCount;
};

#endif /* _INC_UNLOADPOLICY_3A1D309500CE_INCLUDED */

