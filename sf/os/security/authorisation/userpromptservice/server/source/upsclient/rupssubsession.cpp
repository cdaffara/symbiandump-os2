/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* RUpsSubsession implementation.	See class and function definitions
* for more detail.
*
*/


/**
 @file
*/

#include <ups/upsclient.h>
#include "upsclientconfig.h"
#include "upscommon.h"
#include <ups/upserr.h>

namespace UserPromptService
	{


EXPORT_C RUpsSubsession::RUpsSubsession()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
:	RScsClientSubsessionBase(),
	iDecPtr(NULL, 0),
	iSubsessionCreated(EFalse), iClientTid(), iClientSid(TUid::Null()), iClientPid(0), iUpsSession(0)
	{
	// empty.
	}

EXPORT_C TInt RUpsSubsession::Initialise(RUpsSession& aSession, const RThread& aClient)
/**
Saves the details required to create a subsession specific to the client thread.

The actual subsession creation is defered until the subsession is required (which
may be never if the policy says server checks are sufficient).

Several RUpsSubsession objects can share a single RUpsSession.

If any of the RUpsSubsession objects are to be used in a different thread to the RUpsSession, then
ShareAuto() must be called on the RUpsSession.

@see RSubSessionBase::CreateSubsession

@param	aSession		Connected session to the UPS server.
@param	aClient			SS client for whom this session is set up.
@return					Symbian OS error code where KErrNone indicates success
						and any other value indicates failure.
*/
	{
	iClientTid = aClient.Id();
	iClientSid = aClient.SecureId();

	// Need to obtain the drive letter for the exe
	RProcess clientProcess;
	TInt r = aClient.Process(clientProcess);
	if(r != KErrNone)
				{
				return r; // Presumably it exited...
				}
	iClientPid = clientProcess.Id();
	clientProcess.Close();
	
	iUpsSession = &aSession;
	return KErrNone;
	}

TInt RUpsSubsession::CreateSubsession()
/**
	Create a subsession for a specific SS client over the supplied session.

	@param	aSession		Connected session to the UPS server.
	@param	aClient			SS client for whom this session is set up.
	@return					Symbian OS error code where KErrNone indicates success
							and any other value indicates failure.
	@capability ProtServ 
 */
	{
	__ASSERT_ALWAYS(iUpsSession, User::Panic(KUpsClientPanicCat, EUpsClientNotInitialised));
	TPckgBuf<TThreadId> tidBuf = iClientTid;
	TPckgBuf<TProcessId> pidBuf = iClientPid;
	TIpcArgs args(&tidBuf, &pidBuf);
	return RScsClientSubsessionBase::CreateSubsession(*iUpsSession, ESessSubsessFromThreadId, args);
	}

EXPORT_C void RUpsSubsession::Authorise(TBool aServerCheckOk,
										const TServiceId& aServiceId, const TDesC& aDestination,
										TUpsDecision& aDecision, TRequestStatus& aStatus)
/**
	Determines whether the system server should perform the service requested
	by the client application. Depending on licensee configuration this function
	will either complete immediately if the client passed the system servers
	security policy check. Alternatively, every request may require additional
	authorisation by the User Prompt Service.
	
	@see RUpsSubsession::AuthoriseInternal

	@param	aServerCheckOk	Whether the client request passed the security check
							implemented by the system server e.g. does the client
							have the correct capabilities for the requested service.
	@param	aServiceId		Service which the client wants to use.
	@param	aDestination	More information about the service, e.g. this
							could be a telephone number is the client wanted
							to make a call.
	@param	aDecision		When the request completes successfully, the verdict
							is written to this variable.
	@param	aStatus			The server completes this request object when it
							has finished handling the request.
	@capability ProtServ 
 */
	{
	__ASSERT_ALWAYS(iUpsSession, User::Panic(KUpsClientPanicCat, EUpsClientNotInitialised));

	AuthoriseInternal(aServerCheckOk, aServiceId, aDestination, KNullDesC8, aDecision, aStatus);
	}

EXPORT_C void RUpsSubsession::Authorise(TBool aServerCheckOk, 
										const TServiceId& aServiceId, const TDesC& aDestination, 
										const TDesC8& aOpaqueData, 
										TUpsDecision& aDecision, TRequestStatus& aStatus)
/**
	Determines whether the system server should perform the service requested
	by the client application. Depending on licensee configuration this function
	will either complete immediately if the client passed the system servers
	security policy check. Alternatively, every request may require additional
	authorisation by the User Prompt Service.
	
	@see RUpsSubsession::AuthoriseInternal

	@param	aServerCheckOk	Whether the client request passed the security check
							implemented by the system server e.g. does the client
							have the correct capabilities for the requested service.
	@param	aServiceId		Service which the client wants to use.
	@param	aDestination	More information about the service, e.g. this
							could be a telephone number is the client wanted
							to make a call.
	@param	aOpaqueData		Additional information to describe the request.
	@param	aDecision		When the request completes successfully, the verdict
							is written to this variable.
	@param	aStatus			The server completes this request object when it
							has finished handling the request.
	@capability ProtServ 
 */
	{
	__ASSERT_ALWAYS(iUpsSession, User::Panic(KUpsClientPanicCat, EUpsClientNotInitialised));

	AuthoriseInternal(aServerCheckOk, aServiceId, aDestination, aOpaqueData, aDecision, aStatus);
	}


void RUpsSubsession::AuthoriseInternal(
	TBool aServerCheckOk,
	const TServiceId& aServiceId,
	const TDesC& aDestination,
	const TDesC8& aOpaqueData,
	TUpsDecision& aDecision,
	TRequestStatus& aStatus)
/**
	This helper function is called by the exported overloads of Authorise.
	It sends the data supplied by the SS to the UPS server.

	@param	aServerCheckOk	Did the system server checks pass?
	@param	aServiceId		Service which the client wants to use.
	@param	aDestination	More information about the service, e.g. this
							could be a telephone number is the client wanted
							to make a call.
	@param	aOpaqueData		Additional information to describe the request.
							This is KNullDesC8 if the SS used the Authorise overload
							which did not take opaque data.
	@param	aDecision		When the request completes successfully, the verdict
							is written to this variable.
	@param	aStatus			The server completes this request object when it
							has finished handling the request.
	@capability ProtServ 
 */
	{
	TBool decided = EFalse;
	TInt err = KErrNone;	

	__ASSERT_ALWAYS(iUpsSession, User::Panic(KUpsClientPanicCat, EUpsClientNotInitialised));
	CUpsClientConfig::TQueryUpsResult result = iUpsSession->iClientConfig->QueryUps(aServerCheckOk, aServiceId, iClientSid, iClientPid);
	switch(result)
		{
		case CUpsClientConfig::EAllow:
			decided = ETrue;
			aDecision = EUpsDecYes;
			break;

		case CUpsClientConfig::EQueryUps:
			break;

		case CUpsClientConfig::EReject:
			decided = ETrue;
			aDecision = EUpsDecNo;
			break;
		BULLSEYE_OFF
		default:
			break;	
		}
		BULLSEYE_RESTORE
			
	if(!decided && !iSubsessionCreated)
		{
		// We need to query the UPS, but have not created the subsession yet.
		err = CreateSubsession();
		if(err != KErrNone)
			{
			aDecision = EUpsDecNo;
			decided = ETrue;
			}
		else
			{
			iSubsessionCreated = ETrue;
			// aDecision will be set when we query the UPS
			}
		}

	if(decided)
		{
		// Either do not need to query UPS, or an error occured, so complete client
		// and return
		aStatus = KRequestPending;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		return; // Either do not need to query UPS, or an error occured, so return
		}

	//
	// Query UPS
	//
	// only three arguments can be sent to a subsession, so split
	// this operation into two stages.

	TInt r = PreparePrompt(aServiceId, aDestination, aOpaqueData);
	if (r != KErrNone)
		{
		TRequestStatus* prs = &aStatus;
		User::RequestComplete(prs, r);
		return;
		}
	
	ExecutePrompt(aServerCheckOk, aDecision, aStatus);
	}

TInt RUpsSubsession::PreparePrompt(const TServiceId& aServiceId, const TDesC& aDestination, const TDesC8& aOpaqueData)
/**
	Ask the UPS server to prepare to make a decision.  This will be followed
	by a call to ExecutePrompt.

	@param	aServiceId		Service which the client wants to use.
	@param	aDestination	More information about the service, e.g. this
							could be a telephone number is the client wanted
							to make a call.
	@param	aOpaqueData		Additional information to describe the request.
							This is KNullDesC8 if the SS used the Authorise overload
							which did not take opaque data.
	@return					Error code with which the server completed the request.
	@capability ProtServ 
	@see ExecutePrompt
 */
	{
	TIpcArgs args(aServiceId.iUid, &aDestination, &aOpaqueData);
	return CallSubsessionFunction(ESubsessPreparePrompt, args);
	}

void RUpsSubsession::ExecutePrompt(TBool aServerCheckOk, TUpsDecision& aDecision, TRequestStatus& aStatus)
/**
	Ask the UPS to execute the request which was set up with PreparePrompt.

	@param	aDecision		When the request completes successfully, this argument
							is updated with the verdict.
	@param	aStatus			The server completes this request object when it
							has finished handling the request.
	@capability ProtServ 
	@see PreparePrompt
 */
	{
	TUint8* decPtr = reinterpret_cast<TUint8*>(&aDecision);
	iDecPtr.Set(decPtr, sizeof(TUpsDecision), sizeof(TUpsDecision));
	
	TIpcArgs args(&iDecPtr, aServerCheckOk);
	CallSubsessionFunction(ESubsessExecutePrompt, args, aStatus);
	}

EXPORT_C void RUpsSubsession::CancelPrompt()
/**
	Cancel the prompt request which was launched by calling Authorise.

	This function has no effect if there is no outstanding request.
	@capability ProtServ 
 */
	{
	if(iSubsessionCreated)
		{
		CancelSubsessionFunction(ESubsessExecutePrompt);
		}
	}

EXPORT_C void RUpsSubsession::Close()
	/**
	   Close and clean up this subsession object.
	*/
	{
	if(iSubsessionCreated)
		{
		RScsClientSubsessionBase::Close();
		}
	}



} // End of namespace UserPromptService

// End of file
