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
// Server session object implementation.
// 
//

#include "EComServerStart.h"
#include "EComMessageIds.h"
#include <ecom/ecomerrorcodes.h>
#include <ecom/implementationinformation.h>
#include "EComServerSession.h"
#include "EComSessionAux.h"
#include "EComServer.h"
#include "TestUtilities.h"	// For __FILE__LINE__
#include "EComPerformance.h"
#include "EComDebug.h"

//
// RMessage::Panic() completes the message. This is:
// (a) important for efficient cleanup within the kernel.
// (b) a problem if the message is completed a second time.
//
void PanicClient(const TClientRequest& aMessage, TInt aPanicCode)
	{
	aMessage.Panic(KEComServerPanicCategory, aPanicCode);
	}

//
//	class CEComServerSession
//
CEComServerSession::CEComServerSession()
	: CSession2()
	{
	// Do nothing
	}

//
// 2nd phase construct for sessions - called by the CServer framework
//
void CEComServerSession::CreateL()
	{
	Server().AddSession();
	}

CEComServerSession::~CEComServerSession()
	{
	CleanupInternalList();
	CompleteNotifications(KErrCancel);
	delete iMemoryStore;
	Server().DropSession();
	}

//
// Deliver the notification message to the client
//
void CEComServerSession::CompleteNotifications(TInt aCompletionCode)
	{
	const TInt count = iNotificationRequests.Count();
	for(TInt i = 0; i < count; ++i)
		{
		  iNotificationRequests[i].iMessage.Complete(aCompletionCode);

		}
	iNotificationRequests.Reset();
	}

//
// Handle a client request.
// Leaving is handled by CEComServer::RunError() which reports the error code
// to the client.
//
void CEComServerSession::ServiceL(const RMessage2& aMessage)
{
	const TClientRequest msg(aMessage);
	ServiceL(msg);
}

void CEComServerSession::ServiceL(const TClientRequest& aMessage)
	{
	TInt completionCode = KErrNone;
	TBool asyncRequest = EFalse;

	switch (aMessage.Function())
		{
	case ENotifyOnChange:
		{
RECORD_CLIENT_REQUEST_START_TIMER_RESULT(EEComNotifyOnChangeRequestType, Server().GetCurrentStartupState());
		SEComNotification notif;
		notif.iMessage=aMessage;
		// the TRequestStatus as a TInt is stored for later comparisons
		notif.iRequestStatusHandle=aMessage.Int0();
		//Check that this TRequestStatus is not already being used.
		const TInt count = iNotificationRequests.Count();
		for(TInt i = 0; i < count; ++i)
		{
			if(iNotificationRequests[i].iRequestStatusHandle == notif.iRequestStatusHandle)
				User::Leave(KErrArgument);

		}
		User::LeaveIfError(iNotificationRequests.Append(notif));
		asyncRequest = ETrue;
RECORD_CLIENT_REQUEST_END_TIMER_RESULT;
		break;
		}

	case ECancelNotifyOnChange:
			{
RECORD_CLIENT_REQUEST_START_TIMER_RESULT(EEComCancelNotifyOnChangeRequestType, Server().GetCurrentStartupState());
			TInt statusHandle = aMessage.Int0();

			const TInt count = iNotificationRequests.Count();
			for(TInt i = 0; i < count; ++i)
				{
				if(iNotificationRequests[i].iRequestStatusHandle == statusHandle)
					{
						iNotificationRequests[i].iMessage.Complete(KErrCancel);
						iNotificationRequests.Remove(i);
						break;  // Terminate the loop
					}
				}
			}
RECORD_CLIENT_REQUEST_END_TIMER_RESULT;
		break;

	case EListImplementations:
	case EListResolvedImplementations:
	case EListCustomResolvedImplementations:
		if(Server().RegistryIndexValid())
			{
RECORD_CLIENT_REQUEST_START_TIMER_RESULT(EEComListRequestType, Server().GetCurrentStartupState());
			DoListImplementationsL(aMessage);
RECORD_CLIENT_REQUEST_END_TIMER_RESULT;
			}
		else
			{
			if(ReceivePending())
				User::Leave(KEComErrListInvalidAwaitNotification);
			else
				User::Leave(KEComErrListCurrentlyUnavailable);
			}
		break;

	case ECollectImplementationsList:
RECORD_CLIENT_REQUEST_START_TIMER_RESULT(EEComCollectImplementationsRequestType, Server().GetCurrentStartupState());
		if(!DoCollectListL(aMessage))
			completionCode = KErrNotReady;
RECORD_CLIENT_REQUEST_END_TIMER_RESULT;
		break;

	case EGetImplementationCreationMethod:
	case EGetResolvedCreationMethod:
	case EGetCustomResolvedCreationMethod:
RECORD_CLIENT_REQUEST_START_TIMER_RESULT(EEComCreateRequestType, Server().GetCurrentStartupState());
		DoGetResolvedImplementationL(aMessage);
RECORD_CLIENT_REQUEST_END_TIMER_RESULT;
		break;

	case EListExtendedInterfaces:
RECORD_CLIENT_REQUEST_START_TIMER_RESULT(EEComListExtendedInterfacesRequestType, Server().GetCurrentStartupState());
		DoListExtendedInterfacesL(aMessage);
RECORD_CLIENT_REQUEST_END_TIMER_RESULT;
		break;

	case EEnableImplementation:
	case EDisableImplementation:
		// Ommissions for 6.2
		completionCode = KErrNotSupported;
		break;
#if defined(__ECOM_SERVER_TESTABILITY__) || defined(__ECOM_SERVER_PERFORMANCE__)
	case ESetGetParameters:
		DoSetGetParametersL(aMessage);
		break;
#endif
	//EDestroyedImplementation obsolete due to implementation creation
	//relocation to client side from server
	case EDestroyedImplementation:
	default:
		// Something badly wrong if we get here.
		PanicClient(aMessage,KEComErrUnknownService);
		// RMessage::Panic() has completed the message
		// so treat this as an asynch request.
		asyncRequest = ETrue;
		}
	if (!asyncRequest)
		aMessage.Complete(completionCode);
	}



	
TInt const KDefaultStoreSize = 256;	// Enough space for approx 1 implementation : will grow to fit if required

/**
UnPack the match string and extended interface from the client supplied parameters.
@param			aMessage 
@param			aExtendedInterfaces Return value consisting of an array containing the extended interfaces.
@param			aMatchStr Return value consisting of the matching string.
*/
void CEComServerSession::UnpackMatchStrAndExtendedInterfacesFromClientL(const TClientRequest& aMessage,
													RExtendedInterfacesArray& aExtendedInterfaces,
													RBuf8& aMatchStr)
	{
	
	//now get the matchString and extendedInterfaces
	TInt sizeOfMatchStrExtInfBuf = aMessage.GetDesLength(KIPCParameterMatchStrExtInf);
	User::LeaveIfError(sizeOfMatchStrExtInfBuf);
	RBuf8 matchStrExtInfBuf;
	matchStrExtInfBuf.CreateMaxL(sizeOfMatchStrExtInfBuf);
	matchStrExtInfBuf.CleanupClosePushL();
	
	aMessage.ReadL(KIPCParameterMatchStrExtInf,matchStrExtInfBuf);
	RDesReadStream readStream;
	CleanupClosePushL(readStream);
	readStream.Open(matchStrExtInfBuf);
	TInt lenOfMatchStr = readStream.ReadInt32L();
	aMatchStr.CreateMaxL(lenOfMatchStr);
	aMatchStr.CleanupClosePushL();
	if (lenOfMatchStr>0)
		{
		readStream.ReadL(aMatchStr,lenOfMatchStr);		
		}
	TInt numOfExtendedInterfaces = readStream.ReadInt32L();
	CleanupClosePushL(aExtendedInterfaces);
	for(TInt i = 0; i < numOfExtendedInterfaces; i++)
		{
		aExtendedInterfaces.AppendL(TUid::Uid(readStream.ReadInt32L()));
		}
	
	CleanupStack::Pop(&aExtendedInterfaces);
	CleanupStack::Pop(&aMatchStr);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&matchStrExtInfBuf); 
	}


// Note that this method for returning the arbitrary sized data set 
// will not work IF the session is shared so...
// DO NOT SHARE SERVER SIDE SESSIONS BETWEEN CLIENTS
void CEComServerSession::DoListImplementationsL(const TClientRequest& aMessage)
	{
	// Unpack the client supplied parameters
	// Firstly get the uids
	TUidType uids;
	TPckg<TUidType> uidsPkg(uids);	
	aMessage.ReadL(KIPCParameterUids, uidsPkg);
	
	if(uids[KInterfaceUidIndex] == KNullUid)
		{
		User::Leave(KEComErrMissingParameter);
		}

	//now get the TListImplParam parameters
  	TListImplParam listParam;
  	TPckg<TListImplParam> listParamPkg(listParam);
  	aMessage.ReadL(2,listParamPkg);
	
	// Now rebuild the TEComResolverParams
	TEComResolverParams resolverParameters;
	resolverParameters.SetGenericMatch(listParam.iMatchType);
	
	//now get the matchString and extendedInterfaces
	RBuf8 matchStr;
	RExtendedInterfacesArray extendedInterfaces;
	UnpackMatchStrAndExtendedInterfacesFromClientL(aMessage,extendedInterfaces,matchStr);
	matchStr.CleanupClosePushL();
	CleanupClosePushL(extendedInterfaces);
	
	if(matchStr.Length()>0)
		{	
		resolverParameters.SetDataType(matchStr);
		}
	// Else the client's resolver params are default constructed i.e. invalid
	// data type descriptor or its length is zero, so use empty RBuf8 above.
	
		
	// Pass to the server
	iListContext = aMessage.Function();
	switch(iListContext)
		{
		case EListImplementations:
			iList = Server().ListImplementationsL( uids[KInterfaceUidIndex], extendedInterfaces, aMessage );
			break;
		case EListResolvedImplementations:
			if(matchStr.Length() == 0)
				{
				User::Leave(KEComErrMissingParameter);
				}
			iList = Server().ListImplementationsL( uids[KInterfaceUidIndex], resolverParameters, extendedInterfaces, aMessage);
			break;
		case EListCustomResolvedImplementations:
			if(uids[KResolverUidIndex] == KNullUid)
				{
				User::Leave(KEComErrMissingParameter);
				}
			iList = Server().ListImplementationsL( uids[KInterfaceUidIndex], resolverParameters, uids[KResolverUidIndex], extendedInterfaces, aMessage);
			break;
		default:
			break;
		}
	//Now cleanup the extended interface
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	CleanupStack::PopAndDestroy(&matchStr);

	TInt bufferSizeRequired=0;
	// Package the array for return
	if(iList)
		{
		if(iList->Count()>0)
			{
			// Allocate a new store and replace the old one first
			CBufFlat* memoryStore = CBufFlat::NewL(KDefaultStoreSize);
			delete iMemoryStore;
			iMemoryStore = memoryStore;

			// Note : There is no need to push
			// the write stream onto the cleanup stack
			// because it has no internal resources.
			RBufWriteStream writeStream;
			writeStream.Open(*iMemoryStore);

			// Build the store data then calculate the end size;
			const TInt entryCount = iList->Count();		
			writeStream.WriteInt32L(entryCount);
			
			for(TInt i = 0; i < entryCount; ++i)
				(*iList)[i]->ExternalizeL(ETrue,writeStream);
	
			writeStream.CommitL();

			// Set to actual size
			bufferSizeRequired=iMemoryStore->Size();
			__ECOM_TRACE1("ECOM ListImplementations request buffer size required=%d",bufferSizeRequired);
			}
			CleanupInternalList();	
		}
	
	//if nothing is returned we should still indicate this to the client side
	if (bufferSizeRequired==0)
		{
		//write back the bufferSize
		listParam.iBufferSize=0;
  		aMessage.WriteL(2,listParamPkg);
		return;
		}
		
	//if the preallocated size is big enough to hold our entry
	//copy it to the client
	if (listParam.iBufferSize >= bufferSizeRequired)
		{
		if (iMemoryStore)
			{
			//write back the bufferSize
			listParam.iBufferSize=bufferSizeRequired;
  			aMessage.WriteL(2,listParamPkg);
			TPtr8 data=iMemoryStore->Ptr(0);
			aMessage.WriteL(3,data);
			delete iMemoryStore;
			iMemoryStore=NULL;
			}
		}
	//if not rewrite back to the client the size that is required
	//and signal with KErrOverFlow to the client
	else
		{
		//write back the bufferSize
		listParam.iBufferSize=bufferSizeRequired;
  		aMessage.WriteL(2,listParamPkg);
		User::Leave(KErrOverflow);
		}
	}

TBool CEComServerSession::DoCollectListL(const TClientRequest& aMessage)
	{
	TBool success = EFalse;
	if(iMemoryStore)
		{
		TPtr8 data=iMemoryStore->Ptr(0);
		aMessage.WriteL(0, data);
		delete iMemoryStore;
		iMemoryStore = NULL;
		success = ETrue;
		}
	return success;
	}

void CEComServerSession::DoGetResolvedImplementationL(const TClientRequest& aMessage)
	{
	// Unpack the client supplied parameters
	// Firstly get the uids
	TUidType uids;
	TPckg<TUidType> uidsPkg(uids);
	aMessage.ReadL(KIPCParameterUids, uidsPkg);

	// Now rebuild the TEComResolverParams
	TEComResolverParams resolverParameters;
	resolverParameters.SetGenericMatch(KIPCParameterResolverParamsTypePtr);	
	
	//now get the matchString and extendedInterfaces
	RBuf8 matchStr;
	matchStr.CleanupClosePushL();
	RExtendedInterfacesArray extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	UnpackMatchStrAndExtendedInterfacesFromClientL(aMessage,extendedInterfaces,matchStr);
    if(matchStr.Length()>0)
		{	
		resolverParameters.SetDataType(matchStr);
		}
	// Else the client's resolver params are default constructed i.e. invalid
	// data type descriptor or its length is zero, so use empty HBufC8 above.
	// Set up for the return value
	TUid dtorIdKey(KNullUid);
	TEntry loadInfo;
	
	switch(aMessage.Function())
		{
		case EGetImplementationCreationMethod:
			Server().GetResolvedDllInfoL(uids[KInterfaceUidIndex], 
										 loadInfo, 
										 dtorIdKey,
										 aMessage);
			break;
		case EGetResolvedCreationMethod:
			Server().GetResolvedDllInfoL(uids[KInterfaceUidIndex], 
										 resolverParameters, 
										 extendedInterfaces,
										 loadInfo, 
										 dtorIdKey,
										 aMessage);
			break;
		case EGetCustomResolvedCreationMethod:
			Server().GetResolvedDllInfoL(uids[KInterfaceUidIndex], 
										 resolverParameters, 
										 uids[KResolverUidIndex], 
										 extendedInterfaces,
										 loadInfo, 
										 dtorIdKey,
										 aMessage);
			break;
		default:
			break;
		}
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	CleanupStack::PopAndDestroy(&matchStr);
	
// ??? Compile time assert that sizeof(TProxyNewLPtr) == sizeof(TAny*)?
// Currently I'm not arranging for the client-side of the session to
// convert from TAny* to TProxyNewLPtr, and using this to avoid the
// full agony of following through with conversion...

	// Then re-package the results for return
	// Firstly the Interface Implementation creation method pointer
	TPckg<TEntry> result(loadInfo);
	
	aMessage.WriteL(KIPCReturnParameterCreationMethodPtr, result);
	// Next the destruction identification uid
	TUidType type(KNullUid,dtorIdKey,KNullUid);
	TPckg<TUidType> dtorIdKeyPkg(type);
	
	aMessage.WriteL(KIPCReturnParameterUidsPtr, dtorIdKeyPkg);
	}

// Return the list of interfaces to the client. If not enough space
// has been allocated by the client, KErrOverflow will be returned.
//
void CEComServerSession::DoListExtendedInterfacesL(const TClientRequest& aMessage)
	{
	// Unpack the client supplied parameters
	TInt bufferSize = 0;
	TUid implementationUid(KNullUid);
	TPckg<TUid> implementationUidDes(implementationUid);
	TPckg<TInt> bufferSizeDes(bufferSize);
	aMessage.ReadL(KIPCParameterImplementationUid,implementationUidDes);
	aMessage.ReadL(KIPCParameterBufferSize,bufferSizeDes);
		
	// Get the implementation information for this implementation UID.
	CImplementationInformation* implInfo = NULL;
	Server().GetImplementationInformationL(implementationUid,implInfo,aMessage);
	
	TInt numExtendedInterfaces = 0; // Number of extended interfaces to return to client.

	if(implInfo)
		{
		TInt bufferSizeRequired = 0; // Buffer required to send extended interface data back to client
		
		// Fetch the list of extended interfaces
		RExtendedInterfacesArray* extendedInterfaceList = implInfo->GetExtendedInterfaceList();
		if (extendedInterfaceList != NULL)
			{
			numExtendedInterfaces = extendedInterfaceList->Count();
			}
		if (numExtendedInterfaces > 0)
			{
			bufferSizeRequired = numExtendedInterfaces * sizeof(TUid);
			__ECOM_TRACE1("ECOM ListInterfaces request buffer size required=%d",bufferSizeRequired);
			
			//if the preallocated size is big enough to hold our entry
			//copy it to the client
			if (bufferSize >= bufferSizeRequired)
				{
				RBuf8 buf;
				CleanupClosePushL(buf);
				buf.CreateL(bufferSizeRequired);   // Create the RBuf.
				
				// Note : There is no need to push the write stream onto the cleanup stack
				// because it has no internal resources.
				RDesWriteStream writeStream;
				writeStream.Open(buf);

				// Build the data of extendedInterfaces;
				for(TInt i = 0; i < numExtendedInterfaces; ++i)
					{
					writeStream.WriteInt32L((*extendedInterfaceList)[i].iUid);
					}

				writeStream.CommitL();
				
				// Copy the data to the client.
				bufferSize=bufferSizeRequired;
				aMessage.WriteL(KIPCParameterBufferSize,bufferSizeDes);
				aMessage.WriteL(KIPCParameterInterfaceData,buf);
				
				CleanupStack::PopAndDestroy(&buf);
				}
			//if not rewrite back to the client the size that is required
			//and signal with KErrOverFlow to the client
			else
				{
				bufferSize=bufferSizeRequired;
				aMessage.WriteL(KIPCParameterBufferSize,bufferSizeDes);
				User::Leave(KErrOverflow);
				}
			}
		}
		
	//if nothing is returned we should still indicate this to the client side
	if (numExtendedInterfaces == 0)
		{
		bufferSize=0;
		aMessage.WriteL(KIPCParameterBufferSize,bufferSizeDes);
		}
	}

#if defined(__ECOM_SERVER_TESTABILITY__) || defined(__ECOM_SERVER_PERFORMANCE__)
/**
This method is provided for testability. It allows the user to 
send and receive any parameters. 
@param aMessage IPC message between server and client
*/
void CEComServerSession::DoSetGetParametersL(const TClientRequest& aMessage)
	{
	TInt parameterType = aMessage.Int0();
	
	switch(parameterType)
		{
#ifdef __ECOM_SERVER_TESTABILITY__
		case EChangeStartupState:
			Server().ChangeStartupStateL(aMessage.Int1());
			break;
		case EProcessStartupState:
			Server().ProcessCurrentStartupStateL();
			break;
		case EGetStartupState:
			{
			TInt state = Server().GetCurrentStartupState();
			TPckg<TInt> pckgState(state);
		    aMessage.Write(1, pckgState);
			break;
			}
#endif
#ifdef __ECOM_SERVER_PERFORMANCE__
		case EGetStartupStateTimerResult:
			{
			TStartupStateTimerEntry timerEntry;
			
			TInt ret = EComPerformance::GetStartupStateTimerResult(aMessage.Int1(), timerEntry.iTimerResult, timerEntry.iState);
			
			TPckg<TInt> pckgRetValue(ret);
			aMessage.Write(2, pckgRetValue);
			TPckg<TStartupStateTimerEntry> pckgTimerEntry(timerEntry);
			aMessage.Write(3, pckgTimerEntry);
			break;
			}

		case EGetAccumulatedClientRequestsTimerResult:
			{
			TClientRequestTimerEntry timerEntry;
			TInt ret = EComPerformance::GetAccumulatedClientRequestTimerResult(aMessage.Int1(), timerEntry);
			TPckg<TInt> pckgRetValue(ret);
			aMessage.Write(2, pckgRetValue);
			TPckg<TClientRequestTimerEntry> pckgTimerEntry(timerEntry);
			aMessage.Write(3, pckgTimerEntry);
			break;
			}
		case EGetRegistryCounts:
			{
			RegistryCounts::TRegistryCounts counts;
			Server().GetRegistryCountsL(aMessage.Int1(), counts);
			TPckg<RegistryCounts::TRegistryCounts> pckgRegistryCounts(counts);
			aMessage.Write(2, pckgRegistryCounts);
			break;
			}
		case EResetStartupStateTimerCounts:
			{
			EComPerformance::ResetStartupStateTimerResult();
			break;
			}
		case EGetEComPerfTimeRecord:
			{
			TEComPerfTimeRecordEntry timerEntry;
			TInt ret = EComPerformance::GetEComPerfTimeRecord(aMessage.Int1(), timerEntry);
			TPckg<TInt> pckgRetValue(ret);
			aMessage.Write(2, pckgRetValue);
			TPckg<TEComPerfTimeRecordEntry> pckgTimerEntry(timerEntry);
			aMessage.Write(3, pckgTimerEntry);
			break;
			}
		case EResetEComPerfTimeRecords:
			{
			EComPerformance::ResetEComPerfTimeRecords();
			}
		case EGetEComServerHeapResult:
			{
			TEComPerfHeapUsage heapEntry;
			TInt ret= EComPerformance::GetEComHeapSize(aMessage.Int1(),heapEntry);
			TPckg<TInt> pckgRetValue(ret);
			aMessage.Write(2, pckgRetValue);
			TPckg<TEComPerfHeapUsage> pckgHeapEntry(heapEntry);
			aMessage.Write(3, pckgHeapEntry);
			break;			
			}
#endif
		default:
			break;
		}
	}
#endif

void CEComServerSession::CleanupInternalList()
	{
	if (iList != NULL)
		{
		iList->Reset();
		delete iList;
		iList = NULL;
		}
	}

