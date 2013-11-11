/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
 @file
 @internalComponent
*/

#include "eventhandler.h"
#include <usb/hostms/srverr.h>
#include <usb/usblogger.h>
#include "msmmserver.h"
#include "msmmengine.h"
#include "subcommands.h"
#include "msmmnodebase.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eventhandlerTraces.h"
#endif



// Push a sub-command into the queue and transfer the owership 
// to the queue
void RSubCommandQueue::PushL(TSubCommandBase* aCommand)
    {
    OstTraceFunctionEntry0( RSUBCOMMANDQUEUE_PUSHL_ENTRY );
    
    CleanupStack::PushL(aCommand);
    iQueue.AppendL(aCommand);
    CleanupStack::Pop(aCommand);
    OstTraceFunctionExit0( RSUBCOMMANDQUEUE_PUSHL_EXIT );
    }

// Pop the head entity from the queue and destroy it
void RSubCommandQueue::Pop()
    {
    OstTraceFunctionEntry0( RSUBCOMMANDQUEUE_POP_ENTRY );  
    if (iQueue.Count() == 0)
        {
        OstTraceFunctionExit0( RSUBCOMMANDQUEUE_POP_EXIT );
        return;
        }
    
    TSubCommandBase* command = iQueue[0];
    iQueue.Remove(0);
    delete command;
    command = NULL;
    OstTraceFunctionExit0( RSUBCOMMANDQUEUE_POP_EXIT_DUP1 );
    }
    
// Insert a sub-command sequence after head entities
void RSubCommandQueue::InsertAfterHeadL(TSubCommandBase* aCommand)
    {
    OstTraceFunctionEntry0( RSUBCOMMANDQUEUE_INSERTAFTERHEADL_ENTRY );
    
    if (!aCommand)
        {
        User::Leave(KErrArgument);
        }
    
    iQueue.InsertL(aCommand, 1);
    OstTraceFunctionExit0( RSUBCOMMANDQUEUE_INSERTAFTERHEADL_EXIT );
    }
    
// Execute the head sub-comment
void RSubCommandQueue::ExecuteHeadL()
    {
    OstTraceFunctionEntry0( RSUBCOMMANDQUEUE_EXECUTEHEADL_ENTRY );
    Head().ExecuteL();
    OstTraceFunctionExit0( RSUBCOMMANDQUEUE_EXECUTEHEADL_EXIT );
    }
    
// Get a reference of head sub-command in queue
TSubCommandBase& RSubCommandQueue::Head()
    {
    OstTraceFunctionEntry0( RSUBCOMMANDQUEUE_HEAD_ENTRY ); 
    return *iQueue[0];
    }
    
// Destory all entities and release the memory of queue
void RSubCommandQueue::Release()
    {
    OstTraceFunctionEntry0( RSUBCOMMANDQUEUE_RELEASE_ENTRY );
    
    iQueue.ResetAndDestroy();
    OstTraceFunctionExit0( RSUBCOMMANDQUEUE_RELEASE_EXIT );
    }

/*
 *  Public member functions
 */
CDeviceEventHandler::~CDeviceEventHandler()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CDEVICEEVENTHANDLER_DES_ENTRY );
    
    Cancel();
    delete iErrNotiData;
    iSubCommandQueue.Release();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CDEVICEEVENTHANDLER_DES_EXIT );
    }

CDeviceEventHandler* CDeviceEventHandler::NewL(MMsmmSrvProxy& aServer)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_NEWL_ENTRY );
    
    CDeviceEventHandler* self = CDeviceEventHandler::NewLC(aServer);
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_NEWL_EXIT );
    return self;
    }

CDeviceEventHandler* CDeviceEventHandler::NewLC(MMsmmSrvProxy& aServer)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_NEWLC_ENTRY );
    
    CDeviceEventHandler* self = 
        new (ELeave) CDeviceEventHandler(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_NEWLC_EXIT );
    return self;
    }

void CDeviceEventHandler::CreateSubCmdForRetrieveDriveLetterL(
        TInt aLogicalUnitCount)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CREATESUBCMDFORRETRIEVEDRIVELETTERL_ENTRY );
    
    TRetrieveDriveLetter* command(NULL);
    THostMsSubCommandParam parameter(iServer, *this, *this, iIncomingEvent);
    for (TInt index = 0; index < aLogicalUnitCount; index++)
        {
        command = new (ELeave) TRetrieveDriveLetter(parameter, index);
        iSubCommandQueue.PushL(command);
        }
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CREATESUBCMDFORRETRIEVEDRIVELETTERL_EXIT );
    }

void CDeviceEventHandler::CreateSubCmdForMountingLogicalUnitL(TText aDrive, 
        TInt aLuNumber)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CREATESUBCMDFORMOUNTINGLOGICALUNITL_ENTRY );
    
    THostMsSubCommandParam parameter(iServer, *this, *this, iIncomingEvent);
    TMountLogicalUnit* command = new (ELeave) TMountLogicalUnit(
            parameter, aDrive, aLuNumber);
    iSubCommandQueue.InsertAfterHeadL(command);
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CREATESUBCMDFORMOUNTINGLOGICALUNITL_EXIT );
    }

void CDeviceEventHandler::CreateSubCmdForSaveLatestMountInfoL(TText aDrive, 
        TInt aLuNumber)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CREATESUBCMDFORSAVELATESTMOUNTINFOL_ENTRY );
    
    THostMsSubCommandParam parameter(iServer, *this, *this, iIncomingEvent);
    TSaveLatestMountInfo* command = 
        new (ELeave) TSaveLatestMountInfo(parameter, aDrive, aLuNumber);
    iSubCommandQueue.InsertAfterHeadL(command);
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CREATESUBCMDFORSAVELATESTMOUNTINFOL_EXIT );
    }

void CDeviceEventHandler::Start()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_START_ENTRY );
    
    if (IsActive())
        {
        OstTraceFunctionExit0( CDEVICEEVENTHANDLER_START_EXIT );
        return;
        }
    iStatus = KRequestPending;
    SetActive();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_START_EXIT_DUP1 );
    }

void CDeviceEventHandler::Complete(TInt aError)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_COMPLETE_ENTRY );
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aError);
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_COMPLETE_EXIT );
    }

TRequestStatus& CDeviceEventHandler::Status() const
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_STATUS_ENTRY );
    
    const TRequestStatus& status = iStatus;
    return const_cast<TRequestStatus&>(status);
    }

void CDeviceEventHandler::HandleEventL(TRequestStatus& aStatus, 
        const TDeviceEvent& aEvent)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_HANDLEEVENTL_ENTRY );
    
    if (IsActive())
        {
        // An event is being handled. Currently handler is busy.
        User::Leave(KErrInUse);
        }
    
    // Copy incoming event
    iIncomingEvent = aEvent;

    // Create sub-commands and append them to queue
    CreateSubCmdForDeviceEventL();
    
    aStatus = KRequestPending;
    iEvtQueueStatus = &aStatus;
    
    // Start the handler to handle the incoming event
    Start();
    Complete();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_HANDLEEVENTL_EXIT );
    }

/*
 * Protected member functions 
 */

void CDeviceEventHandler::DoCancel()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_DOCANCEL_ENTRY );
    
    // Complete client with KErrCancel
    CompleteClient(KErrCancel);

    // Cancel current pending command
	if (iSubCommandQueue.Count())
        {
		iSubCommandQueue.Head().CancelAsyncCmd();
    	}
	OstTraceFunctionExit0( CDEVICEEVENTHANDLER_DOCANCEL_EXIT );
    }

void CDeviceEventHandler::RunL( )
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_RUNL_ENTRY );
    
    if (iSubCommandQueue.Count() == 0)
        {
        // Error occurs in lastest sub-command's DoExecuteL()
        // Or current command has been cancelled.
        OstTraceFunctionExit0( CDEVICEEVENTHANDLER_RUNL_EXIT );
        return;
        }
    
    if (iSubCommandQueue.Head().IsExecuted())
        {
        // Complete the current sub-command
        iSubCommandQueue.Head().AsyncCmdCompleteL();
        iSubCommandQueue.Pop();
        }

    // Move to the next sub-command
    if (iSubCommandQueue.Count())
        {
        iSubCommandQueue.ExecuteHeadL();
        }
    else
        {
        // Run out of sub-commands. Current handling event achieved.
        // Complete client
        CompleteClient();
        }
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_RUNL_EXIT_DUP1 );
    }

TInt CDeviceEventHandler::RunError(TInt aError)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_RUNERROR_ENTRY );
    
	if (iSubCommandQueue.Count())
        {
    	// Retrieve sub-command related error notification data
    	iSubCommandQueue.Head().HandleError(*iErrNotiData, aError);

    	// If current sub-command isn't a key one, the handler will continue to
    	// execute rest sub-command in the queue. But, if current sub-command
    	// is the last one in the queue, handler shall complete the client also. 
    	if (iSubCommandQueue.Head().IsKeyCommand() || 
            (iSubCommandQueue.Count() == 1))
        	{
        	CompleteClient(aError);
			}
		iSubCommandQueue.Pop();
        }

    if( IsActive() )
        {
        Complete(aError);
        }
    else if (iSubCommandQueue.Count())
        {
        Start();
   	    Complete();
        }

    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_RUNERROR_EXIT );
    return KErrNone;
    }

// Private member functions
CDeviceEventHandler::CDeviceEventHandler(MMsmmSrvProxy& aServer):
    CActive(EPriorityStandard),
    iServer(aServer)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CDEVICEEVENTHANDLER_ENTRY );
    
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CDEVICEEVENTHANDLER_EXIT );
    }

void CDeviceEventHandler::ConstructL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CONSTRUCTL_ENTRY );
    
    iErrNotiData = new (ELeave) THostMsErrData;
    ResetHandler();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CONSTRUCTL_EXIT );
    }

void CDeviceEventHandler::CreateSubCmdForDeviceEventL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CREATESUBCMDFORDEVICEEVENTL_ENTRY );
    
    switch (iIncomingEvent.iEvent)
        {
    case EDeviceEventAddFunction:
        CreateSubCmdForAddingUsbMsFunctionL();
        break;
    case EDeviceEventRemoveDevice:
        CreateSubCmdForRemovingUsbMsDeviceL();
        break;
        }
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CREATESUBCMDFORDEVICEEVENTL_EXIT );
    }

void CDeviceEventHandler::CreateSubCmdForAddingUsbMsFunctionL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CREATESUBCMDFORADDINGUSBMSFUNCTIONL_ENTRY );
    
    THostMsSubCommandParam parameter(iServer, *this, *this, iIncomingEvent);
    TRegisterInterface* command = new (ELeave) TRegisterInterface(parameter);
    iSubCommandQueue.PushL(command);
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CREATESUBCMDFORADDINGUSBMSFUNCTIONL_EXIT );
    }

void CDeviceEventHandler::CreateSubCmdForRemovingUsbMsDeviceL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_CREATESUBCMDFORREMOVINGUSBMSDEVICEL_ENTRY );
    
    CMsmmEngine& engine = iServer.Engine();
    TUsbMsDevice* device = engine.SearchDevice(iIncomingEvent.iDeviceId);
    if (!device)
        {
        User::Leave(KErrNotFound);
        }

    THostMsSubCommandParam parameter(iServer, *this, *this, iIncomingEvent);
    TRemoveUsbMsDevice* removeMsDevice = new (ELeave) TRemoveUsbMsDevice(
            parameter);
    iSubCommandQueue.PushL(removeMsDevice);
    
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_CREATESUBCMDFORREMOVINGUSBMSDEVICEL_EXIT );
    }

void CDeviceEventHandler::ResetHandler()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_RESETHANDLER_ENTRY );
    
    ResetHandlerData();
    ResetHandlerError();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_RESETHANDLER_EXIT );
    }

void CDeviceEventHandler::ResetHandlerData()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_RESETHANDLERDATA_ENTRY );
    
    // Reset event buffer
    iIncomingEvent.iDeviceId = 0;
    iIncomingEvent.iEvent = EDeviceEventEndMark;
    iIncomingEvent.iInterfaceNumber = 0;
    
    // Destory sub-command queue
    iSubCommandQueue.Release();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_RESETHANDLERDATA_EXIT );
    }

void CDeviceEventHandler::ResetHandlerError()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_RESETHANDLERERROR_ENTRY );
    
    // Reset error notification data
    iErrNotiData->iDriveName = 0x0;
    iErrNotiData->iError = EHostMsErrorEndMarker;
    iErrNotiData->iE32Error = KErrNone;
    iErrNotiData->iManufacturerString.Zero();
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_RESETHANDLERERROR_EXIT );
    }

void CDeviceEventHandler::CompleteClient(TInt aError/* = KErrNone*/)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTHANDLER_COMPLETECLIENT_ENTRY );
    
    if (iEvtQueueStatus)
        {
        User::RequestComplete(iEvtQueueStatus, aError);
        }
    OstTraceFunctionExit0( CDEVICEEVENTHANDLER_COMPLETECLIENT_EXIT );
    }

// End of file
