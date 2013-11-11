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
// Implementation of the CLoadManager class
// 
//

/**
 @internalComponent
 @file
*/

#include "EComDebug.h"
#include "UnloadPolicy.h"
#include "LoadManager.h"
#include "EComUidCodes.h"
#include <ecom/ecomerrorcodes.h>
#include "e32math.h"
#include "EComInternalErrorCodes.h"
#include <ecom/ecomextendedinterfaceerrorcodes.h>
/**
Standardized safe construction which leaves nothing on the cleanup stack.
@return			A pointer to the new class
@post			CLoadManager is fully constructed, and initialized.
 */
CLoadManager* CLoadManager::NewL()
	{
	return new(ELeave) CLoadManager();
	}

CLoadManager::~CLoadManager()
	{
	iInstanceInfoList.ResetAndDestroy();
	iAllUnloadPolicies.ResetAndDestroy();
	ClearGarbage();
	}

/**
Notifies the interface implementation DLL that one of its objects has been destroyed if it exists,
otherwise returns false to indicate no exist aImplementationUid.
@param          aInstanceKey A key specifying a previously created implementation.
@pre 			CLoadManager is fully constructed,
@post			CLoadManager's interface implementation DLL references
				are decreased by one. The instance info representing the implementation
				is destroyed.
*/
TBool CLoadManager::DestroyedThis(TUid aInstanceKey)
	{
	// Clear the garbage list because we know we have finished with them
	ClearGarbage();

	__ECOM_TRACE1("ECOM: Implementation Instance destroyed %x", aInstanceKey.iUid);

	// Sanity check that the pointer is divisible by four. A binary number is divisible
	// by four if and only if the two rightmost bits are both zero.
	// This is a compromised check for checking that the pointer is an address.
    if ((aInstanceKey.iUid == 0) || ((aInstanceKey.iUid & 0x00000003) != 0))
        {
	    __ASSERT_DEBUG(EFalse, User::Panic(KEComClientDLLPanicCategory, EEComPanic_InvalidImplementationInstanceKey));
	    User::Leave(KErrNotFound);
        }

	// The instance info pointer is stored in the instance key.
	CInstanceInfo* instanceInfo = reinterpret_cast<CInstanceInfo*>(aInstanceKey.iUid);

	// Check that the pointer exists before using it
	TInt position = iInstanceInfoList.FindInAddressOrder(instanceInfo);
	if(position == KErrNotFound)
		{
		return EFalse;
		}
	
	CUnloadPolicy* policy = instanceInfo->UnloadPolicy();
	if (policy != NULL)
		{
		// If needed, will move the policy to the garbage list, and remove policy from the unload policy list.
		Cleanup(policy);
		}
	iInstanceInfoList.Remove(position);
	// Remove the instance info item, finished with it.
	delete instanceInfo;
	return ETrue;
	}

/**
Check whether the policy Arrays are empty or not.
@return            Returns True if the policy Arrays are empty, otherwise return False.
@pre             CLoadManager is fully constructed,
@post            CLoadManager remains the same.
*/
TBool CLoadManager::PolicyArraysEmpty() const 
    {
    if( (iAllUnloadPolicies.Count() == 0)&&(iInstanceInfoList.Count() == 0) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

/**
Returns an implementation object to satisfy the specified interface.
@param			aUniqueImplementationUid The implementation to find.
@param			aEntry Information on the dll containing the implementation
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of
				aCreationParameters is NULL.
@param          aInstanceKey A key specifying a previously created implementation.
@return			TAny* a pointer to the fully constructed instantiation. The pointer is
				guaranteed to be valid only until DestroyedThis is called.
@pre 			CLoadManager is fully constructed,
@post			Fully constructed implementation is returned to the
				caller, and aUniqueUid contains the implementation Dll's
				unique UID.
*/
TAny* CLoadManager::ImplementationObjectL(const TUid& aUniqueImplementationUid,
										 const TEntry& aEntry,
										 TAny* aCreationParameters,
										 TBool aCreationParamsFlag,
										 TUid& aInstanceKey)
	{
	//if the implementation Uid here is Null or the entry returned by the ecomserver
	//contains nothing, we should leave with KErrNotFound
	if(aUniqueImplementationUid == KNullUid || (aEntry.iName).Length()==0)
		{
		User::Leave(KErrNotFound);
		}

	TAny* object = NULL;               // Instantiation object
	CUnloadPolicy* policy = NULL;      // Policy of implementation
	TLibraryFunction libFunctionProxy; // Library function proxy pointer

	GetUnloadPolicy(aUniqueImplementationUid,aEntry,policy);
	if (policy == NULL)
		{
		// No policy found, so create a new CUnloadPolicy and load DLL.
		policy = CUnloadPolicy::NewLC(aEntry);
		libFunctionProxy = policy->LoadDllAndReturnProxyL();
		if (libFunctionProxy==NULL)
			{
			User::Leave(KErrNotFound);
			}
		iAllUnloadPolicies.AppendL(policy);
		CleanupStack::Pop(policy);	// owned by iAllUnloadPolicies
		}
	else
		{
		// Found a policy. Load Dll if not already loaded.
		libFunctionProxy = policy->LoadDllAndReturnProxyL();
		if (libFunctionProxy==NULL)
			{
			User::Leave(KErrNotFound);
			}
		}
	// Initial count of instances. This is used for cleanup purposes, to see if an instance
	// was added to the instance info list during the object creation. If a failure occurs
	// than the instance info that was added to the list will be removed.
	TInt initialCount = iInstanceInfoList.Count();
	TInt err = KErrNone;

	if (aEntry[1] == KUidInterfaceImplementationCollection)
		{
		// PLUGIN1 dll. Create the implementation object.
		TRAP(err,object = ImplementationObject1L(aUniqueImplementationUid,aCreationParameters,
			aCreationParamsFlag,aInstanceKey,policy,libFunctionProxy));
		}
	else if (aEntry[1] == KUidInterfaceImplementationCollection3)
		{
		// PLUGIN3 dll. Create the implementation object.
		TRAP(err,object = ImplementationObject3L(aUniqueImplementationUid,aCreationParameters,
			aCreationParamsFlag, aInstanceKey,policy, libFunctionProxy));
		}
	else
		{
		err = KErrNotSupported;
		}
	
	if (err != KErrNone)
		{
		if (iInstanceInfoList.Count() > initialCount)
			{
			// If an instance was added to the instance list, remove it here.
			// The instance info pointer is stored in the instance key. We know its
			// valid because it was set by ecom in call to ImplementationObject1L or
			// ImplementationObject3L
			CInstanceInfo* instanceInfo = reinterpret_cast<CInstanceInfo*>(aInstanceKey.iUid);
			TInt pos = iInstanceInfoList.FindInAddressOrder(instanceInfo);
			if(pos != KErrNotFound)
				{
				iInstanceInfoList.Remove(pos);
				}
			}

		ClearGarbage();
		// If needed, will move the policy to the garbage list, and remove policy from the unload policy list.
		Cleanup(policy);
		User::Leave(err);
		}
		
	__ECOM_TRACE2("ECOM: Implementation created (%03d) %x", ++iDebugInstantiationCounter, aUniqueImplementationUid.iUid);
			
	return object;
	}

/**
Returns an implementation object to satisfy the specified interface.
@param			aUniqueImplementationUid The implementation to find.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of
				aCreationParameters is NULL.
@param          aInstanceKey A key specifying a previously created implementation.
@param          aPolicy policy of implementation
@param          aLibFunctionProxy Library function proxy pointer.
@return			TAny* a pointer to the fully constructed instantiation. The pointer is
				guaranteed to be valid only until DestroyedThis is called.
*/
TAny* CLoadManager::ImplementationObject1L(const TUid& aUniqueImplementationUid,
										 TAny* aCreationParameters,
										 TBool aCreationParamsFlag,
										 TUid& aInstanceKey,
										 CUnloadPolicy* aPolicy,
										 TLibraryFunction& aLibFunctionProxy)
	{
	TAny* object = NULL; // Instantiation object
	TInstantiationL proxy = reinterpret_cast<TInstantiationL>(aLibFunctionProxy);

	TImplementationProxy* implementationProxyRow = NULL;
	TAny* newLPointer = GetNewLPointerAndProxyTableRowL<TImplementationProxy,TInstantiationL>(aUniqueImplementationUid,implementationProxyRow,proxy);
	// Now create an instance info object to store the information about this instance derived from
	// the parameters just fetched. This must be created here since no leaves can occur after the object
	// instantiation below.
	CInstanceInfoSimple* instanceInfoSimple = CInstanceInfoSimple::NewL(aPolicy,implementationProxyRow);
	CleanupStack::PushL(instanceInfoSimple);

	// The pointer to instanceInfo will be used to identify this instance, and will
	// be returned to the caller for future identification of this instance.
	aInstanceKey.iUid = reinterpret_cast<TInt32>(instanceInfoSimple);

	// Add item to instance info list. This list will contain all of the instance objects.
	iInstanceInfoList.InsertInAddressOrderL(instanceInfoSimple);

	// Get the implementation object using the instantiation pointer fetched earlier. Note
	// that the object creation must be the last leaving operation to be performed. No leaves can occur
	// after this operation, as the instantiation object cannnot be deleted on the cleanup stack if
	// a leave occurs (cannot delete a TAny* pointer, the type is not known within ECOM).
	object = ImplementationObjectL(aCreationParameters, 
	 aCreationParamsFlag,newLPointer);

	CleanupStack::Pop(instanceInfoSimple);

	return object;
	}

/**
Returns an implementation object to satisfy the specified interface.
@param			aUniqueImplementationUid The implementation to find.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of
				aCreationParameters is NULL.
@param          aInstanceKey A key specifying a previously created implementation.
@param          aPolicy policy of implementation
@param          aLibFunctionProxy Library function proxy pointer.
@return			TAny* a pointer to the fully constructed instantiation. The pointer is
				guaranteed to be valid only until DestroyedThis is called.
*/
TAny* CLoadManager::ImplementationObject3L(const TUid& aUniqueImplementationUid,
										 TAny* aCreationParameters,
										 TBool aCreationParamsFlag,
										 TUid& aInstanceKey,
										 CUnloadPolicy* aPolicy,
										 TLibraryFunction& aLibFunctionProxy)
	{
	TAny* object = NULL;  // Instantiation object
	TInstantiation3L proxy = reinterpret_cast<TInstantiation3L>(aLibFunctionProxy);

	TImplementationProxy3* implementationProxyRow = NULL;
	TAny* newLPointer = GetNewLPointerAndProxyTableRowL<TImplementationProxy3,TInstantiation3L>(aUniqueImplementationUid,implementationProxyRow,proxy);

	// Now create an instance info object to store the information about this instance derived from
	// the parameters just fetched. This must be created here since no leaves can occur after the object
	// instantiation below.
	CInstanceInfoExtended* instanceInfoExtended = CInstanceInfoExtended::NewL(aPolicy,implementationProxyRow);
	CleanupStack::PushL(instanceInfoExtended);

	// The pointer to instanceInfo will be used to identify this instance, and will
	// be returned to the caller for future identification of this instance.
	aInstanceKey.iUid = reinterpret_cast<TInt32>(instanceInfoExtended);

	// Add item to instance info list. This list will contain all of the instance objects.
	iInstanceInfoList.InsertInAddressOrderL(instanceInfoExtended);

	// Get the implementation object using the instantiation pointer fetched earlier. Note
	// that the object creation must be the last leaving operation to be performed. No leaves can occur
	// after this operation, as the instantiation object cannnot be deleted on the cleanup stack if
	// a leave occurs (cannot delete a TAny* pointer, the type is not known within ECOM).
	object = ImplementationObjectL(aCreationParameters, 
	 aCreationParamsFlag,newLPointer);

	// The extended instance info requires an object set. This is done here
	// as the object is not known until now, but the instance info object was required to
	// be created earlier to avoid a leave after the object creation.
	instanceInfoExtended->SetObject(object);
	CleanupStack::Pop(instanceInfoExtended);

	return object;
	}

/**
Gets the main implementation object using the instantiation pointer provided
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of
				aCreationParameters is NULL.
@param			aNewLpointer Instantation pointer.
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed instantiation.
*/
TAny* CLoadManager::ImplementationObjectL(TAny* aCreationParameters,
										 TBool aCreationParamsFlag,
										 const TAny* aNewLpointer)
	{
	TAny* object=NULL;

	// So cast to the correct type : This gives an ANSI C++ warning
	// When using a REINTERPRET_CAST so simply cast instead
	// Two different object creation one with creation parameters
	if (aCreationParamsFlag)
		{
		typedef TAny* (*TNewL)(TAny*);
		TNewL creationL = (TNewL)(aNewLpointer);
		object=creationL(aCreationParameters);
		}
	else
		{
		typedef TAny* (*TNewL)();
		TNewL creationL = (TNewL)(aNewLpointer);
		object=creationL();
		}

	return object;
	}

/**
Get the unload policy.
@param			aUniqueImplementationUid The implementation to find.
@param			aEntry Information on the dll containing the implementation
@param			aPolicy Return parameter containing the policy
@return			None
*/
void CLoadManager::GetUnloadPolicy(TUid aUniqueImplementationUid,
							  const TEntry& aEntry,
							  CUnloadPolicy*& aPolicy)
	{
	const TInt numImps = iInstanceInfoList.Count();
	TBool foundImp = EFalse;
	TInt matchingDllIndex=0;
	TBool matchingDllFound=EFalse;
	CUnloadPolicy* policy = NULL;

	for (TInt index = 0; (index<numImps) && !foundImp; ++index)
		{
		//Check if there is existing mapping between the supplied implUid and a policy
		if(iInstanceInfoList[index]->ImplementationUid() == aUniqueImplementationUid)
			{
			policy = iInstanceInfoList[index]->UnloadPolicy();

			foundImp = ETrue;
			}
		else
			{
			//if cannot find a mapping entry but current index has the same DLL as the one requested from
			//the client, store the index to this unloadPolicy for use later on if we have finished
			//searching the entire mapping list
			if (!matchingDllFound && (iInstanceInfoList[index]->UnloadPolicy()->DllEntryInformation().GetName()).CompareF(aEntry.iName)==0)
				{
				matchingDllIndex=index;
				matchingDllFound=ETrue;
				}
			}
		}

	//If we cannot find any mapping in the policy index array(iUnloadPolicyMapping)
	if(!foundImp)
		{
		//if not found but there is a matching DLL,we can simply create a new policy index mapping without
		//having to load the library again.
		if (matchingDllFound)
			{
			policy = iInstanceInfoList[matchingDllIndex]->UnloadPolicy();
			}
		}

	aPolicy = policy;
	}

/**
Clears the policy inside the iGarbagePolicies attribute.
@pre			CLoadManager is fully constructed
@post			CLoadManager iGarbagePolicies is zero'd
*/
void CLoadManager::ClearGarbage()
	{
	if (iGarbagePolicy != NULL)
		{
		delete iGarbagePolicy;
		iGarbagePolicy=0;
		}
	}

CLoadManager::CLoadManager() :
	CBase()
	{
	// Do nothing here
	}

/**
Returns the implementation ID for a given instance Key.
@leave			KErrNotFound
@param			aInstanceKey A key specifying a previously created implementation instance.
@return			TUid The uid of the corresponding implementation.
@pre 			CLoadManager is fully constructed,
@post			CLoadManager remains the same.
*/
TUid CLoadManager::GetImplementationUidL(TUid aInstanceKey)
	{
	// Sanity check that the pointer is divisible by four. A binary number is divisible
	// by four if and only if the two rightmost bits are both zero.
	// This is a compromised check for checking that the pointer is an address.
    if ((aInstanceKey.iUid == 0) || ((aInstanceKey.iUid & 0x00000003) != 0))
        {
	    __ASSERT_DEBUG(EFalse, User::Panic(KEComClientDLLPanicCategory, EEComPanic_InvalidImplementationInstanceKey));
	    User::Leave(KErrNotFound);
        }

	// The instance info pointer is stored in the instance key.
	CInstanceInfo* instanceInfo = reinterpret_cast<CInstanceInfo*>(aInstanceKey.iUid);

	// Check that the pointer exists before using it - leaves with KErrNotFound
	iInstanceInfoList.FindInAddressOrderL(instanceInfo);
	return instanceInfo->ImplementationUid();
	}

/**
Fetches the requested extended interface from a specified implementation instance.
@leave			KErrNotFound
@param 			aInstanceKey A key specifying a previously created implementation.
@param 			aExtendedInterfaceUid Identifies an interface to fetch from the plug-in instance.
@return			TAny* A pointer to the extended interface, will be NULL if it does not exist.
*/
TAny* CLoadManager::GetExtendedInterfaceL(const TUid& aInstanceKey, const TUid& aExtendedInterfaceUid)
	{
	// Sanity check that the pointer is divisible by four. A binary number is divisible
	// by four if and only if the two rightmost bits are both zero.
	// This is a compromised check for checking that the pointer is an address.
    if ((aInstanceKey.iUid == 0) || ((aInstanceKey.iUid & 0x00000003) != 0))
        {
	    __ASSERT_DEBUG(EFalse, User::Panic(KEComClientDLLPanicCategory, EEComPanic_InvalidImplementationInstanceKey));
	    User::Leave(KErrNotFound);
        }

	// The instance info pointer is stored in the instance key.
	CInstanceInfo* instanceInfo = reinterpret_cast<CInstanceInfo*>(aInstanceKey.iUid);

	// Check that the pointer exists before using it - leaves with KErrNotFound
	iInstanceInfoList.FindInAddressOrderL(instanceInfo);
	
	// Create the extension object. The instance info object will be populated with
	// the extension info during creation.
	TAny* object = instanceInfo->CreateExtObjectL(aExtendedInterfaceUid);
	return object;
	}

/**
Manually releases the requested interface. Does nothing if it does not exist.
This interface is optional, normally the interfaces are cleaned up automatically.
@leave			KErrNotFound
@param			aInstanceKey A key specifying a previously created implementation.
@param			aExtendedInterfaceUid Identifies the interface to release
@return None.
*/
void CLoadManager::ManuallyReleaseExtendedInterfaceL(const TUid& aInstanceKey, const TUid& aExtendedInterfaceUid)
	{
	// Sanity check that the pointer is divisible by four. A binary number is divisible
	// by four if and only if the two rightmost bits are both zero.
	// This is a compromised check for checking that the pointer is an address.
    if ((aInstanceKey.iUid == 0) || ((aInstanceKey.iUid & 0x00000003) != 0))
        {
	    __ASSERT_DEBUG(EFalse, User::Panic(KEComClientDLLPanicCategory, EEComPanic_InvalidImplementationInstanceKey));
	    User::Leave(KErrNotFound);
        }
        
	// The instance info pointer is stored in the instance key.
	CInstanceInfo* instanceInfo = reinterpret_cast<CInstanceInfo*>(aInstanceKey.iUid);

	// Check that the pointer exists before using it - leaves with KErrNotFound
	iInstanceInfoList.FindInAddressOrderL(instanceInfo);
	
	instanceInfo->DestroyExtObject(aExtendedInterfaceUid);
	}

/**
Utility method to move the policy to the garbage list, remove
policy from the unload policy list.
@param aPolicy Unload policy to clean up
@return None.
*/
void CLoadManager::Cleanup(CUnloadPolicy* aPolicy)
	{
	if (aPolicy->DecreaseReference() == EDeleteMe)
		{
		// Move the policy to the garbage list
		iGarbagePolicy=aPolicy;

		TInt index = iAllUnloadPolicies.Find(aPolicy);
		if (index != KErrNotFound)
			{
			iAllUnloadPolicies.Remove(index);
			}
		}
	}

//
// CInstanceInfo
/**
Default constructor of CInstanceInfo
@param			aUnloadPolicy The CUnloadPolicy of the dll
*/
CInstanceInfo::CInstanceInfo(CUnloadPolicy* aUnloadPolicy):
	iUnloadPolicy(aUnloadPolicy)
	{
	// Do nothing here
	}

/** 
Destructor of CInstanceInfo
*/
CInstanceInfo::~CInstanceInfo()
	{
	// Do nothing here
	}


//
// CInstanceInfoExtended
/**
Default constructor of CInstanceInfoExtended
@param			aUnloadPolicy The CUnloadPolicy of the dll
@param			aImplementationProxyRow The interface implementation proxy row entry
*/
CInstanceInfoExtended::CInstanceInfoExtended(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy3* aImplementationProxyRow):
	CInstanceInfo(aUnloadPolicy),iImplementationProxyRow(aImplementationProxyRow)
	{
	// Do nothing here
	}

/**
create an instance of CInstanceInfoExtended
@param			aUnloadPolicy The CUnloadPolicy of the dll
@param			aImplementationProxyRow The interface implementation proxy row entry
@return			A pointer to the newly created object.
*/
CInstanceInfoExtended* CInstanceInfoExtended::NewL(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy3* aImplementationProxyRow)
	{
	return new(ELeave) CInstanceInfoExtended(aUnloadPolicy,aImplementationProxyRow);
	}

/**
Sets the implementation object.
@param			aImplementationObject The object instance of this instances' implementation
@return			None
*/
void CInstanceInfoExtended::SetObject(TAny* aImplementationObject)
	{
	iImplementationObject = aImplementationObject;
	}

/**
Creates the extension interface object. This will use the get extended interface
function pointer from the proxy table to fetch the extended interface from the
plug-in implementation.
@param			aExtendedInterfaceUID The extended interface UID
@return			TAny* A pointer to an instance of an extended interface created
*/
TAny* CInstanceInfoExtended::CreateExtObjectL(const TUid& aExtendedInterfaceUID)
	{
	// Fetch the function pointer to create the extended interface
	TProxyExtendedInterfaceGetPtrL createFunctionPtrL = iImplementationProxyRow->iFuncPtrInterfaceGetL;
	if (createFunctionPtrL == NULL)
		{
		// No extension interface object can be created. Return NULL indicating that
		// no extended interface object is available.
		return NULL;
		}

	// Valid function pointer exists in proxy table.
	TAny* object = NULL;         // Extended interface object (this points to the interface
							     // within the object)
	TAny* releaseObject = NULL;  // Eextended interface object (this points to the extended
								 // object itself). Used to delete the extended interface
								 // object later.
	TUint32 flags = 0;           // Flags to allow the plug-in and ECOM to communicate
	// Create the extension object.
	object = createFunctionPtrL(iImplementationObject,aExtendedInterfaceUID,flags,releaseObject);

	if (flags & KReleaseRequiredMask)
		{
		// If release of the extended interface is required then save the release object pointer.
		// The interface object (returned by the function pointer call) and the release object
		// are not necessarily the same pointer. This is because the  interface pointer is not
		// guaranteed to be the same as the pointer to the extended interface object. That
		// is why the release object is required to be fetched from the plug-in.

        // First perform some checks to ensure that the plugin is consistent 
        TProxyExtendedInterfaceReleasePtr release = iImplementationProxyRow->iFuncPtrInterfaceRelease;
                
        if (release == NULL)
            {
            // ...the release object pointer must not be non null
            __ECOM_TRACE("ECOM: PANIC in CInstanceInfoExtended::CreateExtObjectL, release required but release function missing");
	        __ASSERT_DEBUG(EFalse, User::Panic (KEComClientDLLPanicCategory, EEComPanic_CInstanceInfoExtended_CreateExtObjectL_NoReleaseFunc));
            User::Leave(KEComErrNoExtendedInterfaceReleaseFunction);
            }
            
        if (releaseObject == NULL)
            {
            // ... the releaseObject must be non null
            __ECOM_TRACE("ECOM: PANIC in CInstanceInfoExtended::CreateExtObjectL, release required but release object missing");
	        __ASSERT_DEBUG(EFalse, User::Panic (KEComClientDLLPanicCategory, EEComPanic_CInstanceInfoExtended_CreateExtObjectL_NoReleaseObj));
           User::Leave(KEComErrNoExtendedInterfaceReleaseObject);
 		    }
            
		//Create the extended object info type and add it to the extended object info array.
		TExtendedObjectInfo extendedObjectInfo;
		extendedObjectInfo.iExtendedInterfaceObject = releaseObject;
		extendedObjectInfo.iExtendedInterfaceUID = aExtendedInterfaceUID;

		TInt err = iExtendedObjectInfo.Append(extendedObjectInfo);

		if (err != KErrNone)
			{
			if (release != NULL)
				{
				// Release the extended interface. Release must not leave.
				release(extendedObjectInfo.iExtendedInterfaceObject,extendedObjectInfo.iExtendedInterfaceUID);
				}
			User::Leave(err);
			}
		}

	return object;
	}


/**
Destroys the extension interface object.
@param			aExtendedInterfaceUID The extended interface UID
@return			None
*/
void CInstanceInfoExtended::DestroyExtObject(const TUid& aExtendedInterfaceUID)
	{
	// Get release interface
	TProxyExtendedInterfaceReleasePtr release = iImplementationProxyRow->iFuncPtrInterfaceRelease;

	if (release != NULL)
		{
		// Release extended interface. Find the extended object info.
		for(TInt i = 0; i < iExtendedObjectInfo.Count(); i++)
			{
			if (iExtendedObjectInfo[i].iExtendedInterfaceUID == aExtendedInterfaceUID)
				{
				TAny* releaseObject = iExtendedObjectInfo[i].iExtendedInterfaceObject;
                if (releaseObject == NULL)
                    {
                    // ... the releaseObject must be non null
                    __ECOM_TRACE("ECOM: PANIC in CInstanceInfoExtended::DestroyExtObject, release required but release object missing");
	                __ASSERT_DEBUG(EFalse, User::Panic (KEComClientDLLPanicCategory, EEComPanic_CInstanceInfoExtended_DestroyExtObject_NoReleaseObj));	              
		            }

				// Release the extended interface. Release should not be leaving.
				release(releaseObject, iExtendedObjectInfo[i].iExtendedInterfaceUID);
				// Remove the extended object info element from the array.
				iExtendedObjectInfo.Remove(i);
				break;
				}
			}
		}
	}

/**
Destructor of CInstanceInfoExtended
*/
CInstanceInfoExtended::~CInstanceInfoExtended()
	{
	// Get release interface
	if (iImplementationProxyRow != NULL)
		{
		TProxyExtendedInterfaceReleasePtr release = iImplementationProxyRow->iFuncPtrInterfaceRelease;

		if (release != NULL)
			{
			// Release all extended interfaces (if any still to be released)
			for(TInt i = 0; i < iExtendedObjectInfo.Count(); i++)
				{
				// Release is supposed to be non-leavable.
				release(iExtendedObjectInfo[i].iExtendedInterfaceObject,iExtendedObjectInfo[i].iExtendedInterfaceUID);
				}
			}
		}

	iExtendedObjectInfo.Close();
	}

//
// CInstanceInfoSimple
/**
Default constructor of CInstanceInfoSimple
@param			aUnloadPolicy The CUnloadPolicy of the dll
@param			aImplementationProxyRow The interface implementation proxy row entry
*/
CInstanceInfoSimple::CInstanceInfoSimple(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy* aImplementationProxyRow):
	CInstanceInfo(aUnloadPolicy),iImplementationProxyRow(aImplementationProxyRow)
	{
	// Do nothing here
	}

/**
Create an instance of CInstanceInfoSimple
@param			aUnloadPolicy The CUnloadPolicy of the dll
@param			aImplementationProxyRow The interface implementation proxy row entry
@return			A pointer to the newly created object.
*/
CInstanceInfoSimple* CInstanceInfoSimple::NewL(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy* aImplementationProxyRow)
	{
	return new(ELeave) CInstanceInfoSimple(aUnloadPolicy,aImplementationProxyRow);
	}
