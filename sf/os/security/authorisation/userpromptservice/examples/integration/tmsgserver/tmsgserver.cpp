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
* Example code
*
*/


#include "tmsgcommon.h"
#include "tmsgserver.h"
#include <e32cmn.h>
#include <e32debug.h>

inline CShutdown::CShutdown() :CTimer(-1) {CActiveScheduler::Add(this);}
inline void CShutdown::ConstructL() {CTimer::ConstructL();}
inline void CShutdown::Start() {After(KMyShutdownDelay);}

void CShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}


// CMsgServer ****************************************************************
inline UserPromptService::RUpsSession& CMsgServer::Ups() {return iUps;}

const TInt CMsgServer::iRanges[KPolicyRanges] =
	{
	ESendMsg,			// network services requried
	ECancelSendMsg,		// no capability needed to request
	ESeperatorEndAll
	};

const TUint8 CMsgServer::iElementsIndex[KPolicyRanges] = 
	{
	0,								// capability check for networkservices
	EAlwaysPass,					// no check needed to cancel a request
	CPolicyServer::ENotSupported	
	};

static const TInt KMsgServerCustomFailureAction = -1;

const CPolicyServer::TPolicyElement CMsgServer::iPolicyElements[KPolicyElements] =
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices), KMsgServerCustomFailureAction}
	};

const CPolicyServer::TPolicy CMsgServer::iPolicy =
	{
	CPolicyServer::EAlwaysPass, // so that any client can connect	
	KPolicyRanges,									 
	iRanges,
	iElementsIndex,
	iPolicyElements,
	};


CMsgServer::CMsgServer()
	: CPolicyServer(EPriorityStandard, iPolicy, ESharableSessions)
	{
	}

CMsgServer::~CMsgServer()
	{
	iUps.Close();
	}

CMsgServer* CMsgServer::NewLC()
	{
	CMsgServer* self = new(ELeave) CMsgServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CMsgServer::ConstructL() 
	{
	// Create the RUpsSession
	// A sub-session is created for each client
	User::LeaveIfError(iUps.Connect());

	StartL(KMsgServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	}

CPolicyServer::TCustomResult CMsgServer::CustomFailureActionL(const RMessage2& aMessage, TInt /*aAction*/, const TSecurityInfo& /*aMissing*/)
/** This function is only invoked for requests that allow UPS authorisation. The 
 * message may be rejected later if the user denies the request.
 */
	{
	CSession2* s = aMessage.Session();
	__ASSERT_ALWAYS(s != NULL, User::Invariant());
	CMsgServerSession* session = static_cast<CMsgServerSession*>(s);
	session->SetPlatsecResult(EFalse);	
	return CPolicyServer::EPass;	// The request may be rejected later by the UPS
	}

CSession2* CMsgServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	TVersion v(KMsgServerVersionMajor, KMsgServerVersionMinor, KMsgServerVersionBuild);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);		
		}
	return new(ELeave) CMsgServerSession();
	}

void CMsgServer::AddSession()
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

void CMsgServer::DropSession()
	{
	if (--iSessionCount==0)	iShutdown.Start();
	}

void PanicClient(const RMessagePtr2& aMessage,TMsgServerPanic aPanic)
	{
	aMessage.Panic(KMsgServerName,aPanic);
	}

// CMsgServerSession *********************************************************
inline CMsgServer& CMsgServerSession::Server()
{return *static_cast<CMsgServer*>(const_cast<CServer2*>(CSession2::Server()));}

CMsgServerSession::CMsgServerSession()
	: iPlatsecResult(ETrue)
	{
	}

CMsgServerSession::~CMsgServerSession()
	{
	delete iProcessor;	
	iAuth.Close();
	Server().DropSession();
	}

void CMsgServerSession::CreateL()
	{
	Server().AddSession();
	iProcessor = CMsgProcessor::NewL(iAuth);
	}

void CMsgServerSession::ServiceL(const RMessage2& aMessage)
	{
	if (! iAuthInitialised)
		{
		RThread client;
		aMessage.ClientL(client);
		CleanupClosePushL(client);
		User::LeaveIfError(iAuth.Initialise(Server().Ups(), client));
		CleanupStack::PopAndDestroy(&client);
		iAuthInitialised = ETrue;
		}	
	
	iProcessor->ProcessL(aMessage, iPlatsecResult);
	// Reset to 'passed' for next message because SetPlatsecResult is only ever called
	// when a check fails.
	iPlatsecResult = ETrue;	
	}

void CMsgServerSession::SetPlatsecResult(TBool aResult)
	{
	iPlatsecResult = aResult;
	}

// CMsgProcessor *********************************************************((((
CMsgProcessor* CMsgProcessor::NewL(UserPromptService::RUpsSubsession& aAuth)
	{
	CMsgProcessor* self = new(ELeave) CMsgProcessor(aAuth);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMsgProcessor::CMsgProcessor(UserPromptService::RUpsSubsession& aAuth)
	: CActive(EPriorityStandard), iState(EIdle), iAuth(aAuth)
	{
	CActiveScheduler::Add(this);
	}

void CMsgProcessor::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	}

CMsgProcessor::~CMsgProcessor()
	{	
	Deque();
	Reset();
	iTimer.Close();
	}

TInt CMsgProcessor::RunError(TInt aError)
	{
	if (iState != EIdle) iMessage.Complete(aError);

	Reset();
	return KErrNone;
	}

void CMsgProcessor::Reset() 
/**
 * Resets the processor for the next message.
 */
	{
	iMsgTo.Close();
	iMsgBody.Close();
	iState = EIdle;
	}

void CMsgProcessor::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	switch (iState)
		{
		case EAuthorising:			
			// Check whether the request was authorised before proceeding
			if (iDecision == EUpsDecSessionNo || iDecision == EUpsDecNo)
				{ // The user or UPS denied the client's request so fail with security error.				  
				iMessage.Complete(KErrPermissionDenied);
				Reset();
				}
			else
				{ // Request authorised so carry on as normal
				SendL();	
				}
			break;
		case ESending:						
			// Message send so complete request and return
			iMessage.Complete(KErrNone);
			Reset();
			break;
		default: // invalid state - should never happen
			__ASSERT_ALWAYS(EFalse, User::Invariant()); 
			break;
		}
	}

void CMsgProcessor::DoCancel()
	{
	switch (iState)
		{
		case EIdle:
			break; // nothing to do
		case EAuthorising:
			iAuth.CancelPrompt();			
			break;
		case ESending:
			iTimer.Cancel();
			break;
		default: // invalid state - should never happen
			__ASSERT_ALWAYS(EFalse, User::Invariant());
		}
	if (iState != EIdle) iMessage.Complete(KErrCancel);
	Reset();
	}

void CMsgProcessor::GetParamsL(const RMessage2& aMessage)
/**
 * Reads the parameters for sending a message. This data is also needed for the authorisation prompt
 */
	{
	TInt l = aMessage.GetDesLength(0);
	if (l <= 0) User::Leave(KErrArgument);
	iMsgTo.CreateL(l);
	aMessage.ReadL(0, iMsgTo);
	l = aMessage.GetDesLength(1);
	if (l <= 0) User::Leave(KErrArgument);
	iMsgBody.CreateL(l);
	aMessage.ReadL(1, iMsgBody);
	}

void CMsgProcessor::AuthoriseL(const RMessage2& aMessage, TBool aPlatsecResult)
/**
 * Implementation of the new authorisation state
 * @param	aMessage		The message handle containing the SendMsg parameters
 * @param	aPlatsecResult	Whether the client has the correct capabilities etc
 */
	{
	iState = EAuthorising;
	iPlatsecResult = aPlatsecResult;
	iMessage = aMessage;
	GetParamsL(iMessage);	// message parameters are used to identify the request 
	iAuth.Authorise(iPlatsecResult, TUid::Uid(KServiceId), iMsgTo, iDecision, iStatus);
	SetActive();
	}

void CMsgProcessor::SendL() 
/**
 * Simulatate the sending of a message by displaying the body
 * to the screen and waiting for three seconds.
 * 
 * No changes necessary to support user prompts.
 */
	{
	iState = ESending;
	HBufC* tmp = HBufC::NewMaxLC(iMsgTo.Length() + iMsgBody.Length() + 3);
	tmp->Des().Format(_L("%S: %S"), &iMsgTo, &iMsgBody);
	User::LeaveIfError(User::InfoPrint(*tmp));
	CleanupStack::PopAndDestroy(tmp);
	iTimer.After(iStatus, 3 * 1000 * 1000);
	SetActive();
	}

void CMsgProcessor::ProcessL(const RMessage2& aMessage, TBool aPlatsecResult)
	{
	switch (aMessage.Function())
		{
		case ESendMsg:
			if (iState == EIdle)
				{ // The first state is now authorisation				
				AuthoriseL(aMessage, aPlatsecResult);
				}
			else
				{ // Concurrent requests not supported
				aMessage.Complete(KErrInUse); 
				}
			break;
		case ECancelSendMsg:
			Cancel();
			aMessage.Complete(KErrNone);
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	}

static void RunServerL()
	{
	// normal server startup code
	User::LeaveIfError(RThread::RenameMe(KMsgServerName));
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);	
	CMsgServer::NewLC();
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, s); 
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}

