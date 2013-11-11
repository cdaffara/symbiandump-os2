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
* Description:
*
*/

#include <memspy/engine/memspyengineprocessmemorytracker.h>

// System includes
#include <e32debug.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>

// Constants
const TInt KMemSpyNumberOfChangesReceivedProcessNotYetOpen = -1;
const TInt KMemSpyNumberOfChangesReceivedProcessNowOpen = 0;


CMemSpyEngineProcessMemoryTracker::CMemSpyEngineProcessMemoryTracker( CMemSpyEngine& aEngine, const TProcessId& aProcessId )
:   CActive( EPriorityStandard ), iEngine( aEngine ), iProcessId( aProcessId ), iNumberOfChangesReceived( KMemSpyNumberOfChangesReceivedProcessNotYetOpen )
    {
    CActiveScheduler::Add( this );
    }


CMemSpyEngineProcessMemoryTracker::~CMemSpyEngineProcessMemoryTracker()
    {
    Cancel();
    
    // Don't close the process if we didn't successfully open it...
    if  ( iNumberOfChangesReceived != KMemSpyNumberOfChangesReceivedProcessNotYetOpen )
        {
        iEngine.Driver().ProcessInspectionClose( iProcessId );
        }

    iObservers.Close();
    delete iProcessName;
    }


void CMemSpyEngineProcessMemoryTracker::ConstructL()
    {
    // Cache name    
    RProcess process;
    User::LeaveIfError( iEngine.Driver().OpenProcess( iProcessId, process ) );
    CleanupClosePushL( process );
    iProcessName = process.FullName().AllocL();
    iSID = process.SecureId();
    CleanupStack::PopAndDestroy( &process );

    // Indicate that we've opened the process
    iNumberOfChangesReceived = KMemSpyNumberOfChangesReceivedProcessNowOpen;

    // Open driver session
    const TInt error = iEngine.Driver().ProcessInspectionOpen( iProcessId );
    User::LeaveIfError( error );

    // Pull out the first change which the driver always prepares for us when
    // we open an initial connection.
    TRequestStatus status = KRequestPending;
    iEngine.Driver().ProcessInspectionRequestChanges( iProcessId, status, iInfoCurrent );
    User::WaitForRequest( status );
    
    // And update our HWM value based upon this initial seed...
    UpdatePeaks( iInfoCurrent );
    UpdateHWMIncShared( iInfoCurrent );
    UpdateHWMExcShared( iInfoCurrent );
    }


CMemSpyEngineProcessMemoryTracker* CMemSpyEngineProcessMemoryTracker::NewLC( CMemSpyEngine& aEngine, const TProcessId& aProcessId )
    {
    CMemSpyEngineProcessMemoryTracker* self = new(ELeave) CMemSpyEngineProcessMemoryTracker( aEngine, aProcessId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TBool CMemSpyEngineProcessMemoryTracker::AmTracking() const
    {
    return iAmTracking;
    }


EXPORT_C void CMemSpyEngineProcessMemoryTracker::StartL()
    {
    Cancel();
    iNumberOfChangesReceived = 0;
    //
    iEngine.Driver().ProcessInspectionRequestChanges( iProcessId, iStatus, iInfoCurrent );
    SetActive();
    iAmTracking = ETrue;
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineProcessMemoryTrackerObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->HandleMemoryTrackingStartedL() );
        }
    }


EXPORT_C void CMemSpyEngineProcessMemoryTracker::Stop()
    {
    Cancel();
    //
    iAmTracking = EFalse;
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineProcessMemoryTrackerObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->HandleMemoryTrackingStoppedL() );
        }
    }


EXPORT_C void CMemSpyEngineProcessMemoryTracker::AddObserverL( MMemSpyEngineProcessMemoryTrackerObserver& aObserver )
    {
    RemoveObserver( aObserver );
    User::LeaveIfError( iObservers.Append( &aObserver ) );
    }
 

EXPORT_C void CMemSpyEngineProcessMemoryTracker::RemoveObserver( MMemSpyEngineProcessMemoryTrackerObserver& aObserver )
    {
    TInt pos = iObservers.Find( &aObserver );
    while( pos >= 0 )
        {
        iObservers.Remove( pos );
        pos = iObservers.Find( &aObserver );
        }
    }


EXPORT_C void CMemSpyEngineProcessMemoryTracker::ResetHWML()
    {
    iInfoHWMIncShared.iMemoryStack = 0;
    iInfoHWMIncShared.iMemoryHeap = 0;
    iInfoHWMIncShared.iMemoryChunkLocal = 0;
    iInfoHWMIncShared.iMemoryChunkShared = 0;
    iInfoHWMIncShared.iMemoryGlobalData = 0;
    //
    iInfoHWMExcShared.iMemoryStack = 0;
    iInfoHWMExcShared.iMemoryHeap = 0;
    iInfoHWMExcShared.iMemoryChunkLocal = 0;
    iInfoHWMExcShared.iMemoryChunkShared = 0;
    iInfoHWMExcShared.iMemoryGlobalData = 0;
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineProcessMemoryTrackerObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->HandleMemoryChangedL( iProcessId, InfoCurrent(), InfoHWMIncShared(), InfoHWMExcShared() ) );
        }
    }


void CMemSpyEngineProcessMemoryTracker::RunL()
    {
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::RunL() - START" );
    ++iNumberOfChangesReceived;
    
    // Copy current results and request next event providing the driver didn't
    // indicate an error (e.g. KErrDied in the case of process termination)
    TMemSpyDriverProcessInspectionInfo info( iInfoCurrent );
    if  ( iStatus.Int() >= KErrNone )
        {
        iEngine.Driver().ProcessInspectionRequestChanges( iProcessId, iStatus, iInfoCurrent );
        SetActive();
        }
    else
        {
        //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::RunL() - iStatus: %d, not requesting further changes...", iStatus.Int() );
        }

    // Update stats
    UpdatePeaks( info );
    UpdateHWMIncShared( info );
    UpdateHWMExcShared( info );

    // Print out change to file/trace
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::RunL() - printing info..." );
    PrintInfoL( info );
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::RunL() - done printing info" );

    // Reset peak flags for next cycle
    iInfoPeaks.ResetPeakFlags();

    // Notify observers
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineProcessMemoryTrackerObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->HandleMemoryChangedL( iProcessId, info, InfoHWMIncShared(), InfoHWMExcShared() ) );
        }

    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::RunL() - END" );
    }


void CMemSpyEngineProcessMemoryTracker::DoCancel()
    {
    ////RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::DoCancel() - START - iStatus: %d", iStatus.Int() );
    iEngine.Driver().ProcessInspectionRequestChangesCancel( iProcessId );
    ////RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::DoCancel() - END - iStatus: %d", iStatus.Int() );
    }


void CMemSpyEngineProcessMemoryTracker::UpdatePeaks( const TMemSpyDriverProcessInspectionInfo& aLatestInfo )
    {
    if  ( aLatestInfo.iMemoryStack > iInfoPeaks.iMemoryStack )
        {
        iInfoPeaks.iPeakStack = ETrue;
        iInfoPeaks.iMemoryStack = aLatestInfo.iMemoryStack;
        }
    if  ( aLatestInfo.iMemoryHeap > iInfoPeaks.iMemoryHeap )
        {
        iInfoPeaks.iPeakHeap = ETrue;
        iInfoPeaks.iMemoryHeap = aLatestInfo.iMemoryHeap;
        }
    if  ( aLatestInfo.iMemoryChunkLocal > iInfoPeaks.iMemoryChunkLocal )
        {
        iInfoPeaks.iPeakChunkLocal = ETrue;
        iInfoPeaks.iMemoryChunkLocal = aLatestInfo.iMemoryChunkLocal;
        }
    if  ( aLatestInfo.iMemoryChunkShared > iInfoPeaks.iMemoryChunkShared )
        {
        iInfoPeaks.iPeakChunkShared = ETrue;
        iInfoPeaks.iMemoryChunkShared = aLatestInfo.iMemoryChunkShared;
        }
    if  ( aLatestInfo.iMemoryGlobalData > iInfoPeaks.iMemoryGlobalData )
        {
        iInfoPeaks.iPeakGlobalData = ETrue;
        iInfoPeaks.iMemoryGlobalData = aLatestInfo.iMemoryGlobalData;
        }
    }


void CMemSpyEngineProcessMemoryTracker::UpdateHWMIncShared( const TMemSpyDriverProcessInspectionInfo& aLatestInfo )
    {
    const TUint32 totalCurrent = aLatestInfo.TotalIncShared();
    const TUint32 totalHWM = iInfoHWMIncShared.TotalIncShared();
    //
    if  ( totalCurrent > totalHWM )
        {
        // Got new HWM
        iInfoHWMIncShared = aLatestInfo;
        }
    }


void CMemSpyEngineProcessMemoryTracker::UpdateHWMExcShared( const TMemSpyDriverProcessInspectionInfo& aLatestInfo )
    {
    const TUint32 totalCurrent = aLatestInfo.TotalExcShared();
    const TUint32 totalHWM = iInfoHWMExcShared.TotalExcShared();
    //
    if  ( totalCurrent > totalHWM )
        {
        // Got new HWM
        iInfoHWMExcShared = aLatestInfo;
        }
    }


void CMemSpyEngineProcessMemoryTracker::PrintInfoL( const TMemSpyDriverProcessInspectionInfo& aInfo )
    {
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - START" );
    TBuf<128> timeBuf;
    TTime time( aInfo.iTime );
    MemSpyEngineUtils::FormatTimeSimple( timeBuf, time );
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - got time..." );

    // Output changes
    const TBool noOverwrite = EFalse;
    const TBool noTimeStamp = EFalse;

    // Prepare prefix
    _LIT( KMemSpyProcessMemoryTrackerPrefix, "[Change %05d] " );
    TBuf<25> prefixBuf;
    prefixBuf.Format( KMemSpyProcessMemoryTrackerPrefix, iNumberOfChangesReceived );
    iEngine.Sink().OutputPrefixSetLC( prefixBuf );
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - set prefix" );

    // Resume (or create) a data stream
    _LIT( KMemSpyFolder, "Process Memory Tracking" );
    _LIT( KMemSpyContext, "Process [0x%08x]" );
    HBufC* context = HBufC::NewLC( KMaxFileName );
    TPtr pContext( context->Des() );
    pContext.Format( KMemSpyContext, (TUint) iSID );
    iEngine.Sink().DataStreamBeginL( pContext, KMemSpyFolder, KNullDesC, noOverwrite, noTimeStamp );
    CleanupStack::PopAndDestroy( context );
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - set context" );

    // Preapre peak characters
    const TText KItemPeaked     = '*';
    const TText KItemDidNotPeak = ' ';
    const TText peakStack       = ( iInfoPeaks.iPeakStack       ? KItemPeaked : KItemDidNotPeak );
    const TText peakHeap        = ( iInfoPeaks.iPeakHeap        ? KItemPeaked : KItemDidNotPeak );
    const TText peakChunkLocal  = ( iInfoPeaks.iPeakChunkLocal  ? KItemPeaked : KItemDidNotPeak );
    const TText peakChunkShared = ( iInfoPeaks.iPeakChunkShared ? KItemPeaked : KItemDidNotPeak );
    const TText peakGlobalData  = ( iInfoPeaks.iPeakGlobalData  ? KItemPeaked : KItemDidNotPeak );

    // Write data
    _LIT( KMemSpyMemChangeFormat, "%S - pID: 0x%04x, stack[%c]: %6d, heap[%c]: %10d, local[%c]: %10d, shared[%c]: %10d, globalD[%c]: %10d, totalC: [%10d/%10d], totalHWM: [%10d/%10d] %S" );
    HBufC* buf = HBufC::NewLC( 1024 );

    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - formatting buffer..." );
    buf->Des().Format( KMemSpyMemChangeFormat, &timeBuf, (TUint) iProcessId, 
                                               peakStack, aInfo.iMemoryStack,
                                               peakHeap, aInfo.iMemoryHeap,
                                               peakChunkLocal, aInfo.iMemoryChunkLocal,
                                               peakChunkShared, aInfo.iMemoryChunkShared, 
                                               peakGlobalData, aInfo.iMemoryGlobalData, 
                                               aInfo.TotalIncShared(), aInfo.TotalExcShared(), 
                                               InfoHWMIncShared().TotalIncShared(), InfoHWMExcShared().TotalExcShared(), 
                                               iProcessName );
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - outputting - 1, sinkType: %d", iEngine.Sink().Type() );
    //RDebug::Print( _L("%S"), buf );
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - outputting - 2" );
    iEngine.Sink().OutputLineL( *buf, ETrue );
    CleanupStack::PopAndDestroy( buf );

    // End stream
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - ending stream" );
    iEngine.Sink().DataStreamEndL();

    // Remove prefix
    CleanupStack::PopAndDestroy();
    //RDebug::Printf( "CMemSpyEngineProcessMemoryTracker::PrintInfoL() - END" );
    }



