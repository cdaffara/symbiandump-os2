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

#include <memspy/engine/memspyengineobjectthread.h>

// System includes
#include <e32svr.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>

// Constants
_LIT( KMemSpyUnknownExitCategory, "Unknown ExitCat" );
_LIT( KMemSpyUnknownThreadName, "Unknown Thread" );
_LIT( KMemSpyThreadDoubleColon, "::" );


CMemSpyThread::CMemSpyThread( TThreadId aId, CMemSpyProcess& aProcess )
:   CMemSpyEngineObject( aProcess ), iId( aId ), iProcess( &aProcess )
    {
    }


CMemSpyThread::~CMemSpyThread()
    {
    if  ( iInfoContainer )
        {
        iInfoContainer->Close();
        }
    delete iName;
    }


void CMemSpyThread::ConstructL()
    {
    RefreshL();
    }


CMemSpyThread* CMemSpyThread::NewL( TThreadId aId, CMemSpyProcess& aProcess )
    {
    CMemSpyThread* self = CMemSpyThread::NewLC( aId, aProcess );
    CleanupStack::Pop( self );
    return self;
    }


CMemSpyThread* CMemSpyThread::NewLC( TThreadId aId, CMemSpyProcess& aProcess )
    {
    CMemSpyThread* self = new(ELeave) CMemSpyThread( aId, aProcess );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C void CMemSpyThread::Open()
    {
    if  ( !OpenOrCloseInProgress() )
        {
        SetOpenOrCloseInProgress( ETrue );
        CMemSpyEngineObject::Open();
        SetOpenOrCloseInProgress( EFalse );
        }
    }


EXPORT_C void CMemSpyThread::Close()
    {
    if  ( !OpenOrCloseInProgress() )
        {
        SetOpenOrCloseInProgress( ETrue );
        CMemSpyEngineObject::Close();
        SetOpenOrCloseInProgress( EFalse );
        }
    }


void CMemSpyThread::AppendPriority( TDes& aDes, TThreadPriority aPriority )
    {
    switch( aPriority )
        {
    case EPriorityNull:
        aDes += _L("[Null]");
        break;
    case EPriorityMuchLess:
        aDes += _L("[Much Less]");
        break;
    case EPriorityLess:
        aDes += _L("[Less]");
        break;
    case EPriorityNormal:
        aDes += _L("[Normal]");
        break;
    case EPriorityMore:
        aDes += _L("[More]");
        break;
    case EPriorityMuchMore:
        aDes += _L("[Much More]");
        break;
    case EPriorityRealTime:
        aDes += _L("[Real Time]");
        break;

    // Absolute values
    case EPriorityAbsoluteVeryLow:
        aDes += _L("[Abs Very Low]");
        break;
    case EPriorityAbsoluteLowNormal:
        aDes += _L("[Abs Low Norm]");
        break;
    case EPriorityAbsoluteLow:
        aDes += _L("[Abs Low]");
        break;
    case EPriorityAbsoluteBackground:
        aDes += _L("[Abs Bgnd]");
        break;
    case EPriorityAbsoluteBackgroundNormal:
        aDes += _L("[Abs Bgnd Norm]");
        break;
    case EPriorityAbsoluteForeground:
        aDes += _L("[Abs Fgnd]");
        break;
    case EPriorityAbsoluteForegroundNormal:
        aDes += _L("[Abs Fgnd Norm]");
        break;
    case EPriorityAbsoluteHigh:
        aDes += _L("[Abs High]");
        break;
    case EPriorityAbsoluteHighNormal:
        aDes += _L("[Abs High Norm]");
        break;
    case EPriorityAbsoluteRealTime1:
    case EPriorityAbsoluteRealTime2:
    case EPriorityAbsoluteRealTime3:
    case EPriorityAbsoluteRealTime4:
    case EPriorityAbsoluteRealTime5:
    case EPriorityAbsoluteRealTime6:
    case EPriorityAbsoluteRealTime7:
    case EPriorityAbsoluteRealTime8:
        aDes.AppendFormat( _L("[Abs RT %d]"), ( aPriority - EPriorityAbsoluteRealTime1 ) + 1 );
        break;
    default:
        aDes += _L("[Unknown Pri.]");
        break;
        }
    }


void CMemSpyThread::AppendExitType( TDes& aDes, TExitType aType )
    {
    _LIT( KExitTypeKilled, "Killed" );
    _LIT( KExitTypeTerminated, "Terminated" );
    _LIT( KExitTypePanicked, "Panicked" );
    _LIT( KExitTypePending, "Pending" );
    
    // Panic and Terminate are exceptional exit conditions.
    // Kill, is ironically, not an exceptional condition.
    switch( aType )
        {
    case EExitKill:
        aDes += KExitTypeKilled;
        break;
    case EExitTerminate:
        aDes += KExitTypeTerminated;
        break;
    case EExitPanic:
        aDes += KExitTypePanicked;
        break;
    default:
    case EExitPending:
        aDes += KExitTypePending;
        break;
        }
    }


void CMemSpyThread::AppendExitInfo( TDes& aDes, TExitType aType, TInt aExitReason, const TDesC& aExitCategory )
    {
    aDes.Append( '[' );
    const TInt length = aDes.Length();
    AppendExitType( aDes, aType );
    aDes.SetLength( length + 1 ); // Remove all but the first letter
    aDes.Append( ']' );
    
    if  ( aType == EExitKill || aType == EExitPending )
        {
        // Kill implies "clean" exit. Pending implies not yet dead.
        }
    else
        {
        TMemSpyTruncateOverflow overflow;

        // Terminate or Panic implies abnormal exit condition, so
        // show full exit info.
        _LIT( KAbnormalFormatSpec, " %S-%d" );
        aDes.AppendFormat( KAbnormalFormatSpec, &overflow, &aExitCategory, aExitReason );
        }
    }


CMemSpyEngine& CMemSpyThread::Engine() const
    {
    return Process().Engine();
    }


void CMemSpyThread::OpenLC( RThread& aThread )
    {
    const TInt error = Open( aThread );
    User::LeaveIfError( error );
    CleanupClosePushL( aThread );
    }


TInt CMemSpyThread::Open( RThread& aThread )
    {
    CMemSpyEngine& engine = iProcess->Engine();
    RMemSpyDriverClient& driver = engine.Driver();
    const TInt error = driver.OpenThread( iId, aThread );
    return error;
    }


EXPORT_C TPtrC CMemSpyThread::Name() const
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


EXPORT_C TFullName CMemSpyThread::FullName() const
    {
    TFullName name( iProcess->Name() );
    name += KMemSpyThreadDoubleColon;
    name += Name();
    //
    return name;
    }


EXPORT_C TBool CMemSpyThread::IsSystemPermanent() const
    {
    const TBool ret = ( iFlags & KThreadFlagSystemPermanent );
    return ret;
    }


EXPORT_C TBool CMemSpyThread::IsSystemCritical() const
    {
    const TBool ret = ( iFlags & KThreadFlagSystemCritical );
    return ret;
    }


EXPORT_C CMemSpyThreadInfoContainer& CMemSpyThread::InfoContainerL()
    {
    if  ( iInfoContainer == NULL )
        {
        const TBool KConstructAsynchronously = ETrue;
        iInfoContainer = CMemSpyThreadInfoContainer::NewL( *this, KConstructAsynchronously );
        }
    //
    return *iInfoContainer;
    }


EXPORT_C CMemSpyThreadInfoContainer& CMemSpyThread::InfoContainerForceSyncronousConstructionL()
    {
    if  ( iInfoContainer == NULL )
        {
        const TBool KConstructSynchronously = EFalse;
        iInfoContainer = CMemSpyThreadInfoContainer::NewL( *this, KConstructSynchronously );
        }
    //
    return *iInfoContainer;
    }


EXPORT_C void CMemSpyThread::KillL()
    {
    CMemSpyEngine& engine = iProcess->Engine();
    RMemSpyDriverClient& driver = engine.Driver();
    //
    User::LeaveIfError( driver.ThreadEnd( Id(), EExitKill ) );
    }


EXPORT_C void CMemSpyThread::TerminateL()
    {
    CMemSpyEngine& engine = iProcess->Engine();
    RMemSpyDriverClient& driver = engine.Driver();
    //
    User::LeaveIfError( driver.ThreadEnd( Id(), EExitTerminate ) );
    }


EXPORT_C void CMemSpyThread::PanicL()
    {
    CMemSpyEngine& engine = iProcess->Engine();
    RMemSpyDriverClient& driver = engine.Driver();
    //
    User::LeaveIfError( driver.ThreadEnd( Id(), EExitPanic ) );
    }


EXPORT_C void CMemSpyThread::SetPriorityL( TThreadPriority aPriority )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThread::SetPriorityL() - START - aPriority: %d, orig pri: %d", aPriority, iPriority );
#endif
    CMemSpyEngine& engine = iProcess->Engine();
    RMemSpyDriverClient& driver = engine.Driver();
    //
    const TInt err = driver.SetPriority( Id(), aPriority );
#ifdef _DEBUG
    TInt newPri = -1;
    RThread thread;
    if ( driver.OpenThread( iId, thread ) == KErrNone )
        {
        newPri = thread.Priority();
        thread.Close();
        }
    RDebug::Printf( "CMemSpyThread::SetPriorityL() - err: %d, newPri: %d", err, newPri );
#endif

    User::LeaveIfError( err );
    RefreshL();
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThread::SetPriorityL() - END" );
#endif
    }


void CMemSpyThread::SetDeadL()
    {
    RefreshL();
    }


void CMemSpyThread::SetDeadL( const RThread& aThread )
    {
    RefreshL( aThread );
	}


void CMemSpyThread::FullName( TDes& aName ) const
    {
    iProcess->FullName( aName );
    aName.Append( KMemSpyThreadDoubleColon );
    aName.Append( Name() );
    }


EXPORT_C TBool CMemSpyThread::IsDead() const
    {
    const TBool isDead = ( iExitType != EExitPending );
    return  isDead;
    }

EXPORT_C TThreadPriority CMemSpyThread::Priority() const
	{
	return iPriority;
	}


void CMemSpyThread::RefreshL()
    {
    CMemSpyEngine& engine = iProcess->Engine();
    RMemSpyDriverClient& driver = engine.Driver();
    
    // Try to open thread. We use the device driver since
    // it doesn't check (i.e. it bypasses) some of the conditions which the
    // default RThread::Open() implementation enforces...
    //
    // Deliberately ignore error. The other overload of RefreshL will
    // cope with the failure.
    RThread thread;
    driver.OpenThread( iId, thread );
    CleanupClosePushL( thread );

    // Call refresh with thread to perform actual heavy lifting...
    RefreshL( thread );

    // Clean up. This thread handle might actually not even be open, but that's okay...
    CleanupStack::PopAndDestroy( &thread );
    }


void CMemSpyThread::RefreshL( const RThread& aThread )
    {
    const TBool handleValid = aThread.Handle() != KNullHandle;
    if  ( handleValid )
        {
        // Annoyingly, we request the entire thread info structure just to get the thread flags...
        iFlags = 0;
        const User::TCritical critType = User::Critical( aThread );
        if  ( critType == User::ESystemPermanent )
            {
            iFlags |= KThreadFlagSystemPermanent;
            }
        else if ( critType == User::ESystemCritical )
            {
            iFlags |= KThreadFlagSystemCritical;
            }

#ifdef _DEBUG
        TMemSpyDriverThreadInfo threadInfo;
        User::LeaveIfError( iProcess->Engine().Driver().GetThreadInfo( iId, threadInfo ) );
        RDebug::Print( _L("CMemSpyThread::RefreshL() - old user pri: %d, curr user pri: %d, curr kernel pri: %d, iFlags: %d, name: %S"), iPriority, aThread.Priority(), threadInfo.iThreadPriority, iFlags, &threadInfo.iFullName );
#endif
        }

    // Get exit info
    iExitType = handleValid ? aThread.ExitType() : EExitKill;
    iPriority = handleValid ? aThread.Priority() : EPriorityNormal;

    // If the thread is dead then we may not be able to get some attributes
    // (it depends on whether the thread handle is valid anymore).
    iExitReason = 0;
    iExitCategory.Zero();

    if  ( IsDead() )
        {
        if  ( handleValid )
            {
            iExitReason = aThread.ExitReason();
            iExitCategory = aThread.ExitCategory(); 
            }
        else
            {
            iExitCategory = KMemSpyUnknownExitCategory;
            }
        }
    else
        {
        }

    // Get raw thread name
    HBufC* rawThreadName = GetThreadNameLC( aThread ); 

    // Full name is enough for the thread name as well as the extra info
    // we show
    TFullName name;

    // Build S60 listbox formatted name
    _LIT( KMemSpyThreadNameFormatSpecBasicName, " \t%S\t\t" );
    name.Format( KMemSpyThreadNameFormatSpecBasicName, rawThreadName );
    CleanupStack::PopAndDestroy( rawThreadName );

    // If the thread is dead show exit info
    if  ( IsDead() )
        {
        AppendExitInfo( name, iExitType, iExitReason, iExitCategory );
        }
    else
        {
        // Otherwise, show priority
        AppendPriority( name, iPriority );
        }

    // Save new fully formatted name
    HBufC* newName = name.AllocL();
    delete iName;
    iName = newName;
    }


HBufC* CMemSpyThread::GetThreadNameLC( const RThread& aThreadOrNull ) const
    {
    TName threadName;
    //
    const TBool handleValid = aThreadOrNull.Handle() != KNullHandle;
    //
    if  ( handleValid )
        {
        // Easy case - we have a valid thread handle.
        threadName.Append( aThreadOrNull.Name() );
        }
    else
        {
        // Since we don't have the possibility to enquire after the thread's name
        // we'll assume that it used to be alive and therefore at some point we did
        // manage to grep it's name...
        if  ( iName )
            {
            const TPtrC pOriginalName( Name() );
            threadName.Append( pOriginalName );
            }
        else
            {
            // Don't have a thread handle, don't have any possibility to get the
            // name from a prior cached version. Must use "unknown"
            threadName.Append( KMemSpyUnknownThreadName );
            }
        }
    //
    HBufC* ret = threadName.AllocLC();
    return ret;
    }



