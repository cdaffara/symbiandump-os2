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
// Implementation of CActiveBackupClient class.
// 
//

/**
 @file
*/

#include "abachandler.h"
#include "abclientsession.h"
#include "panic.h"

namespace conn
	{
	CActiveBackupCallbackHandler* CActiveBackupCallbackHandler::NewL(MActiveBackupDataClient* aABDC,
		RABClientSession& aClientSession)
	/**
	Symbian first phase constructor
	@param aABDC pointer to the client's callback implementation
	@param aClientSession reference to the client's session
	@return Pointer to a created CActiveBackupCallbackHandler object
	*/
		{
		CActiveBackupCallbackHandler* self = new (ELeave) CActiveBackupCallbackHandler(aABDC, aClientSession);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		}
	
	CActiveBackupCallbackHandler::CActiveBackupCallbackHandler(MActiveBackupDataClient* aABDC,
		RABClientSession& aClientSession) : CActive(EPriorityNormal), iActiveBackupDataClient(aABDC), 
		iClientSession(aClientSession), iTransferBuffer(NULL)
	/**
	C++ Constructor
	@param aABDC pointer to the client's callback implementation
	@param aClientSession reference to the client's session
	@panic KErrArgument if the pointer aABDC is NULL
	*/
		{
		__ASSERT_DEBUG(aABDC, Panic(KErrArgument));
		}

	CActiveBackupCallbackHandler::~CActiveBackupCallbackHandler()
	/**
	C++ Destructor
	*/
		{
		// Cancel any outstanding Async requests (i.e. close down the callback interface)
		Cancel();
		delete iTransferBuffer;
		iTransferBuffer = 0;
		}
		
	void CActiveBackupCallbackHandler::RunL()
	/**
	Handle messages "initiated" by the server. Because of IPC limitations, the client primes the 
	server with a message containing modifiable package buffers that are filled with identifiers 
	and arguments for the callback interface
	*/
		{
		if (iStatus == KErrNone)
			{
			switch(iCallbackCommand())
				{
				case EABCallbackAllSnapshotsSupplied:
					{
  					iActiveBackupDataClient->AllSnapshotsSuppliedL();
				
					PrimeServerForCallbackL();
					} break;
				case EABCallbackReceiveSnapshotData:
					{
					TDriveNumber driveNum = EDriveC;		// Initialise to keep the compiler happy

					HBufC8* pReceivedData = iClientSession.GetDataFromServerLC(iCallbackArg1(),
						EABCallbackReceiveSnapshotData, driveNum);
						
					// Call the client method for handling receipt of the snapshot
					iActiveBackupDataClient->ReceiveSnapshotDataL(driveNum, 
						*pReceivedData, static_cast<TBool>(iCallbackArg2()));
						
					// Reprime the server
					PrimeServerForCallbackL();
					
					CleanupStack::PopAndDestroy(pReceivedData);
					} break;
				case EABCallbackGetExpectedDataSize:
					{
					TUint dataSize = iActiveBackupDataClient->GetExpectedDataSize(static_cast<TDriveNumber>(iCallbackArg1()));
					
					// Reprime the server
					PrimeServerForCallbackWithResponseL(dataSize);
					} break;
				case EABCallbackGetSnapshotData:
					{
					// Create an empty TPtr8 to point at the buffer to fill
					TPtr8 bufferToSend(CreateFixedBufferL());
					
					TBool finished = ETrue;
					
					// Zero our descriptor so that it can be refilled
					bufferToSend.Zero();
					
					// Callback the AB client to populate our descriptor
					iActiveBackupDataClient->GetSnapshotDataL(static_cast<TDriveNumber>(iCallbackArg1()), 
						bufferToSend, finished);
					
					// Send the length and some other info to the server to allow it to prepare for the actual transfer
					iClientSession.SendDataLengthToServerL(bufferToSend, finished, EABCallbackGetSnapshotData);
					
					iTransferBuffer->Des().SetLength(bufferToSend.Length());
					
					// Send the actual data back to the server and prime for the next callback
					PrimeServerForCallbackWithResponseL(*iTransferBuffer);
					} break;
				case EABCallbackInitialiseGetBackupData:
					{
					iActiveBackupDataClient->InitialiseGetBackupDataL(static_cast<TDriveNumber>(iCallbackArg1()));

					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackGetBackupDataSection:
					{
					// Create an empty TPtr8 to point at the buffer to fill
					TPtr8 bufferToSend(CreateFixedBufferL());
					
					TBool finished = ETrue;

					// Zero our descriptor so that it can be refilled
					bufferToSend.Zero();
					
					// Callback the AB client to populate our descriptor
					iActiveBackupDataClient->GetBackupDataSectionL(bufferToSend, finished);
					
					// Send the length and some other info to the server to allow it to prepare for the actual transfer
					iClientSession.SendDataLengthToServerL(bufferToSend, finished, EABCallbackGetBackupDataSection);
					
					iTransferBuffer->Des().SetLength(bufferToSend.Length());
					
					// Send the actual data back to the server and prime for the next callback
					PrimeServerForCallbackWithResponseL(*iTransferBuffer);
					} break;
				case EABCallbackInitialiseRestoreBaseDataSection:
					{
					iActiveBackupDataClient->InitialiseRestoreBaseDataL(static_cast<TDriveNumber>(iCallbackArg1()));

					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackRestoreBaseDataSection:
					{
					HBufC8* pReceivedData = iClientSession.GetDataFromServerLC(iCallbackArg1(),
						EABCallbackRestoreBaseDataSection);
					
					// Call the client method for handling receipt of the snapshot
					iActiveBackupDataClient->RestoreBaseDataSectionL(*pReceivedData, 
						static_cast<TBool>(iCallbackArg2()));
						
					// Reprime the server
					PrimeServerForCallbackL();
					
					CleanupStack::PopAndDestroy(pReceivedData);
					} break;
				case EABCallbackInitialiseRestoreIncrementData:
					{
					iActiveBackupDataClient->InitialiseRestoreIncrementDataL(static_cast<TDriveNumber>(iCallbackArg1()));

					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackRestoreIncrementDataSection:
					{
					HBufC8* pReceivedData = iClientSession.GetDataFromServerLC(iCallbackArg1(),
						EABCallbackRestoreIncrementDataSection);
					
					// Call the client method for handling receipt of the snapshot
					iActiveBackupDataClient->RestoreIncrementDataSectionL(*pReceivedData, 
						static_cast<TBool>(iCallbackArg2()));
						
					// Reprime the server
					PrimeServerForCallbackL();
					
					CleanupStack::PopAndDestroy(pReceivedData);
					} break;
				case EABCallbackRestoreComplete:
					{
					iActiveBackupDataClient->RestoreComplete(static_cast<TDriveNumber>(iCallbackArg1()));

					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackTerminateMultiStageOperation:
					{
					iActiveBackupDataClient->TerminateMultiStageOperation();

					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackGetDataChecksum:
					{
					iActiveBackupDataClient->GetDataChecksum(static_cast<TDriveNumber>(iCallbackArg1()));

					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackInitialiseGetProxyBackupData:
					{
					iActiveBackupDataClient->InitialiseGetProxyBackupDataL(static_cast<TSecureId>(iCallbackArg1()),
						static_cast<TDriveNumber>(iCallbackArg2()));
					
					// Reprime the server
					PrimeServerForCallbackL();
					} break;
				case EABCallbackInitialiseRestoreProxyBaseData:
					{
					iActiveBackupDataClient->InitialiseRestoreProxyBaseDataL(static_cast<TSecureId>(
						iCallbackArg1()), static_cast<TDriveNumber>(iCallbackArg2()));

					// Reprime the server				
					PrimeServerForCallbackL();
					} break;
				default:
					{
					// Call the server to leave with KErrNotSupported
					User::Leave(KErrNotSupported);
					}
				}
				
			// Set us up for another call
			SetActive();

			}
		}
		
	void CActiveBackupCallbackHandler::PrimeServerForCallbackL()
	/**
	Reprime the server with a new IPC message to respond to
	*/
		{
		iClientSession.PrimeServerForCallbackL(iCallbackCommand, iCallbackArg1, iCallbackArg2, iStatus);
		}
		
	void CActiveBackupCallbackHandler::PrimeServerForCallbackWithResponseL(TInt aResponse)
	/**
	Reprime the server with a new IPC message to respond to, sending the result of the previous callback
	
	@param aResponse The response to send back to the server i.e. the result of the last callback made
	*/
		{
		iClientSession.PrimeServerForCallbackWithResponseL(iCallbackCommand, iCallbackArg1, iCallbackArg2, aResponse, iStatus);
		}

	void CActiveBackupCallbackHandler::PrimeServerForCallbackWithResponseL(TDesC8& aResponse)
	/**
	Reprime the server with a new IPC message to respond to, sending the result of the previous callback
	
	@param aResponse The response to send back to the server i.e. the result of the last callback made
	*/
		{
		iClientSession.PrimeServerForCallbackWithResponseL(iCallbackCommand, iCallbackArg1, iCallbackArg2, aResponse, iStatus);
		}
	
	TInt CActiveBackupCallbackHandler::RunError(TInt aError)
	/**
	Handle any leaves that occur from within the RunL and hence from the callback methods. In order 
	to propagate leaves over to the PC client, they must be sent to the backup engine to leave to the 
	client
	
	@param aError The leave code that has been trapped from within the RunL. Propagate this code to the engine
	@return Any unhandled leave code
	*/
		{
		iClientSession.PropagateLeave(aError);
		
		PrimeServerForCallbackL();
		SetActive();
		
		return KErrNone;	// Is this correct or do we return the error code even though it's been handled?
		}
		
	void CActiveBackupCallbackHandler::DoCancel()
	/**
	Immediately cancel any outstanding calls to the backup engine
	*/
		{
		// we can't do anything with the error code here
		TRAP_IGNORE(iClientSession.CancelServerCallbackL());
		}
		
	void CActiveBackupCallbackHandler::ConstructL()
	/**
	Add this object to the scheduler
	
	@panic KErrNotFound Debug only - If an ActiveScheduler is not installed
	@leave Release only - If an ActiveScheduler is not installed
	*/
		{
		if (!CActiveScheduler::Current())
			{
			__ASSERT_DEBUG(0, Panic(KErrNotFound));
			}
		
		// Add this AO to the scheduler
		CActiveScheduler::Add(this);
		SetActive();
		}
		
	void CActiveBackupCallbackHandler::StartListeningForServerMessagesL()
	/**
	Send an asynchronous IPC message to the server in order that it has a vehicle for "initiating" 
	a callback on iActiveBackupDataClient. This should only be called once as it will start the 
	active scheduler
	*/
		{
		// Prime the server with the first prime for callback IPC message
		PrimeServerForCallbackL();
		}

	TPtr8 CActiveBackupCallbackHandler::CreateFixedBufferL()
	/**
	Creates a buffer of the exact size.
	*/
		{
		delete iTransferBuffer;
		iTransferBuffer = NULL;
					
		iTransferBuffer = HBufC8::NewL(iCallbackArg1());
		
		return TPtr8(const_cast<TUint8*>(iTransferBuffer->Ptr()), iCallbackArg1());
		}
	}
	
