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

#include <memspy/engine/memspyenginehelperstack.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include "MemSpyEngineOutputListItem.h"
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>

// Literal constants
_LIT( KMemSpyPrefixStackData, "StackData - %S - " );
_LIT( KMemSpyMarkerStackData, "<%SMEMSPY_STACK_DATA_%03d>" );


CMemSpyEngineHelperStack::CMemSpyEngineHelperStack( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperStack::~CMemSpyEngineHelperStack()
    {
    }


void CMemSpyEngineHelperStack::ConstructL()
    {
    }


CMemSpyEngineHelperStack* CMemSpyEngineHelperStack::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperStack* self = new(ELeave) CMemSpyEngineHelperStack( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperStack::OutputStackInfoL( const CMemSpyThread& aThread )
    {
    const TFullName pName( aThread.FullName() );
    //
    _LIT(KHeader, "STACK INFO FOR THREAD '%S'");
    TBuf<KMaxFullName + 100> printFormat;
    printFormat.Format( KHeader, &pName );
    iEngine.Sink().OutputSectionHeadingL( printFormat, '=' );

    OutputStackInfoL( aThread.Process().Id(), aThread.Id(), printFormat );
    }


EXPORT_C void CMemSpyEngineHelperStack::OutputStackInfoL( TProcessId aPid, TThreadId aTid, TDes& aLineBuffer )
    {
    TMemSpyDriverStackInfo info;
    //
    iEngine.ProcessSuspendLC( aPid );
    const TInt r = iEngine.Driver().GetStackInfo( aTid, info );
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC
    //
    if  ( r == KErrNone )
        {
        CMemSpyEngineOutputList* list = CMemSpyEngineOutputList::NewLC( iEngine.Sink() );

        {
        // Header - user stack
        list->AddItemL( _L("USER STACK") );
        list->AddUnderlineForPreviousItemL();

        // Summary
        list->AddItemFormatL( _L("Address range"), _L("0x%08x - 0x%08x (%8d)"), info.iUserStackBase, info.iUserStackBase + info.iUserStackSize, info.iUserStackSize );
        list->AddItemHexL( _L("Current stack pointer"), info.iUserStackPointer );

        // Calculate usage
        const TInt usedUserStack = (TInt) ( info.iUserStackBase + info.iUserStackSize ) - info.iUserStackPointer;
        const TInt userStackUsagePct = (TInt) (( (TReal) usedUserStack / (TReal) info.iUserStackSize) * 100.0);
        aLineBuffer.Format(_L("%d (%3d"), usedUserStack, userStackUsagePct);
        aLineBuffer.Append(_L(" pct)"));
        list->AddItemL( _L("Stack usage"), aLineBuffer );

        // High watermark
        list->AddItemHexL( _L("High watermark"), info.iUserStackHighWatermark );
        const TInt userHighWaterMarkUsage = (TInt) ( info.iUserStackBase + info.iUserStackSize ) - info.iUserStackHighWatermark;
        const TInt userStackHighWaterMarkUsagePct = (TInt) (( (TReal) userHighWaterMarkUsage / (TReal) info.iUserStackSize) * 100.0);
        aLineBuffer.Format(_L("%d (%3d"), userHighWaterMarkUsage, userStackHighWaterMarkUsagePct);
        aLineBuffer.Append(_L(" pct)"));
        list->AddItemL( _L("High watermark usage"), aLineBuffer );
        }


        {
        // Header - supervisor stack
        list->AddItemL( _L("SUPERVISOR STACK") );
        list->AddUnderlineForPreviousItemL();

        // Summary
        list->AddItemFormatL( _L("Address range"), _L("0x%08x - 0x%08x (%8d)"), info.iSupervisorStackBase, info.iSupervisorStackBase + info.iSupervisorStackSize, info.iSupervisorStackSize );
        list->AddItemHexL( _L("Current stack pointer"), info.iSupervisorStackPointer );

        // Calculate usage
        const TInt usedSupervisorStack = (TInt) ( info.iSupervisorStackBase + info.iSupervisorStackSize ) - info.iSupervisorStackPointer;
        const TInt supervisorStackUsagePct = (TInt) (( (TReal) usedSupervisorStack / (TReal) info.iSupervisorStackSize) * 100.0);
        aLineBuffer.Format(_L("%d (%3d"), usedSupervisorStack, supervisorStackUsagePct );
        aLineBuffer.Append(_L(" pct)"));
        list->AddItemL( _L("Stack usage"), aLineBuffer );

        // High watermark
        list->AddItemHexL( _L("High watermark"), info.iSupervisorStackHighWatermark );
        const TInt supervisorStackHighWaterMarkUsage = (TInt) ( info.iSupervisorStackBase + info.iSupervisorStackSize ) - info.iSupervisorStackHighWatermark;
        const TInt supervisorStackHighWaterMarkUsagePct = (TInt) (( (TReal) supervisorStackHighWaterMarkUsage / (TReal) info.iSupervisorStackSize) * 100.0);
        aLineBuffer.Format(_L("%d (%3d"), supervisorStackHighWaterMarkUsage, supervisorStackHighWaterMarkUsagePct );
        aLineBuffer.Append(_L(" pct)"));
        list->AddItemL( _L("High watermark usage"), aLineBuffer );
        }

        list->PrintL();
        CleanupStack::PopAndDestroy( list );
        }

    }


EXPORT_C void CMemSpyEngineHelperStack::OutputStackDataL( const CMemSpyThread& aThread, TMemSpyDriverDomainType aType )
    {
    OutputStackDataL( aThread, aType, ETrue );
    }


EXPORT_C void CMemSpyEngineHelperStack::OutputStackDataL( const CMemSpyThread& aThread, TMemSpyDriverDomainType aType, TBool aEntireStack )
    {
    TBuf<KMaxFullName + 100> printFormat;
    printFormat = aThread.FullName();

    // Begin a new data stream
    _LIT( KMemSpyFolder, "Stack" );
    HBufC* context = HBufC::NewLC( KMaxFileName );
    TPtr pContext( context->Des() );
    if  ( aType == EMemSpyDriverDomainUser )
        {
        _LIT(KMemSpyContext, "Data (User) - %S");
        pContext.Format( KMemSpyContext, &printFormat );
        }
    else if ( aType == EMemSpyDriverDomainKernel )
        {
        _LIT(KMemSpyContext, "Data (Supervisor) - %S");
        pContext.Format( KMemSpyContext, &printFormat );
        }
    iEngine.Sink().DataStreamBeginL( pContext, KMemSpyFolder );
    CleanupStack::PopAndDestroy( context );

    // Suspend all threads in the process
    iEngine.ProcessSuspendLC( aThread.Process().Id() );

    // Start marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerStackData, &KNullDesC, (TUint) aThread.Id() );

    // Set overall prefix
    iEngine.Sink().OutputPrefixSetFormattedLC( KMemSpyPrefixStackData, &printFormat );

    // Prepare data buffer
    HBufC8* data = HBufC8::NewLC( 4096 * 4 );
    TPtr8 pData(data->Des());
    TUint remaining = 0;

    TMemSpyDriverStackInfo info;
    TInt r = iEngine.Driver().GetStackInfo( aThread.Id(), info );
    if  ( r == KErrNone )
        {
        TUint spAddress = 0;
        if  ( aType == EMemSpyDriverDomainUser )
            {
            _LIT(KHeaderUser, "USER STACK DATA");
            iEngine.Sink().OutputSectionHeadingL(KHeaderUser, '-');
            spAddress = info.iUserStackPointer;
            }
        else if ( aType == EMemSpyDriverDomainKernel )
            {
            _LIT(KHeaderKernel, "SUPERVISOR STACK DATA");
            iEngine.Sink().OutputSectionHeadingL(KHeaderKernel, '-');
            spAddress = info.iSupervisorStackPointer;
            }

        // Print header information
        // ========================
        TBuf<240> buf;

        // Stack pointer
        _LIT( KLine1, "Current stack pointer: 0x%08x");
        buf.Format( KLine1, spAddress );
        iEngine.Sink().OutputLineL( buf );

        // Stack address range
        _LIT( KLine2, "Stack address range:   0x%08x - 0x%08x");
        if  ( aType == EMemSpyDriverDomainUser )
            {
            buf.Format( KLine2, info.iUserStackBase, info.iUserStackBase + info.iUserStackSize );
            }
        else
            {
            buf.Format( KLine2, info.iSupervisorStackBase, info.iSupervisorStackBase + info.iSupervisorStackSize );
            }
        iEngine.Sink().OutputLineL( buf );

        // Stack size
        _LIT( KLine3, "Stack size:              %d");
        buf.Format( KLine3, ( aType == EMemSpyDriverDomainUser ) ? info.iUserStackSize : info.iSupervisorStackSize );
        iEngine.Sink().OutputLineL( buf );
        iEngine.Sink().OutputBlankLineL();

        // If we are only fetching the 'current' part of the stack, then we need to maniuplate the
        // printing address used to display the stack content
        if  ( !aEntireStack )
            {
            // We start at the stack pointer address and work towards the end of the stack.
            info.iUserStackBase = spAddress;
            }

        // Code segments (needed for map file reading...)
        _LIT(KCodeSegInfoPrefix, "CodeSeg - ");
        iEngine.HelperCodeSegment().OutputCodeSegmentsL( aThread.Process().Id(), printFormat, KCodeSegInfoPrefix );
        
        // Get the stack data
        // ==================
        _LIT(KStackDataPrefix, "%S");
        r = iEngine.Driver().GetStackData( aThread.Id(), pData, remaining, aType, aEntireStack );

        if  ( r == KErrNone )
            {
            while ( r == KErrNone )
                {
                iEngine.Sink().OutputBinaryDataL( KStackDataPrefix, pData.Ptr(), (const TUint8*) info.iUserStackBase, pData.Length() );
                //
                if  ( remaining > 0 )
                    {
                    info.iUserStackBase += pData.Length();
                    r = iEngine.Driver().GetStackDataNext( aThread.Id(), pData, remaining, aType, aEntireStack );
                    }
                else
                    {
                    break;
                    }
                }
            }

        }
    CleanupStack::PopAndDestroy( data );

    CleanupStack::PopAndDestroy(); // clear prefix
    CleanupStack::PopAndDestroy(); // resume process

    // End marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerStackData, &KMemSpySinkTagClose, (TUint) aThread.Id() );
    iEngine.Sink().DataStreamEndL();
    }


EXPORT_C void CMemSpyEngineHelperStack::OutputStackInfoForDeviceL()
    {
    const TInt count = iEngine.Container().Count();
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());
    //
    _LIT( KMemSpyContext, "Stack" );
    _LIT( KMemSpyFolder, "Device-Wide" );
    _LIT( KMemSpyExtension, ".csv" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder, KMemSpyExtension );

    // Set overall prefix
    _LIT(KOverallPrefix, "[Stack Summary]");
    iEngine.Sink().OutputPrefixSetLC( KOverallPrefix );

    _LIT(KListingHeader, "Thread, US. Base Address, US. Size, US. Addr, US. Usage, US. Usage Pct, US. HWM Addr, US. HWM Usage, US. HWM Usage Pct, SS. Base Address, SS. Size, SS. Addr, SS. Usage, SS. Usage Pct., SS. HWM Addr, SS. HWM Usage, SS. HWM Usage Pct");
    iEngine.Sink().OutputBlankLineL();
    iEngine.Sink().OutputLineL(KListingHeader);
    
    for(TInt ii=0; ii<count; ii++)
        {
        const CMemSpyProcess& process = iEngine.Container().At( ii );
        const TPtrC procName( process.Name() );
        //
        if  ( iEngine.ProcessSuspendAndGetErrorLC( process.Id() ) == KErrNone )
            {
            TMemSpyDriverStackInfo info;
            const TInt threadCount = process.Count();
            //
            for(TInt j=0; j<threadCount; j++)
                {
                const CMemSpyThread& thread = process.At( j );
                const TPtrC threadName(thread.Name());

                const TInt error = iEngine.Driver().GetStackInfo( thread.Id(), info );
                if  ( error == KErrNone )
                    {
                    const TInt userStackUsage = (TInt) ( info.iUserStackBase + info.iUserStackSize ) - info.iUserStackPointer;
                    const TInt userStackUsagePct = (TInt) (( (TReal) userStackUsage / (TReal) info.iUserStackSize) * 100.0);
                    const TInt userStackHighWaterMarkUsage = (TInt) ( info.iUserStackBase + info.iUserStackSize ) - info.iUserStackHighWatermark;
                    const TInt userStackHighWaterMarkUsagePct = (TInt) (( (TReal) userStackHighWaterMarkUsage / (TReal) info.iUserStackSize) * 100.0);
                    const TInt supervisorStackUsage = (TInt) ( info.iSupervisorStackBase + info.iSupervisorStackSize ) - info.iSupervisorStackPointer;
                    const TInt supervisorStackUsagePct = (TInt) (( (TReal) supervisorStackUsage / (TReal) info.iSupervisorStackSize) * 100.0);
                    const TInt supervisorStackHighWaterMarkUsage = (TInt) ( info.iSupervisorStackBase + info.iSupervisorStackSize ) - info.iSupervisorStackHighWatermark;
                    const TInt supervisorStackHighWaterMarkUsagePct = (TInt) (( (TReal) supervisorStackHighWaterMarkUsage / (TReal) info.iSupervisorStackSize) * 100.0);

                    _LIT(KFormat, "%S::%S, 0x%08x, %8d, 0x%08x, %8d, %8d, 0x%08x, %8d, %8d, 0x%08x, %8d, 0x%08x, %8d, %8d, 0x%08x, %8d, %8d");
                    pBuf.Format(  KFormat, &procName, &threadName, 
                                 info.iUserStackBase, 
                                 info.iUserStackSize, 
                                 info.iUserStackPointer, 
                                 userStackUsage,
                                 userStackUsagePct,
                                 info.iUserStackHighWatermark,
                                 userStackHighWaterMarkUsage,
                                 userStackHighWaterMarkUsagePct,
                                 info.iSupervisorStackBase,
                                 info.iSupervisorStackSize,
                                 info.iSupervisorStackPointer,
                                 supervisorStackUsage,
                                 supervisorStackUsagePct,
                                 info.iSupervisorStackHighWatermark,
                                 supervisorStackHighWaterMarkUsage,
                                 supervisorStackHighWaterMarkUsagePct
                                 );
                    iEngine.Sink().OutputLineL( pBuf );
                    }
                }
            }
        
        CleanupStack::PopAndDestroy(); // ProcessSuspendLC
        }

    CleanupStack::PopAndDestroy(); // clear prefix

    CleanupStack::PopAndDestroy( buf );

    _LIT(KEndOfHeapListing, "<= End Stack Summary =>");
    iEngine.Sink().OutputLineL( KEndOfHeapListing );

    iEngine.Sink().DataStreamEndL();
    }


EXPORT_C TInt CMemSpyEngineHelperStack::CalculateStackSizes( const CMemSpyProcess& aProcess )
    {
    TInt ret = 0;
    //
    TRAPD( error, ret = CalculateStackSizesL( aProcess ) );
    //
    if ( error != KErrNone )
        {
        ret = error;
        }
    //
    return ret;
    }


TInt CMemSpyEngineHelperStack::CalculateStackSizesL( const CMemSpyProcess& aProcess )
    {
	TInt ret = 0;
	//
    iEngine.ProcessSuspendLC( aProcess.Id() );

    TMemSpyDriverStackInfo info;
    //
    const TInt threadCount = aProcess.Count();
    for( TInt i=0; i<threadCount; i++ )
        {
        const CMemSpyThread& thread = aProcess.At( i );
        //
        TInt r = iEngine.Driver().GetStackInfo( thread.Id(), info );
        if  ( r == KErrNone )
            {
            ret += info.iUserStackSize;
            ret += info.iSupervisorStackSize;
            }
        }
    //
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC
	return ret;
    }



