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
// The Definition of the CEComServer singleton class which
// instantiates an instance of the requested ECom Interface Implementation.
// 
//

/**
 @internalComponent
 @file
*/

#ifndef __ECOMSERVER_H__
#define __ECOMSERVER_H__

#include <f32file.h>
#include "clientrequest.h"
#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include <ecom/implementationproxy.h>
#include "RegistrarObserver.h"
#include "EComPerformance.h"
#include <babitflags.h>
#include "callback.h"

// Forward declarations
class CRegistryData;
class CRegistrar;
class CResolver;
class CDefaultResolver;
class CRomOnlyResolver;
class TEComResolverParams;
class CServerStartupMgr;
class CRegistryResolveTransaction;
class CCustomResolverCache;

//#define _TRANSIENT_SERVER

#ifdef _TRANSIENT_SERVER
/**
Specifies that the server should shut down after a specified time
If not defined then the server never shuts down
@internalComponent
*/
const TInt KEComShutdownDelay = 0x500000;	// approx 0.5 seconds
#endif


/**
	@internalComponent
	@since 7.0
	A shutdown control object that activates the 
	destruction of the ECom server when its RunL
	fires, by stopping the local scheduler.
 */

class CShutdown : public CTimer
	{
public:
/**
	@internalComponent
	Intended Usage	: Default c'tor	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CShutdown is fully constructed.
 */
	
	inline CShutdown();

/**
	Intended Usage	: Standardised 2nd, (Initialisation) phase of two 
					phase construction.
	Error Condition	: None	
	@leave			KErrNoMemory
	@since			7.0
	@pre 			CShutdown is fully constructed.
	@post			CShutdown is fully initialised.
 */
	
	inline void ConstructL();

/**
	Intended Usage	: Activate the shutdown timeout.
	Error Condition	: None	
	@since			7.0
	@pre 			CShutdown is fully constructed.
	@post			CShutdown is active pending timeout.
 */
	
	inline void Start();

private:
/**
	Intended Usage	: The timeout has expired, so begin server destruction.
	Error Condition	: None	
	@since			7.0
	@pre 			CShutdown is fully constructed.
	@post			the local CActiveScheduler::Stop() has been called.
 */
	
	void RunL();
	};


/**
	@internalComponent
	@since 7.0
	A concrete implementation which supplies instantiation & 
	destruction of a class via the ECom framework.
 */

class CEComServer : private CServer2, private MRegistrarObserver
	{
	public:
/**
	Intended Usage	: Standardised safe construction which 
					leaves the returned object on the cleanup stack.
	Error Condition	: Cannot fully construct because of 
					memory limitations.	
	@leave  		KErrNoMemory
	@leave  		@see RFs, CRegistryData, CRegistrar, CDefaultResolver
	@leave  		@see CShutdownTimer & CServer::StartL
	@since			7.0
	@return			CEComServer* a pointer to the new class
	@pre 			None
	@post			CEComServer is fully constructed, initialised,
					and remains upon the cleanup stack.
*/
	
	static CEComServer* NewLC();

/**
	Intended Usage	: d'tor
	Error Condition	: None	
	@since			7.0
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer is destroyed.
 */
	~CEComServer();

/**
	Intended Usage	: Reads the EComSrvr.ini file to determine if
					  ECOM should be SSA or not.
	@param			aFs A reference to a connected file server session.
	@pre 			CEComServer must have been fully constructed.
	*/
	TBool IsSSA(RFs& aFs);

/**
	Intended Usage	: Retrieve a list of all the implementations which 
					satisfy the specified interface.
	Error Condition	: Lack of memory.	
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aAdditionalParameters A descriptor specifying any additional
					implementation characteristics to be fulfilled.
	@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
					Must match all extended interfaces for a match to occur.
	@param			aMessage Client's request					
	@return			RImplInfoArray* A pointer to the array of CImplementationInformation pointers. 
		            Ownership of the created array is returned to the caller,
					who must take the responsibility of destroying it. However, the caller must
					not destroy the list's contents as it is not owned by the list.
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has not changed, and RImplInfoArray* contains the list
					of CImplementationInformations regigistered for the interface.		
 */
	
	RImplInfoArray* ListImplementationsL(TUid aInterfaceUid, 
										 const TEComResolverParams& aAdditionalParameters,
										 const RExtendedInterfacesArray& aExtendedInterfaces,
										 const TClientRequest& aMessage);

/**
	Intended Usage	: Retrieve a list of all the implementations which 
					satisfy the specified interface.
	Error Condition	: Lack of memory.	
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
					Must match all extended interfaces for a match to occur.
	@param			aMessage Client's request					
	@return			RImplInfoArray* A pointer to the array of CImplementationInformation pointers. 
	                Ownership of the created array is returned to the caller,
					who must take the responsibility of destroying it. However, the caller must
					not destroy the list's contents as it is not owned by the list.
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has not changed, and RImplInfoArray* contains the list
					of CImplementationInformations regigistered for the interface.			
 */
	
	RImplInfoArray* ListImplementationsL(TUid aInterfaceUid, 
										 const RExtendedInterfacesArray& aExtendedInterfaces, 
										 const TClientRequest& aMessage);

/**
	Intended Usage	: Retrieve a list of all the implementations which 
					satisfy the specified interface.
	Error Condition	: Lack of memory.	
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aAdditionalParameters A descriptor specifying any additional
					implementation characteristics to be fulfilled.
	@param			aResolverUid The CResolver implementation to use.
	@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
					Must match all extended interfaces for a match to occur.
	@param			aMessage Client's request										
	@return			RImplInfoArray* A pointer to the array of CImplementationInformation pointers.
					Ownership of the created array is returned to the caller,
					who must take the responsibility of destroying it. However, the caller must
					not destroy the list's contents as it is not owned by the list.
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has not changed, and RImplInfoArray* contains the list
					of CImplementationInformations regigistered for the interface.
 */
	
	RImplInfoArray* ListImplementationsL(TUid aInterfaceUid, 
										 const TEComResolverParams& aAdditionalParameters, 
										 TUid aResolverUid,
										 const RExtendedInterfacesArray& aExtendedInterfaces,
										 const TClientRequest& aMessage);
/**
	Intended Usage	: Retrieve a list of all the implementations which 
					satisfy the specified interface.
	Error Condition	: Lack of memory.	
	@leave			KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aResolverUid The CResolver implementation to use.
	@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
					Must match all extended interfaces for a match to occur.
	@param			aMessage Client's request					
	@return			RPointerArray<TUid>* A pointer to the array of class identification
					data.
					Ownership of the created array is returned to the caller,
					who must take the responsibility of destroying it. However, the caller must
					not destroy the list's contents as it is not owned by the list.	
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has not changed, and RPointerArray<TUid>* contains the list
					of Implementations for the interface.		
 */
	
	RImplInfoArray* ListImplementationsL(TUid aInterfaceUid, 
										 TUid aResolverUid,
										 const RExtendedInterfacesArray& aExtendedInterfaces,
										 const TClientRequest& aMessage);

/**
	Intended Usage	:Instantiate an interface implementation to satisfy the 
					specified interface. 
	Error Condition	: Lack of memory, or not connected.	
	@leave			KErrNoMemory
	@leave			KErrNotConnected
	@since			7.0
	@param			aImplementationUid A UID specifying the required interface implementation.
	@param			aDllInfo containing the Dll info to be loaded on the client side
	@param			aInstanceKey A 32 bit identifer returned by ECom to
					identify this instance to the framework.
	@param			aClientRequest client's request
	@pre 			CEComServer must have been fully constructed.
	@post			The requested interface implementation is identified,
					and the instantiation method pointer is returned.
 */
	
	void GetResolvedDllInfoL(	const TUid aImplementationUid,
								TEntry& aDllInfo,
								TUid& aInstanceKey,
								const TClientRequest& aClientRequest);


/**
	Intended Usage	:Instantiate an interface implementation to satisfy the 
					specified interface. 
	Error Condition	: Lack of memory	
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aAdditionalParameters A descriptor specifying any additional
					implementation characteristics to be fulfilled.
	@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
					Must match all extended interfaces for a match to occur.					
	@param			aDllInfo containing the Dll info to be loaded on the client side
	@param			aInstanceKey A 32 bit identifer returned by ECom to
					identify this instance to the framework.
	@param			aClientRequest client's request
	@pre 			CEComServer must have been fully constructed.
	@post			The requested interface implementation is identified,
					and the instantiation method pointer is returned.
 */
	
	void GetResolvedDllInfoL(	const TUid aInterfaceUid,
								const TEComResolverParams& aAdditionalParameters,
								const RExtendedInterfacesArray& aExtendedInterfaces,
								TEntry& aDllInfo,
								TUid& aInstanceKey,
								const TClientRequest& aClientRequest);

/**
	Intended Usage	:Instantiate an interface implementation to satisfy the 
					specified interface. 
					Overload with a client provided CResolver.
	Error Condition	: Lack of memory	
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aAdditionalParameters A descriptor specifying any additional
					implementation characteristics to be fulfilled.
	@param			aResolver A UID specifying the implementation of the CResolver interface
					with client defined behaviour to use for resolution.
	@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
					Must match all extended interfaces for a match to occur.					
	@param			aDllInfo containing the Dll info to be loaded on the client side
	@param			aInstanceKey A 32 bit identifer returned by ECom to
					identify this instance to the framework.
	@param			aClientRequest client's request
	@pre 			CEComServer must have been fully constructed.
	@post			The requested interface implementation is identified,
					and the instantiation method pointer is returned.
 */
	
	void GetResolvedDllInfoL(   const TUid aInterfaceUid, 
								const TEComResolverParams& aAdditionalParameters,
								const TUid aResolverUid,
								const RExtendedInterfacesArray& aExtendedInterfaces,
								TEntry& aDllInfo,
								TUid& aInstanceKey,
								const TClientRequest& aClientRequest);
	
/**
	Get implementation info for one implementation, and return to client
	@param			aImplementationUid A UID specifying the required implementation.
	@param			aImplInfo Return value, implementation info for client. If the call succeeds,
					it will point to the found implementation information in registrydata. The function 
					does not return ownership of the pointer, so the caller must not destroy it.
	@param			aClientRequest client's request
 */							
	void GetImplementationInformationL(const TUid& aImplementationUid,
									   CImplementationInformation*& aImplInfo,
									   const TClientRequest& aClientRequest);

#ifdef __ECOM_SERVER_TESTABILITY__
/**
	This method is provided for testability. It allows the user to 
	simulate the domain manager behaviour. If the testing compile
	time flag __ECOM_SERVER_TESTABILITY__ is not defined the method
	is not accessible. If the flag is defined then it is responsible
	for setting the startup state. After the state is changed
	the ProcessCurrentStartupStateL() method should be called for 
	correct simulation of domain manager behaviour.
	@param aState the state to change to
*/
	void ChangeStartupStateL(TInt aState) const;
		
/**
	This method is provided for testability. It allows the user to 
	simulate the domain manager behaviour. If the testing compile
	time flag __ECOM_SERVER_TESTABILITY__ is not defined the method
	is not accessible. If the flag is defined then it is responsible
	for processing the startup state. ChangeStartupState() method 
	should be called prior to calling this method.
	@leave System-wide error codes, including KErrNoMemory.
*/
	void ProcessCurrentStartupStateL() const;
	
/**
	This method is provided for testability. It allows the user to 
	simulate the domain manager behaviour. If the testing compile
	time flag __ECOM_SERVER_TESTABILITY__ is not defined the method
	is not accessible. It is an accessor
	to test the current state of the ServerStartupManager.
	@return The current startup state of the CServerStartupManager.
*/
	TInt GetCurrentStartupState() const;
#endif //__ECOM_SERVER_TESTABILITY__

#ifdef __ECOM_SERVER_PERFORMANCE__
/**
	This method is provided for performance testability. If the testing 
	compile	time flag __ECOM_SERVER_PERFORMANCE__ is not defined the method
	is not accessible. It is an accessor to get the plugins counts discovered
	during the startup of EComServer.
	@param aType the type of counts to read
	@param aCounts the count readings
*/
	void GetRegistryCountsL(TInt aType, RegistryCounts::TRegistryCounts& aCounts) const;
	
#endif //__ECOM_SERVER_PERFORMANCE__

/**
	Intended Usage	: (Server Session management) Add a new session connection for a client.	
	Error Condition	: None.
	@since			7.0
	@pre 			CEComServer must have been fully constructed.
	@post			The new CEComServerSession to handle service provision for a new 
					ECom client has been recorded.
					If the server was preparing to shutdown, that shutdown is cancelled
 */
	
	void AddSession();

/**
	Intended Usage	: (Server Session management) Destroy a session connection with a client.	
	Error Condition	: None.
	@since			7.0
	@pre 			CEComServer must have been fully constructed.
	@post			The appropriate CEComServerSession has been destroyed and 
					the ECom client has been disconnected from this server.
 */
	
	void DropSession();

/**
	Intended Usage	: The overloaded method from the MRegistrarObserver class
					  Used for firing notifications to the client sessions that
					  the underlying registry data has changed.
	Error Condition	: None	
	@since			7.0
	@pre 			CEComServer must have been fully constructed.
	@post			The CEComServer has notified all its client sessions
					registered for notifications that the data changed. 
 */
	
	void Notification(TInt aNotificationCode);

/**
	Intended Usage	: Indicates whether the registry index is currently valid.  The 
					index will not be valid if discoveries are currently taking place.
					If the index is not currently valid then calls to 
					ListImplementationsL() cannot be serviced.
	Error Condition	: None.
	@return			ETrue if the index is currently valid, EFalse otherwise.
	@since			7.0
	@pre 			CEComServer is fully constructed
 */
	
	TBool RegistryIndexValid() const;

	// Callback function to be invoked by CDiscoverer and CRegistryData
	static TInt NotifyEvents(TAny* aObj, TInt aEvent, TAny* aData);

private:
/**
	@internalComponent
	Intended Usage	: Default c'tor	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CEComServer is fully constructed
 */
	
	CEComServer();

/**
	Intended Usage	: Standardised 2nd, (Initialisation) phase of two 
					phase construction.
	Error Condition	: None	
	@leave  		KErrNoMemory
	@since			7.0
	@return			void 
	@pre 			CEComServer is fully constructed.
	@post			CEComServer is fully initialised.
 */
	
	void ConstructL();

/**
	Intended Usage	: Helper method to retrieve a list of all the 
					implementations which satisfy the specified interface. 
					Overload with a provided CResolver*.
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aAdditionalParameters A descriptor specifying any additional
					implementation characteristics to be fulfilled.
	@param			aResolver A CResolver with client defined behaviour.
	@return			RPointerArray<TUid>* A pointer to the array of class identification
					data. Ownership of the created array is returned to the caller,
					who must take the responsibility of destroying it. However, the caller must
					not destroy the list's contents as it is not owned by the list.
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has not changed, and RPointerArray<TUid>* contains the list
					of Implementations for the interface.			
 */
	
	RImplInfoArray* ListImplementationsL(TUid aInterfaceUid, 
										 const TEComResolverParams& aAdditionalParameters, 
										 CResolver* aResolver) const;
/**
	Intended Usage	: Helper method to retrieve a list of all the 
					implementations which satisfy the specified interface. 
					Overload with a provided CResolver*.
	@leave  		KErrNoMemory
	@since			7.0
	@param			aInterfaceUid A UID specifying the required interface.
	@param			aResolver A CResolver with client defined behaviour.
	@return			A pointer to the array of class identification data. 
                 	Ownership of the created array is returned to the caller,
					who must take the responsibility of destroying it. However, the caller must
					not destroy the list's contents as it is not owned by the list.
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has not changed, and RPointerArray<TUid>* contains the list
					of Implementations for the interface.		
 */
	
	RImplInfoArray* ListImplementationsL(TUid aInterfaceUid, 
										 CResolver* aResolver) const;

/**
	Intended Usage	: Creates a new CEComServerSession to handle client
					service requests.
	@leave			KErrNoMemory
	@leave			@see CSharableSession
	@pre 			CEComServer must have been fully constructed.
	@post			CEComServer has created a new CEComServerSession.
 */
	
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

/**
	Intended Usage	: Intercept the panic caused by a RunL leave,
					to restore the CEComServer object to a sensible state.
					(called by the Active Scheduler immediately before the Panic).
	Error Condition	: None.
	@since			7.0
	@return			TInt KErrNone if cleanup successful, otherwise return aError
	@pre 			CEComServer is fully constructed.
	@post			CEComServer has been restored to a sensible state.
 */
	
	TInt RunError(TInt aError);

/**
	Intended Usage: Creates custom resolver object with aResolverUid UID.
	The method leaves custom resolver onto the cleanup stack.
	@param aResolverUid Custom resolver UID.
	@param			aRegistryResolveTransaction A pointer to Registry resolve transaction object
	@return A pointer to the created CResolver object.
	@leave System-wide error codes, including KErrNoMemory.
*/
	CResolver* CreateCustomResolverLC(TUid aResolverUid,CRegistryResolveTransaction* aRegistryResolveTransaction);

/**
	Intended Usage: Creates resolver object with aResolverUid UID.
	The method leaves resolver onto the cleanup stack.
	@param aResolverUid Resolver UID.
	@param			aRegistryResolveTransaction A pointer to Registry resolve transaction object
	@return A pointer to the created CResolver object.
	@leave System-wide error codes, including KErrNoMemory.
*/
	CResolver* CreateResolverLC(const TUid& aResolverUid,CRegistryResolveTransaction* aRegistryResolveTransaction);

	void NotifyUpgrade(const TUid aImplementationUid);
	void NotifySWIEvent(TAny* aData);
	void NotifyBUREvent(TAny* aData);
	void UpdateSpecialEvents(TUint32 aBit, TCallBackState aState);
	TBool IsCachable(TBool aEntryIsOnRWDrive);

// Attributes
private:
	/** Singleton connection to the file server */
	RFs iFs;
	/** Singleton registry information handler */
	
	CRegistryData*	iRegistryData;
	
	/** Registry resolve transaction object */	
	CRegistryResolveTransaction* iRegistryResolveTransaction;
	
	/** Singleton Registrar to handle the interface registration */
	
	CRegistrar* iRegistrar;

	/** The count of the number of sessions open upon this server */
	
	TInt iSessionCount;
	/** The timer controlling the server shutdown process */
	
	CShutdown iShutdown;
	
	/** CServerStartupMgr object which interacts with Domain Manager*/
	CServerStartupMgr* iServerStartupMgr;
	
	/** Library which is used to load a non-default resolver when required */
	RLibrary iResolverLibrary;
	
	/** cache of recently used custom resolvers. */
	CCustomResolverCache* iResolverCache;

	/** Keep track of special system events such as BUR / SWI in progress */
	TBitFlags32 iSpecialEvents;

	// And the Test bed state accessor
	friend class TEComServer_StateAccessor;
	};

// Inline implementations for CShutdown
// Set the priority for shutdown as marginally less than normal
// operation.
const TInt KEComShutdownPriority = CActive::EPriorityStandard - 1;
inline CShutdown::CShutdown()
:CTimer(KEComShutdownPriority)
	{ 
	CActiveScheduler::Add(this); 
	}

inline void CShutdown::ConstructL()
	{ 
	CTimer::ConstructL();
	}

inline void CShutdown::Start()
	{
// Kick off the shutdown timer if this is a debug run
// Otherwise NEVER close.
#ifdef _TRANSIENT_SERVER
	After(KEComShutdownDelay);
#endif
	}

#endif // __ECOMSERVER_H__

