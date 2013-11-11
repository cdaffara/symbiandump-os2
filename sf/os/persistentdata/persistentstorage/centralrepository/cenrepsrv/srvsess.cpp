// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "panic.h"
#include "srvreqs.h"
#include "sessmgr.h"
#include "cachemgr.h"
#include "srvsubsess.h"
#include "srvsess.h"

CServerSession::~CServerSession()
	{
	// Delete the subsession index.
	delete iSubSessionIx;
	
	//Delete the subsession container via the CObjectConIx's remove function  
	((CSessionManager*)Server())->RemoveContainer(iContainer);
	}

//Called by client/server framework after session has been successfully created.
//In effect, a second-phase constructor.
//Creates the object index and the object container for this session.
void CServerSession::CreateL()
	{
	// Create new object index
	iSubSessionIx = CObjectIx::NewL();
	
	// Initialize the object container
	// using the object container index in the server.
	iContainer = ((CSessionManager*)Server())->NewContainerL();
	}

//helper method to resolve and verify subsession using RMessage and subsession handle
CServerSubSession* CServerSession::SubSessionFromHandle(const RMessage2& aMessage, TInt aHandle)
	{
	CServerSubSession* subSession = (CServerSubSession*)iSubSessionIx->At(aHandle);
	if (subSession == NULL)
	    {
		PanicClient(EBadSubsessionHandle, aMessage);	    
	    }
	    
	return subSession;
	}

// if ServiceL Leaves, execution resumes in this method.
// this allows us to panic clients using bad descriptors, to deal with OOM conditions
// and to fail transactions with the correct reason: OOM etc.
void CServerSession::ServiceError(const RMessage2 &aMessage, TInt aError)
	{
	TServerRequest fn = static_cast<TServerRequest>(aMessage.Function());

	// If we have failed during initialisation the subsession is no longer available.
	// Perform additional cleanup by removing the subsession's handle from the server's 
	// subsession index. 
	if (fn == EInitialise)
		{
		// Retrieve handle
		TPckgBuf<TInt> handlePckg;
		TRAPD(res,aMessage.ReadL(3,handlePckg));
		 
		if (res == KErrNone)
		 	{
		 	TInt subSessionHandle = handlePckg();
			iSubSessionIx->Remove(subSessionHandle);
		 	}
		#ifdef _DEBUG
			else
				{
				RDebug::Print(_L("CServerSession::ServiceError - Can't remove subsession handle; aError = %d; res = %d"), aError, res); 
				}   	
		#endif 	 
		}
	
	//under following conditions the subsession handles the error
	if(fn > EInitialise && fn < ELastInTable)
		{
		CServerSubSession* subSession = SubSessionFromHandle(aMessage, aMessage.Int3());
		if(subSession)
			{
			subSession->ServiceError(aError);   
			}
		#ifdef _DEBUG
			else
				{
				RDebug::Print(_L("CServerSession::ServiceError - bad subsession handle. aError = %d"), aError); 
				}   	
		#endif  
		}
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
	//for multi ROFS instead of leaving we panic the client instead
	switch (aError)
		{
		case KErrMultiRofsOldCreUsed:
			PanicClient(EMultiRofsPanicOldCre,aMessage);
			return;
		case KErrMultiRofsGlobalOverride:
			PanicClient(EMultiRofsPanicGlobalOverride,aMessage);
			return;
		case KErrMultiRofsTypeOverride:
			PanicClient(EMultiRofsPanicTypeOveride,aMessage);
			return;
		case KErrMultiRofsIllegalRofs:
			PanicClient(EMultiRofsPanicIllegalRofs,aMessage);
			return;				
		}
#endif				
		
	CSession2::ServiceError(aMessage, aError);
	}

void CServerSession::ServiceL(const RMessage2& aMessage)
	{
	TServerRequest fn = static_cast<TServerRequest>(aMessage.Function());
	
#if defined(__CENTREP_SERVER_PERFTEST__) || defined(__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
	if (fn == EGetSetParameters)
		{
		TInt r = GetSetParameters(aMessage);
		aMessage.Complete(r);
		return;
		}
#endif

	if(fn > ELastInTable)
		{
		PanicClient(EBadMessageNumber, aMessage);
		}
	
	CServerSubSession* subSession = NULL;

#if defined (SYMBIAN_CENTREP_SUPPORT_MULTIROFS) && defined(CENTREP_TRACE)
	TUint32 startTick=0;
	TUid repUid;
	startTick=User::FastCounter();
#endif
	PERF_TEST_EVENT_START(subSession, aMessage);

	if(fn == EInitialise)
		{
		//create subsession
		subSession = NewSubSessionL(aMessage);
#if defined (SYMBIAN_CENTREP_SUPPORT_MULTIROFS) && defined(CENTREP_TRACE)		
		repUid=TUid::Uid(aMessage.Int0());
		subSession->iRepositoryUid=repUid;
#endif		
		}
	else
		{
		subSession = SubSessionFromHandle(aMessage, aMessage.Int3());
#if defined (SYMBIAN_CENTREP_SUPPORT_MULTIROFS) && defined(CENTREP_TRACE)
		repUid=subSession->iRepositoryUid;
#endif		
		}

	if(subSession)
		{
		
		TInt r = KErrNone;
		
		if(fn == EClose)
			{
			//delete subsession	
			DeleteSubSession(aMessage.Int3());
			}
		else
			{
			//ask subsession to handle the message
			r = subSession->ServiceL(aMessage);
			}

		PERF_TEST_EVENT_END(subSession, aMessage);
	
#if defined (SYMBIAN_CENTREP_SUPPORT_MULTIROFS) && defined(CENTREP_TRACE)
		TUint32 endTick=User::FastCounter();
		RDebug::Print(_L("[CENTREP],TimeStamp=,%d,Repository=,%x,Function=,%d,TickCount=,%d"),startTick,repUid.iUid,fn,endTick-startTick);
#endif
		if (r != CServerSubSession::KDontCompleteMessage)
			{
			aMessage.Complete(r);
			}
		}
	//If (subsession == NULL) we don't need to complete the message, as the message is completed already when the client panicked
	#ifdef _DEBUG   
		else
			{
			RDebug::Print(_L("CServerSession::ServiceL - bad subsession handle. TServerRequest = %d"), fn); 
			}
	#endif
	}

//Creates a new subsession object, and writes its handle to the message.
//A subsession object is the server side "partner" to the client side subsession.
//On return last parameter of aMessage is filled with the handle of the subsession
//and handle is also returned
CServerSubSession* CServerSession::NewSubSessionL(const RMessage2& aMessage)
	{
	//create a new subsession
	CServerSubSession* subSession = new (ELeave) CServerSubSession(this);
	CleanupStack::PushL(subSession);
	
	//add the subsession object to this session's object container to generate a unique ID
	iContainer->AddL(subSession);
	CleanupStack::Pop(subSession);
	
	//add the object to the subsessions index, this returns a unique handle so that we can
	//refer to the object later
	TInt handle = iSubSessionIx->AddL(subSession);
	
	//write the handle to the client's message
	TPckgBuf<TInt> handlePckg(handle);
	TRAPD(res,aMessage.WriteL(3,handlePckg));
	if (res!=KErrNone)
		{
		iSubSessionIx->Remove(handle);
		PanicClient(EBadSubsessionHandle, aMessage);
		subSession = NULL;
		}

	return subSession;
	}
	
//Deletes a subsession object through its handle.
void CServerSession::DeleteSubSession(TInt aHandle)
	{
	iSubSessionIx->Remove(aHandle);
	}
	
inline CSessionManager* CServerSession::Server()
	{
	return static_cast<CSessionManager*>(const_cast<CServer2*>(CSession2::Server()));
	}

#if defined(__CENTREP_SERVER_PERFTEST__) || defined (__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
// GetSetParameters
// The function code EGetSetParameters is a generic msg reserved
// for testing purpose. Int0 specifies the function to perform.
TInt CServerSession::GetSetParameters(const TClientRequest& aMessage)
	{
	TServerGetSetParametersSubCmd cmd = static_cast<TServerGetSetParametersSubCmd>(aMessage.Int0());

#ifdef __CENTREP_SERVER_PERFTEST__
	if (cmd == EGetPerfResults)
		{
		TInt desSize = aMessage.GetDesMaxLength(1);
		TInt numVals = desSize / sizeof(TUint32);
		if (numVals < KCentRepPerfTestArraySize)
			{
			return KErrOverflow;
			}
		TPtrC8 p(reinterpret_cast<const TUint8*>(TServerResources::iPerfTestMgr.Entries()),
				   KCentRepPerfTestArraySize * sizeof(TUint32));
		TInt ret = aMessage.Write(1, p);
		if (ret == KErrNone)
			{
			TUint lastCompleteAccess = TServerResources::iPerfTestMgr.LastCompleteAccess();
			TPckg<TUint> p2(lastCompleteAccess);
			ret = aMessage.Write(2, p2);
			}
		return ret;
		}
	else if (cmd == ERestartPerfTests)
		{
		TServerResources::iPerfTestMgr.Reset();
		return KErrNone;
		}
	else if (cmd == EStopPerfTests)
		{
		TServerResources::iPerfTestMgr.Stop();
		return KErrNone;
		}
#endif // __CENTREP_SERVER_PERFTEST__

#ifdef __CENTREP_SERVER_MEMTEST__
	if(cmd == ERestartMemTests)
		{
		TServerResources::StartRecordTimerResult();
		return KErrNone;
		}
	else if(cmd == ESingleMemTest)
		{
		RECORD_HEAP_SIZE(EMemLcnOnDemand, aMessage.Int1());
		return KErrNone;
		}
	else if(cmd == EGetMemResults)
		{
		TInt count = TServerResources::iMemTestDataCount;
		TPckg<TInt> pCount(count);
		
		TInt err = aMessage.Write(1, pCount);
		if(err == KErrNone && count > 0)
			{
			TPtrC8 pBuf(reinterpret_cast<TUint8*>(TServerResources::iMemTestData), (TServerResources::iMemTestDataCount)*sizeof(TInt32));
			err = aMessage.Write(2, pBuf);
			}
		// Stop recording results
		TServerResources::StopRecordTimerResult();
		return err;
		}
#endif // __CENTREP_SERVER_MEMTEST__

#ifdef __CENTREP_SERVER_CACHETEST__
	if (cmd == EEnableCache)
		{
		// First parameter is Timer Interval, second is cache size
		TServerResources::iCacheManager->EnableCache(aMessage.Int1(), aMessage.Int2());
		return KErrNone;
		}
	else if (cmd == EDisableCache)
		{
		TServerResources::iCacheManager->DisableCache(ETrue);
		return KErrNone;
		}
#endif  // __CENTREP_SERVER_CACHETEST__

	return KErrNotSupported;
	}
#endif // __CENTREP_SERVER_PERFTEST__ || __CENTREP_SERVER_MEMTEST__ || __CENTREP_SERVER_CACHETEST__

