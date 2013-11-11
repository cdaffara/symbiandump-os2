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

#include <memspy/engine/memspyengineobjectcontainer.h>

// System includes
#include <e32svr.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperstack.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>

// Literal constants
_LIT(KProcessFilter, "*");
_LIT(KEKernProcessName, "ekern");


CMemSpyEngineObjectContainer::CMemSpyEngineObjectContainer( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
	/*
    TFindProcess procFinder( KProcessFilter );
    TFullName nextMatch;
    //
    while( procFinder.Next( nextMatch ) == KErrNone )
        {
        _LIT(KProcListing, "%S");
        RDebug::Print(KProcListing, &nextMatch);


        _LIT(KFindMask, "*");
        nextMatch += KFindMask;
        //
        TFindThread threadFinder( nextMatch );
        //
        while( threadFinder.Next( nextMatch ) == KErrNone )
            {
            _LIT(KThreadListing, "+--- %S");
            RDebug::Print(KThreadListing, &nextMatch);
            }
        }
	*/
    }


CMemSpyEngineObjectContainer::~CMemSpyEngineObjectContainer()
    {
    DestroyProcesses( iGarbage );
    DestroyProcesses( iProcesses );
    delete iUndertaker;
    delete iMidwife;
    delete iIdleNotifyContainerChanged;
    }


void CMemSpyEngineObjectContainer::ConstructL( const TDesC& aFilter )
    {
    iUndertaker = CMemSpyEngineUndertaker::NewL( iEngine.Driver() );
    iUndertaker->AddObserverL( *this );
    //
    iMidwife = CMemSpyEngineMidwife::NewL( iEngine.Driver() );
    iMidwife->AddObserverL( *this );
    //
    iIdleNotifyContainerChanged = CIdle::NewL( CActive::EPriorityIdle );
    //
    RefreshL( aFilter );
    }


CMemSpyEngineObjectContainer* CMemSpyEngineObjectContainer::NewL( CMemSpyEngine& aEngine )
    {
    return NewL( KProcessFilter, aEngine );
    }


CMemSpyEngineObjectContainer* CMemSpyEngineObjectContainer::NewL( const TDesC& aFilter, CMemSpyEngine& aEngine )
    {
    CMemSpyEngineObjectContainer* self = new(ELeave) CMemSpyEngineObjectContainer( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aFilter );
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::Count() const
    {
    return iProcesses.Count();
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::CountAll() const
    {
    TInt ret = 0;
    //
    const TInt count = iProcesses.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyProcess* process = iProcesses[ i ];
        ret += process->Count();
        }
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineObjectContainer::RefreshL()
    {
    RefreshL( KProcessFilter );
    }


EXPORT_C void CMemSpyEngineObjectContainer::RefreshL( const TDesC& aFilter )
    {
    LocateProcessesL( aFilter );
    }


EXPORT_C CMemSpyProcess& CMemSpyEngineObjectContainer::At( TInt aIndex ) const
    {
    CMemSpyProcess* ret = iProcesses[ aIndex ];
    return *ret;
    }


EXPORT_C CMemSpyProcess& CMemSpyEngineObjectContainer::ProcessByIdL( TProcessId aId ) const
    {
    CMemSpyProcess* ret = NULL;
    //
    const TInt count = iProcesses.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyProcess* process = iProcesses[ i ];
        if  ( process->Id() == aId )
            {
            ret = process;
            break;
            }
        }
    //
    if  ( ret == NULL )
        {
        User::Leave( KErrNotFound );
        }
    //
    return *ret;
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::ProcessIndexById( TProcessId aId ) const
    {
    TInt index = KErrNotFound;
    //
    const TInt count = iProcesses.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyProcess* process = iProcesses[ i ];
        if  ( process->Id() == aId )
            {
            index = i;
            break;
            }
        }
    //
    return index;
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::ProcessAndThreadByThreadId( TThreadId aTid, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread ) const
    {
    aProcess = NULL;
    aThread = NULL;
    TInt error = KErrNotFound;
    //
    const TInt count = iProcesses.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyProcess* process = iProcesses[ i ];

        // Check whether this process contains the thread we are looking for...
        const TInt index = process->ThreadIndexById( aTid );
        if  ( index >= 0 )
            {
            // Found it
            aProcess = process;
            aThread = &process->At( index );
            error = KErrNone;
            break;
            }
        }
    //
    if  ( error == KErrNotFound )
        {
        CMemSpyEngineObjectContainer* self = const_cast< CMemSpyEngineObjectContainer* >( this );
        TRAP( error, self->TryToCreateProcessAndThreadL( aTid, aProcess, aThread ) );
        }
    //
    return error;
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::ProcessAndThreadByFullName( const TDesC& aFullName, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread ) const
    {
    aProcess = NULL;
    aThread = NULL;
    TInt error = KErrNotFound;
    //
    const TInt count = iProcesses.Count();
    for( TInt i=0; i<count && aThread == NULL; i++ )
        {
        CMemSpyProcess& process = *iProcesses[ i ];
        const TInt threadCount = process.Count();
        //
        for(TInt j=0; j<threadCount; j++)
            {
            CMemSpyThread& thread = process.At( j );
            const TFullName threadName( thread.FullName() );
            //
            if  ( threadName.CompareF( aFullName ) == 0 )
                {
                // Found it
                aProcess = &process;
                aThread = &thread;
                error = KErrNone;
                break;
                }
            }

        }
    //
    if  ( error == KErrNotFound )
        {
        // NB: cannot use driver API as we must open thread by name, and that only supports opening
        // by id.
        RThread thread;
        error = thread.Open( aFullName );
        if ( error == KErrNone )
            {
            const TThreadId threadId = thread.Id();
            thread.Close();

            CMemSpyEngineObjectContainer* self = const_cast< CMemSpyEngineObjectContainer* >( this );
            TRAP( error, self->TryToCreateProcessAndThreadL( threadId, aProcess, aThread ) );
            }
        }
    //
    return error;
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::ProcessAndThreadByPartialName( const TDesC& aPartialName, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread ) const
    {
    aProcess = NULL;
    aThread = NULL;
    TInt error = KErrNotFound;
    //
    const TInt count = iProcesses.Count();
    for( TInt i=0; i<count && aThread == NULL; i++ )
        {
        CMemSpyProcess& process = *iProcesses[ i ];
        const TInt threadCount = process.Count();
        //
        for(TInt j=0; j<threadCount; j++)
            {
            CMemSpyThread& thread = process.At( j );
            const TFullName threadName( thread.FullName() );
            //
            if  ( threadName.FindF( aPartialName ) >= 0 )
                {
                // Found it
                aProcess = &process;
                aThread = &thread;
                error = KErrNone;
                break;
                }
            }

        }
    //
    return error;
    }


EXPORT_C TBool CMemSpyEngineObjectContainer::IsAlive( TProcessId aPid ) const
    {
    const TInt processIndex = ProcessIndexById( aPid );
    return ( processIndex >= 0 );
    }


EXPORT_C TBool CMemSpyEngineObjectContainer::IsAlive( TProcessId aPid, TThreadId aTid ) const
    {
    TBool isAlive = IsAlive( aPid );
    if  ( isAlive )
        {
        const TInt processIndex = ProcessIndexById( aPid );
        const CMemSpyProcess& process = At( processIndex );

        // Check whether the thread is alive
        isAlive = ( process.ThreadIndexById( aTid ) >= 0 );
        }
    //
    return isAlive;
    }


EXPORT_C TInt CMemSpyEngineObjectContainer::MdcaCount() const
    {
    return iProcesses.Count();
    }


EXPORT_C TPtrC CMemSpyEngineObjectContainer::MdcaPoint(TInt aIndex) const
    {
    const CMemSpyProcess* process = iProcesses[ aIndex ];
    return TPtrC( process->NameForListBox() );
    }


void CMemSpyEngineObjectContainer::LocateProcessesL( const TDesC& aFilter )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineObjectContainer::LocateProcessesL() - START" );
#endif
    DestroyProcesses( iProcesses );
    //
    const TProcessId myProcessId = RProcess().Id();
	//
    TFindProcess finder( aFilter );
    TFullName nextMatch;
    //
    while( finder.Next( nextMatch ) == KErrNone )
        {
        if  ( nextMatch.FindF(KEKernProcessName) == KErrNotFound )
            {
#ifdef _DEBUG
            RDebug::Print(_L( "CMemSpyEngineObjectContainer::LocateProcessesL() - found Proc: %S"), &nextMatch );
#endif

            RProcess p;
            const TInt r = p.Open( nextMatch );
            TProcessId processId( KNullProcessId );
            if ( r == KErrNone )
                {
                processId = p.Id();
                }
            p.Close();

#ifdef _DEBUG
            RDebug::Printf( "CMemSpyEngineObjectContainer::LocateProcessesL() - open err: %d", r );
#endif

            if  ( r == KErrNone )
                {
                if  ( processId != myProcessId )
                    {
                    TRAPD( err, CreateProcessL( processId ) );
#ifdef _DEBUG
                    RDebug::Printf( "CMemSpyEngineObjectContainer::LocateProcessesL() - CreateProcessL err: %d", err );
#endif
                    err = err;
                    }
                }
            }
        }
    //
    SortByName();

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineObjectContainer::LocateProcessesL() - END" );
#endif
    }


CMemSpyProcess* CMemSpyEngineObjectContainer::CreateProcessL( const TProcessId& aId )
    {
#ifdef _DEBUG
	RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::CreateProcessL() - START - pid: 0x%04x", (TUint) aId );
#endif

    CMemSpyProcess* processObj = CMemSpyProcess::NewLC( aId, iEngine );
    const TPtrC pName( processObj->Name() );

    if  ( pName.FindF( KEKernProcessName ) == KErrNotFound )
        {
        if  ( processObj->Count() == 0 )
            {
            // Calls delete
#ifdef _DEBUG
	        RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::CreateProcessL() - process has no threads... deleting it!" );
#endif
            processObj->Close();
            processObj = NULL;
            }
        else
            {
            // Make sure we don't add duplicate processes...
            AppendL( processObj );
            }
        }
    else
        {
        // We don't support kernel-side threads
#ifdef _DEBUG
        RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::CreateProcessL() - kernel process... deleting it!" );
#endif
        processObj->Close();
        processObj = NULL;
        }

    CleanupStack::Pop();

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::CreateProcessL() - END - pid: 0x%04x, processObj: 0x%08x", (TUint) aId, processObj );
#endif
    return processObj;
    }


void CMemSpyEngineObjectContainer::DestroyProcesses( RArray< CMemSpyProcess* >& aList )
    {
    while( aList.Count() ) 
        {
        CMemSpyProcess* process = aList[ 0 ];
        aList.Remove( 0 );
        process->Close();
        }

    aList.Close();
    }


void CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL( const TThreadId& aTid, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread )
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(1) - START - aTid: 0x%04x", (TUint) aTid );
#endif

    // Must open thread and process in order to get process id.
    RThread thread;
    User::LeaveIfError( iEngine.Driver().OpenThread( aTid, thread ) );
    CleanupClosePushL( thread );
    RProcess process;
    User::LeaveIfError( thread.Process( process ) );
    const TProcessId processId = process.Id();
    process.Close();

    // Call overload
    TryToCreateProcessAndThreadL( thread, aProcess, aThread );

    CleanupStack::PopAndDestroy( &thread );

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(1) - END - aTid: 0x%04x", (TUint) aTid );
#endif
    }


void CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL( const RThread& aRThread, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread )
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - START - aTid: 0x%04x", (TUint) aRThread.Id() );
#endif

    // Full name of process
    TFullName* name = new(ELeave) TFullName();
    CleanupStack::PushL( name );

    // Must open thread and process in order to get process id.
    RProcess process;
    User::LeaveIfError( aRThread.Process( process ) );
    process.FullName( *name );
    const TProcessId processId = process.Id();
    process.Close();

    const TBool isKernel = ( name->FindF( KEKernProcessName ) >= KErrNone );

#ifdef _DEBUG
    RDebug::Print( _L("[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - pid: 0x%04x, name: %S"), (TUint) processId, name );
#endif

    CleanupStack::PopAndDestroy( name );

    // See if we have already created a process for this process id...
    CMemSpyProcess* processObj = NULL;
    const TInt procIndex = ProcessIndexById( processId );

#ifdef _DEBUG
    RDebug::Print( _L("[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - pid: 0x%04x, index: %d"), (TUint) processId, procIndex );
#endif

    if  ( isKernel )
        {
#ifdef _DEBUG
        RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - caller requested a kernel thread/process..." );
#endif
        __ASSERT_ALWAYS( procIndex == KErrNotFound, MemSpyEngineUtils::Panic( EMemSpyEnginePanicEncounteredKernelUnexpectedly ) );

#ifdef _DEBUG
        RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - leaving with KErrNotSupported" );
#endif
        User::Leave( KErrNotSupported );
        }
    else if ( aRThread.Id() == RThread().Id() )
        {
        TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - trying to create MemSpy -> leaving with KErrNotSupported" ) );
        User::Leave( KErrNotSupported );
        }
    else
        {
        if  ( procIndex >= 0 )
            {
            // Existing process, but new thread?
            processObj = iProcesses[ procIndex ];

            // Make new thread
            processObj->HandleThreadIsBornL( aRThread.Id() );
            }
        else
            {
            // Must also create a new process. This should, in theory, ensure
            // the thread is also created.
            processObj = CMemSpyProcess::NewLC( processId, iEngine );
            if  ( processObj->Count() == 0 )
                {
                // No threads, discard process and leave
                User::Leave( KErrNotFound );
                }
            else
                {
                // Save process
                AppendL( processObj );
                }

            // Tidy up
            CleanupStack::Pop();
            }

        // Check to see if the process contains the specified thread (it should do, since we just found
        // the process using it).
        CMemSpyThread& threadObj = processObj->ThreadByIdL( aRThread.Id() );

        // We're done now.
        aProcess = processObj;
        aThread = &threadObj;

        // Update sort order
        Resort();

        // Signal UI        
        AsyncNotifyUiOfContainerChanges();
        }

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::TryToCreateProcessAndThreadL(2) - END - aTid: 0x%04x, pid: 0x%04x", (TUint) aRThread.Id(), (TUint) processId );
#endif
    }


void CMemSpyEngineObjectContainer::Resort()
    {
    switch( iSortType )
        {
    case ESortById:
        {
        TLinearOrder< CMemSpyProcess* > comparer( CompareById );
        iProcesses.Sort( comparer );
        }
        break;
    default:
    case ESortByName:
        {
        TLinearOrder< CMemSpyProcess* > comparer( CompareByName );
        iProcesses.Sort( comparer );
        }
        break;
    case ESortByThreadCount:
        {
        TLinearOrder< CMemSpyProcess* > comparer( CompareByThreadCount );
        iProcesses.Sort( comparer );
        }
        break;
    case ESortByCodeSegs:
        {
        TLinearOrder< CMemSpyProcess* > comparer( CompareByCodeSegs );
        iProcesses.Sort( comparer );
        }
        break;
    case ESortByHeapUsage:
        {
        TLinearOrder< CMemSpyProcess* > comparer( CompareByHeapUsage );
        iProcesses.Sort( comparer );
        }
        break;
    case ESortByStackUsage:
        {
        TLinearOrder< CMemSpyProcess* > comparer( CompareByStackUsage );
        iProcesses.Sort( comparer );
        }
        break;
        }
    }


EXPORT_C void CMemSpyEngineObjectContainer::SortById()
    {
    iSortType = ESortById;
    Resort();
    }


EXPORT_C void CMemSpyEngineObjectContainer::SortByName()
    {
    iSortType = ESortByName;
    Resort();
    }


EXPORT_C void CMemSpyEngineObjectContainer::SortByThreadCount()
    {
    iSortType = ESortByThreadCount;
    Resort();
    }


EXPORT_C void CMemSpyEngineObjectContainer::SortByCodeSegs()
    {
    iSortType = ESortByCodeSegs;
    Resort();
    }


EXPORT_C void CMemSpyEngineObjectContainer::SortByHeapUsage()
    {
    iSortType = ESortByHeapUsage;
    Resort();
    }


EXPORT_C void CMemSpyEngineObjectContainer::SortByStackUsage()
    {
    iSortType = ESortByStackUsage;
    Resort();
    }


void CMemSpyEngineObjectContainer::Remove( CMemSpyProcess& aProcess )
    {
    const TInt index = ProcessIndexById( aProcess.Id() );
    if ( index >= 0 )
        {
        iProcesses.Remove( index );
        }
    }
    
    
void CMemSpyEngineObjectContainer::AppendL( CMemSpyProcess* aProcess )
    {
    const TInt error = iProcesses.Append( aProcess );
    User::LeaveIfError( error );    
    }


TInt CMemSpyEngineObjectContainer::CompareById( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight )
    {
    TInt ret = 1;
    //
    if  ( aLeft->Id() < aRight->Id() )
        {
        ret = -1;
        }
    else if ( aLeft->Id() == aRight->Id() )
        {
        ret = 0;
        }
    //
    return ret;
    }


TInt CMemSpyEngineObjectContainer::CompareByThreadCount( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight )
    {
    TInt ret = -1;
    //
    if  ( aLeft->Count() < aRight->Count() )
        {
        ret = 1;
        }
    else if ( aLeft->Count() == aRight->Count() )
        {
        // Sort by name when thread counts are the same
        ret = aLeft->Name().CompareF( aRight->Name() );
        }
    //
    return ret;
    }


TInt CMemSpyEngineObjectContainer::CompareByName( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight )
    {
    return aLeft->Name().CompareF( aRight->Name() );
    }


TInt CMemSpyEngineObjectContainer::CompareByCodeSegs( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight )
    {
    CMemSpyEngine& engine = aLeft->Engine();
    CMemSpyEngineHelperCodeSegment& helper = engine.HelperCodeSegment();
    //
    TInt leftCount = 0;
    TInt rightCount = 0;
    //
    TRAP_IGNORE( 
        CMemSpyEngineCodeSegList* leftList = helper.CodeSegmentListL( aLeft->Id() );
        leftCount = leftList->Count();
        delete leftList;
        //
        CMemSpyEngineCodeSegList* rightList = helper.CodeSegmentListL( aRight->Id() );
        rightCount = rightList->Count();
        delete rightList;
        );
    //
    TInt ret = -1;
    //
    if  ( leftCount < rightCount )
        {
        ret = 1;
        }
    else if ( leftCount == rightCount )
        {
        ret = 0;
        }
    //
    return ret;
    }


TInt CMemSpyEngineObjectContainer::CompareByHeapUsage( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight )
    {
    CMemSpyEngine& engine = aLeft->Engine();
    CMemSpyEngineHelperHeap& helper = engine.HelperHeap();
    //
    TInt leftSize = 0;
    TInt rightSize = 0;
    //
    TRAP_IGNORE( 
        RArray< TMemSpyHeapInfo > leftInfos;
        CleanupClosePushL( leftInfos );
        helper.GetHeapInfoUserL( *aLeft, leftInfos );
        const TInt leftCount = leftInfos.Count();
        for( TInt i=0; i<leftCount; i++ )
            {
            const TMemSpyHeapInfo& info = leftInfos[ i ];
            if ( info.Type() != TMemSpyHeapInfo::ETypeUnknown )
                {
                leftSize += (TInt) info.AsRHeap().MetaData().iHeapSize;
                }
            }
        CleanupStack::PopAndDestroy( &leftInfos );
        );
    //
    TRAP_IGNORE( 
        RArray< TMemSpyHeapInfo > rightInfos;
        CleanupClosePushL( rightInfos );
        helper.GetHeapInfoUserL( *aRight, rightInfos );
        const TInt rightCount = rightInfos.Count();
        for( TInt i=0; i<rightCount; i++ )
            {
            const TMemSpyHeapInfo& info = rightInfos[ i ];
            if ( info.Type() == TMemSpyHeapInfo::ETypeUnknown )
                {
                rightSize += (TInt) info.AsRHeap().MetaData().iHeapSize;
                }
            }
        CleanupStack::PopAndDestroy( &rightInfos );
        );
    //
    TInt ret = -1;
    //
    if  ( leftSize < rightSize )
        {
        ret = 1;
        }
    else if ( leftSize == rightSize )
        {
        ret = 0;
        }
    //
    return ret;
    }


TInt CMemSpyEngineObjectContainer::CompareByStackUsage( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight )
    {
    CMemSpyEngine& engine = aLeft->Engine();
    CMemSpyEngineHelperStack& helper = engine.HelperStack();
    //
    const TInt leftCount = helper.CalculateStackSizes( *aLeft );
    const TInt rightCount = helper.CalculateStackSizes( *aRight );
    //
    TInt ret = -1;
    //
    if  ( leftCount < rightCount )
        {
        ret = 1;
        }
    else if ( leftCount == rightCount )
        {
        ret = 0;
        }
    //
    return ret;
    }


TBool CMemSpyEngineObjectContainer::MoveToGarbageL( const TProcessId& aId )
    {
    const TInt pos = ProcessIndexById( aId );

    if  ( pos >= 0 )
        {
        CMemSpyProcess* proc = iProcesses[ pos ];

#ifdef _DEBUG
        const TPtrC pName( proc->Name() );
        RDebug::Print( _L("[MemSpy] CMemSpyEngineObjectContainer::MoveToGarbageL() - pid: 0x%04x, proc: 0x%08x, pos: %d, id: 0x%04x, name: %S"), (TUint) aId, proc, pos, (TUint) proc->Id(), &pName );
#endif

        User::LeaveIfError( iGarbage.Append( proc ) );
        iProcesses.Remove( pos );
        }
	else
        {
#ifdef _DEBUG
        RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::MoveToGarbageL() - pid: 0x%04x, pos: KErrNotFound", (TUint) aId );
#endif
        }
        
    return ( pos >= 0 );
    }


















void CMemSpyEngineObjectContainer::ThreadIsDeadL( const TThreadId& aId, const RThread& aThread )
    {
    // Try to find the thread in question...
    CMemSpyProcess* process = NULL;
    CMemSpyThread* thread = NULL;
    //
    const TInt err = ProcessAndThreadByThreadId( aId, process, thread );
    if  ( err == KErrNone && thread )
        {
        // This will force the thread to detect if it is dead or not...
        thread->SetDeadL( aThread );

        // Signal UI        
        AsyncNotifyUiOfContainerChanges();
        }
    }
    

void CMemSpyEngineObjectContainer::ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess )
    {
    const TInt index = ProcessIndexById( aId );
    if  ( index >= 0 )
        {
        CMemSpyProcess* process = iProcesses[ index ];
        process->SetDeadL( aProcess );

        // Signal UI        
        AsyncNotifyUiOfContainerChanges();
        }
    }


void CMemSpyEngineObjectContainer::ThreadIsBornL( const TThreadId& /*aId*/, const RThread& aThread )
    {
    if  ( aThread.Handle() != KNullHandle )
        {
        // The thread and process objects that will be created (or found, if they already exist).
        CMemSpyProcess* process = NULL;
        CMemSpyThread* thread = NULL;

        // Create the objects if needed
        TryToCreateProcessAndThreadL( aThread, process, thread );
        }
    }


void CMemSpyEngineObjectContainer::ProcessIsBornL( const TProcessId& aId, const RProcess& /*aProcess*/ )
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::ProcessIsBornL() - START - pid: 0x%04x", (TUint) aId );
#endif
    (void) aId;

    // This gets handled by the new thread creation. When the first thread in the process
    // is created, we'll also prepare a new process object.

#ifdef _DEBUG
	RDebug::Printf( "[MemSpy] CMemSpyEngineObjectContainer::ProcessIsBornL() - END - pid: 0x%04x", (TUint) aId );
#endif
    }
    

void CMemSpyEngineObjectContainer::AsyncNotifyUiOfContainerChanges()
    {
    iIdleNotifyContainerChanged->Cancel();
    iIdleNotifyContainerChanged->Start( TCallBack( NotifyUiOfContainerChanges, this ) );
    }


TBool CMemSpyEngineObjectContainer::NotifyUiOfContainerChanges( TAny* aSelf )
    {
    CMemSpyEngineObjectContainer* self = reinterpret_cast< CMemSpyEngineObjectContainer* >( aSelf );
    TRAP_IGNORE( self->iEngine.NotifyContainerChangeL() );
    return EFalse;
    }


    
