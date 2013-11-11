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
// The Implementation of the CEComServer singleton class which
// instantiates an instance of the requested ECom Interface Implementation.
// 
//

/**
 @internalComponent
 @file
*/
#include <e32base.h>
#include <bautils.h>
#include <startupdomaindefs.h>
#include "EComDebug.h"
#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>

#include "ServerStartupManager.h"
#include "RegistryData.h"
#include "Registrar.h"
#include "DefaultResolver.h"
#include "RomOnlyResolver.h"
#include "TlsData.h"
#include "EComServerStart.h"
#include "EComMessageIds.h"
#include "EComServerSession.h"
#include "EComServer.h"
#include "EComUidCodes.h"
#include "EComPerformance.h"
#include "DriveInfo.h"
#include "FileUtils.h"
#include "RegistryResolveTransaction.h"
#include "resolvercache.h"
#include "EComPatchDataConstantv2.h"

#define UNUSED_VAR(a) a = a

/** enum for special system events */
enum TSpecialEvents
	{
	EBURInProgress,
	ESWIInProgress
	};

/** The location of server INI file. */
_LIT(KEComSrvrIniFile,"_:\\private\\10009D8F\\EComSrvr.ini");
/** Buffer descriptor to hold full path and name of server INI file. */
typedef TBuf<32> TEComSrvrIniFileName;

_LIT(KEComSrvrIniFileROM,"Z:\\private\\10009D8F\\EComSrvr.ini");

static void CloseAndDeleteImplInfoArray(TAny* aObject)
	{
	RImplInfoArray* array=reinterpret_cast<RImplInfoArray*>(aObject);
	if (array)
		{
		array->Close();
		}
	delete array;
	}
//
// Initiate server exit when the timer expires
// by stopping the local scheduler.
void CShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

CEComServer* CEComServer::NewLC()
	{
	// Standard 2 phase construction code
	CEComServer* instance = new(ELeave) CEComServer;
	CleanupStack::PushL(instance);
	instance->ConstructL();
	return instance;
	}


CEComServer::~CEComServer()
	{
	// Uninstall callbacks first. Do not want to receive any callback
	// in destructor. TCallBackWithArg constructed with no argument
	// is a null callback.
	TCallBackWithArg nullCallback;
	if (iRegistrar)
		{
		iRegistrar->InstallSwiEventCallBack(nullCallback);
		iRegistrar->InstallBurEventCallBack(nullCallback);
		}
	if (iRegistryData)
		{
		iRegistryData->SetImplUpgradeCallBack(nullCallback);
		}

	// Ensure this deletion order is maintained - in particular the registrydata 
	// must last longer than the others which hold a reference to it.

	//remove the CServerStartupMgr
	delete iServerStartupMgr;
	// Then destroy the registrar object
	delete iRegistrar;
	// remove the registry data
	delete iRegistryData;
	
	delete iResolverCache;

	// Finally close down the file server connection
	iFs.Close();
	// Make sure the non-default resolver library is closed
	iResolverLibrary.Close();
	}


CEComServer::CEComServer()
: CServer2(CActive::EPriorityStandard)
	{
	// Do nothing
	}


void CEComServer::ConstructL()
	{
	START_TIMER
	START_HEAP
	StartL(KEComServerName);
	// Connect to the file server
	User::LeaveIfError(iFs.Connect());
	__ECOM_TRACE("ECOM: Server INIT - File Server session initialised");
	// construct the registry data handling object here
	iRegistryData = CRegistryData::NewL(iFs);
	__ECOM_TRACE("ECOM: Server INIT - Registry Data initialised");
	// Then the registrar
	iRegistrar = CRegistrar::NewL(*iRegistryData, *this, iFs);
	__ECOM_TRACE("ECOM: Server INIT - Registrar initialised");
	
	//Then the CServerStartupMgr

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iServerStartupMgr = new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KSM2OSServicesDomain3, iFs);
#else
	iServerStartupMgr = new(ELeave) CServerStartupMgr(KDmHierarchyIdStartup, KBaseServicesDomain3, iFs);
#endif //SYMBIAN_SYSTEM_STATE_MANAGEMENT

	__ECOM_TRACE("ECOM: Server INIT - ServerStartupMgr initialised");
	iServerStartupMgr->RegisterObserverL(iRegistrar);
	
	iServerStartupMgr->InitialiseL(IsSSA(iFs));
	
	iResolverCache = CCustomResolverCache::NewL(KCustomResolverCacheSize,
		KCustomResolverCacheTimeout);

	TCallBackWithArg eventCallback(&CEComServer::NotifyEvents, this);
	iRegistrar->InstallSwiEventCallBack(eventCallback);
	iRegistrar->InstallBurEventCallBack(eventCallback);
	iRegistryData->SetImplUpgradeCallBack(eventCallback);

	// The server is about to start so construct the transient shutdown guy
	iShutdown.ConstructL();
	// ensure that the server still exits even
	// if the server start fails or the
	// 1st client fails to connect
	iShutdown.Start();
	RECORD_INITIALISE_HEAP
	RECORD_INITIALISE_RESULT
	}
	
TBool CEComServer::IsSSA(RFs& aFs)
	{
	// Check Z drive first.
	TBool isFileExisting = BaflUtils::FileExists(aFs, KEComSrvrIniFileROM);
	// Check system drive next
	if(!isFileExisting)
		{
		TEComSrvrIniFileName iniFile(KEComSrvrIniFile);
		TEComFileUtils::SetToDrive(iniFile,iRegistryData->iSystemDrive);
		isFileExisting = BaflUtils::FileExists(aFs, iniFile);
		}
	return !isFileExisting;		
	}

// Get implementation info for one implementation, and return to client
void CEComServer::GetImplementationInformationL(const TUid& aImplementationUid,
												CImplementationInformation*& aImplInfo,
												const TClientRequest& aClientRequest)
	{
	TEntry dllInfo;
	User::LeaveIfError(iRegistryData->GetImplementationDllInfoForClientL(
		aClientRequest, aImplementationUid, KNullUid, dllInfo, aImplInfo, ETrue));
	}

RImplInfoArray* CEComServer::ListImplementationsL(TUid aInterfaceUid, 
												   const TEComResolverParams& aAdditionalParameters, 
												   TUid aResolverUid,
												   const RExtendedInterfacesArray& aExtendedInterfaces,
												   const TClientRequest& aMessage
												   ) 
	{
	RImplInfoArray* result = NULL;
	CResolver* resolver = NULL;
	//create registry resolver transaction
	//get the TListImplParam parameters
	TBool capability= ETrue;
	if(!(aMessage.IsNull()))
		{
		TListImplParam listParam;
  		TPckg<TListImplParam> listParamPkg(listParam);
  		aMessage.ReadL(2,listParamPkg);
  		capability=listParam.iCapabilityCheck;
		}
  	
  	
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,
																								aExtendedInterfaces,
																								aMessage,capability);
	CleanupStack::PushL(registryResolveTransaction);
	//create resolver
	resolver = CreateResolverLC(aResolverUid,registryResolveTransaction);
	result = ListImplementationsL(aInterfaceUid, aAdditionalParameters, resolver);
	//clean up
	CleanupStack::PopAndDestroy(resolver);
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	if ((aResolverUid != KDefaultResolverUid) && (aResolverUid != KRomOnlyResolverUid))
		{
		iResolverLibrary.Close();
		}
	return result;
	}

RImplInfoArray* CEComServer::ListImplementationsL(TUid aInterfaceUid, 
												   TUid aResolverUid,
												   const RExtendedInterfacesArray& aExtendedInterfaces,
												   const TClientRequest& aMessage
												   ) 
	{
	RImplInfoArray* result = NULL;
	CResolver* resolver = NULL;
	//create registry resolver transaction
	//get the TListImplParam parameters
  		//get the TListImplParam parameters
	TBool capability= ETrue;
	if(!(aMessage.IsNull()))
		{
		TListImplParam listParam;
  		TPckg<TListImplParam> listParamPkg(listParam);
  		aMessage.ReadL(2,listParamPkg);
  		capability=listParam.iCapabilityCheck;
		}
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,
																								aExtendedInterfaces,
																								aMessage,capability);
	CleanupStack::PushL(registryResolveTransaction);
	//create resolver
	resolver = CreateResolverLC(aResolverUid,registryResolveTransaction);
	result = ListImplementationsL(aInterfaceUid, resolver);
	//clean up
	CleanupStack::PopAndDestroy(resolver);
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	if ((aResolverUid != KDefaultResolverUid) && (aResolverUid != KRomOnlyResolverUid))
		{
		iResolverLibrary.Close();
		}
	return result;
	}


RImplInfoArray* CEComServer::ListImplementationsL(TUid aInterfaceUid, 
												   const TEComResolverParams& aAdditionalParameters,
												   const RExtendedInterfacesArray& aExtendedInterfaces,
												   const TClientRequest& aMessage) 
	{
	// Use the default resolver in the overloaded method.
	return ListImplementationsL(aInterfaceUid, aAdditionalParameters, KDefaultResolverUid,aExtendedInterfaces, aMessage);
	}

RImplInfoArray* CEComServer::ListImplementationsL(TUid aInterfaceUid,
												  const RExtendedInterfacesArray& aExtendedInterfaces,	
												  const TClientRequest& aMessage) 
	{
	// Use the default resolver in the overloaded method.
	return ListImplementationsL(aInterfaceUid, KDefaultResolverUid,aExtendedInterfaces, aMessage);
	}

// The private helper

RImplInfoArray* CEComServer::ListImplementationsL(TUid aInterfaceUid, 
												   const TEComResolverParams& aAdditionalParameters, 
												   CResolver* aResolver) const
	{
	if(!aResolver)
		User::Leave(KEComErrNoResolver);
	// Use the client provided resolver to build up the list.
	RImplInfoArray* infoArray = aResolver->ListAllL(aInterfaceUid, aAdditionalParameters);
	return infoArray;
	}

RImplInfoArray* CEComServer::ListImplementationsL(TUid aInterfaceUid, 
												  CResolver* aResolver) const
	{
	if(!aResolver)
		User::Leave(KEComErrNoResolver);
	// Use the provided resolver to build up the list.
	RImplInfoArray* infoArray = &aResolver->ListAllL(aInterfaceUid);
	// infoArray points to iImplementationInfo, which is owned by CRegistryResolveTransaction. 
	// CRegistryResolveTransaction object is transient and will be destroyed before return the implementation
	// info list to the CEComServerSession. Therefore, we need to have a copy to return
	RImplInfoArray* retList = new (ELeave) RImplInfoArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteImplInfoArray,retList));
	const TInt numImps = infoArray->Count();
	for(TInt index = 0; index < numImps; ++index)
		{	
		retList->AppendL((*infoArray)[index]);
		}
	// Reset the member variable because we are passing ownership back
	CleanupStack::Pop();
	return retList;
	}

void CEComServer::GetResolvedDllInfoL(	const TUid aImplementationUid,
										TEntry& aDllInfo,
										TUid& aDtor_Key,
										const TClientRequest& aClientRequest)
	{
	// No resolution to do create directly.
	aDtor_Key = aImplementationUid;
	CImplementationInformation* implInfo = NULL;
	TUid dummyUid={0x00000000};
	//We need to do the security check for the case that implementationUid is known.
	//if implementationUid is unknown, the security check will be done in ListImplementationsL.
	User::LeaveIfError(iRegistryData->GetImplementationDllInfoForClientL(
		aClientRequest, aImplementationUid, dummyUid, aDllInfo, implInfo, ETrue));
	}


void CEComServer::GetResolvedDllInfoL(  const TUid aInterfaceUid,
										const TEComResolverParams& aAdditionalParameters,
										const RExtendedInterfacesArray& aExtendedInterfaces,
										TEntry& aDllInfo, 
										TUid& aDtor_Key,
										const TClientRequest& aClientRequest)
	{
	GetResolvedDllInfoL(aInterfaceUid, aAdditionalParameters, KDefaultResolverUid, aExtendedInterfaces, aDllInfo, aDtor_Key, aClientRequest);
	}


void CEComServer::GetResolvedDllInfoL(  const TUid aInterfaceUid,
										const TEComResolverParams& aAdditionalParameters,
										const TUid aResolverUid,
										const RExtendedInterfacesArray& aExtendedInterfaces,
										TEntry& aDllInfo,
										TUid& aDtor_Key,
										const TClientRequest& aClientRequest)
	{
	CResolver* resolver = NULL;
	TBool capability= ETrue;
	//create registry resolver transaction
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,
																								aExtendedInterfaces,
																								aClientRequest,capability);
	CleanupStack::PushL(registryResolveTransaction);
	//create resolver
	resolver = CreateResolverLC(aResolverUid,registryResolveTransaction);
	aDtor_Key = resolver->IdentifyImplementationL(aInterfaceUid, aAdditionalParameters);

	//clean up
	CleanupStack::PopAndDestroy(resolver);
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	if ((aResolverUid != KDefaultResolverUid) && (aResolverUid != KRomOnlyResolverUid))
		{
		iResolverLibrary.Close();
		}
	CImplementationInformation* implInfo = NULL;
	//Don't need to do the security check because it has been done in IdentifyImplementationL.
	User::LeaveIfError(iRegistryData->GetImplementationDllInfoForClientL(
		aClientRequest, aDtor_Key, aInterfaceUid, aDllInfo, implInfo, EFalse));
	}

// Server Session management
CSession2* CEComServer::NewSessionL(const TVersion& aVersion,const RMessage2& /* aMessage*/) const
	{
	const TVersionName version = CONST_CAST(TVersion&,aVersion).Name();
	const TVersionName thisVersion = TVersion(KEComServerMajorVN,KEComServerMinorVN,KEComServerBuildVN).Name();
	if(thisVersion != version)
		User::Leave(KErrNotSupported);
	return new(ELeave) CEComServerSession();
	}

//
// A new session is being created
// Cancel the shutdown timer if it was running
//
void CEComServer::AddSession()
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
void CEComServer::DropSession()
	{
	if (--iSessionCount==0)
		iShutdown.Start();
	}

void CEComServer::Notification(TInt aCompletionCode)
//
// Pass on the signal to all clients
//
	{
	iSessionIter.SetToFirst();
	CSession2* s;
	while ((s = iSessionIter++)!=0)
		STATIC_CAST(CEComServerSession*,s)->CompleteNotifications(aCompletionCode);
	}

TInt CEComServer::RunError(TInt aError)
//
// Handle an error from CMySession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise report the error to the client
//
	{
	if (aError == KErrBadDescriptor)
		{
		PanicClient(Message(), aError);
		}
	else
		{
		Message().Complete(aError);
		}
	//
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	ReStart();
	return KErrNone;	// handled the error fully
	}

/**
Creates resolver object with aResolverUid UID.
The method leaves resolver onto the cleanup stack.
@param aResolverUid Resolver UID.
@param			aRegistryResolveTransaction A pointer to Registry resolve transaction object
@return A pointer to the created CResolver object.
@leave System-wide error codes, including KErrNoMemory.
*/
CResolver* CEComServer::CreateResolverLC(const TUid& aResolverUid,CRegistryResolveTransaction* aRegistryResolveTransaction)
	{
	CResolver* resolver = NULL;
	if(aResolverUid == KDefaultResolverUid)
		{
		// Create default resolver
		resolver = static_cast<CResolver*>(CDefaultResolver::NewL(*aRegistryResolveTransaction));
		CleanupStack::PushL(resolver);
		}
	else if(aResolverUid == KRomOnlyResolverUid)
		{
		// Create Rom Only resolver
		resolver = static_cast<CResolver*>(CRomOnlyResolver::NewL(*aRegistryResolveTransaction));
		CleanupStack::PushL(resolver);
		}
	else
		{
		// Create Custom Resolver
		resolver = CreateCustomResolverLC(aResolverUid, aRegistryResolveTransaction);
		}
	return resolver;
	}

/**
Creates custom resolver object with aResolverUid UID.
The method leaves custom resolver onto the cleanup stack.
@param aResolverUid Custom resolver UID.
@param			aRegistryResolveTransaction A pointer to Registry resolve transaction object
@return A pointer to the created CResolver object.
@leave System-wide error codes, including KErrNoMemory.
*/
CResolver* CEComServer::CreateCustomResolverLC(TUid aResolverUid,CRegistryResolveTransaction* aRegistryResolveTransaction)
	{
	typedef CResolver* (*TNewL)(MPublicRegistry&);
	TNewL newL = NULL;
	CResolver* resolver=NULL;

	TProxyNewLPtr tmpPtr;
	if (iResolverCache->CacheLookup(aResolverUid, tmpPtr)) // cache hit
		{
		newL = reinterpret_cast<TNewL>(tmpPtr);
		resolver = newL(*aRegistryResolveTransaction);
		CleanupStack::PushL(resolver);
		return resolver;
		}

	TEntry resolverDllInfo;
	// We should only load custom resolvers that are in the ROM
	//Initialize the server cap to ProtServ
	TCapabilitySet servercap(ECapabilityProtServ);
	CImplementationInformation* implInfo = NULL;
	TBool onWritableDrv = EFalse;  
	User::LeaveIfError(iRegistryData->GetImplementationDllInfoForServer(
		servercap, aResolverUid, KEComResolverInterfaceUid, resolverDllInfo,
		implInfo, onWritableDrv));

	// Type of the function pointer which is the proxy into the interface implementation collection
	typedef TImplementationProxy* (*TInstantiationL)(TInt&);
	// Function at ordinal 1 is InstantiationMethodL()
	const TInt KImplementationGroupProxy = 1;
	// So cast to the correct type : This gives an ANSI C++ warning
	// When using a REINTERPRET_CAST so simply cast instead

	const TDesC& libraryPath = resolverDllInfo.iName;
	// Make sure the non-default resolver library is closed
	iResolverLibrary.Close();
	User::LeaveIfError(iResolverLibrary.Load(libraryPath, resolverDllInfo.iType));
	__ECOM_TRACE2("ECOM: Resolver Loaded UID:0x%X - %S", aResolverUid.iUid, &resolverDllInfo.iName);
	TInstantiationL proxy= REINTERPRET_CAST(TInstantiationL, iResolverLibrary.Lookup(KImplementationGroupProxy));

	// Scan the returned table for a UID match, and return the associated
	// creation method if found.
	TInt count = 0;
	TImplementationProxy* implementationTable = proxy(count);
	for(TInt i = 0; i < count; ++i)
		{
		if(aResolverUid == implementationTable[i].iImplementationUid)
			{
			newL = (TNewL)(implementationTable[i].iNewLFuncPtr);
			}
		}

	if(newL)
		{
		if (IsCachable(onWritableDrv))
			{
			TUint32 flags = (onWritableDrv) ? EEntryIsOnReadWriteDrive : EEntryFlagsNone;
			User::LeaveIfError(iResolverCache->CacheResolver(aResolverUid,
				iResolverLibrary, (TProxyNewLPtr)newL, flags));
			// Handle is now owned by iResolverCache.
			iResolverLibrary.SetHandle(KNullHandle);
			}

		// Create the non-default resolver
		resolver = newL(*aRegistryResolveTransaction);
		CleanupStack::PushL(resolver);
		}
	else
		{
		User::Leave(KEComErrNoResolver);
		}
	return resolver;
	}


TBool CEComServer::RegistryIndexValid() const
	{
	return iRegistryData->IndexValid();
	}
	
/** Callback function. CRegistryData uses this to notify of implementation
upgrade. CDiscoverer uses this to notify state changes in SWI/BUR.
@param aObj Pointer to CEComServer object.
@param aEvent Identify the event.
@param aData Data associated with the callback.
@return none, not-used, ignored.
*/
TInt CEComServer::NotifyEvents(TAny* aObj, TInt aEvent, TAny* aData)
	{
	CEComServer* self = static_cast<CEComServer*>(aObj);
	switch (aEvent)
		{
		case ECallBackId_ImplUpgrade:
			{
			TUid* uid = static_cast<TUid*>(aData);
			self->NotifyUpgrade(*uid);
			}
			break;
		case ECallBackId_SwiEvent:
			self->NotifySWIEvent(aData);
			break;
		case ECallBackId_BurEvent:
			self->NotifyBUREvent(aData);
			break;
		default:
			__ECOM_TRACE1("ECOM: CEComServer::NotifyEvents received unknown event %d", aEvent);
		}

	return 0;
	}

/** This method is called when an implementation is upgraded.
@param aImplementationUid identify the implementation being upgraded.
*/
void CEComServer::NotifyUpgrade(const TUid aImplementationUid)
	{
	// Ignore return code which indicates if the UID is actually in cache.
	(void)iResolverCache->Remove(aImplementationUid);
	}

/** Called when there is SWI status change.
@param aData is TCallBackState* indicating start or end of SWI. 
*/
void CEComServer::NotifySWIEvent(TAny* aData)
	{
	TCallBackState* state = static_cast<TCallBackState*>(aData);
	UpdateSpecialEvents(ESWIInProgress, *state);
	}

/** Called when there is BUR status change.
@param aData is TCallBackState* indicating start or end of BUR. 
*/
void CEComServer::NotifyBUREvent(TAny* aData)
	{
	TCallBackState* state = static_cast<TCallBackState*>(aData);
	UpdateSpecialEvents(EBURInProgress, *state);
	}

/** Updates the BUR/SWI status.
@param aBit Indicate which bit to update.
@param aState Indicate start or end of event.
*/
void CEComServer::UpdateSpecialEvents(TUint32 aBit, TCallBackState aState)
	{
	TBitFlags32 oldstate = iSpecialEvents;

	if (aState == ECallBackState_EventStart)
		{
		iSpecialEvents.Set( aBit );
		}
	else
		{
		iSpecialEvents.Clear( aBit );
		}

	if (oldstate.Value() == 0 && iSpecialEvents.Value() != 0)
		{
		// BUR or SWI start. Need to evict cached resolvers on RW drives.
		iResolverCache->RemoveItemsWithFlags(EEntryIsOnReadWriteDrive);
		}
	}

/** Determine if a resolver entry is cachable.
@param aResolverEntry the resolver to check.
@return ETrue if the resolver should be added to cache. EFalse otherwise.
*/
TBool CEComServer::IsCachable(TBool aEntryIsOnRWDrive)
	{
	// Check the following conditions:
	// 1. DLL is on RW drive with BUR or SWI in progress.
	// 2. Cache size and cache timeout non-zero.
	return	iResolverCache->CachingEnabled() &&
			!(iSpecialEvents.Value() && aEntryIsOnRWDrive);
	}

#ifdef __ECOM_SERVER_TESTABILITY__
void CEComServer::ChangeStartupStateL(TInt aState) const
	{
	iServerStartupMgr->ChangeStartupStateL(aState);
	}

void CEComServer::ProcessCurrentStartupStateL() const
	{
	iServerStartupMgr->ResetRequestTransitionNotificationL();
	iServerStartupMgr->ResetLastStateAcknowledgedL();
	iServerStartupMgr->RunL();
	}
	
TInt CEComServer::GetCurrentStartupState() const
	{
	return iServerStartupMgr->CurrentStartupState();
	}
#endif //__ECOM_SERVER_TESTABILITY__

#ifdef __ECOM_SERVER_PERFORMANCE__
void CEComServer::GetRegistryCountsL(TInt aType, RegistryCounts::TRegistryCounts& aCounts) const
	{
	iRegistryData->GetRegistryCountsL(aType, aCounts);
	}	
#endif //__ECOM_SERVER_PERFORMANCE__
