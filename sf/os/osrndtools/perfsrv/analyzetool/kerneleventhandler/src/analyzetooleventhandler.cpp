/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for the class DAnalyzeToolEventHandler.
*
*/


// INCLUDE FILES
#include "analyzetooleventhandler.h"
#include <kernel/kern_priv.h>

// CONSTANTS

// The handler mutex literal
_LIT( KHandlerMutexName, "AnalyzeToolHandlerMutex" );

// The handle data mutex literal
_LIT( KDataMutexName, "AnalyzeToolDataMutex" );


// ================= MEMBER FUNCTIONS =========================================

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::Create()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolEventHandler::Create( DLogicalDevice* aDevice,
    const TUint aProcessId )
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::Create()" );
    TInt ret( KErrNone );
    
    // Store owner process ID
    iProcessId = aProcessId;
    LOGSTR2( "ATDD DAnalyzeToolEventHandler::Create > iProcessId %d", 
            iProcessId );
    
    // Open the device
    ret = aDevice->Open();
    if ( ret != KErrNone )
        return ret;
    iDevice = aDevice;

    // Create mutex for the handler
    ret = Kern::MutexCreate( iHandlerMutex, KHandlerMutexName, KMutexOrdDebug );
    if ( ret != KErrNone )
        return ret;
    // Create mutex for the data
    ret = Kern::MutexCreate( iDataMutex, KDataMutexName, KMutexOrdDebug-1 );
    if ( ret != KErrNone )
        return ret;

    // Add handler to the handler queue
    return Add();
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::~DAnalyzeToolEventHandler()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
DAnalyzeToolEventHandler::~DAnalyzeToolEventHandler()
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::~DAnalyzeToolEventHandler()" );
    
    CancelInformLibraryEvent();
    
    // Close the data mutex
    if ( iDataMutex )
        {
        iDataMutex->Close( NULL );
        }

    // Close the handler mutex
    if ( iHandlerMutex )
        {
        iHandlerMutex->Close( NULL );
        }

    // Close the device mutex
    if ( iDevice )
        {
        iDevice->Close( NULL );
        }
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::EventHandler()
// Function for receiving kernel events
// -----------------------------------------------------------------------------
//
TUint DAnalyzeToolEventHandler::EventHandler( TKernelEvent aEvent, 
    TAny* a1, TAny* a2, TAny* aThis )
    {
    
    // Clarify the event type
    switch ( aEvent)
        {
        case EEventRemoveLibrary:
            {
            LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventHandler() - EEventRemoveLibrary" );
            // Cast the pointer to the handler
            DAnalyzeToolEventHandler* handler = 
            ( ( DAnalyzeToolEventHandler* ) aThis );
            // Create variable for library information
            TLibraryEventInfo info;
            // Set as library remove event
            info.iEventType = TLibraryEventInfo::ELibraryRemoved;
            // Handle the event
            handler->HandleLibraryEvent( ( DLibrary* ) a1, ( DThread* ) a2, info );
            break;
            }
        case EEventAddLibrary:
            {
            LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventHandler() - EEventAddLibrary" );
            // Cast the pointer to the handler
            DAnalyzeToolEventHandler* handler = 
            ( ( DAnalyzeToolEventHandler* ) aThis );
            // Create variable for library information
            TLibraryEventInfo info;
            // Set as library remove event
            info.iEventType = TLibraryEventInfo::ELibraryAdded;
            // Handle the event
            handler->HandleLibraryEvent( ( DLibrary* ) a1, ( DThread* ) a2, info );
            break;
            }
        case EEventHwExc:
            {
            LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventHandler() - EEventHwExc" );
            break;
            }
        case EEventSwExc:
            {
            LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventHandler() - EEventSwExc" );
            break;
            }
        case EEventRemoveThread:
            {
            LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventHandler() - EEventRemoveThread" );
            break;
            }
        case EEventKillThread:
            {
            LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventHandler() - EEventKillThread" );
            
            // Cast the pointer to the handler
            DAnalyzeToolEventHandler* handler = 
                ( ( DAnalyzeToolEventHandler* ) aThis );
            
            // Create variable for library information
            TLibraryEventInfo info;
            
            // Set as kill thread event
            info.iEventType = TLibraryEventInfo::EKillThread;
            // Handle the event
            handler->HandleKillThreadEvent( ( DThread* ) a1, info );
            break;
            }
        default:
            {
            }
            break;
        }  
    return ERunNext;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::HandleLibraryEvent()
// Handles the EEventAddLibrary and EEventRemoveLibrary events.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolEventHandler::HandleLibraryEvent( DLibrary* aLib, 
    DThread* aThread, TLibraryEventInfo& aInfo )
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::HandleLibraryEvent()" );
    
    // Aqcuire the handler mutex
    Kern::MutexWait( *iHandlerMutex );

    // Aqcuire the data mutex
    Kern::MutexWait( *iDataMutex );
    
    TBool addInfo( EFalse );
    
    // Check the library event type
    if ( aInfo.iEventType == TLibraryEventInfo::ELibraryAdded )
        {
        if ( aThread != NULL )
            {
            if ( iProcessId == aThread->iOwningProcess->iId )
                {
                LOGSTR1( "ATDD > Process id match" );
                aInfo.iProcessId = aThread->iOwningProcess->iId;
                addInfo = ETrue;
                }
            }
        }
    else if ( aInfo.iEventType == TLibraryEventInfo::ELibraryRemoved )
        {
        if ( aThread != NULL )
            {
            aInfo.iProcessId = aThread->iOwningProcess->iId;
            }
        else
            {
            aInfo.iProcessId = 0;
            }
        addInfo = ETrue;
        }
    
    if ( addInfo )
        {
        // Store lib info
        aLib->AppendName( aInfo.iLibraryName );//lint !e64 !e1514
        aInfo.iSize = aLib->iCodeSeg->iSize;
        aInfo.iRunAddress = aLib->iCodeSeg->iRunAddress;
        // Store library event info to the array
        iEventArray.Append( aInfo );
        LOGSTR2( "ATDD > iEventArray.Count() = %d", iEventArray.Count() );
        
        // if client has subscribed the event it is queued
        if ( iClientThread != NULL )
            {
            iEventDfc.Enque();
            }
        }
     
    // Release the data mutex
    Kern::MutexSignal( *iDataMutex );

    // Release the handler mutex
    Kern::MutexSignal( *iHandlerMutex );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::InformLibraryEvent()
// Subscribes library event.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolEventHandler::InformLibraryEvent( TRequestStatus* aStatus, 
                                                   TAny* aLibraryInfo,
                                                   TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::InformLibraryEvent()" );

    // Aqcuire the data mutex
    Kern::MutexWait( *iDataMutex );  
    
    // Check if request from client which is already pending
    DThread* current = aMessage.Client();
    
    LOGSTR2( "ATDD > Current Thread ID = %d", current->iId );
 
    // Ensure that client doesn't subscribe service when there is a pending
    // subscription
    if ( NULL != iClientThread )
        {
        aMessage.PanicClient( KClientPanic, EPanicRequestPending );
        }
    else
        {
        // Store the client variable pointers
        iClientThread         = current;
        iClientRequestStatus  = aStatus;
        iClientInfo           = aLibraryInfo;
        }
    
    // Release the data mutex
    Kern::MutexSignal( *iDataMutex );
    
    // Queue the event since now the client has subscribed it
    iEventDfc.Enque();
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::CancelInformLibraryEvent
// Cancels subscription of the library event.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolEventHandler::CancelInformLibraryEvent()
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::CancelInformLibraryEvent()" );
    
    iEventDfc.Cancel();
    
    // Aqcuire the data mutex
    Kern::MutexWait( *iDataMutex );

    if ( NULL != iClientThread && iEventArray.Count() > 0 )
        {
        // Signal the request as complete
        Kern::RequestComplete( iClientThread, 
                               iClientRequestStatus, 
                               KErrCancel );
        
        iClientThread         = NULL;
        iClientRequestStatus  = NULL;
        iClientInfo           = NULL;

        // Reset the event array
        iEventArray.Reset();
        }
   
    // Release the data mutex
    Kern::MutexSignal( *iDataMutex );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::HandleKillThreadEvent()
// Handles the EEventKillThread events.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolEventHandler::HandleKillThreadEvent( DThread* aThread, 
                                                      TLibraryEventInfo& aInfo )
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::HandleKillThreadEvent()" );
    
    // Aqcuire the handler mutex
    Kern::MutexWait( *iHandlerMutex );

    // Aqcuire the data mutex
    Kern::MutexWait( *iDataMutex );
    
    if ( aThread )
        {
        aInfo.iProcessId = aThread->iOwningProcess->iId;
        TBool alone( aThread->iOwningProcess->iThreadQ.First()->Alone() );
        LOGSTR2( "ATDD > Is alone = %d", alone );
        
        // Check if this our process and is the only thread item.
        if ( aInfo.iProcessId == iProcessId && !alone )
            {
            // Set current Thread id
            LOGSTR2( "ATDD > Thread ID = %d", aThread->iId );
            aInfo.iThreadId = aThread->iId;
            // Append event to array (beginning of the array)
            iEventArray.Insert( aInfo, 0 );
            
            // if client has subscribed the event it is queued
            if ( iClientThread != NULL )
                {
                iEventDfc.Enque();
                }
            }
        }
    
    // Release the data mutex
    Kern::MutexSignal( *iDataMutex );

    // Release the handler mutex
    Kern::MutexSignal( *iHandlerMutex );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::DoEventComplete()
// Informs client about the occured event.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolEventHandler::DoEventComplete()
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::DoEventComplete()" );
    
    // Aqcuire the handler mutex
    Kern::MutexWait( *iHandlerMutex );

    // Aqcuire the data mutex
    Kern::MutexWait( *iDataMutex );
    
    if ( NULL != iClientThread && iEventArray.Count() > 0 )
        {
        TInt ret = Kern::ThreadRawWrite( iClientThread, 
                                         iClientInfo, 
                                         &iEventArray[0],
                                         sizeof( iEventArray[0] ) );
        
        LOGSTR2( "ATDD > ThreadRawWrite err = %d", ret );
        
        // Signal the request as complete
        Kern::RequestComplete( iClientThread, 
                               iClientRequestStatus, 
                               ret );
        
        // Remove first item to array
        iEventArray.Remove( 0 );
        
        // Compresses the array down to a minimum
        iEventArray.Compress();
        
        // Ensure that pointers are set to NULL
        iClientThread         = NULL;
        iClientRequestStatus  = NULL;
        iClientInfo           = NULL;
        
        LOGSTR2( "ATDD > iEventArray = %d", iEventArray.Count() );
        }
    
    // Release the data mutex
    Kern::MutexSignal( *iDataMutex );

    // Release the handler mutex
    Kern::MutexSignal( *iHandlerMutex );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::EventDfc()
// Static function for DFC events.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolEventHandler::EventDfc(TAny* aPtr)
    {
    LOGSTR1( "ATDD DAnalyzeToolEventHandler::EventDfc()" ); 
    ( (DAnalyzeToolEventHandler*) aPtr )->DoEventComplete();
    }

// End of File
