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
// The Implementation of the REComSession singleton class which
// maintains the connection to ECom framework services.
// Include Files
// 
//

/**
 @internalComponent
 @file
*/

#include <s32mem.h>
#include "EComDebug.h"
#include <ecom/ecom.h>
#include "LoadManager.h"
#include "TlsData.h"
#include "EComServerStart.h"
#include "EComMessageIds.h"

//patchable constant defining the default size of buffer used during ListImplementations
#include "EComPatchData.h"
#include "EComUidCodes.h"

//
// Forward declarations

//
// Global Constants

// The maximum number of attempts the client should make to
// start up the server.
const TInt KMaxStartAttempts = 2;
// The buffer for passing the extended interface list. Each extended interface
// takes up 4 bytes. Currently the maximum extended interface is 8. By setting to the maximum, 
// the KErrOverFlow should not happen during the request of ListExtendedInterfacesL.
const TInt KListExtendedInterfaceBufferSize = 32; 

//
// Functions & Methods
static void CleanupFinalClose(TAny* /*aPtr*/)
	{
	REComSession::FinalClose();
	}

static void ResetAndDestroyArray(TAny* aPtr)
	{
	(static_cast<RImplInfoPtrArray*>(aPtr))->ResetAndDestroy();
	}
	
static void ResetArray(TAny* aPtr)
	{
	(static_cast<RExtendedInterfacesArray*>(aPtr))->Reset();
	}	
	
// Start the server process/thread which lives in an EPOCEXE object
//
static TInt StartServer()
	{
	const TUidType serverUid(KNullUid,KNullUid,KEComServerUid3);

	//
	//
	// EPOC is easy, we just create a new server process. Simultaneous launching
	// of two such processes should be detected when the second one attempts to
	// create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt error = server.Create(KEComServerImg,KNullDesC,serverUid);

	if (error != KErrNone)
		{
		return error;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	error=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return error;
	}

EXPORT_C REComSession::REComSession() :
	iReserved1(0),
	iReserved2(0)
	{
	}

/**
This method returns a reference to the singleton client/server session object 
maintained by the ECOM client library, referenced counted. If it does
not exist it is initialised and then returned. Clients should store the handle 
returned by refernce or by pointer. Storage by value is highly discouraged.

It is only necessary to use the Open()/Close() API directly if you need access 
to the notification methods. Access to the static API does not require these
to be used. Please remeber each call to Open() must have an equivalent Close().

@return	Reference to the open singleton session
@leave  KErrNoMemory
@leave  One of the system-wide error codes		 
@post REComSession is connected and ready to issue ECOM requests.
*/
EXPORT_C REComSession& REComSession::OpenL()
	{
	//CGlobalData::NewL() will create just one CGlobalData instance.
	//Every next CGlobalData::NewL() call will return a pointer to the already created
	//CGlobalData instance.
	CGlobalData* globalData = CGlobalData::NewL();
	globalData->IncRefCnt();
	return globalData->EComSession();
	}

/**
This method returns a reference to the singleton client/server session object 
maintained by the ECOM client library, referenced counted. If it does
not exist it is initialised and then returned.

@return	Reference to the open singleton session which is on the cleanupstack
@leave  KErrNoMemory
@leave  One of the system-wide error codes			 
@post REComSession is connected and ready to issue ECOM requests.
*/
REComSession& REComSession::OpenLC()
	{
	REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);
	return ecomSession;
	}
	
/**
Initialisation phase of two phase construction.

@leave  One of the system-wide error codes		 
@post  REComSession is fully initialised.
*/
void REComSession::ConstructL()
	{
	// Now connect to the ECom server
	TInt retry=KMaxStartAttempts;
	for (;;)
		{
		// create session with unlimited message slots due to  notification API being asynchronous
		TInt error = CreateSession(KEComServerName,TVersion(KEComServerMajorVN,KEComServerMinorVN,KEComServerBuildVN),-1);
		if(error == KErrNone)
			{
			return;	// Success
			}
		else if (error != KErrNotFound && error != KErrServerTerminated)
			User::Leave(error);
		if (--retry==0)
			User::Leave(error);
		error = StartServer();
		if (error != KErrNone && error != KErrAlreadyExists)
			User::Leave(error);
		}
	}

/**
Closes the open handle on the ECOM framework.
Reference count is decremented and the ECOM client/server session is closed.
 
@pre  REComSession must have been opened.
@post REComSession reference count is decremented, server session closed
*/
EXPORT_C void REComSession::Close()
	{
	// Switch close protection flag
	
	CGlobalData* globalData = CGlobalData::Instance();
	if(globalData)
		{
		//Decrement the reference count. Do not destroy the object. It will be used in FinalClose().
		globalData->DecRefCnt();
		}
	else
		{
		ReallyClose();
		}
	}

/**
The method will close the ECOM session connection.
@pre  REComSession must have been opened
@post The session is closed
*/
void REComSession::ReallyClose()
	{
	// Now disconnect from the ECom server
	RSessionBase::Close();
	}

/**
This method is used in processes that have utilised the ECOM framework, it 
does nothing if the ECOM framework has not been initialised. Its main purpose
is to release memory and close handles held on unused plug-in implementations 
and their associated DLLs. If is found (through reference counting) that the 
ECOM framework is no longer in use in the calling thread the session to the 
ECOM's server is also closed.
  
This method is called by direct users of the ECOM framework wherever possible 
(e.g. library, server or test code). It is safe to call it at any time outside 
of plug-in code as the user's scenario dictates. This maybe during a test case 
or just before process termination. 

Note: It must never be called from within a plug-in implementations class 
destructor, especially following a DestroyImplementation() as there is a risk 
that the plug-in's supporting DLL may immediately be unloaded by the Kernel due 
to the closure of RLibrary handles. It can result in a KERN-EXEC if the 
destructor call stack is still inside the DLL that is unloaded.

Note: Processes that utilise the ECOM framework that do not use this call 
are at risk of a UHEAP_MARKEND generated panic in debug builds.
*/
EXPORT_C void REComSession::FinalClose()
	{
	CGlobalData* globalData = CGlobalData::Instance();
	if(globalData)
		{
		// Clear out any garbage unload policies that may exist in the LoadManager.
		globalData->LoadManager().ClearGarbage();
		// With ref count greater than 0 indicates there is still open session
		// within this thread, otherwise clean up
		if(globalData->RefCnt() <= 0)
			{
			__ASSERT_DEBUG(globalData->LoadManager().PolicyArraysEmpty(), User::Invariant());
			delete globalData;
			}
		}
	}

/**
Registers for notification messages when the underlying ECOM registration
data changes. 
The client must not call this api again until their request is Completed
as this could result in a 'stray signal'.
This api should be placed first in the RunL of an Active Object otherwise
changes could be lost.
RunError should be implemented to cater for any Leaves.
For example, it could retry after a second if the ECom server is busy rebuilding its
indexes.
CancelNotifyOnChange should be called to cancel this request and should NOT be
part of the RunL.

@param			aStatus A request status object to complete for notification signalling.
@pre 			REComSession must have been opened.
@post			The caller is registered for receipt of notifications
				if the server's registry data changes.
*/
EXPORT_C void REComSession::NotifyOnChange(TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;
	SendReceive(ENotifyOnChange,TIpcArgs(&aStatus), aStatus);
	}

/**
De-registers for notification messages. 
@param			aStatus The request status object originally passed
				to NotifyOnChange() for notification signalling.
@pre 			REComSession must have been opened.
@post			The caller's registeration for reciept of notifications
				of registry data changes has been cancelled.
*/
EXPORT_C void REComSession::CancelNotifyOnChange(TRequestStatus& aStatus)
	{
	if (aStatus!=KRequestPending)
		return;
 
	SendReceive(ECancelNotifyOnChange,TIpcArgs(&aStatus));
	}
	
// ___________________________________________________________________________
// Message passing methods

//ListImplementationsL - LI1
/**
Retrieves a list of all the implementations which satisfy the specified interface.
The aImplInfoArray on exit contains the plug-in implementations who's plug-in
DLLs have sufficient capabilities to be loaded by the calling client process.
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@param			aInterfaceUid A UID specifying the required interface.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  RImplInfoPtrArray& aImplInfoArray)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	ListImplementationsL(aInterfaceUid,extendedInterfaces,aImplInfoArray);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}

//ListImplementationsL - LI2
/**
Retrieves a list of all the implementations which 
satisfy the specified interface with selection restriction to
the specified parameters.
The aImplInfoArray on exit contains the plug-in implementations who's plug-in
DLLs have sufficient capabilities to be loaded by the calling client process.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@since			7.0
@param			aInterfaceUid A UID specifying the required interface.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.		
@see TEComResolverParams
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  const TEComResolverParams& aResolutionParameters, 
												  RImplInfoPtrArray& aImplInfoArray)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	ListImplementationsL(aInterfaceUid,extendedInterfaces,aResolutionParameters,aImplInfoArray);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}
	
//ListImplementationsL - LI3
/**
Retrieves a list of all the implementations which satisfy the 
specified interface with selection restriction to
the specified parameters. 
The aImplInfoArray on exit contains the plug-in implementations who's plug-in
DLLs have sufficient capabilities to be loaded by the calling client process.
Overload with a client provided CResolver.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied
@param			aInterfaceUid A UID specifying the required interface.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The CResolver UID which identifies the resolver 
				implementation with the required client defined behaviour.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.		
@see TEComResolverParams
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  const TEComResolverParams& aResolutionParameters, 
												  TUid aResolverUid, 
												  RImplInfoPtrArray& aImplInfoArray)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	ListImplementationsL(aInterfaceUid,extendedInterfaces,aResolutionParameters,aResolverUid,aImplInfoArray);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}

//ListImplementationsL - LI4
/**
Retrieves a list of all the implementations which satisfy the specified interface, extended Interfaces.
The aImplInfoArray on exit contains the plug-in implementations who's plug-in
DLLs have sufficient capabilities to be loaded by the calling client process.
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or many extended interfaces to match.
				Must match all extended interfaces for a match to occur.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.
@publishedPartner
@released				
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  RExtendedInterfacesArray& aExtendedInterfaces,
												  RImplInfoPtrArray& aImplInfoArray)
	{
	if( aInterfaceUid.iUid == 0 ) 
		User::Leave(KErrArgument);

	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));
	
	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	TEComResolverParams resolutionParameters;
	ecomSession.ListImplementationsL(EListImplementations, 
						 aInterfaceUid, 
						 aExtendedInterfaces,
						 resolutionParameters, 
						 KNullUid,
						 aImplInfoArray);

	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	}

//ListImplementationsL - LI5
/**
Retrieves a list of all the implementations which 
satisfy the specified interface, extended interfaces, with selection restriction to
the specified parameters.
The aImplInfoArray on exit contains the plug-in implementations who's plug-in
DLLs have sufficient capabilities to be loaded by the calling client process.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
				Must match all extended interfaces for a match to occur.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.		
@see TEComResolverParams
@publishedPartner
@released
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  RExtendedInterfacesArray& aExtendedInterfaces,
												  const TEComResolverParams& aResolutionParameters, 
												  RImplInfoPtrArray& aImplInfoArray)
	{
	if( aInterfaceUid.iUid == 0) 
		User::Leave(KErrArgument);
	
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	ecomSession.ListImplementationsL(EListResolvedImplementations, 
						 aInterfaceUid, 
						 aExtendedInterfaces,
						 aResolutionParameters, 
						 KNullUid,
						 aImplInfoArray);

	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	}
	
//ListImplementationsL - LI6
/**
Retrieves a list of all the implementations which satisfy the 
specified interface, extended Interfaces, with selection restriction to
the specified parameters. 
The aImplInfoArray on exit contains the plug-in implementations who's plug-in
DLLs have sufficient capabilities to be loaded by the calling client process.
Overload with a client provided CResolver.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
				Must match all extended interfaces for a match to occur.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The CResolver UID which identifies the resolver 
				implementation with the required client defined behaviour.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.		
@see TEComResolverParams
@publishedPartner
@released
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  RExtendedInterfacesArray& aExtendedInterfaces,
												  const TEComResolverParams& aResolutionParameters, 
												  TUid aResolverUid, 
												  RImplInfoPtrArray& aImplInfoArray)
	{
	if( aInterfaceUid.iUid == 0 ||
		aResolverUid.iUid == 0)
		User::Leave(KErrArgument);

	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	ecomSession.ListImplementationsL(EListCustomResolvedImplementations, 
						 aInterfaceUid,
						 aExtendedInterfaces,
						 aResolutionParameters, 
						 aResolverUid,
						 aImplInfoArray);

	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	}


//ListImplementationsL - LI7
/**
Retrieves a list of all the implementations which satisfy the specified interface.
This method does not do capability check.
The aImplInfoArray on exit contains the plug-in implementations .
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@param			aInterfaceUid A UID specifying the required interface.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@param			aCapabilityCheck A boolean value EFalse , if no capability check is required ETrue otherwise.				
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  RImplInfoPtrArray& aImplInfoArray,TBool aCapabilityCheck)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	if( aInterfaceUid.iUid == 0 ) 
	User::Leave(KErrArgument);
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));
	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
													// using the global instance which is returned.
	TEComResolverParams resolutionParameters;
	ecomSession.ListImplementationsL(EListImplementations, 
						 aInterfaceUid, 
						 extendedInterfaces,
						 resolutionParameters, 
						 KNullUid,
						 aImplInfoArray,
						 aCapabilityCheck);

	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	
	
	}

//ListImplementationsL - LI8
/**
Retrieves a list of all the implementations which 
satisfy the specified interface with selection restriction to
the specified parameters.This method does not do capability check.
The aImplInfoArray on exit contains the plug-in implementations .

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@since			7.0
@param			aInterfaceUid A UID specifying the required interface.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
				@param			aCapabilityCheck A boolean value EFalse , if no capability check is required ETrue otherwise.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.		
@see TEComResolverParams
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  const TEComResolverParams& aResolutionParameters, 
												  RImplInfoPtrArray& aImplInfoArray,TBool aCapabilityCheck)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	if( aInterfaceUid.iUid == 0) 
	User::Leave(KErrArgument);
	
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	ecomSession.ListImplementationsL(EListResolvedImplementations, 
						 aInterfaceUid, 
						 extendedInterfaces,
						 aResolutionParameters, 
						 KNullUid,
						 aImplInfoArray,
						 aCapabilityCheck);

	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	
		
	
	}
	
//ListImplementationsL - LI9
/**
Retrieves a list of all the implementations which satisfy the 
specified interface with selection restriction to
the specified parameters.This method does not do capability check. 
The aImplInfoArray on exit contains the plug-in implementations .
Overload with a client provided CResolver.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied
@param			aInterfaceUid A UID specifying the required interface.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The CResolver UID which identifies the resolver 
				implementation with the required client defined behaviour.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
				@param			aCapabilityCheck A boolean value EFalse , if no capability check is required ETrue otherwise.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.		
@see TEComResolverParams
*/
EXPORT_C void REComSession::ListImplementationsL(TUid aInterfaceUid, 
												  const TEComResolverParams& aResolutionParameters, 
												  TUid aResolverUid, 
												  RImplInfoPtrArray& aImplInfoArray,TBool aCapabilityCheck)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	if( aInterfaceUid.iUid == 0 ||
	aResolverUid.iUid == 0)
	User::Leave(KErrArgument);

	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	ecomSession.ListImplementationsL(EListCustomResolvedImplementations, 
						 aInterfaceUid,
						 extendedInterfaces,
						 aResolutionParameters, 
						 aResolverUid,
						 aImplInfoArray,
						 aCapabilityCheck);

	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	CleanupStack::PopAndDestroy(&extendedInterfaces);	
	
	
	}




//CreateImplementationL - CI1
/**
Instantiates an interface implementation to satisfy the specified interface. 
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aImplementationUid A UID specifying the required interface implementation.
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aImplementationUid, 
													TUid& aInstanceKey)
	{
	TUid tempInstanceKey = aInstanceKey = KNullUid;
	TBool instParamsFlag = EFalse;

	if( aImplementationUid.iUid == 0 ) 
		User::Leave(KErrArgument);
	
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	// Use the default resolver in the overloaded method.
	TAny* result = ecomSession.ResolveAndCreateImplL(aImplementationUid, tempInstanceKey, NULL, instParamsFlag);
	
	CleanupStack::Pop(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose

	aInstanceKey = tempInstanceKey;
	
	return result;
	}

//CreateImplementationL - CI2
/**
Instantiates an interface implementation to satisfy the specified interface. 
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aImplementationUid A UID specifying the required interface implementation.
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aImplementationUid, 
													TInt32 aKeyOffset)
	{
	TUid key;

	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(aImplementationUid, key);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}

//CreateImplementationL - CI3
/**
Instantiates an interface implementation to satisfy the specified interface. 
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aImplementationUid A UID specifying the required interface implementation.
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aImplementationUid, 
													TUid& aInstanceKey,
													TAny* aInstantiationParams)
	{
	TUid tempInstanceKey = aInstanceKey = KNullUid;
	TBool instParamsFlag = ETrue;

	if( aImplementationUid.iUid == 0) 
		User::Leave(KErrArgument);
	
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	// Use the default resolver in the overloaded method.
	TAny* result = ecomSession.ResolveAndCreateImplL(aImplementationUid, tempInstanceKey, aInstantiationParams, instParamsFlag);
	
	CleanupStack::Pop(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose

	aInstanceKey = tempInstanceKey;
	
	return result;
	}

//CreateImplementationL - CI4
/**
Instantiates an interface implementation to satisfy the specified interface.
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aImplementationUid A UID specifying the required interface implementation.
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aImplementationUid, 
													TInt32 aKeyOffset,
													TAny* aInstantiationParams)
	{
	TUid key;
	
	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(aImplementationUid, key, aInstantiationParams);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;
	return instance;
	}

//CreateImplementationL - CI5
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TUid& aInstanceKey,
													const TEComResolverParams& aResolutionParameters)

	{
	TUid tempInstanceKey = aInstanceKey = KNullUid;
	TBool instParamsFlag = EFalse;

	if( aInterfaceUid.iUid == 0) 
		User::Leave(KErrArgument);
	
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom

	// Use the default resolver in the overloaded method.
	TAny* result = ecomSession.ResolveAndCreateImplL(aInterfaceUid, aResolutionParameters, tempInstanceKey, NULL,instParamsFlag);
	
	CleanupStack::Pop(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	
	aInstanceKey = tempInstanceKey;
	
	return result;
	}

//CreateImplementationL - CI6
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TInt32 aKeyOffset, 
													const TEComResolverParams& aResolutionParameters)
	{
	TUid key;

	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(	aInterfaceUid, 
											key,
											aResolutionParameters);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}

//CreateImplementationL - CI7
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TUid& aInstanceKey, 
													TAny* aInstantiationParams, 
													const TEComResolverParams& aResolutionParameters)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TAny* instance = CreateImplementationL(aInterfaceUid,
						  				 extendedInterfaces,
						  				 aInstanceKey,
						  				 aResolutionParameters,
						  				 aInstantiationParams);
	CleanupStack::PopAndDestroy(&extendedInterfaces);						  
	return instance;
	}

//CreateImplementationL - CI8
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TInt32 aKeyOffset, 
													TAny* aInstantiationParams, 
													const TEComResolverParams& aResolutionParameters)
	{
	TUid key;
	
	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(	aInterfaceUid, 
											key,
											aInstantiationParams, 
											aResolutionParameters);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}

//CreateImplementationL - CI9
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TUid& aInstanceKey, 
													const TEComResolverParams& aResolutionParameters,
													TUid aResolverUid)
	{
	TUid tempInstanceKey = aInstanceKey = KNullUid;
	TBool instParamsFlag = EFalse; 

	if( aInterfaceUid.iUid == 0 ||
		aResolverUid.iUid == 0) 
		User::Leave(KErrArgument);

	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	// Use the default resolver in the overloaded method.
	TAny* result = ecomSession.ResolveAndCreateImplL(aInterfaceUid, aResolutionParameters, aResolverUid, tempInstanceKey, NULL,instParamsFlag);

	CleanupStack::Pop(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose

	aInstanceKey = tempInstanceKey;
	
	return result;
	}

//CreateImplementationL - CI10
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TInt32 aKeyOffset,
													const TEComResolverParams& aResolutionParameters, 
													TUid aResolverUid)
	{
	TUid key;

	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(	aInterfaceUid, 
											key, 
											aResolutionParameters,
											aResolverUid);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}

//CreateImplementationL - CI11
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TUid& aInstanceKey, 
													TAny* aInstantiationParams,
													const TEComResolverParams& aResolutionParameters, 
													TUid aResolverUid)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TAny* instance = CreateImplementationL(aInterfaceUid,
										   extendedInterfaces,
										   aInstanceKey,
										   aResolutionParameters,
										   aResolverUid,
										   aInstantiationParams);
	CleanupStack::PopAndDestroy(&extendedInterfaces);			
	return instance;
	}

//CreateImplementationL - CI12
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@param			aInterfaceUid A UID specifying the required interface.
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													TInt32 aKeyOffset, 
													TAny* aInstantiationParams,
													const TEComResolverParams& aResolutionParameters,
													TUid aResolverUid)
	{
	TUid key;

	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(	aInterfaceUid, 
											key, 
											aInstantiationParams,
											aResolutionParameters, 
											aResolverUid);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}
	
//CreateImplementationL - CI13
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface implementation.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
				Must match all extended interfaces for a match to occur. 
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
				Default value is NULL.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
@publishedPartner
@released
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
														const RExtendedInterfacesArray& aExtendedInterfaces,
														TUid& aInstanceKey, 
														const TEComResolverParams& aResolutionParameters,
														TAny* aInstantiationParams)
	{
	TUid tempInstanceKey = aInstanceKey = KNullUid;
	TBool instParamsFlag = ETrue;
	
	if( aInterfaceUid.iUid == 0) 
		User::Leave(KErrArgument);

	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	// Use the default resolver in the overloaded method.
	TAny* result = ecomSession.ResolveAndCreateImplL(aInterfaceUid, aExtendedInterfaces, aResolutionParameters, tempInstanceKey, aInstantiationParams, instParamsFlag);
	
	CleanupStack::Pop(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose

	aInstanceKey = tempInstanceKey;
	return result;
	}

//CreateImplementationL - CI14
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface implementation.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
				Must match all extended interfaces for a match to occur. 
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
				Default value is NULL.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
@publishedPartner
@released
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													const RExtendedInterfacesArray& aExtendedInterfaces,
													TInt32 aKeyOffset, 
													const TEComResolverParams& aResolutionParameters,
													TAny* aInstantiationParams)
	{
	TUid key;
	
	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(	aInterfaceUid, 
											aExtendedInterfaces, 
											key,
											aResolutionParameters,
											aInstantiationParams);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}

//CreateImplementationL - CI15
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface implementation.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
				Must match all extended interfaces for a match to occur. 
@param			aInstanceKey A 32-bit instance key that is returned by the ECom framework. 
				This instance key should be passed as an argument to the DestroyedImplementation() 
				method to identify the implementation for destruction. 
				No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
				Default value is NULL.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
@publishedPartner
@released
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													const RExtendedInterfacesArray& aExtendedInterfaces,
													TUid& aInstanceKey, 
													const TEComResolverParams& aResolutionParameters, 
													TUid aResolverUid,
													TAny* aInstantiationParams)
	{
	TUid tempInstanceKey = aInstanceKey = KNullUid;
	TBool instParamsFlag = ETrue;

	if( aInterfaceUid.iUid == 0 ||
		aResolverUid.iUid == 0) 
		User::Leave(KErrArgument);

	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.
	// Use the custom resolver with specific aResolverUid in the overloaded method
	TAny* result = ecomSession.ResolveAndCreateImplL(aInterfaceUid, 
													 aExtendedInterfaces, 
													 aResolutionParameters, 
													 aResolverUid, 
													 tempInstanceKey,
													 aInstantiationParams,
													 instParamsFlag);

	CleanupStack::Pop(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	
	aInstanceKey = tempInstanceKey;
	
	return result;
	}	

//CreateImplementationL - CI16
/**
Instantiates an interface implementation to satisfy the specified interface. 

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrArgument
@leave			KErrPermissionDenied. This leave error can happen under the following conditions:
				1. There is a capability mismatch between the process creating the implementation
				 and the DLL containing the implementation
				2. The plugin DLL installed on the media card was not properly installed, 
				either the system hash value of this plugin is missing or it is inconsistent
				with that calculated for the plugin DLL at load time.
@leave 			Or any of the System Wide error codes
@param			aInterfaceUid A UID specifying the required interface implementation.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match. 
				Must match all extended interfaces for a match to occur. 
@param			aKeyOffset An offset to the 32 bit identifer returned by the ECom framework to
				identify this instance to the framework.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@param			aInstantiationParams The parameter structure to pass to the object creation method.
				Default value is NULL.
@return			TAny* A pointer to the instantiated interface implementation.
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
@publishedPartner
@released
*/
EXPORT_C TAny* REComSession::CreateImplementationL(TUid aInterfaceUid, 
													const RExtendedInterfacesArray& aExtendedInterfaces,	
													TInt32 aKeyOffset, 
													const TEComResolverParams& aResolutionParameters,
													TUid aResolverUid,
													TAny* aInstantiationParams)
	{
	TUid key;

	// Get the instance using the overload which gives us the key back
	TAny* instance = CreateImplementationL(	aInterfaceUid, 
											aExtendedInterfaces,
											key, 
											aResolutionParameters, 
											aResolverUid,
											aInstantiationParams);
	
	// Find the key location using the offset and store it
	TUint8* offsetPtr = REINTERPRET_CAST(TUint8*, instance) + aKeyOffset;
	TUid* instanceKey = REINTERPRET_CAST(TUid*, offsetPtr);
	*instanceKey = key;

	return instance;
	}
	
/**
Returns the requested interface, NULL if it does not exist
@leave			KErrNotFound When the supplied 32-bit implementation instance key is not recognised by ECom.
@leave			KErrArgument When a null implementation instance key is passed as an argument to ECom.
@leave			Any of the other System wide errors
@param			aInstanceKey A 32-bit implementation instance key that is returned from a call to one of the CreateImplementationL()
				methods. ECOM uses this implementation instance key when it is passed as an argument to the
				DestroyedImplementation() method to identify an implementation for destruction. No meaning should be attached to
				this implementation instance key by ECOM clients and it should not be used for any other purpose.
@param			aExtendedInterfaceUid Identifies an interface to fetch from the plug-in instance.
@return			TAny* A pointer to the extended interface implementation if the implementation identified by aInstanceKey
                supports it, or NULL if the implementation does not support the extended interface.
@publishedPartner
@released
*/
EXPORT_C TAny* REComSession::GetExtendedInterfaceL(const TUid& aInstanceKey, const TUid& aExtendedInterfaceUid)
	{
	if(aInstanceKey == KNullUid)
		User::Leave(KErrArgument);
	
	TAny* object = NULL;
	
	CGlobalData* globalData = CGlobalData::Instance();
	if (globalData == 0)
        {
        __ECOM_LOG("ECOM: PANIC in REComSession::GetExtendedInterfaceL(), no global data");
    	User::Panic(KEComClientDLLPanicCategory, EEComPanic_LibraryNotInitialised);
        }


	// Leaves with KErrNotFound if aInstanceKey not known.
	object = globalData->LoadManager().GetExtendedInterfaceL(aInstanceKey, aExtendedInterfaceUid);	

	return object;
	}	

/**
Releases the extended interface associated with the supplied instance. Does nothing if the interface
does not exist. This interface is optional, normally the interfaces are cleaned up automatically. This
can be used if it is necessary to clean up an extended interface before the automatic cleanup occurs when
the instance is destroyed.
@leave	KErrNotFound When the supplied 32-bit implementation instance key is not recognised by ECom.
@leave	KErrArgument When a null implementation instance key is passed as an argument to ECom.
@leave	Any of the other System wide errors
@param	aInstanceKey A 32-bit implementation instance key that is returned from a call to one of the CreateImplementationL()
		methods. ECOM uses this implementation instance key when it is passed as an argument to the
		DestroyedImplementation() method to identify an implementation for destruction. No meaning should be attached to
		this implementation instance key by ECOM clients and it should not be used for any other purpose.
@param	aExtendedInterfaceUid Identifies an extended interface to release.
@publishedPartner
@released
*/
EXPORT_C void REComSession::ManuallyReleaseExtendedInterfaceL(const TUid& aInstanceKey, const TUid& aExtendedInterfaceUid)
	{
	if(aInstanceKey == KNullUid)
		User::Leave(KErrArgument);
	
	CGlobalData* globalData = CGlobalData::Instance();
	if (globalData == 0)
        {
        __ECOM_LOG("ECOM: PANIC in REComSession::GetExtendedInterfaceL(), no global data");
    	User::Panic(KEComClientDLLPanicCategory, EEComPanic_LibraryNotInitialised);
        }
        
	// Leaves with KErrNotFound if aInstanceKey not known.
    globalData->LoadManager().ManuallyReleaseExtendedInterfaceL(aInstanceKey, aExtendedInterfaceUid);
	}
		
/**
Retrieves a list of all the extended interfaces for the provided implementation.
@leave          KErrNoMemory
@leave          One of the system-wide error codes
@param			aImplementationUid Implementation UID to fetch the extended interfaces for.
@param			aIfInfo Return value consisting of an array containing the extended interfaces.
@publishedPartner
@released
*/
EXPORT_C void REComSession::ListExtendedInterfacesL(const TUid& aImplementationUid,RExtendedInterfacesArray& aIfInfo)
	{
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may not be created yet.
	REComSession& ecomSession = REComSession::OpenLC();	// NOTE: This will connect to ECom
														// using the global instance which is returned.

	ecomSession.ProcessListExtendedInterfacesL(aImplementationUid, aIfInfo);
	
	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	}

/**
Process the list extended interface request.
@leave          KErrNoMemory
@leave          One of the system-wide error codes	
@param			aImplementationUid Implementation UID to fetch the extended interfaces for.
@param			aIfInfo Return value consisting of an array containing the extended interfaces.
*/
void REComSession::ProcessListExtendedInterfacesL(const TUid& aImplementationUid, RExtendedInterfacesArray& aIfInfo)
	{
	// Ensure the array we have been given is empty
	aIfInfo.Reset();

	__ECOM_TRACE1("ECOM: List Extended Interfaces request for implementation %X", aImplementationUid); 
	
	// Package the parameters
	TPckg<TUid> implementationUidDes(aImplementationUid);
	TInt bufferSize = KListExtendedInterfaceBufferSize;
	TPckg<TInt> bufferSizeDes(bufferSize);
		
	RBuf8 buf;
	buf.CreateMaxL(KListExtendedInterfaceBufferSize);
	buf.CleanupClosePushL();
	TIpcArgs arguments(&implementationUidDes, &bufferSizeDes, &buf);
	
	TInt error = SendReceive(EListExtendedInterfaces,arguments);

	if (error==KErrNone)
		{	
		// When no extended interface is found, KErrNone is returned with bufferSize = 0
		if (bufferSize>0)
			{
			// Now unpack the streamed data into a reconstructed array
			RDesReadStream readStream;
			readStream.Open(buf);
			CleanupClosePushL(readStream);
			CleanupStack::PushL(TCleanupItem(ResetArray, &aIfInfo));
			
			TInt entryCount = bufferSize / static_cast<TInt>(sizeof(TUid));
			if(entryCount > 0)
				{
				TUid interfaceUid;
				// Build the store data then calculate the end size;
				for(TInt i = 0; i < entryCount; ++i)
					{
					interfaceUid.iUid = readStream.ReadInt32L();
					aIfInfo.AppendL(interfaceUid);
					}
				}
			CleanupStack::Pop(); // NOT PopAndDestroy - we don't want to destroy the aIfInfo we've just filled
			CleanupStack::PopAndDestroy(&readStream);
			__ECOM_TRACE1("ECOM: Request completed OK - %d interfaces in list", entryCount);
			}
		}
	else 
		{
		// There was a problem on the server side.
		__ECOM_TRACE1("ECOM: Request error %d", error);
		User::Leave(error);
		}
	CleanupStack::PopAndDestroy(&buf);
	}

/**
Gets the corresponding implementation uid for an instance key.
This is typically used after a call to one of the CreateImplementationL() methods
that returns the aInstanceKey value as an output arg or by an offset (aKeyOffset).
@leave			KErrNotFound When the supplied 32-bit implementation instance key is not recognised by ECom.
@leave			KErrArgument When a null implementation instance key is passed as an argument to ECom.
@leave			Any of the other System wide errors
@param			aInstanceKey A 32-bit implementation instance key that is returned from a call to one of the CreateImplementationL()
				methods. ECOM uses this implementation instance key when it is passed as an argument to the
				DestroyedImplementation() method to identify an implementation for destruction. No meaning should be attached to
				this implementation instance key by ECOM clients and it should not be used for any other purpose.
@return			TUid The uid of the corresponding implementation.
@post			REComSession has not changed
*/
EXPORT_C TUid REComSession::GetImplementationUidL(TUid aInstanceKey)
	{
	if(aInstanceKey == KNullUid)
		User::Leave(KErrArgument);
	
	TUid implId = KNullUid;
	CGlobalData* globalData = CGlobalData::Instance();
	if (globalData == 0)
        {
        __ECOM_LOG("ECOM: PANIC in REComSession::ManuallyReleaseExtendedInterfaceL(), no global data");
    	User::Panic(KEComClientDLLPanicCategory, EEComPanic_LibraryNotInitialised);
        }

	// Leaves with KErrNotFound if aInstanceKey not known.
	implId = globalData->LoadManager().GetImplementationUidL(aInstanceKey);

	return implId;
	}
	
/**
Signals the destruction of an interface implementation to ECOM.
LoadManager is responsible for destroying the implementation
@param			aInstanceKey A 32-bit instance key that is returned from a call to one of the 
				CreateImplementationL() methods. This instance key identifies the implementation 
				for destruction. No meaning should be attached to this instance key by ECOM clients 
				and it should not be used for any other purpose.
@post			The destruction of the Interface Implementation referred 
				to by aInstanceKey
*/
EXPORT_C void REComSession::DestroyedImplementation(TUid aInstanceKey)
	{
	if(aInstanceKey != KNullUid)
		{
		CGlobalData* globalData = CGlobalData::Instance();
		if(globalData)
			{
			if(globalData->LoadManager().DestroyedThis(aInstanceKey))
				{
				globalData->EComSession().Close();
				}
			}
		}
	}
/**
Pack the match string of the resolution parameter and extended interfaces into a buffer for sending to server.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match,
				Must match all extended interfaces for a match to occur.
@param			aMatchStrExtInfBuf The return buffer containing the information of match string and extended interfaces.
*/	
void REComSession::PackMatchStrAndExtendedInterfacesL(const TEComResolverParams& aResolutionParameters, 
													  const RExtendedInterfacesArray& aExtendedInterfaces, 
													  RBuf8& aMatchStrExtInfBuf)
	{
    // Note the data type might be an invalid descriptor
    // because the client caller may have used a default constructed 
    // TEComResolverParams. Do not access the descriptor data here. The server
    // does not access the data as it checks the size supplied below and 
    // creates a zero length descriptor in such cases.
	TPtrC8 matchString = aResolutionParameters.DataType();
		
	TInt lenOfMatchString = matchString.Length();
	TInt numOfExtendedInterfaces = aExtendedInterfaces.Count();
	// sizeOfMatchStrExtInfBuf being the combined size of lenOfmatchType(TInt), matchString, 
	// numOfExtendedInterfaces(TInt) and extended interfaces
	TInt sizeOfMatchStrExtInfBuf = sizeof(TInt) + lenOfMatchString + sizeof(TInt) + numOfExtendedInterfaces * sizeof(TUid);
	aMatchStrExtInfBuf.CreateMaxL(sizeOfMatchStrExtInfBuf);
	aMatchStrExtInfBuf.CleanupClosePushL();	
	RDesWriteStream writeStream;
	writeStream.Open(aMatchStrExtInfBuf);
	writeStream.WriteInt32L(lenOfMatchString);
	//write matchString to the stream only when it's size is not zero
	if (lenOfMatchString > 0)
		{
		writeStream.WriteL(matchString.Ptr(),lenOfMatchString);	
		}
	writeStream.WriteInt32L(numOfExtendedInterfaces);
	for(TInt i = 0; i < numOfExtendedInterfaces; ++i)
		{
		writeStream.WriteInt32L(aExtendedInterfaces[i].iUid);
		}
	writeStream.CommitL();		
	CleanupStack::Pop(&aMatchStrExtInfBuf);
	}

/**
Retrieves a list of all the implementations which satisfy the 
specified interface with selection restriction to
the specified parameters. 
Overload for internal use to ensure consistency between calls.
This is the method which actually makes the IPC call.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aServiceId An integer specifying the ECom service variant to use.
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match,
				Must match all extended interfaces for a match to occur.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The CResolver UID which identifies the resolver 
				implementation with the required client defined behaviour.
@param			aImplInfoArray A reference to a client owned array which will be filled 
				with interface implementation data. The array will first be cleared and
				all items destroyed before adding new data.
@post			REComSession has not changed, and aImplInfoArray 
				contains the list of Implementation information for the interface.
@see TEComResolverParams
*/
void REComSession::ListImplementationsL(
            TInt aServiceId, 
			TUid aInterfaceUid, 
			const RExtendedInterfacesArray& aExtendedInterfaces,
			const TEComResolverParams& aResolutionParameters, 
			TUid aResolverUid, 
			RImplInfoPtrArray& aImplInfoArray,
			TBool aCapabilityCheck)
	{
	// Ensure the array we have been given is empty
	aImplInfoArray.ResetAndDestroy();

	__ECOM_TRACE2("ECOM: List implementations request for i/f %X using resolver %08X", aInterfaceUid, aResolverUid); 

    // Package the parameters
    // Use the UidType because it will hold our 3 possible UIDs
    // neatly, and save space in the message parameter passing structure.
	TUidType uidtype(aInterfaceUid,KNullUid,aResolverUid);
	//1st argument
	TPckg<TUidType> uids(uidtype);
   	
	// 2nd argument which includes lenOfmatchString(TInt), matchString, 
	// numOfExtendedInterfaces(TInt) and extended Interfaces
	RBuf8 matchStrExtInfBuf;
	PackMatchStrAndExtendedInterfacesL(aResolutionParameters,aExtendedInterfaces,matchStrExtInfBuf);
	matchStrExtInfBuf.CleanupClosePushL();
	
	//3rd argument
	// This KListIMplBufferSize is a patchable constant and this can be modified during buildrom time
 	TInt bufferSize=GetListImplBufferSize();
 	TInt matchType = aResolutionParameters.IsGenericMatch();
	TListImplParam listParam={matchType,bufferSize,aCapabilityCheck};	
  	TPckg<TListImplParam> listParamPckg(listParam);
  		
	//4th argument
	RBuf8 buf;
	buf.CreateMaxL(bufferSize);	
	buf.CleanupClosePushL();

	TIpcArgs aArgs(&uids,&matchStrExtInfBuf,&listParamPckg,&buf);
	
	TInt error= SendReceive(aServiceId,aArgs);
	if (error == KErrOverflow)
		{
		//resend the request with the correct buffer size
		//Get the new buffer size.
		if (listParam.iBufferSize)
			{
			//re-allocate the buffer
			buf.ReAllocL(listParam.iBufferSize);
			TIpcArgs args(&buf);
			User::LeaveIfError(SendReceive(ECollectImplementationsList,args));
			error=KErrNone;
			}
		}
	if (error==KErrNone)
		{	
		if (listParam.iBufferSize)
			{
			// Now unpack the streamed data into a reconstructed array
			RDesReadStream readStream;
			readStream.Open(buf);

			CleanupClosePushL(readStream);
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyArray, &aImplInfoArray));

			TInt entryCount = readStream.ReadInt32L();
			if(entryCount)
				{
				// Build the store data then calculate the end size;
				for(TInt i = 0; i < entryCount; ++i)
					{
					CImplementationInformation* info = CImplementationInformation::NewLC(ETrue,readStream);
					aImplInfoArray.AppendL(info);
					CleanupStack::Pop(info);	// Now owned by implementations
					}
				}

			CleanupStack::Pop(); // NOT PopAndDestroy - we don't want to destroy the aImplInfoArray we've just filled
			CleanupStack::PopAndDestroy(&readStream);
			__ECOM_TRACE1("ECOM: Request completed OK - %d implementations in list", entryCount);
			}
		}
	// if error == KEComErrNoInterfaceIdentified then do nothing so the client gets
	// an empty list.  Otherwise leave with the error.
	else if(error != KEComErrNoInterfaceIdentified) 
		{
		CleanupStack::PopAndDestroy(&buf);
		// There was a problem on the server side
		__ECOM_TRACE1("ECOM: Request error %d", error);
		User::Leave(error);
		}
	CleanupStack::PopAndDestroy(&buf);	
	CleanupStack::PopAndDestroy(&matchStrExtInfBuf);
	}

/**
Provides the instantiation method for an interface implementation 
that satisfies the specified interface. 

@leave 			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aImplementationUid A UID specifying the required interface implementation.
@param			aInstanceKey A 32 bit instance key returned by the ECom framework that
				identifies the returned instance.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of 
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed implementation
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
*/
TAny* REComSession::ResolveAndCreateImplL(TUid aImplementationUid, 
											   TUid& aInstanceKey,
											   TAny* aCreationParameters, TBool aCreationParamsFlag)
	{
	TEComResolverParams resolutionParameters;
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TAny* object = ResolveAndCreateImplL(EGetImplementationCreationMethod,
									  aImplementationUid, 
									  extendedInterfaces,
									  resolutionParameters, 
									  KNullUid, 
									  aInstanceKey,
									  aCreationParameters, aCreationParamsFlag);
	CleanupStack::PopAndDestroy(&extendedInterfaces);									  
	return object;									  
	}

/**
Provides the instantiation method for an interface implementation 
that satisfies the specified interface.
 
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aInterfaceUid A UID specifying the required interface.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aInstanceKey A 32 bit instance key returned by the ECom framework that
				identifies the returned instance.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of 
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed implementation
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
TAny* REComSession::ResolveAndCreateImplL(TUid aInterfaceUid, 
											   const TEComResolverParams& aResolutionParameters, 
											   TUid& aInstanceKey, 
											   TAny* aCreationParameters, TBool aCreationParamsFlag)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TAny* object = ResolveAndCreateImplL(EGetResolvedCreationMethod,
									  aInterfaceUid, 
									  extendedInterfaces,
									  aResolutionParameters, 
									  KNullUid, 
									  aInstanceKey,
									  aCreationParameters, aCreationParamsFlag);
	CleanupStack::PopAndDestroy(&extendedInterfaces);									  
	return object;
	}

/**
Provides the instantiation method for an interface implementation 
that satisfies the specified interface.
 
@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
				Must match all extended interfaces for a match to occur.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aInstanceKey A 32 bit instance key returned by the ECom framework that
				identifies the returned instance.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of 
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed implementation
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
TAny* REComSession::ResolveAndCreateImplL(TUid aInterfaceUid, 
											const RExtendedInterfacesArray& aExtendedInterfaces,
											const TEComResolverParams& aResolutionParameters, 
											TUid& aInstanceKey, 
											TAny* aCreationParameters, TBool aCreationParamsFlag)
	{
	return ResolveAndCreateImplL(EGetResolvedCreationMethod,
									  aInterfaceUid, 
									  aExtendedInterfaces,
									  aResolutionParameters, 
									  KNullUid, 
									  aInstanceKey,
									  aCreationParameters, aCreationParamsFlag);
	}
/**
Provides the instantiation method for an interface implementation 
that satisfies the specified interface.
Overload with a client provided CResolver.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aInterfaceUid A UID specifying the required interface.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@param			aInstanceKey A 32 bit instance key returned by the ECom framework that
				identifies the returned instance.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of 
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed implementation
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
TAny* REComSession::ResolveAndCreateImplL(TUid aInterfaceUid, 
											   const TEComResolverParams& aResolutionParameters, 
											   TUid aResolverUid, 
											   TUid& aInstanceKey,
											   TAny* aCreationParameters, TBool aCreationParamsFlag)
	{
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TAny* object = ResolveAndCreateImplL(EGetCustomResolvedCreationMethod,
									  aInterfaceUid, 
									  extendedInterfaces,	
									  aResolutionParameters, 
									  aResolverUid, 
									  aInstanceKey,
									  aCreationParameters, aCreationParamsFlag);
	CleanupStack::PopAndDestroy(&extendedInterfaces);									  
	return object;
	}

/**
Provides the instantiation method for an interface implementation 
that satisfies the specified interface.
Overload with a client provided CResolver.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
				Must match all extended interfaces for a match to occur.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@param			aInstanceKey A 32 bit instance key returned by the ECom framework that
				identifies the returned instance.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of 
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed implementation
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
TAny* REComSession::ResolveAndCreateImplL(TUid aInterfaceUid, 
											   const RExtendedInterfacesArray& aExtendedInterfaces,
											   const TEComResolverParams& aResolutionParameters, 
											   TUid aResolverUid, 
											   TUid& aInstanceKey,
											   TAny* aCreationParameters, TBool aCreationParamsFlag)
	{
	
	return ResolveAndCreateImplL(EGetCustomResolvedCreationMethod,
									  aInterfaceUid, 
									  aExtendedInterfaces,
									  aResolutionParameters, 
									  aResolverUid, 
									  aInstanceKey,
									  aCreationParameters, aCreationParamsFlag);
	}

/**
Provides the instantiation method for an interface implementation 
that satisfies the specified interface.
Overload for internal use to ensure consistency between calls.
This is the method which actually makes the IPC call.

@leave			KErrNoMemory
@leave			KErrNotConnected
@leave			KErrPermissionDenied
@param			aServiceId An integer specifying the ECom service variant to use.
@param			aInterfaceUid A UID specifying the required interface.
@param			aExtendedInterfaces Identifies a set of zero or more extended interfaces to match.
				Must match all extended interfaces for a match to occur.
@param			aResolutionParameters Specifies any additional implementation 
                characteristics to be fulfilled, maybe empty.
@param			aResolverUid The Uid of a CResolver with client defined behaviour.
@param			aInstanceKey A 32 bit instance key returned by the ECom framework that
				identifies the returned instance.
@param			aCreationParameters A pointer to the creation parameter
				structure passed to the creation method when called.
@param			aCreationParamsFlag A boolean flag to indicate the existence or non-existence
				of aCreationParameters. Will be ETrue even for if the value of 
				aCreationParameters is NULL.
@return			TAny* a pointer to the fully constructed implementation			
@post			The requested interface implementation is identified,
				and the instantiation method pointer is returned.
@see TEComResolverParams
*/
TAny* REComSession::ResolveAndCreateImplL(TInt aServiceId,
											   TUid aInterfaceUid, 
											   const RExtendedInterfacesArray& aExtendedInterfaces,
											   const TEComResolverParams& aResolutionParameters, 
											   TUid aResolverUid, 
											   TUid& aInstanceKey,
											   TAny* aCreationParameters, TBool aCreationParamsFlag)
	{
	// Package the parameters
	// 1st argument. Use the UidType because it will hold our 3 possible UIDs 
	// (aInterfaceUid, aInstanceKey and aResolverUid)
	// neatly, and save space in the message parameter passing structure.
	TUidType uidStore(aInterfaceUid, aInstanceKey, aResolverUid);
	TPckg<TUidType> uids(uidStore);
    // Store the arguments, note the data type might ba an invalid descriptor
    // beacuse the client caller may have used a default constructed 
    // TEComResolverParams. Do not access the descriptor data here. The server
    // does not access the data as it checks the size supplied below and 
    // creates a zero length descriptor in such cases.
	
	// 2nd argument which includes lenOfmatchString(TInt), matchString, 
	// numOfExtendedInterfaces(TInt) and extended Interfaces
	RBuf8 matchStrExtInfBuf;
	PackMatchStrAndExtendedInterfacesL(aResolutionParameters,aExtendedInterfaces,matchStrExtInfBuf);
	matchStrExtInfBuf.CleanupClosePushL();	
	
	// 3rd argument 
	TInt matchType = aResolutionParameters.IsGenericMatch();

	// 4th argument
	TEntry dllInfo;
	TPckg<TEntry> dllPkg(dllInfo);

// And call the server
	User::LeaveIfError(SendReceive(aServiceId, TIpcArgs(&uids,&matchStrExtInfBuf,matchType,&dllPkg)));
	CleanupStack::PopAndDestroy(&matchStrExtInfBuf);
// delegate to LoadManager to instantiate the object
	CGlobalData* globalData = CGlobalData::Instance();
	TAny* object=(TAny*)globalData->LoadManager().ImplementationObjectL(uidStore[KDtorKeyUidIndex], 
																		dllInfo,
																		aCreationParameters,
																		aCreationParamsFlag,
																		aInstanceKey);
	return object;
	}


/**
This method is provided for the internal testing of the ECOM component.
@param aArgs Arguments used for sending and receiving parameters
@leave ... One of the system-wide error codes		 
@internalComponent
*/
EXPORT_C void REComSession::SetGetParametersL(const TIpcArgs &aArgs)
	{
	// Open a local instance for the
	// interface creation, because this is a static
	// method and we may be not created yet.
	
	// call FinalClose() in the event of a leave
	CleanupStack::PushL(TCleanupItem(CleanupFinalClose, NULL));

	// NOTE: This will connect to ECom
	// using the global instance which is returned.
	REComSession& ecomSession = REComSession::OpenLC();	

	TInt error = ecomSession.SendReceive(ESetGetParameters, aArgs);
	if(error != KErrNone)
		{
		// There was a problem on the server side
		__ECOM_TRACE1("ECOM: Request error %d.", error);
		User::Leave(error);
		}
	
	CleanupStack::PopAndDestroy(&ecomSession);
	CleanupStack::Pop(); // CleanupFinalClose
	}


