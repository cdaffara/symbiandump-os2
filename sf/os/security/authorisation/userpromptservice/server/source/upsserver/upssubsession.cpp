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
* Implements CUpsSession.	 See class and function definitions for
* more information.
*
*/


/**
 @file
*/

#include "upsserver.h"
#include "authoriser.h"
#include <ups/upserr.h>

namespace UserPromptService
{

CUpsSubsession* CUpsSubsession::NewL(CUpsSession &aSession, const RMessage2& aMessage)
/**
	Factory function allocates a new, initialized instance of CUpsSubsession.

	@param	aMessage		Standard server-side handle to message.
	@return					New, initialized instance of CUpsSubsession which
							is owned by the caller.
 */
	{
	CUpsSubsession* self = new(ELeave) CUpsSubsession(aSession);
	// Note that CUpsSubsession ulitmately derives from CObject and therefore it MUST NOT be deleted directly,
	// instead it should be closed if we leave. 
	// nb. CUpsSession does NOT derive from CObject...
	CleanupClosePushL(*self);
	self->ConstructL(aMessage);
	CleanupStack::Pop(self);
	return self;
	}

CUpsSubsession::CUpsSubsession(CUpsSession &aSession)
/**
	This private constructor prevents direct instantiation and provides
	a single point of definition from which to call the superclass c'tor.
 */
:	CScsSubsession(aSession)
	{
	// empty.
	//RDebug::Printf("0x%x CUpsSubsession(session %x)\n", this, &aSession);
	}

void CUpsSubsession::ConstructL(const RMessage2& aMessage)
/**
	Initialize this subsession object by opening a handle to the
	thread whose identifier has been sent.

	@param	aSession		Ref to session creating us
	@param	aMessage		Standard server-side handle to message.
 */
	{
	// ARGS: TThreadId, TProcessId

	TPckg<TThreadId> tidBuf(iClientTid);
	aMessage.ReadL(0, tidBuf);

	TPckg<TProcessId> pidBuf(iClientPid);
	aMessage.ReadL(1, pidBuf);
	}

CUpsSubsession::~CUpsSubsession()
/**
	Close this object's handle to the SS client thread.
 */
	{
	//RDebug::Printf("0x%x ~CUpsSubsession()\n", this);
	iDestination.Close();
	iOpaqueData.Close();
	}

TBool CUpsSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSubsession by handling the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.
	@return ETrue means complete client request now.
 */
	{
	UserPromptService::TSubsessionFunction f =
		static_cast<UserPromptService::TSubsessionFunction>(aFunction);
	//RDebug::Printf("0x%x CUpsSubsession::DoServiceL function %d\n", this, f);
	switch (f)
		{
	case UserPromptService::ESubsessPreparePrompt:
		PreparePromptL(aMessage);
		break;

	case UserPromptService::ESubsessExecutePrompt:
		ExecutePromptL(aMessage);
		return EFalse; // If ExecutePrompt returns, instead of leaving, it must have setup an async req
	BULLSEYE_OFF
	default:
		User::Leave(KErrNotSupported);
		break;
	BULLSEYE_RESTORE
		}
	return ETrue;
	}

void CUpsSubsession::PreparePromptL(const RMessage2& aMessage)
	/**
	   Save service, description, and opaque data for use in the
	   following execute prompt command.
	*/
	{
	// TIpcArgs is TServiceId aServiceId, const TDesC* aDestination, const TDesC8* aOpaqueData
	
	iServiceId.iUid = aMessage.Int0();

	// Get Description
	TInt destinationLength = aMessage.GetDesLengthL(1);
	iDestination.Close();
	iDestination.CreateL(destinationLength);
	aMessage.ReadL(1, iDestination);

	// Get Opaque Data
	TInt opaqueDataLength = aMessage.GetDesLengthL(2);
	iOpaqueData.Close();
	if(opaqueDataLength)
		{
		iOpaqueData.CreateL(opaqueDataLength);
		aMessage.ReadL(2, iOpaqueData);
		}
	}

void CUpsSubsession::ExecutePromptL(const RMessage2& aMessage)
	/**
	  Create and start the CAuthoriser to process the request.
	*/
	{
	// TIpcArgs is OUT:TUpsDecision& aDecision, IN:TBool aServerCheckOk

	// The authorizer object is derived from CAsyncRequest and its
	// lifecycle is automatically managed by the SCS framework
	//
	// iDestination and iOpaqueData are transfered to the CAuthoriser,
	// our handles will be closed.
	TBool serverCheckOk = aMessage.Int1();
	CUpsSession *session = static_cast<CUpsSession*>(&iSession);
	RPolicyCacheCountedHandle &cacheManager = session->UpsServer()->iPolicyCache;
	CleanupReleasePushL(cacheManager);
	if(!cacheManager.IsOpen())
		{
		cacheManager.OpenL();
		}
	CAuthoriser *authoriser = CAuthoriser::NewL(cacheManager,
												session, this, serverCheckOk,
												iClientTid, iClientPid,
												aMessage, iServiceId, iDestination, iOpaqueData);
	CleanupStack::Pop(&cacheManager); // transfered ownership to the new CAuthoriser
	CleanupStack::PushL(authoriser);
	authoriser->TransferToScsFrameworkL();
	CleanupStack::Pop(authoriser); // authoriser now owned by SCS framework

	/**
	   The authoriser is now responsible for completing the request,
	   so we must NOT leave.
	   
	   We could start the request processing off by calling an
	   authoriser function from within a TRAP handler, but for future
	   proofing we tell the authoriser to self complete so the
	   processing all happens within the active scheduler framework
	   and the authoriser state machine. This will make it much easier
	   to completly restart request processing (if we decide to when
	   policies are changed).
	*/
	authoriser->Wakeup();
	}


} // End of namespace UserPromptService
// End of file
