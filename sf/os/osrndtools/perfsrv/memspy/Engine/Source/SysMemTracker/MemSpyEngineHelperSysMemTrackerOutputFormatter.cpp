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

#include "MemSpyEngineHelperSysMemTrackerOutputFormatter.h"

// System includes
#include <e32debug.h>

// User includes
#include "MemSpyEngineOutputListItem.h"
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginehelperrom.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>

// Constants
_LIT( KMemSpyHeapDeltaTrackerFormatPrefix, "[MemSpy] [SMT %04d] " );
_LIT( KMemSpyHeapDeltaTrackerFormatHeaderStart, "<SYSTEM WIDE MEMORY TRACKER>" );
_LIT( KMemSpyHeapDeltaTrackerFormatFooterEnd, "</SYSTEM WIDE MEMORY TRACKER>" );
const TInt KMemSpySWMTDataFileFormat = 2;


CMemSpyEngineHelperSysMemTrackerOutputFormatter::CMemSpyEngineHelperSysMemTrackerOutputFormatter( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperSysMemTrackerOutputFormatter::~CMemSpyEngineHelperSysMemTrackerOutputFormatter()
    {
    }


void CMemSpyEngineHelperSysMemTrackerOutputFormatter::ConstructL()
    {
    }


CMemSpyEngineHelperSysMemTrackerOutputFormatter* CMemSpyEngineHelperSysMemTrackerOutputFormatter::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperSysMemTrackerOutputFormatter* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerOutputFormatter( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerOutputFormatter::SetConfig( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
    {
    iConfig = aConfig;
    }


void CMemSpyEngineHelperSysMemTrackerOutputFormatter::OutputL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineOutputSink& sink = iEngine.Sink();
    CMemSpyEngineHelperSysMemTrackerCycle& cycle = const_cast<CMemSpyEngineHelperSysMemTrackerCycle&>( aCycle );

    // Set up prefix for info listing
    sink.OutputPrefixSetFormattedLC( KMemSpyHeapDeltaTrackerFormatPrefix, aCycle.CycleNumber() );

    // Begin a new data stream for heap info
    _LIT( KMemSpyFolder1, "System Wide MT" );
    _LIT( KMemSpyContext1, "Summary - Cycle %03d" );
    HBufC* context1 = HBufC::NewLC( KMaxFileName );
    TPtr pContext1( context1->Des() );
    pContext1.Format( KMemSpyContext1, aCycle.CycleNumber() );
    sink.DataStreamBeginL( pContext1, KMemSpyFolder1 );
    CleanupStack::PopAndDestroy( context1 );

    // Output overall header for cycle
    OutputOverallHeaderL( cycle );

    // Output the cycle information
    TMemSpyEngineSysMemTrackerType lastType = EMemSpyEngineSysMemTrackerTypeCount;
	const TInt changedCount = cycle.ChangeCount();
	for( TInt j=0; j<changedCount; j++ )
		{
		CMemSpyEngineHelperSysMemTrackerCycleChange& item = cycle.ChangeAt( j );
        const TMemSpyEngineSysMemTrackerType type = item.Type();

        // Header (if needed)
        if  ( HaveTypesChanged( type, lastType ) )
            {
            lastType = type;

            CMemSpyEngineOutputSink& sink = iEngine.Sink();
            sink.OutputBlankLineL();
            sink.OutputBlankLineL();
            item.OutputHeaderL( sink, cycle );
            }
        
        // Content
        TRAP_IGNORE( item.OutputContentL( sink, cycle ) );
        }
   
    // Now dump the data for any changed entries
    if  ( iConfig.DumpData() )
        {
		for( TInt j=0; j<changedCount; j++ )
			{
			CMemSpyEngineHelperSysMemTrackerCycleChange& item = cycle.ChangeAt( j );
            TRAP_IGNORE( item.OutputDataL( sink, cycle ) );
			}
		}

    // Output overall footer for cycle
    OutputOverallFooterL( cycle );

    // End summary data stream
    sink.DataStreamEndL();
		
	CleanupStack::PopAndDestroy(); // prefix
    }


void CMemSpyEngineHelperSysMemTrackerOutputFormatter::OutputOverallHeaderL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineOutputSink& sink = iEngine.Sink();
    sink.OutputBlankLineL();
    sink.OutputBlankLineL();
    sink.OutputBlankLineL();
    sink.OutputBlankLineL();
    //
    sink.OutputLineL( KMemSpyHeapDeltaTrackerFormatHeaderStart );
    sink.OutputBlankLineL();

    // Lines
    CMemSpyEngineOutputList* list = CMemSpyEngineOutputList::NewLC( sink );
    
    _LIT( KLine0, "File format version" );
    list->AddItemL( KLine0, KMemSpySWMTDataFileFormat );

    _LIT( KLine1, "Cycle number" );
    list->AddItemL( KLine1, aCycle.CycleNumber() );

    _LIT( KLine2, "Time" );
    list->AddItemL( KLine2, aCycle.TimeFormatted() );

    _LIT( KLine3, "Change count" );
    list->AddItemL( KLine3, aCycle.ChangeCount() );

    _LIT( KLine4, "Free memory (now)" );
    list->AddItemL( KLine4, aCycle.MemoryFree() );

    _LIT( KLine5, "Free memory (previous)" );
    list->AddItemL( KLine5, aCycle.MemoryFreePreviousCycle() );

    _LIT( KLine6, "MemSpy (RAM)" );
    list->AddItemL( KLine6, User::Heap().Size() );

    // Add ROM info
    iEngine.HelperROM().AddInfoL( *list );

    list->PrintL();
    CleanupStack::PopAndDestroy( list );

    sink.OutputBlankLineL();
    sink.OutputBlankLineL();
    }


void CMemSpyEngineHelperSysMemTrackerOutputFormatter::OutputOverallFooterL( CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    CMemSpyEngineOutputSink& sink = iEngine.Sink();
    //
    sink.OutputBlankLineL();
    sink.OutputBlankLineL();
    sink.OutputLineL( KMemSpyHeapDeltaTrackerFormatFooterEnd );
    sink.OutputBlankLineL();
    sink.OutputBlankLineL();
    }


TBool CMemSpyEngineHelperSysMemTrackerOutputFormatter::HaveTypesChanged( TInt aLeft, TInt aRight )
    {
    TBool ret = ( aLeft != aRight );
    //
    if ( ret )
        {
        // Check whether heap types are kernel vs user. We silently ignore this change.
        if ( ( aLeft == EMemSpyEngineSysMemTrackerTypeHeapUser && aRight == EMemSpyEngineSysMemTrackerTypeHeapKernel ) ||
             ( aLeft == EMemSpyEngineSysMemTrackerTypeHeapKernel && aRight == EMemSpyEngineSysMemTrackerTypeHeapUser ) )
            {
            ret = EFalse;
            }
        }
    //
    return ret;
    }

