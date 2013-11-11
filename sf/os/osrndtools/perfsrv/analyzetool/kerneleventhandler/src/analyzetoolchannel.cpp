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
* Description:  Definitions for the class DAnalyzeToolChannel.
*
*/


// INCLUDE FILES
#include "analyzetoolchannel.h"
#include "analyzetooldevice.h"
#include "analyzetooleventhandler.h"

#include <kernel/kern_priv.h>
#ifdef __WINSCW__
#include <emulator.h>
#endif // __WINSCW__

#include "atlog.h"

// ================= MEMBER FUNCTIONS =========================================

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::DoCreate()
// Creates the logical channel.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::DoCreate( TInt /*aUnit*/, 
    const TDesC8* /*aInfo*/, const TVersion &aVer )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::DoCreate()" );

    // Check client version.
    if ( !Kern::QueryVersionSupported( KAnalyzeToolLddVersion(), aVer ) )
        {
        return KErrNotSupported;
        }
     
    TInt error = Kern::DynamicDfcQCreate( iOwnDfcQ, 
                                          KAnalyzeToolThreadPriority, 
                                          KAnalyzeToolThreadName );

    if ( KErrNone != error )
        {
        return error;
        }

    SetDfcQ( iOwnDfcQ );
    
    iMsgQ.Receive();
   
    // Create the event handler
    iEventHandler = new DAnalyzeToolEventHandler( iOwnDfcQ );

    // Check that everything is OK
    if ( !iEventHandler )
        {
        return KErrNoMemory;
        }
    
    // 2nd stage constructor for event handler
    return iEventHandler->Create( iDevice, Kern::CurrentProcess().iId );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::DAnalyzeToolChannel()
// Constructor.
// -----------------------------------------------------------------------------
//
DAnalyzeToolChannel::DAnalyzeToolChannel()
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::DAnalyzeToolChannel()" );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::~DAnalyzeToolChannel()
// Destructor.
// -----------------------------------------------------------------------------
//
DAnalyzeToolChannel::~DAnalyzeToolChannel()
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::~DAnalyzeToolChannel()" );
    
    if ( iEventHandler )
        {
        // Cancel all processing that we may be doing
        DoCancel();
        
        // Client code should use Close() instead the operator delete 
        // to destroy the event handler. 
        TInt error( iEventHandler->Close() );
        if ( KErrNone != error )
            {
            LOGSTR2( "ATDD iEventHandler->Close(%d)", error );
            }
        }
    #ifdef __WINSCW__
        iCodeSeg.Close();
    #endif // __WINSCW__
    
    // Destroy the queqe
    if ( iOwnDfcQ )
        {
        iOwnDfcQ->Destroy();
        }
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::DoControl()
// Handles a client request.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::DoControl( TInt aFunction, 
                                     TAny* a1, 
                                     TAny* /*a2*/, 
                                     TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::Request()" );
    
    TInt ret( KErrNone );

    // Check the requested function
    switch (aFunction)
        {
        case RAnalyzeTool::EGetProcessInfo:
            ret = GetProcessInfo( a1, aMessage );
            break;
            
        case RAnalyzeTool::EGetCodesegInfo:
            ret = GetCodesegInfo( a1, aMessage );
            break;

        case RAnalyzeTool::EGetLibraryInfo:
            ret = GetLibraryInfo( a1, aMessage );
            break;
            
        case RAnalyzeTool::ECancelLibraryEvent:
            iEventHandler->CancelInformLibraryEvent();
            break;
            
        case RAnalyzeTool::ECurrentClientCount:
            ret = ClientCount( a1, aMessage );
            break;

        case RAnalyzeTool::EMainThreadAlloctor:
            ret = MainThreadAllocator( a1, aMessage );
            break;
        
        case RAnalyzeTool::EThreadStack:
             ret = ThreadStack( a1, aMessage );
             break;
             
        case RAnalyzeTool::EGetProcessHandle:
            ret = GetProcessHandleInfo( a1, aMessage );
            break;
        
        case RAnalyzeTool::EGetCurrentHandles:
            ret = GetCurrentHandleCount( a1, aMessage );
            break;
        case RAnalyzeTool::EGetMemoryModel:
            ret = GetMemoryModel( a1, aMessage );
            break;
            
        // Unsupported function. Panic
        default:
            Kern::PanicCurrentThread( KClientPanic, EPanicUnsupportedRequest );
            break;
        }
        
    return ret;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::DoRequest()
// Handles a client asynchronous request.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::DoRequest( TInt aFunction, 
                                     TRequestStatus* aStatus, 
                                     TAny* a1, 
                                     TAny* /*a2*/,
                                     TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::DoRequest()" );
    
    // Check the requested function
    switch (aFunction)
        {
        case RAnalyzeTool::ELibraryEvent:
            iEventHandler->InformLibraryEvent( aStatus, a1, aMessage );
            break;
            
        // Unsupported function. Panic
        default:
            aMessage.PanicClient( KClientPanic, EPanicUnsupportedRequest );
            break;
        }
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::DoCancel()
// Cancels outstanding asynchronous request.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolChannel::DoCancel()
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::DoCancel()" );
    
    iEventHandler->CancelInformLibraryEvent();
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::HandleMsg()
// Processes a message for this logical channel.
// -----------------------------------------------------------------------------
//
void DAnalyzeToolChannel::HandleMsg(TMessageBase* aMsg)
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::HandleMsg()" );

    TThreadMessage& message = *(TThreadMessage*)aMsg;

    // Get message type
    TInt id = message.iValue;

    // Decode the message type and dispatch it to the relevent handler function...
    if ( id == (TInt) ECloseMsg )
        {
        // Channel Close
        DoCancel();
        message.Complete( KErrNone, EFalse );
        }
    else if ( id == KMaxTInt )
        {
        // DoCancel
        DoCancel();
        message.Complete( KErrNone, ETrue );
        }
    else if ( id < 0 )
        {
        // DoRequest
        TRequestStatus* status = (TRequestStatus*) message.Ptr0();
        TInt error = DoRequest( ~id, status, message.Ptr1(), message.Ptr2(), message );
        if ( KErrNone != error )
            {
            Kern::RequestComplete( message.Client(), status, error);
            }
        message.Complete(KErrNone, ETrue );
        }
    else
        {
        // DoControl
        TInt ret = DoControl( id, message.Ptr0(), message.Ptr1(), message );
        message.Complete( ret, ETrue );
        }
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetProcessInfo()
// Acquires current process information
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::GetProcessInfo( TAny* aProcessInfo, 
                                          TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetProcessInfo()" );

    // Variable for reading parameters from user side
    TProcessIdentityParamsBuf params;
    
    // Reads a descriptor from a thread's process.
    TInt error = Kern::ThreadDesRead( aMessage.Client(), aProcessInfo, params, 0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 

    // Gets the current process
    Kern::Containers()[ EProcess ]->Wait();
    DProcess& process = *Kern::ProcessFromId( params().iProcessId );  
    Kern::Containers()[ EProcess ]->Signal();
    
    if ( NULL == &process )
        {        
        return KErrNotFound;
        }
    
    // Temporary variable for collecting information from the process
    TProcessIdentityParamsBuf info;
    
    // Collect needed information from the process
    process.AppendName( info().iProcessName );//lint !e64 !e1514
    
    // Gets the current thread
    Kern::Containers()[ EThread ]->Wait(); 
    DThread& thread = *Kern::ThreadFromId( params().iThreadId );
    Kern::Containers()[ EThread ]->Signal();
   
    if ( NULL == &thread )
        {
        return KErrNotFound;
        }     
    
    // Stack address of the main thread
    info().iStackAddress = thread.iUserStackRunAddress;
    info().iStackSize    = thread.iUserStackSize;  
    
    // Enters thread critical section and acquires code segment mutex.
    Kern::AccessCode();
        
    // Collect needed information from the process
    info().iDynamicCount = process.iDynamicCode.Count();

    // Temporary queue for acquiring the count of codesegments
    SDblQue queue;

    // Acquire the count of codesegments
    TInt codesegCount = process.TraverseCodeSegs( &queue, 
                                                  NULL, 
                                                  DCodeSeg::EMarkDebug, 
                                                  DProcess::ETraverseFlagAdd );
    
    #ifndef __WINSCW__
        info().iCodesegCount = codesegCount;    
    #else
    // Reset codesegment array
    iCodeSeg.Reset();
    
    if ( codesegCount > 0 )
        {
        SDblQueLink* link = queue.iA.iNext;
        TCodesegInfo codeinfo;
        // Iterate through codesegments
        for ( TInt i = 0; i < codesegCount; ++i, link = link->iNext )
            {
            DWin32CodeSeg* codeseg = 
                (DWin32CodeSeg*)_LOFF( link, DCodeSeg, iTempLink );

            // Aqcuire codeseg information
            codeinfo.iFileEntryPoint = codeseg->iFileEntryPoint;
            codeinfo.iSize = codeseg->iSize;
            codeinfo.iFullName.Copy( codeseg->iRootName );
            codeinfo.iRunAddress = codeseg->iRunAddress;
            iCodeSeg.Append( codeinfo );
            }
        }
    
    // Add dependency codesegments
    DWin32CodeSeg* pcodeSeg = (DWin32CodeSeg*)process.iCodeSeg;
    
    // Get dependency codesegments
    GetModuleDependencies( pcodeSeg->iModuleHandle );
    
    // Set codesegment count
    info().iCodesegCount = iCodeSeg.Count();
    #endif
    
    // Removes all code segments from a queue and clear specified mark(s)
    DCodeSeg::EmptyQueue( queue, DCodeSeg::EMarkDebug );

    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();

    // Writes a descriptor to a thread's process.
    error = Kern::ThreadDesWrite( aMessage.Client(), aProcessInfo, info, 0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
        return error;
        } 
   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetCodesegInfo()
// Acquires codeseg information.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::GetCodesegInfo( TAny* aCodesegInfo, 
                                          TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetCodesegInfo()" );

    // Temporary variable for collecting information from the codeseg
    TCodesegInfoBuf params;

    TInt error( KErrArgument );
    
    // Reads a descriptor from a thread's process.
    error = Kern::ThreadDesRead( aMessage.Client(), aCodesegInfo, params, 0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 

    if ( params().iIndex < 0 )
        {
        return KErrArgument;
        }
    
    // Gets the current process
    Kern::Containers()[ EProcess ]->Wait();
    DProcess& process = *Kern::ProcessFromId( params().iProcessId );
    Kern::Containers()[ EProcess ]->Signal();
    
    if ( NULL == &process )
        {
        return KErrNotFound;
        }
    
    // Temporary variable for collecting information 
    TCodesegInfoBuf output;

    // Enters thread critical section and acquires code segment mutex.
    Kern::AccessCode();

    #ifndef __WINSCW__
    // Temporary queue for acquiring the codesegments
    SDblQue queue;
    
    // Acquire the codesegments
    TInt actcount = process.TraverseCodeSegs( &queue, 
                                              NULL, 
                                              DCodeSeg::EMarkDebug, 
                                              DProcess::ETraverseFlagAdd );
    if ( actcount >= params().iIndex )
        {
        LOGSTR1( "ATDD DAnalyzeToolChannel::GetCodesegInfo() - actcount >= params.iIndex" );
        SDblQueLink* link = queue.iA.iNext;
        
        // Iterate through codesegments
        for (TInt i = 0; i < actcount; ++i, link = link->iNext)
            {
            DCodeSeg* codeseg = _LOFF( link, DCodeSeg, iTempLink );

            // Is the codesegments which information client wants
            if ( i == params().iIndex )
                {
                // Aqcuire codeseg information
                output().iFileEntryPoint = codeseg->iFileEntryPoint;
                output().iSize = codeseg->iSize;
                output().iFullName.Copy( codeseg->iRootName );
                output().iRunAddress = codeseg->iRunAddress;
                error = codeseg->GetMemoryInfo( output().iMemoryInfo, &process );
                
                if ( KErrNone == error )
                    {
                    // Writes a descriptor to a thread's process.
                    error = Kern::ThreadDesWrite( aMessage.Client(), 
                                                  aCodesegInfo, 
                                                  output, 
                                                  0 );   
                    if ( KErrNone != error )
                        {
                        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
                        } 
                    }
                break;
                }
            }
        }
    // Removes all code segments from a queue and clear specified mark(s).
    DCodeSeg::EmptyQueue( queue, DCodeSeg::EMarkDebug );
    
    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();
    
    return error;
    #else // WINSCW
    
    if ( iCodeSeg.Count() > params().iIndex )
        {
        // Aqcuire codeseg information
        output().iSize = iCodeSeg[params().iIndex].iSize;
        output().iFullName.Copy( iCodeSeg[params().iIndex].iFullName );
        output().iRunAddress = iCodeSeg[params().iIndex].iRunAddress;
        
        // Writes a descriptor to a thread's process.
        error = Kern::ThreadDesWrite( aMessage.Client(), aCodesegInfo, output, 0 ); 
        
        if ( KErrNone != error )
            {
            LOGSTR2( "ATDD ThreadDesWrite error %d", error );
            }
        }
    
    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();
    
    return error;
    #endif
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetLibraryInfo()
// Acquires library information.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::GetLibraryInfo( TAny* aLibraryInfo, 
                                          TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetLibraryInfo()" );

    // Temporary variable for reading informationfrom the user side
    TLibraryInfoBuf params;

    // Reads a descriptor from a thread's process.
    TInt error = Kern::ThreadDesRead( aMessage.Client(), aLibraryInfo, params, 0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 

    if ( params().iIndex < 0 )
        {
        return KErrArgument;
        }
   
    // Gets the current process
    Kern::Containers()[ EProcess ]->Wait();
    DProcess& process = *Kern::ProcessFromId( params().iProcessId );
    Kern::Containers()[ EProcess ]->Signal();
    
    if ( NULL == &process )
        {
        return KErrNotFound;
        }
    
    // Temporary variable for collecting information from the library
    TLibraryInfoBuf output;
        
    // Enters thread critical section and acquires code segment mutex.
    Kern::AccessCode();

    // Iterate to find the right library
    if ( params().iIndex < process.iDynamicCode.Count() )
        {
        // Acquire entry to the codeseg
        SCodeSegEntry entry = process.iDynamicCode[ params().iIndex ];
        
        // Acquire library information
        entry.iLib->AppendName( output().iLibraryName );//lint !e64 !e1514
        output().iRunAddress = entry.iSeg->iRunAddress;
        output().iSize = entry.iSeg->iSize;
        
        // Writes a descriptor to a thread's process.
        error = Kern::ThreadDesWrite( aMessage.Client(), aLibraryInfo, output, 0 ); 
        
        if ( KErrNone != error )
            {
            LOGSTR2( "ATDD ThreadDesWrite error %d", error );
            } 
        
        // Exits thread critical section and releases code segment mutex.
        Kern::EndAccessCode();
        
        return error;
        }
    else
        {
        // Exits thread critical section and releases code segment mutex.
        Kern::EndAccessCode();
        
        return KErrArgument;
        }
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::MainThreadAllocator()
// Acquires information about process main thread RAllocator
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::MainThreadAllocator( TAny* aMainThreadParams, 
                                               TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::MainThreadAllocator()" );

    // Temporary variable for reading client side parameters
    TMainThreadParamsBuf params;

    // Reads a descriptor from a thread's process.
    TInt error = Kern::ThreadDesRead( aMessage.Client(), 
                                      aMainThreadParams, 
                                      params, 
                                      0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 
    
    // Gets the current process
    Kern::Containers()[ EProcess ]->Wait();
    DProcess& process = *Kern::ProcessFromId( params().iProcessId );
    Kern::Containers()[ EProcess ]->Signal();
    
    if ( NULL == &process )
        {
        return KErrNotFound;
        }

    // Gets the current process
    Kern::AccessCode();

    // Temporary variable for collecting information from the RAllocator
    TMainThreadParamsBuf output;

    // Aqcuire a reference to the main thread RAllocator
    output().iAllocator = process.FirstThread()->iAllocator;

    // Is this only thread in the process
    output().iAlone = process.iThreadQ.First()->Alone();

    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();

    // Writes a descriptor to a thread's process.
    error = Kern::ThreadDesWrite( aMessage.Client(), 
                                  aMainThreadParams, 
                                  output, 
                                  0 ); 
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
        } 

    return error;
    }    

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::ThreadStack()
// Acquires main thread stack address
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::ThreadStack( TAny* aThreadStack, 
                                       TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::ThreadStack()" );

    // Temporary variable for reading client side parameters
    TThreadParamsBuf params;

    // Reads a descriptor from a thread's process.
    TInt error = Kern::ThreadDesRead( aMessage.Client(), 
                                      aThreadStack, 
                                      params, 
                                      0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 
    
    // Gets the current process
    Kern::Containers()[ EThread ]->Wait();
    DThread& thread = *Kern::ThreadFromId( params().iThreadId );
    Kern::Containers()[ EThread ]->Signal();
    
    if ( NULL == &thread )
        {
        return KErrNotFound;
        }
    
    // Gets the current process
    Kern::AccessCode();

    // Temporary variable for collecting information from the RAllocator
    TThreadParamsBuf output;

    // Stack address of the main thread
    output().iStackAddress = thread.iUserStackRunAddress;
    output().iStackSize    = thread.iUserStackSize;

    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();

    // Writes a descriptor to a thread's process.
    error = Kern::ThreadDesWrite( aMessage.Client(), 
                                  aThreadStack, 
                                  output, 
                                  0 ); 
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
        } 
    
    return error;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetProcessHandleInfo()
// Acquires information about process global handles
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::GetProcessHandleInfo( TAny* aProcessHandleInfo,
                                                TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetProcessHandleInfo()" );

    // Temporary variable for collecting information from the codeseg
    TProcessHandleInfoBuf params;

    // Reads a descriptor from a thread's process.
    TInt error = Kern::ThreadDesRead( aMessage.Client(), 
                                      aProcessHandleInfo, 
                                      params, 
                                      0 );  
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 

    // Gets the current process
    Kern::Containers()[ EProcess ]->Wait();
    DProcess& process = *Kern::ProcessFromId( params().iProcessId );
    Kern::Containers()[ EProcess ]->Signal();
    
    if ( NULL == &process )
        {
        return KErrNotFound;
        }
    
    // Variable holding wanted information
    TProcessHandleInfoBuf output;

    // Enters thread critical section and acquires code segment mutex.
    Kern::AccessCode();

    // Get the process thread queue.
    SDblQue queue = process.iThreadQ;
    error = KErrNotFound;
        
    // Tests whether this doubly linked list is empty.
    if ( !queue.IsEmpty() )
        {
        // Gets a pointer to the first item in this doubly linked list.
        SDblQueLink* link = queue.First();
        DThread* thread = _LOFF( link, DThread, iProcessLink );

        if ( thread )
            {
            
#ifdef MCL_ROBJECTIX
            TInt threadHandles( thread->iHandles.ActiveCount() );
#else
            TInt threadHandles( thread->iHandles->ActiveCount() );
#endif
            
            // Aqcuire thread information
            //thread->AppendName( output.iThreadName );
            output().iUserStackRunAddress = thread->iUserStackRunAddress;
            output().iUserStackSize = thread->iUserStackSize;
            output().iThreadHandleCount = threadHandles;
            
#ifdef MCL_ROBJECTIX
            RObjectIx objectIx = process.iHandles;
            output().iProcessHandleCount = objectIx.ActiveCount();
#else
            DObjectIx* objectIx = process.iHandles;
            output().iProcessHandleCount = objectIx->ActiveCount();
#endif
                        
            // Writes a descriptor to a thread's process.
            error = Kern::ThreadDesWrite( aMessage.Client(), 
                                          aProcessHandleInfo, 
                                          output, 
                                          0 ); 
            
            if ( KErrNone != error )
                {
                LOGSTR2( "ATDD ThreadDesWrite error %d", error );
                } 
            }
        }

    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();
    
    return error;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetCurrentHandleCount()
// Acquires a process's current handle count
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::GetCurrentHandleCount( TAny* aProcessHandles,
                                                 TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetCurrentHandleCount()" );

    // Temporary variable for collecting information from the codeseg
    TATProcessHandlesBuf params;
    
    // Reads a descriptor from a thread's process.
    TInt error = Kern::ThreadDesRead( aMessage.Client(), 
                                      aProcessHandles, 
                                      params, 
                                      0 );
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesRead error %d", error );
        return error;
        } 
    
    // Gets the current process
    Kern::Containers()[ EProcess ]->Wait();
    DProcess* process = Kern::ProcessFromId( params().iProcessId );
    Kern::Containers()[ EProcess ]->Signal();
    
    if ( NULL == process )
        {
        return KErrNotFound;
        }

    // Variable holding wanted information
    TATProcessHandlesBuf output;
    
    // Enters thread critical section and acquires code segment mutex.
    Kern::AccessCode();
    
    SDblQue queue = process->iThreadQ;
    SDblQueLink* link = queue.First();
    TInt threadHandles( 0 );
    
    // Iterate through current processes's threads
    while ( link != queue.Last() )
        {
        DThread* thread = _LOFF( link, DThread, iProcessLink );
        
#ifdef MCL_ROBJECTIX
        threadHandles += thread->iHandles.ActiveCount();
#else
        threadHandles += thread->iHandles->ActiveCount();
#endif
        
        link = link->iNext;
        }

    if ( link == queue.Last() )
        {
        DThread* thread = _LOFF( link, DThread, iProcessLink );

#ifdef MCL_ROBJECTIX
        threadHandles += thread->iHandles.ActiveCount();
#else
        threadHandles += thread->iHandles->ActiveCount();
#endif
        }
    
    output().iCurrentHandleCount = threadHandles;
    
    // Writes a descriptor to a thread's process.
    error = Kern::ThreadDesWrite( aMessage.Client(), 
                                  aProcessHandles, 
                                  output, 
                                  0 ); 
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
        } 
    
    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();
    
    return error;
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::ClientCount()
// Acquires the count of current device driver users.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::ClientCount( TAny* aClientCount,
                                       TThreadMessage& aMessage )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::ClientCount()" );
    
    // Enters thread critical section and acquires code segment mutex.
    Kern::AccessCode();
    
    // Variable holding wanted information
    TClientCountBuf output;
    
    // Get the number of DLogicalChannelBase objects currently in existence which
    // have been created from this LDD.
    output().iClientCount = DLogicalChannelBase::iDevice->iOpenChannels;
    LOGSTR2( "ATDD > iOpenChannels count: %d", output().iClientCount ); 
    
    // Writes a descriptor to a thread's process.
    TInt error = Kern::ThreadDesWrite( aMessage.Client(), 
                                       aClientCount, 
                                       output, 
                                       0 ); 
    
    if ( KErrNone != error )
        {
        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
        } 
    
    // Exits thread critical section and releases code segment mutex.
    Kern::EndAccessCode();
    
    return error;
    }

#ifdef __WINSCW__
// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetModuleDependencies()
// Get module dependencies
// -----------------------------------------------------------------------------
//
void DAnalyzeToolChannel::GetModuleDependencies( HMODULE aModule )
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetModuleDependencies()" );

    Emulator::TModule etm( aModule );
    TUint32 dllSize( 0 );
    // Temporary variable for collecting information from the codeseg
    TCodesegInfo info;
    TBool found( EFalse );
    
    const IMAGE_IMPORT_DESCRIPTOR* imports = etm.Imports();
    while( imports->Characteristics != 0 )
        {
        // Reset flag
        found = EFalse;
        
        // Get dll name
        const TUint8* nameAddr = ( const TUint8* )( imports->Name + ( TInt )etm.iBase );
        TPtrC8 namePtr( nameAddr );     
        
        // Get dll run address
        Emulator::TModule imp_etm( ( PCSTR )etm.Translate( imports->Name ) );        
        const TUint8* runAddr = ( const TUint8* )imp_etm.iBase;
        
        // Get dll size
        const IMAGE_NT_HEADERS32* ntHeader = imp_etm.NtHeader();
        dllSize = ntHeader->OptionalHeader.SizeOfImage;       
        
        // Check if DLL already exists in codesegment list
        for( TInt i = 0; i < iCodeSeg.Count(); i++ )
            {
            if ( iCodeSeg[i].iFullName.Compare( namePtr ) == KErrNone )
                {
                found = ETrue;
                break;
                }
            }
        
        if ( !found )
            {
            info.iSize = dllSize;
            info.iFullName.Copy( namePtr );
            info.iRunAddress = (TUint32) runAddr;  
            // Append codesegment to array
            iCodeSeg.Append( info );
            }
        imports++;
        }
    }
  
#endif // __WINSCW__

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetMemoryModel()
// Acquires memory model system uses.
// -----------------------------------------------------------------------------
//
TInt DAnalyzeToolChannel::GetMemoryModel(TAny* aMemoryModel,
                                        TThreadMessage& aMessage)
    {
    LOGSTR1( "ATDD DAnalyzeToolChannel::GetMemoryModel()" );
   
    // Model buffer.
    TATMemoryModelBuf model;
    // Get current model.
    model().iMemoryModel = (TUint32) Kern::HalFunction( EHalGroupKernel, EKernelHalMemModelInfo, NULL, NULL );
    model().iMemoryModel &= EMemModelTypeMask; // Mask out other stuff.
    // Write it to client side.
    TInt error = Kern::ThreadDesWrite( aMessage.Client(),
                                        aMemoryModel,
                                        model,
                                        0);
    if ( error != KErrNone )
        {
        LOGSTR2( "ATDD ThreadDesWrite error %d", error );
        }
    return error;
    }
// End of File
