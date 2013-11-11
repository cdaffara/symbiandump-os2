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

#include <memspy/engine/memspyengineobjectprocess.h>

// System includes
#include <e32svr.h>
#include <u32std.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverpanics.h> // for terminate

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectthread.h>

// Literal constants
_LIT( KMemSpyUnknownProcessName, "Unknown" );
_LIT( KMemSpyUnknownExitCategory, "Unknown ExitCat" );
_LIT( KMemSpyStandardProcessExtension, ".exe" );


CMemSpyProcess::CMemSpyProcess( TProcessId aId, CMemSpyEngine& aEngine  )
:   iId( aId ), iEngine( aEngine )
    {
    }


CMemSpyProcess::~CMemSpyProcess()
    {
#ifdef _DEBUG
    if  ( iName != NULL )
        {
        RDebug::Print(_L( "MEMSPY - dtor - CMemSpyProcess() - this: 0x%08x, name: %S"), this, iName);
        }
    else
        {
        RDebug::Printf( "MEMSPY - dtor - CMemSpyProcess() - this: 0x%08x", this );
        }
#endif

    CloseAllThreads();
    iThreads.Close();
    delete iName;
    delete iInfo;
    }


void CMemSpyProcess::ConstructL()
    {
    iInfo = new(ELeave) TMemSpyDriverProcessInfo();

    RMemSpyDriverClient& driver = iEngine.Driver();
    RProcess process;
    User::LeaveIfError( driver.OpenProcess( iId, process ) );
    CleanupClosePushL( process );

    // Find the threads before we start tinkering with the process
    // name
    LocateThreadsL( process );

    // Now build the process name
    RefreshL( process );

    CleanupStack::PopAndDestroy( &process );

    
#ifdef _DEBUG
    if  ( iName != NULL )
        {
        RDebug::Print(_L( "MEMSPY - ctor - CMemSpyProcess() - this: 0x%08x, name: %S"), this, iName);
        }
    else
        {
        RDebug::Printf( "MEMSPY - ctor - CMemSpyProcess() - this: 0x%08x", this );
        }
#endif
    }


EXPORT_C CMemSpyProcess* CMemSpyProcess::NewL( const CMemSpyProcess& aCopyMe )
    {
    CMemSpyProcess* self = NewL( aCopyMe.Id(), aCopyMe.iEngine );
    return self;
    }


CMemSpyProcess* CMemSpyProcess::NewL( TProcessId aId, CMemSpyEngine& aEngine )
    {
    CMemSpyProcess* self = CMemSpyProcess::NewL( aId, aEngine );
    CleanupStack::Pop( self );
    return self;
    }


CMemSpyProcess* CMemSpyProcess::NewLC( TProcessId aId, CMemSpyEngine& aEngine )
    {
    CMemSpyProcess* self = new(ELeave) CMemSpyProcess( aId, aEngine );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }


EXPORT_C void CMemSpyProcess::Close()
    {
    if  ( !OpenOrCloseInProgress() )
        {
        const TInt ac = AccessCount();
        SetOpenOrCloseInProgress( ETrue );

        // Calling close can do a "delete this" so make sure
        // we don't try to access the object again in that situation...
        CMemSpyEngineObject::Close();
        if  ( ac - 1 > 0 )
            {
            SetOpenOrCloseInProgress( EFalse );
            }
        else
            {
            // We don't care - we've just been deleted!
            }
        }
    }


EXPORT_C void CMemSpyProcess::Open()
    {
    if  ( !OpenOrCloseInProgress() )
        {
        SetOpenOrCloseInProgress( ETrue );
        CMemSpyEngineObject::Open();
        SetOpenOrCloseInProgress( EFalse );
        }
    }


EXPORT_C TInt CMemSpyProcess::MdcaCount() const
    {
    return iThreads.Count();
    }


EXPORT_C TPtrC CMemSpyProcess::MdcaPoint(TInt aIndex) const
    {
    const CMemSpyThread* thread = iThreads[ aIndex ];
    return TPtrC( thread->NameForListBox() );
    }


EXPORT_C TInt CMemSpyProcess::ThreadIndexById( TThreadId aId ) const
    {
    TInt index = KErrNotFound;
    //
    const TInt count = iThreads.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyThread* thread = iThreads[ i ];
        if  ( thread->Id() == aId )
            {
            index = i;
            break;
            }
        }
    //
    return index;
    }


EXPORT_C CMemSpyThread& CMemSpyProcess::ThreadByIdL( TThreadId aId ) const
    {
    CMemSpyThread* ret = NULL;
    //
    const TInt count = iThreads.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyThread* thread = iThreads[ i ];
        if  ( thread->Id() == aId )
            {
            ret = thread;
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



EXPORT_C TPtrC CMemSpyProcess::Name() const
    {
    // Just return the pure name, minus the leading tab
    TPtrC pRet( iName->Mid(2) );
    
    // Find the last tab position
    TInt pos = pRet.Locate(TChar('\t'));
    if  ( pos > 0 )
        {
        pRet.Set( pRet.Left( pos ) );
        }
    //    
    return pRet;
    }


EXPORT_C CMemSpyThread& CMemSpyProcess::At( TInt aIndex ) const
    {
    CMemSpyThread* ret = iThreads[ aIndex ];
    return *ret;
    }


EXPORT_C void CMemSpyProcess::TerminateL()
    {
    RMemSpyDriverClient& driver = iEngine.Driver();
    RProcess process;
    User::LeaveIfError( driver.OpenProcess( iId, process ) );
    process.Terminate( EPanicForcedTerminate );
    process.Close();
    //
    RefreshL();
    }


EXPORT_C void CMemSpyProcess::KillL()
    {
    RMemSpyDriverClient& driver = iEngine.Driver();
    RProcess process;
    User::LeaveIfError( driver.OpenProcess( iId, process ) );
    process.Kill( EPanicForcedKill );
    process.Close();
    //
    RefreshL();
    }


EXPORT_C void CMemSpyProcess::PanicL()
    {
    RMemSpyDriverClient& driver = iEngine.Driver();
    RProcess process;
    User::LeaveIfError( driver.OpenProcess( iId, process ) );
    process.Panic( KMemSpyClientPanic, EPanicForcedPanic );
    process.Close();
    //
    RefreshL();
    }


EXPORT_C TBool CMemSpyProcess::IsSystemPermanent() const
    {
    const TBool ret = ( iInfo->iFlags & KProcessFlagSystemPermanent );
    return ret;
    }


EXPORT_C TBool CMemSpyProcess::IsSystemCritical() const
    {
    const TBool ret = ( iInfo->iFlags & KProcessFlagSystemCritical );
    return ret;
    }


void CMemSpyProcess::LocateThreadsL( RProcess& aProcess )
    {
    __ASSERT_ALWAYS( aProcess.Handle() != KNullHandle, MemSpyEngineUtils::Panic( EMemSpyEnginePanicProcessHandleNullWhenAttemptingToIdentifyThreads ) );

#ifdef _DEBUG
    RDebug::Printf("CMemSpyProcess::LocateThreadsL() - START - this: 0x%08x, pid: %d (0x%04x)", this, (TUint) aProcess.Id(), (TUint) aProcess.Id() );
#endif

    RArray<TThreadId> threadIds;
    CleanupClosePushL( threadIds );

    // Get list of child threads from driver.
    iEngine.Driver().GetThreadsL( aProcess.Id(), threadIds );

    // Create therad object for each thread the driver returned to us...
    const TInt count = threadIds.Count();
#ifdef _DEBUG
    RDebug::Printf("CMemSpyProcess::LocateThreadsL() - got %d threads", count );
#endif

    for( TInt i=0; i<count; i++ )
        {
        const TThreadId threadId( threadIds[ i ] );
#ifdef _DEBUG
        RDebug::Printf("CMemSpyProcess::LocateThreadsL() - thread id: %d (0x%04d)", (TUint) threadId, (TUint) threadId );
#endif
        //
        TRAP_IGNORE( 
            CMemSpyThread* threadObj = CMemSpyThread::NewLC( threadId, *this );
            iThreads.AppendL( threadObj );
            CleanupStack::Pop( threadObj );
            );
        }

    // Tidy up
    CleanupStack::PopAndDestroy( &threadIds );
    
#ifdef _DEBUG
    RDebug::Printf("CMemSpyProcess::LocateThreadsL() - END - this: 0x%08x, pid: %d (0x%04x), thread count: %d", this, (TUint) aProcess.Id(), (TUint) aProcess.Id(), iThreads.Count() );
#endif
    }


void CMemSpyProcess::AppendPriority( TDes& aDes, TProcessPriority aPriority )
    {
    switch( aPriority )
        {
    case EPriorityLow:
        aDes += _L("[L]");
        break;
    case EPriorityBackground:
        aDes += _L("[B]");
        break;
    case EPriorityForeground:
        aDes += _L("[F]");
        break;
    case EPriorityHigh:
        aDes += _L("[H]");
        break;
    case EPriorityWindowServer:
        aDes += _L("[WS]");
        break;
    case EPriorityFileServer:
        aDes += _L("[FS]");
        break;
    case EPriorityRealTimeServer:
        aDes += _L("[RTS]");
        break;
    case EPrioritySupervisor:
        aDes += _L("[SUP]");
        break;
    default:
        aDes += _L("[?]");
        break;
        }
    }


void CMemSpyProcess::GetFileName( TFileName& aFileName )
    {
    // Fallback
    const TPtrC pNameCleaned( Name() );
    aFileName.Zero();
    aFileName.AppendFormat( _L("%S.exe"), &pNameCleaned );

    // Try to get the proper name
    RProcess process;
    RMemSpyDriverClient& driver = iEngine.Driver();
    if  ( driver.OpenProcess( iId, process ) == KErrNone )
        {
        aFileName = process.FileName();
        process.Close();
        }
    }

void CMemSpyProcess::RefreshL()
    {
    RMemSpyDriverClient& driver = iEngine.Driver();
    RProcess process;

    // Deliberately ignore error - the other overload of RefreshL can cope with
    // a null handle.
    driver.OpenProcess( iId, process );
    CleanupClosePushL( process );

    RefreshL( process );

    CleanupStack::PopAndDestroy( &process );
    }


void CMemSpyProcess::RefreshL( const RProcess& aProcess )
    {
    const TBool handleValid = aProcess.Handle() != KNullHandle;
    if  ( handleValid )
        {
        RMemSpyDriverClient& driver = iEngine.Driver();
        User::LeaveIfError( driver.GetProcessInfo( iId, *iInfo ) );
        }

    // Get priority, exit info etc
    iExitType = handleValid ? aProcess.ExitType() : EExitKill;
    iPriority = handleValid ? aProcess.Priority() : EPriorityForeground;

    // If the process is dead then we may not be able to get some attributes
    // (it depends on whether the thread handle is valid anymore).
    iExitReason = 0;
    iExitCategory.Zero();

    if  ( IsDead() )
        {
        if  ( handleValid )
            {
            iExitReason = aProcess.ExitReason();
            iExitCategory = aProcess.ExitCategory(); 
            }
        else
            {
            iExitCategory = KMemSpyUnknownExitCategory;
            }
        }
    else
        {
        // We only reset the flags if the process is still alive.
        // If it is dead (i.e. 'if' branch) then we do not reset because
        // we have no way to fetch them any more.
        iFlags = EFlagsNone;
        }

    // Get raw process name and check whether it includes .exe suffix
    TBool includesExecutableSuffix = EFalse;
    HBufC* rawProcessName = GetProcessNameLC( aProcess, includesExecutableSuffix );
    if ( includesExecutableSuffix )
        {
        iFlags |= EFlagsIncludedExecutableWithinName;
        }

    // Format priority as string
    TBuf<10> priority;
    AppendPriority( priority, iPriority );

    // Convert the full name to the format we want in the UI
    TBuf<KMaxFullName + 60> name;
    TMemSpyTruncateOverflow overflow;

    // FOR ALIVE PROCESSES:
    //
    // 1] Space, followed by tab
    // 2] Process name (minus .exe)
    // 3] Tab, Tab
    // 4] Process uid
    // 5] Thread count
    // 6] Process priority
    //
    // FOR DEAD PROCESSES:
    //
    // 1] Space, followed by tab
    // 2] Process name (minus .exe)
    // 3] Tab, Tab
    // 4] Process uid
    // 5] Exit info

    // Common
    _LIT( KMemSpyProcessNameFormatSpecBasicName, " \t%S\t\t%8x, " );
    name.AppendFormat( KMemSpyProcessNameFormatSpecBasicName, &overflow, rawProcessName, iInfo->SID() );
    CleanupStack::PopAndDestroy( rawProcessName );

    if  ( IsDead() )
        {
        CMemSpyThread::AppendExitInfo( name, iExitType, iExitReason, iExitCategory );
        }
    else
        {
        _LIT( KMemSpyProcessNameFormatSpecAlive, "%2d thr, %S" );
        name.AppendFormat( KMemSpyProcessNameFormatSpecAlive, &overflow, iThreads.Count(), &priority );
        }

    // Save end result
    HBufC* finalName = name.AllocL();
    delete iName;
    iName = finalName;
    }


void CMemSpyProcess::SetDeadL()
    {
    RefreshL();
    }


void CMemSpyProcess::SetDeadL( const RProcess& aProcess )
    {
    RefreshL( aProcess );
    }


EXPORT_C TBool CMemSpyProcess::IsDead() const
    {
    const TBool isDead = ( iExitType != EExitPending );
    return  isDead;
    }


EXPORT_C TUint32 CMemSpyProcess::SID() const
    {
    return iInfo->SID();
    }


EXPORT_C TUint32 CMemSpyProcess::VID() const
    {
    return iInfo->VID();
    }

EXPORT_C TProcessPriority CMemSpyProcess::Priority() const
	{
	return iPriority;
	}

EXPORT_C TExitCategoryName CMemSpyProcess::ExitCategory() const
	{
	return iExitCategory;
	}

EXPORT_C TInt CMemSpyProcess::ExitReason() const
	{
	return iExitReason;
	}

EXPORT_C TExitType CMemSpyProcess::ExitType() const
	{
	return iExitType;
	}

EXPORT_C TUidType CMemSpyProcess::UIDs() const
    {
    return iInfo->iUids;
    }


EXPORT_C SCapabilitySet CMemSpyProcess::Capabilities() const
    {
    return iInfo->iSecurityInfo.iCaps;
    }

void CMemSpyProcess::FullName( TDes& aFullName ) const
    {
    // c32exe.exe[101f7989]0001
    aFullName.Zero();
    aFullName.Append( Name() );
    if ( iFlags & EFlagsIncludedExecutableWithinName )
        {
        // Add .exe
        aFullName.Append( KMemSpyStandardProcessExtension );
        }

    aFullName.Append( '[' );
	aFullName.AppendNumFixedWidth( iInfo->iUids[ 2 ].iUid, EHex, 8 );
    aFullName.Append( ']' );
	aFullName.AppendNumFixedWidth( iInfo->iGeneration, EDecimal, 4 );
    }


void CMemSpyProcess::HandleThreadIsBornL( const TThreadId& aId )
    {
    // A new thread has been created in this process. Just in case, we'll
    // check there isn't already an existing thread with the same id...
    const TInt index = ThreadIndexById( aId );
    if  ( index < 0 )
        {
        TRAP_IGNORE( 
            CMemSpyThread* threadObj = CMemSpyThread::NewLC( aId, *this );
            iThreads.AppendL( threadObj );
            CleanupStack::Pop( threadObj );
            );
        }
    }


HBufC* CMemSpyProcess::GetProcessNameLC( const RProcess& aProcessOrNull, TBool& aProcessNameIncludesExeSuffix ) const
    {
    _LIT( KMemSpySquareBraceOpen, "[" );
    //
    TFullName processName;
    
    // Assume the flags have already been set once previously in order to form default response
    aProcessNameIncludesExeSuffix = ( iFlags & EFlagsIncludedExecutableWithinName );

    const TBool handleValid = aProcessOrNull.Handle() != KNullHandle;
    if  ( handleValid )
        {
        // Easy case - we have a valid thread handle...
        //
        // Get full name, e.g.:
        //
        // c32exe.exe[101f7989]0001
        aProcessOrNull.FullName( processName );

        // ... but we need to clean up the name so that it
        // doesn't include the process UID, and neither does it
        // include the extension (.exe).
        TInt pos = processName.FindF( KMemSpySquareBraceOpen );
        if  ( pos > 0 )
            {
            processName.SetLength( pos );
            }
        
        // Discard '.exe'
        pos = processName.FindF( KMemSpyStandardProcessExtension );
        if  ( pos > 0 )
            {
            aProcessNameIncludesExeSuffix = ETrue;
            processName.SetLength( pos );
            }
        }
    else
        {
        // Since we don't have the possibility to enquire after the process's name
        // we'll assume that it used to be alive and therefore at some point we did
        // manage to grep it's name...
        if  ( iName )
            {
            const TPtrC pOriginalName( Name() );
            processName.Append( pOriginalName );
            }
        else
            {
            // Don't have a process handle, don't have any possibility to get the
            // name from a prior cached version. Must use "unknown"
            processName.Append( KMemSpyUnknownProcessName );
            }
        }
    //
    HBufC* ret = processName.AllocLC();
    return ret;
    }


void CMemSpyProcess::CloseAllThreads()
    {
    const TInt count = iThreads.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        CMemSpyThread* thread = iThreads[ i ];
        thread->Close();
        }
    }

