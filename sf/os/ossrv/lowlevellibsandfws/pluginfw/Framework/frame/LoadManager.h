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
//


#ifndef __LOADMANAGER_H__
#define __LOADMANAGER_H__

#include <e32base.h>
#include <ecom/extendedinterfaceimplementationproxy.h>
//Forward declarations
class CUnloadPolicy;

/**
@internalComponent
Represents an instance of an implementation
*/
NONSHARABLE_CLASS(CInstanceInfo): public CBase
	{
public:
	virtual ~CInstanceInfo();
	CUnloadPolicy* UnloadPolicy() {return iUnloadPolicy;};
public:
	virtual TUid ImplementationUid() = 0;
    virtual TProxyExtendedInterfaceGetPtrL ProxyExtendedIntGetPtr() = 0;
    virtual TProxyExtendedInterfaceReleasePtr ProxyExtendedIntReleasePtr() = 0;
    virtual TAny* CreateExtObjectL(const TUid& aExtendedInterfaceUID) = 0;
    virtual void DestroyExtObject(const TUid& aExtendedInterfaceUID) = 0;
    virtual TAny* ImplementationObject() = 0;
protected:
    CInstanceInfo(CUnloadPolicy* aUnloadPolicy);
private:
   	// The Unload Policy for the dll which contains this implementation
	CUnloadPolicy*	iUnloadPolicy;
	};

/**
@internalComponent
Represents an instance of a PLUGIN3 type implementation
*/
NONSHARABLE_CLASS(CInstanceInfoExtended): public CInstanceInfo
{
public:
	static CInstanceInfoExtended* NewL(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy3* aRow);
	void SetObject(TAny* aImplementationObject);
	~CInstanceInfoExtended(); // Release all extended objects (if not already released)
public:
    TUid ImplementationUid() {return iImplementationProxyRow->iImplementationUid;};
    TProxyExtendedInterfaceGetPtrL ProxyExtendedIntGetPtr() {return iImplementationProxyRow->iFuncPtrInterfaceGetL;};
    TProxyExtendedInterfaceReleasePtr ProxyExtendedIntReleasePtr() {return iImplementationProxyRow->iFuncPtrInterfaceRelease;};
    TAny* CreateExtObjectL(const TUid& aExtendedInterfaceUID);
    void DestroyExtObject(const TUid& aExtendedInterfaceUID);
    TAny* ImplementationObject() {return iImplementationObject;};
private:
	CInstanceInfoExtended(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy3* aImplementationProxyRow);
private:
   // Pointer to proxy implementation table row entry that this instance corresponds to.
   // This is where the implementation UID, get and release extended interface function
   // pointers reside.
   const TImplementationProxy3* iImplementationProxyRow;
   // The instantiation interface object for the dll which contains this implementation
   TAny*	iImplementationObject;
   // The created extended object information structure.
   struct TExtendedObjectInfo
	   {
	   // The extended interface UID
       TUid    iExtendedInterfaceUID;
	   // The extended interface object for the this implementation
	   TAny*   iExtendedInterfaceObject;
	   };

    // List of all of the created extended objects for this implementation
	RArray<TExtendedObjectInfo> iExtendedObjectInfo;

	// Friend classes
	friend class TLoadManager_StateAccessor;
};

/**
@internalComponent
Represents an instance of a PLUGIN1 type implementation
*/
NONSHARABLE_CLASS(CInstanceInfoSimple): public CInstanceInfo
{
public:
	static CInstanceInfoSimple* NewL(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy* aImplementationProxyRow);
public:
    TUid ImplementationUid() {return iImplementationProxyRow->iImplementationUid;};
	// The following functions are not required for CInstanceInfoSimple.
	// PLUGIN1 type is not meant to extend interfaces.  	
  	TProxyExtendedInterfaceGetPtrL ProxyExtendedIntGetPtr(){return NULL;};
 	TProxyExtendedInterfaceReleasePtr ProxyExtendedIntReleasePtr(){return NULL;};
 	TAny* CreateExtObjectL(const TUid& /*aExtendedInterfaceUID*/) {return NULL;};
 	void DestroyExtObject(const TUid& /* aExtendedInterfaceUID*/) {};
	TAny* ImplementationObject() {return NULL;}; 

private:
	CInstanceInfoSimple(CUnloadPolicy* aUnloadPolicy,const TImplementationProxy* aImplementationProxyRow);

private:
   // Pointer to proxy implementation table row entry that this instance corresponds to.
   // This is where the implementation UID resides.
   const TImplementationProxy* iImplementationProxyRow;

};

/**
@internalComponent
Manages the loading and unloading of interface implementation groupings.
*/
NONSHARABLE_CLASS(CLoadManager) : public CBase
{
public:
	static CLoadManager* NewL();
	~CLoadManager();
	TBool DestroyedThis(TUid aInstanceKey);

	TBool PolicyArraysEmpty() const ;
	
	TAny* ImplementationObjectL(const TUid& aUniqueImplementationUid,
							   const TEntry& aEntry,
							   TAny* aCreationParameters,
							   TBool aCreationParamsFlag,
							   TUid& aInstanceKey);

	TAny* GetExtendedInterfaceL(const TUid& aInstanceKey, const TUid& aExtendedInterfaceUid);
	void ManuallyReleaseExtendedInterfaceL(const TUid& aInstanceKey, const TUid& aExtendedInterfaceUid);
	void ClearGarbage();

	TUid GetImplementationUidL(TUid aInstanceKey);

private:
	CLoadManager();

	void GetUnloadPolicy(TUid aUniqueImplementationUid,
						 const TEntry& aEntry,
						 CUnloadPolicy*& aPolicy);

	template<typename TImpProxy,typename T> TAny* GetNewLPointerAndProxyTableRowL(TUid aUniqueImplementationUid,
										 TImpProxy*& aImplementationTableEntry,
										 const T aProxy);

	TAny* ImplementationObject1L(const TUid& aUniqueImplementationUid,
						   	     TAny* aCreationParameters,
								 TBool aCreationParamsFlag,
								 TUid& aInstanceKey,
								 CUnloadPolicy* aPolicy,
								 TLibraryFunction& aLibFunctionProxy);

	TAny* ImplementationObject3L(const TUid& aUniqueImplementationUid,
						   	     TAny* aCreationParameters,
								 TBool aCreationParamsFlag,
								 TUid& aInstanceKey,
								 CUnloadPolicy* aPolicy,
								 TLibraryFunction& aLibFunctionProxy);

	TAny* ImplementationObjectL(TAny* aCreationParameters,
								TBool aCreationParamsFlag,
								const TAny* aNewLpointer);

	void CleanupAfterFailure(CUnloadPolicy* aPolicy);
	void Cleanup(CUnloadPolicy* aPolicy);
private:
#ifdef ECOM_TRACE
	/** Instantiation counter for debug trace */
	TInt iDebugInstantiationCounter;
#endif
	/** List of all the UnloadPolicies for objects which have been created */
	RPointerArray<CUnloadPolicy> iAllUnloadPolicies;
	/** Garbage UnloadPolicy which is not in use anymore */
	CUnloadPolicy* iGarbagePolicy;
	// Array of instance information
	RPointerArray<CInstanceInfo> iInstanceInfoList;

	// Make the test State Accessor a friend
	friend class TLoadManager_StateAccessor;
};

#include "loadmanager.inl"

#endif //__LOADMANAGER_H__
