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

#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>

// System includes
#include <f32file.h>
#include <hal.h>
#include <kernel/arm/arm_types.h>
#include <memspy/driver/memspydriverclient.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperactiveobject.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyengineoutputsink.h>

// User includes
#include "MemSpyEngineOutputListItem.h"

// Constants
const TInt KMemSpyNumericFormatBufferSize = 20;

// Literal constants
_LIT( KMemSpyNumericHexFormat, "0x%08x" );
_LIT( KMemSpyNumericDecFormat, "%d" );
_LIT( KMemSpyNumericLongFormat, "%Ld" );
_LIT( KMemSpyCaptionYes, "Yes" );
_LIT( KMemSpyCaptionNo, "No" );
_LIT( KMemSpyCaptionOn, "On" );
_LIT( KMemSpyCaptionOff, "Off" );
_LIT( KMemSpyUnavailable, "Unavailable" );
_LIT( KMemSpyDead, "Dead" );
_LIT( KMemSpyNoItems, "(No items)" );



CMemSpyThreadInfoItemBase::CMemSpyThreadInfoItemBase( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aType, TBool aAsyncConstruction )
:   CMemSpyEngineObject( aContainer ), iContainer( aContainer ), iCallBack( CActive::EPriorityLow ), iType( aType )
    {
    if  ( aAsyncConstruction )
        {
        TCallBack callBackMethod( CallConstructL, this );
        iCallBack.Set( callBackMethod );
        iCallBack.CallBack();
        }
    }


CMemSpyThreadInfoItemBase::~CMemSpyThreadInfoItemBase()
    {
    TRAP_IGNORE( iContainer.NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemDestroyed, iType ) );
    //
    iItems.ResetAndDestroy();
    iItems.Close();
    }


TInt CMemSpyThreadInfoItemBase::CallConstructL( TAny* aSelf )
    {
    CMemSpyThreadInfoItemBase* self = reinterpret_cast< CMemSpyThreadInfoItemBase* >( aSelf );
    self->iReady = EFalse;
    
    // Don't try to refresh dead thread
    TInt err = KErrNone;
    if ( !self->Container().Thread().IsDead() )
        {
        TRAP(err, self->ConstructL());
        if  ( err != KErrNone )
            {
    #ifdef _DEBUG
            RDebug::Printf( "CMemSpyThreadInfoItemBase::CallConstructL() - construction err: %d, iType: %d", err, self->iType );
    #endif
            self->AddItemL( KMemSpyUnavailable, KNullDesC );
            self->iIsEmpty = ETrue;
            }
        else if ( self->MdcaCount() == 0 )
            {
            self->AddItemL( KMemSpyNoItems, KNullDesC );
            self->iIsEmpty = ETrue;
            }        
        }
    else
        {
        self->AddItemL( KMemSpyDead, KNullDesC );
        self->iIsEmpty = ETrue;
        }
    //
    self->iReady = ETrue;
    return KErrNone;
    }


EXPORT_C TInt CMemSpyThreadInfoItemBase::MdcaCount() const
    {
    return iItems.Count();
    }


EXPORT_C TPtrC CMemSpyThreadInfoItemBase::MdcaPoint( TInt aIndex ) const
    {
    CItem* item = iItems[ aIndex ];
    return TPtrC( item->Combined() );
    }

EXPORT_C TPtrC CMemSpyThreadInfoItemBase::Caption(TInt aIndex ) const
	{
	CItem* item = iItems[ aIndex ];
	return TPtrC( item->Caption() );
	}

EXPORT_C TPtrC CMemSpyThreadInfoItemBase::Value(TInt aIndex ) const
	{
	CItem* item = iItems[ aIndex ];
	return TPtrC( item->Value() );
	}

EXPORT_C CMemSpyEngine& CMemSpyThreadInfoItemBase::Engine() const
    {
    return iContainer.Engine();
    }


EXPORT_C void CMemSpyThreadInfoItemBase::PrintL()
    {
    const TInt count = iItems.Count();
    if  ( count > 0 && !iIsEmpty )
        {
        CMemSpyEngine& engine = Engine();
        CMemSpyEngineOutputSink& sink = engine.Sink();

        HBufC* name = MemSpyEngineUtils::CleanupTextLC( Name() );
        sink.OutputSectionHeadingL( *name, TChar('-') );
        CleanupStack::PopAndDestroy( name );
        sink.OutputPrefixSetLC( _L("  ") ); // Slight insertion

        // First pass to get max lengths
        TInt maxLengthCaption = 0;
        TInt maxLengthValue = 0;

        for( TInt j=0; j<count; j++ )
            {
            const CItem* item = iItems[ j ];
            maxLengthCaption = Max( maxLengthCaption, item->Caption().Length() );
            maxLengthValue = Max( maxLengthValue, item->Value().Length() );
            }

        // Second pass - real this time - to print the values
        HBufC* line = HBufC::NewLC( ( maxLengthCaption + maxLengthValue ) + 20 );
        TPtr pLine( line->Des() );
        //
        for( TInt i=0; i<count; i++ )
            {
            const CItem* item = iItems[ i ];

            // Remove initial tabs in caption
            HBufC* caption = MemSpyEngineUtils::CleanupTextLC( item->Caption() );
        
            // Create value item & replace any further tabs
            HBufC* value = MemSpyEngineUtils::CleanupTextLC( item->Value() );

            // Now format the final line, with padding.
            pLine.Justify( *caption, maxLengthCaption + 3, ELeft, TChar(' ') );
            pLine.Append( *value );
            CleanupStack::PopAndDestroy( 2, caption );

            // Sink output
            sink.OutputLineL( pLine );
            }
        //
        CleanupStack::PopAndDestroy( line );
        sink.OutputBlankLineL();
        CleanupStack::PopAndDestroy(); // clear prefix
        }
    }



void CMemSpyThreadInfoItemBase::AddItemL( const TDesC& aCaption, const TDesC& aValue )
    {
    CItem* item = CItem::NewLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyThreadInfoItemBase::AddItemHexL( const TDesC& aCaption, TUint aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericHexFormat, aValue );
    AddItemL( aCaption, val );
    }


void CMemSpyThreadInfoItemBase::AddItemDecimalL( const TDesC& aCaption, TInt aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericDecFormat, aValue );
    AddItemL( aCaption, val );
    }


void CMemSpyThreadInfoItemBase::AddItemLongL( const TDesC& aCaption, const TInt64& aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericLongFormat, aValue );
    AddItemL( aCaption, val );
    }


void CMemSpyThreadInfoItemBase::AddItemYesNoL( const TDesC& aCaption, TBool aYes )
    {
    CItem* item = CItem::NewYesNoLC( aCaption, aYes );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyThreadInfoItemBase::AddItemOnOffL( const TDesC& aCaption, TBool aOn )
    {
    CItem* item = CItem::NewOnOffLC( aCaption, aOn );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyThreadInfoItemBase::AddItemPercentageL( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue )
    {
    const TMemSpyPercentText val( MemSpyEngineUtils::FormatPercentage( TReal( aOneHundredPercentValue ), TReal( aValue ) ) );
    AddItemL( aCaption, val );
    }


EXPORT_C void CMemSpyThreadInfoItemBase::RebuildL()
    {
    Reset();
    CallConstructL( this );
    }


EXPORT_C TBool CMemSpyThreadInfoItemBase::IsReady() const
    {
    return iReady;
    }


EXPORT_C TMemSpyThreadInfoItemType CMemSpyThreadInfoItemBase::Type() const
    {
    return iType;
    }


void CMemSpyThreadInfoItemBase::Reset()
    {
    iItems.ResetAndDestroy();
    }


void CMemSpyThreadInfoItemBase::StripProcessAndThreadNames( TDes& aText )
    {
    StripProcessName( aText );
    StripThreadName( aText );
    }


void CMemSpyThreadInfoItemBase::StripProcessName( TDes& aText )
    {
    CMemSpyProcess& process = Container().Thread().Process();
    const TPtrC processName( process.Name() );
    TFullName temp;
    //
    _LIT( KProcessNameUidFormat1, "%S.exe[%08x]" );
    temp.Format( KProcessNameUidFormat1, &processName, process.SID() );
    const TBool stripped = MemSpyEngineUtils::StripText( aText, temp );
    //
    if  ( stripped == EFalse )
        {
        _LIT( KProcessNameUidFormat2, "%S[%08x]" );
        temp.Format( KProcessNameUidFormat2, &processName, process.SID() );
        MemSpyEngineUtils::StripText( aText, temp );
        }
    }


void CMemSpyThreadInfoItemBase::StripThreadName( TDes& aText )
    {
    CMemSpyThread& thread = Container().Thread();
    const TPtrC threadName( thread.Name() );
    const TBool stripped = MemSpyEngineUtils::StripText( aText, threadName );
    (void) stripped;
    }


CMemSpyThreadInfoItemBase::CItem& CMemSpyThreadInfoItemBase::Item( TInt aIndex )
    {
    CItem* item = iItems[ aIndex ];
    return *item;
    }


const CMemSpyThreadInfoItemBase::CItem& CMemSpyThreadInfoItemBase::Item( TInt aIndex ) const
    {
    const CItem* item = iItems[ aIndex ];
    return *item;
    }




















CMemSpyThreadInfoItemBase::CItem::CItem()
    {
    }


CMemSpyThreadInfoItemBase::CItem::~CItem()
    {
    delete iCaption;
    delete iValue;
    delete iCombined;
    }


void CMemSpyThreadInfoItemBase::CItem::ConstructL( const TDesC& aCaption, const TDesC& aValue )
    {
    iCaption = aCaption.AllocL();
    iValue = aValue.AllocL();
    //
    UpdateCombinedL();
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewLC( const CItem& aCopyMe )
    {
    CItem* self = new(ELeave) CItem();
    CleanupStack::PushL( self );
    self->ConstructL( aCopyMe.Caption(), aCopyMe.Value() );
    return self;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewLC( const TDesC& aCaption )
    {
    CItem* self = new(ELeave) CItem();
    CleanupStack::PushL( self );
    self->ConstructL( aCaption, KNullDesC );
    return self;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewLC( const TDesC& aCaption, const TDesC& aValue )
    {
    CItem* self = new(ELeave) CItem();
    CleanupStack::PushL( self );
    self->ConstructL( aCaption, aValue );
    return self;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewHexLC( const TDesC& aCaption, TUint aValue )
    {
    CItem* ret = CItem::NewLC( aCaption );
    ret->SetHexL( aValue );
    return ret;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewDecimalLC( const TDesC& aCaption, TInt aValue )
    {
    CItem* ret = CItem::NewLC( aCaption );
    ret->SetDecimalL( aValue );
    return ret;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewLongLC( const TDesC& aCaption, const TInt64& aValue )
    {
    CItem* ret = CItem::NewLC( aCaption );
    ret->SetLongL( aValue );
    return ret;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewYesNoLC( const TDesC& aCaption, TBool aYes )
    {
    CItem* ret = CItem::NewLC( aCaption );
    ret->SetYesNoL( aYes );
    return ret;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewOnOffLC( const TDesC& aCaption, TBool aOn )
    {
    CItem* ret = CItem::NewLC( aCaption );
    ret->SetOnOffL( aOn );
    return ret;
    }


CMemSpyThreadInfoItemBase::CItem* CMemSpyThreadInfoItemBase::CItem::NewPercentageLC( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue )
    {
    CItem* ret = CItem::NewLC( aCaption );
    ret->SetPercentageL( aOneHundredPercentValue, aValue );
    return ret;
    }


void CMemSpyThreadInfoItemBase::CItem::SetValueL( const TDesC& aValue )
    {
    if  ( iValue == NULL )
        {
        iValue = aValue.AllocL();
        }
    else
        {
        if  ( iValue->Des().MaxLength() < aValue.Length() )
            {
            iValue = iValue->ReAllocL( aValue.Length() );
            }
            
        // Now its safe to assign new content
        *iValue = aValue;
        }
        
    UpdateCombinedL();
    }


void CMemSpyThreadInfoItemBase::CItem::SetHexL( TUint aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericHexFormat, aValue );
    SetValueL( val );
    }


void CMemSpyThreadInfoItemBase::CItem::SetDecimalL( TInt aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericDecFormat, aValue );
    SetValueL( val );
    }


void CMemSpyThreadInfoItemBase::CItem::SetLongL( const TInt64& aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericLongFormat, aValue );
    SetValueL( val );
    }


void CMemSpyThreadInfoItemBase::CItem::SetYesNoL( TBool aYes )
    {
    if  ( aYes )
        {
        SetValueL( KMemSpyCaptionYes );
        }
    else
        {
        SetValueL( KMemSpyCaptionNo );
        }
    }


void CMemSpyThreadInfoItemBase::CItem::SetOnOffL( TBool aOn )
    {
    if  ( aOn )
        {
        SetValueL( KMemSpyCaptionOn );
        }
    else
        {
        SetValueL( KMemSpyCaptionOff );
        }
    }


void CMemSpyThreadInfoItemBase::CItem::SetPercentageL( TInt aOneHundredPercentValue, TInt aValue )
    {
    const TMemSpyPercentText val( MemSpyEngineUtils::FormatPercentage( TReal( aOneHundredPercentValue ), TReal( aValue ) ) );
    SetValueL( val );
    }


void CMemSpyThreadInfoItemBase::CItem::UpdateCombinedL()
    {
    const TInt requiredLength = Caption().Length() + Value().Length() + 10;
    //
    if  ( iCombined == NULL )
        {
        iCombined = HBufC::NewL( requiredLength );
        }
    else if ( iCombined->Des().MaxLength() < requiredLength )
        {
        iCombined = iCombined->ReAllocL( requiredLength );
        }
        
    TPtr pCombined( iCombined->Des() );
    pCombined.Zero();
    pCombined.Append( _L("\t") );
    pCombined.Append( Caption() );
    pCombined.Append( _L("\t\t") );
    pCombined.Append( Value() );
    }







































CMemSpyThreadInfoGeneral::CMemSpyThreadInfoGeneral( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeGeneralInfo, aAsyncConstruction )
    {
    }


void CMemSpyThreadInfoGeneral::ConstructL()
    {
    TBuf<50> temp;
    RThread thread;
    Container().Thread().OpenLC( thread );
    const CMemSpyProcess& process = Container().Thread().Process();

    _LIT( KItem1, "Thread Id" );
    AddItemLongL( KItem1, thread.Id() );

    _LIT( KItem1a, "Process Id" );
    AddItemLongL( KItem1a, (TUint) process.Id() );
  
    _LIT( KItem1b, "SID" );
    AddItemHexL( KItem1b, process.SID() );
  
    _LIT( KItem1c, "VID" );
    AddItemHexL( KItem1c, process.VID() );

    _LIT( KItem2, "Thread Priority" );
    CMemSpyThread::AppendPriority( temp, thread.Priority() );
    AddItemL( KItem2, temp );
    temp.Zero();
   
    _LIT( KItem3, "Process Priority" );
    CMemSpyProcess::AppendPriority( temp, thread.ProcessPriority() );
    AddItemL( KItem3, temp );
    temp.Zero();
   
    _LIT( KItem4, "Request Count" );
    AddItemDecimalL( KItem4, thread.RequestCount() );
   
    TInt processHandleCount = 0;
    TInt threadHandleCount = 0;
    thread.HandleCount( processHandleCount, threadHandleCount );

    _LIT( KItem5a, "Process Handles" );
    AddItemDecimalL( KItem5a, processHandleCount );

    _LIT( KItem5b, "Thread Handles" );
    AddItemDecimalL( KItem5b, threadHandleCount );

    // Thread handle info
    THandleInfo handleInfo;
    thread.HandleInfo( &handleInfo );

    _LIT( KItem5c, "Num. Proc. (Using)" );
    AddItemDecimalL( KItem5c, handleInfo.iNumProcesses );

    _LIT( KItem5d, "Num. Thread (Using)" );
    AddItemDecimalL( KItem5d, handleInfo.iNumThreads );

    _LIT( KItem5e, "Attributes" );
    AddItemDecimalL( KItem5e, thread.Attributes() );
    
    // CPU time (request special kernel build)
    TTimeIntervalMicroSeconds cpuTime;
    if  ( thread.GetCpuTime( cpuTime ) == KErrNone )
        {
        _LIT( KItem5f, "CPU Time (us)" );
        const TInt64 time = cpuTime.Int64();
        AddItemLongL( KItem5f, time );
        }

    // Exit info
    _LIT( KItem6, "Exit Type" );
    CMemSpyThread::AppendExitType( temp, thread.ExitType() );
    AddItemL( KItem6, temp );
    temp.Zero();

    if  ( thread.ExitType() != EExitPending )
        {
        _LIT( KItem7, "Exit Reason" );
        AddItemDecimalL( KItem7, thread.ExitReason() );

        _LIT( KItem8, "Exit Category" );
        const TExitCategoryName cat( thread.ExitCategory() );
        AddItemL( KItem8, cat );
        }

    // Registers
    MakeRegisterListingL( thread );
    
    CleanupStack::PopAndDestroy( &thread );

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoGeneral* CMemSpyThreadInfoGeneral::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoGeneral* self = new(ELeave) CMemSpyThreadInfoGeneral( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoGeneral::Name() const
    {
    _LIT(KName, "\tGeneral");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoGeneral::MakeRegisterListingL( RThread& aThread )
    {
#ifndef __WINS__
    _LIT(KRegFormatGeneral, "R%02d");
    _LIT(KRegFormatSP, "SP");
    _LIT(KRegFormatLR, "LR");
    _LIT(KRegFormatPC, "PC");
    _LIT(KRegFormatFlags, "Flags");
    _LIT(KRegFormatDACR, "DACR"); // Data access control register
    //
    TArmRegSet regList;
    TPckg<TArmRegSet> pRegList( regList );
    //
    aThread.Context( pRegList );
	TArmReg* pReg = reinterpret_cast<TArmReg*>( &regList );
    //
    for( TInt i=0; i<KArmRegisterCount; i++ )
        {
        const TArmReg regValue = pReg[ i ];
        //
        if  ( i <= EArmR12 )
            {
            TBuf<128> buf;
            buf.Format( KRegFormatGeneral, i );
            AddItemHexL( buf, regValue );
            }
        else
            {
            TPtrC pCaption( KRegFormatGeneral );
            //
            if  ( i == EArmSp )
                {
                pCaption.Set( KRegFormatSP );
                }
            else if ( i == EArmLr )
                {
                pCaption.Set( KRegFormatLR );
                }
            else if ( i == EArmPc )
                {
                pCaption.Set( KRegFormatPC );
                }
            else if ( i == EArmFlags )
                {
                pCaption.Set( KRegFormatFlags );
                }
            else if ( i == EArmDacr )
                {
                pCaption.Set( KRegFormatDACR );
                }
            //
            AddItemHexL( pCaption, regValue );
            }
        }
#else
    (void) aThread;
#endif
    }














CMemSpyThreadInfoHeap::CMemSpyThreadInfoHeap( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeHeap, aAsyncConstruction )
    {
    }


void CMemSpyThreadInfoHeap::ConstructL()
    {
    CMemSpyEngineHelperHeap& heapHelper = Engine().HelperHeap();

    // Get heap info first of all
    TMemSpyHeapInfo info;
    heapHelper.GetHeapInfoUserL( Container().Thread().Process().Id(), Container().Thread().Id(), info );
    CMemSpyEngineOutputList* list = heapHelper.NewHeapSummaryShortLC( info );

    // Now add each item to our view
    const TInt count = list->Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineOutputListItem& item = list->Item( i );
        //
        AddItemL( item.Caption(), item.Value() );
        }

    // Tidy up
    CleanupStack::PopAndDestroy( list );
    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoHeap* CMemSpyThreadInfoHeap::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoHeap* self = new(ELeave) CMemSpyThreadInfoHeap( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoHeap::Name() const
    {
    _LIT(KName, "\tHeap");
    return TPtrC( KName );
    }
















CMemSpyThreadInfoActiveObjects::CMemSpyThreadInfoActiveObjects( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeActiveObject, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoActiveObjects::~CMemSpyThreadInfoActiveObjects()
    {
    delete iItems;
    }


void CMemSpyThreadInfoActiveObjects::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    engine.ProcessSuspendLC( Container().Thread().Process().Id() );
    //
    CMemSpyEngineActiveObjectArray* activeObjects = engine.HelperActiveObject().ActiveObjectListL( Container().Thread() );
    delete iItems;
    iItems = activeObjects;
    //
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoActiveObjects* CMemSpyThreadInfoActiveObjects::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoActiveObjects* self = new(ELeave) CMemSpyThreadInfoActiveObjects( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoActiveObjects::Name() const
    {
    _LIT(KName, "\tActive Objects");
    return TPtrC( KName );
    }


EXPORT_C TInt CMemSpyThreadInfoActiveObjects::MdcaCount() const
    {
    TInt count = 0;
    //
    if  ( iItems )
        {
        count = iItems->MdcaCount();
        }
    //
    return count;
    }


EXPORT_C TPtrC CMemSpyThreadInfoActiveObjects::MdcaPoint(TInt aIndex) const
    {
    TPtrC ret( KNullDesC );
    //
    if  ( iItems )
        {
        ret.Set( iItems->MdcaPoint( aIndex ) );
        }
    //
    return ret;
    }












CMemSpyThreadInfoOpenFiles::CMemSpyThreadInfoOpenFiles( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeOpenFiles, aAsyncConstruction )
    {
    }


void CMemSpyThreadInfoOpenFiles::ConstructL()
    {
    _LIT(KSpace, " ");
    //
    const TThreadId myThreadId = Container().Thread().Id();
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    RFs& fsSession = engine.FsSession();
    //
    TMemSpySizeText valueBuf;
    TBuf<128> timeBuf;
    TOpenFileScan scanner( fsSession );
    //
    CFileList* list = NULL;
    scanner.NextL( list );

    while( list != NULL )
        {
        if  ( scanner.ThreadId() == myThreadId )
            {
            CleanupStack::PushL( list );
    
            const TInt entryCount = list->Count();
            for(TInt i=0; i<entryCount; i++)
                {
                const TEntry& entry = (*list)[ i ];

                // Get time and size format strings
                valueBuf = MemSpyEngineUtils::FormatSizeText( entry.iSize );
                MemSpyEngineUtils::FormatTimeL( timeBuf, entry.iModified );
                timeBuf.Insert( 0, KSpace );
                timeBuf.Insert( 0, valueBuf );

                // Get just file name
                TParsePtrC parser( entry.iName );
                const TPtrC pJustName( parser.NameAndExt() );

                // Create item 
                AddItemL( pJustName, timeBuf );
                }

            CleanupStack::Pop( list );
            }

        delete list;
        list = NULL;
        scanner.NextL( list );
        }

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoOpenFiles* CMemSpyThreadInfoOpenFiles::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoOpenFiles* self = new(ELeave) CMemSpyThreadInfoOpenFiles( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoOpenFiles::Name() const
    {
    _LIT(KName, "\tOpen Files");
    return TPtrC( KName );
    }
















CMemSpyThreadInfoStack::CMemSpyThreadInfoStack( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeStack, aAsyncConstruction )
    {
    }


void CMemSpyThreadInfoStack::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    engine.ProcessSuspendLC( Container().Thread().Process().Id() );
    //
    TMemSpyDriverStackInfo info;
    const TInt error = engine.Driver().GetStackInfo( Container().Thread().Id(), info );
    User::LeaveIfError( error );
    
    _LIT( KItem1, "Size" );
    AddItemDecimalL( KItem1, info.iUserStackSize );

#ifndef __WINS__
    const TInt userStackUsage = (TInt) ( info.iUserStackBase + info.iUserStackSize ) - info.iUserStackPointer;
    const TInt userStackHighWaterMarkUsage = (TInt) ( info.iUserStackBase + info.iUserStackSize ) - info.iUserStackHighWatermark;

    _LIT( KItem2, "Stack used" );
    AddItemDecimalL( KItem2, userStackUsage );
    
    _LIT( KItem3, "(percentage)" );
    AddItemPercentageL( KItem3, info.iUserStackSize, userStackUsage );

    _LIT( KItem4, "High watermark" );
    AddItemDecimalL( KItem4, userStackHighWaterMarkUsage );
    
    _LIT( KItem5, "(percentage)" );
    AddItemPercentageL( KItem5, info.iUserStackSize, userStackHighWaterMarkUsage );
#endif

    _LIT( KItem6, "Base address" );
    AddItemHexL( KItem6, info.iUserStackBase );

#ifndef __WINS__
    _LIT( KItem7, "Current pointer" );
    AddItemHexL( KItem7, info.iUserStackPointer );
#endif
    //
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoStack* CMemSpyThreadInfoStack::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoStack* self = new(ELeave) CMemSpyThreadInfoStack( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoStack::Name() const
    {
    _LIT(KName, "\tStack");
    return TPtrC( KName );
    }






























CMemSpyThreadInfoChunk::CMemSpyThreadInfoChunk( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeChunk, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoChunk::~CMemSpyThreadInfoChunk()
    {
    delete iList;
    }


void CMemSpyThreadInfoChunk::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    engine.ProcessSuspendLC( Container().Thread().Process().Id() );
    //
    CMemSpyEngineChunkList* list = engine.HelperChunk().ListForThreadL( Container().Thread().Id() );
    delete iList;
    iList = list;
    //
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoChunk* CMemSpyThreadInfoChunk::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoChunk* self = new(ELeave) CMemSpyThreadInfoChunk( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoChunk::Name() const
    {
    _LIT(KName, "\tChunks");
    return TPtrC( KName );
    }


EXPORT_C TInt CMemSpyThreadInfoChunk::MdcaCount() const
    {
    TInt count = 0;
    //
    if  ( iList )
        {
        count = iList->MdcaCount();
        }
    //
    return count;
    }


EXPORT_C TPtrC CMemSpyThreadInfoChunk::MdcaPoint(TInt aIndex) const
    {
    TPtrC ret( KNullDesC );
    //
    if  ( iList )
        {
        ret.Set( iList->MdcaPoint( aIndex ) );
        }
    //
    return ret;
    }


















CMemSpyThreadInfoCodeSeg::CMemSpyThreadInfoCodeSeg( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeCodeSeg, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoCodeSeg::~CMemSpyThreadInfoCodeSeg()
    {
    delete iList;
    }


void CMemSpyThreadInfoCodeSeg::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    engine.ProcessSuspendLC( Container().Thread().Process().Id() );
    //
    CMemSpyEngineCodeSegList* list = engine.HelperCodeSegment().CodeSegmentListL( Container().Thread().Process().Id() );
    delete iList;
    iList = list;
    //
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


CMemSpyThreadInfoCodeSeg* CMemSpyThreadInfoCodeSeg::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoCodeSeg* self = new(ELeave) CMemSpyThreadInfoCodeSeg( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoCodeSeg::Name() const
    {
    _LIT(KName, "\tCode Segments");
    return TPtrC( KName );
    }


EXPORT_C TInt CMemSpyThreadInfoCodeSeg::MdcaCount() const
    {
    TInt count = 0;
    //
    if  ( iList )
        {
        count = iList->MdcaCount();
        }
    //
    return count;
    }


EXPORT_C TPtrC CMemSpyThreadInfoCodeSeg::MdcaPoint(TInt aIndex) const
    {
    TPtrC ret( KNullDesC );
    //
    if  ( iList )
        {
        ret.Set( iList->MdcaPoint( aIndex ) );
        }
    //
    return ret;
    }

























CMemSpyThreadInfoHandleObjectBase::CMemSpyThreadInfoHandleObjectBase( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aItemType, TMemSpyDriverContainerType aContainerType, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, aItemType, aAsyncConstruction ), iContainerType( aContainerType )
    {
    }


CMemSpyThreadInfoHandleObjectBase::~CMemSpyThreadInfoHandleObjectBase()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::~CMemSpyThreadInfoHandleObjectBase() - this: 0x%08x", this );
#endif
    iInfoItems.Close();
    }


void CMemSpyThreadInfoHandleObjectBase::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::ConstructL() - START" );
#endif
    iInfoItems.Reset();
    //
    CMemSpyProcess& process = Container().Thread().Process();
    CMemSpyEngine& engine = process.Engine();
    engine.ProcessSuspendLC( process.Id() );
    //
    RArray<THandleWrapper> handles;
    CleanupClosePushL( handles );
    GetHandlesL( handles );
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::ConstructL() - got %d handle entries...", handles.Count() );
#endif
    //
    TFullName name;
    TMemSpyDriverHandleInfoGeneric info;
    //
    const TInt count = handles.Count();
    for (TInt i=0; i<count; i++)
    	{
    	const THandleWrapper& handleWrapper = handles[ i ];
        //
    	const TInt r = engine.Driver().GetGenericHandleInfo( Container().Thread().Id(), handleWrapper.iType, handleWrapper.iHandle, info );
        //
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::ConstructL() - handle[%3d] 0x%08x, type: %d, refCount: %d, r: %d", i, handleWrapper.iHandle, handleWrapper.iType, handleWrapper.iRefCount, r );
#endif
        //
        if  (r == KErrNone)
    		{
            name.Copy( info.iName );
#ifdef _DEBUG
            RDebug::Print( _L( "CMemSpyThreadInfoHandleObjectBase::ConstructL() - HANDLE [%3d] %S"), handleWrapper.iRefCount, &name );
#endif
            StripProcessAndThreadNames( name );
            //
            iInfoItems.AppendL( info );
            HandleContainerItemL( info, handleWrapper.iRefCount, name );
            }
     	}

    CleanupStack::PopAndDestroy( &handles );
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC

    HandleAllItemsLocatedL();

    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


TBool CMemSpyThreadInfoHandleObjectBase::THandleWrapper::Match( const THandleWrapper& aLeft, const THandleWrapper& aRight )
    {
    return ( aLeft.iHandle == aRight.iHandle );
    }


EXPORT_C TInt CMemSpyThreadInfoHandleObjectBase::DetailsIndexByEntry( const TMemSpyDriverHandleInfoGeneric& aEntry ) const
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::DetailsIndexByEntry() - START - this: 0x%08x, aEntry.iHandle: 0x%08x", this, aEntry.iHandle );
#endif
    //
    const TInt ret = DetailsIndexByHandle( aEntry.iHandle );
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::DetailsIndexByEntry() - END - ret: %d", ret );
#endif
    return ret;
    }


EXPORT_C TInt CMemSpyThreadInfoHandleObjectBase::DetailsIndexByHandle( TAny* aHandle ) const
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::DetailsIndexByHandle() - START - this: 0x%08x, aHandle: 0x%08x", this, aHandle );
#endif
    TInt ret = KErrNotFound;
    //
    const TInt count = DetailsCount();
    for(TInt i=0; i<count; i++)
        {
        const TMemSpyDriverHandleInfoGeneric& item = DetailsAt( i );
        if  ( item.iHandle == aHandle )
            {
            ret = i;
            break;
            }
        }
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleObjectBase::DetailsIndexByHandle() - END - ret: %d", ret );
#endif
    return ret;
    }














CMemSpyThreadInfoHandleByContainer::CMemSpyThreadInfoHandleByContainer( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aItemType, TMemSpyDriverContainerType aContainerType, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleObjectBase( aContainer, aItemType, aContainerType, aAsyncConstruction )
    {
    }


void CMemSpyThreadInfoHandleByContainer::GetHandlesL( RArray<THandleWrapper>& aArray )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoHandleByContainer::GetHandlesL() - START - container: %d", ContainerType() );
#endif

    aArray.Reset();

    // Our handles will be stored here... duplicates are filtered out
    TInt r = KErrNone;
	TInt c = KMemSpyDefaultMaxHandleCount;
	TAny* handles[ KMemSpyDefaultMaxHandleCount ];

    CMemSpyProcess& process = Container().Thread().Process();
    CMemSpyEngine& engine = process.Engine();
    TIdentityRelation<CMemSpyThreadInfoHandleObjectBase::THandleWrapper> finder( THandleWrapper::Match );

    // First get the handles for the process
    if  ( r == KErrNone )
        {
        c = KMemSpyDefaultMaxHandleCount;
        r = engine.Driver().GetProcessHandlesByType( process.Id(), ContainerType(), handles, c );
        if  ( r == KErrNone && c > 0 )
    	    {
            c = Min( c, KMemSpyDefaultMaxHandleCount );
    	    for( TInt i=0; i<c; i++ )
    		    {
    		    TAny* handle = handles[ i ];

                // Create temporary entry that we'll use as the key in our array...
                CMemSpyThreadInfoHandleObjectBase::THandleWrapper entry( handle, ContainerType() );
                
                // Find existing duplicate entry (if there is one...)
                const TInt errorOrIndex = aArray.Find( entry, finder );
#ifdef _DEBUG
                RDebug::Printf( "CMemSpyThreadInfoHandleByContainer::GetHandlesL() - PROC[%03d/%03d] - handle: 0x%08x, foundIndex: %d", i+1, c, handle, errorOrIndex );
#endif

    		    if  ( errorOrIndex == KErrNotFound )
        		    {
        		    // Not a duplicate handle, so keep it
        		    aArray.AppendL( entry );
#ifdef _DEBUG
                    RDebug::Printf( "      new entry: 0x%08x", handle );
#endif
        		    }
                else if ( errorOrIndex >= 0 )
                    {
                    // Increment reference count for duplicates...
                    CMemSpyThreadInfoHandleObjectBase::THandleWrapper& existingEntry = aArray[ errorOrIndex ];
                    ++existingEntry.iRefCount;
#ifdef _DEBUG
                    RDebug::Printf( "      dupe entry - count is now: %d", existingEntry.iRefCount );
#endif
                    }
     		    }
            }
        }

    // Next get the handles for the thread
    if  ( r == KErrNone )
        {
        c = KMemSpyDefaultMaxHandleCount;
        r = engine.Driver().GetThreadHandlesByType( Container().Thread().Id(), ContainerType(), handles, c );
        if  ( r == KErrNone && c > 0 )
    	    {
            c = Min( c, KMemSpyDefaultMaxHandleCount );
    	    for( TInt i=0; i<c; i++ )
    		    {
    		    TAny* handle = handles[ i ];

                // Create temporary entry that we'll use as the key in our array...
                CMemSpyThreadInfoHandleObjectBase::THandleWrapper entry( handle, ContainerType() );
                
                // Find existing duplicate entry (if there is one...)
                const TInt errorOrIndex = aArray.Find( entry, finder );
#ifdef _DEBUG
                RDebug::Printf(  "CMemSpyThreadInfoHandleByContainer::GetHandlesL() - THRD[%03d/%03d] - handle: 0x%08x, foundIndex: %d", i+1, c, handle, errorOrIndex );
#endif
    		    
    		    if  ( errorOrIndex == KErrNotFound )
        		    {
        		    // Not a duplicate handle, so keep it
        		    aArray.AppendL( entry );
#ifdef _DEBUG
                    RDebug::Printf( "      new entry: 0x%08x", handle );
#endif
        		    }
                else if ( errorOrIndex >= 0 )
                    {
                    // Increment reference count for duplicates...
                    CMemSpyThreadInfoHandleObjectBase::THandleWrapper& existingEntry = aArray[ errorOrIndex ];
                    ++existingEntry.iRefCount;
#ifdef _DEBUG
                    RDebug::Printf( "      dupe entry - count is now: %d", existingEntry.iRefCount );
#endif
                    }
     		    }
            }
        }

#ifdef _DEBUG
    RDebug::Printf(  "CMemSpyThreadInfoHandleByContainer::GetHandlesL() - final handle listing: " );

    const TInt finalCount = aArray.Count();
    for( TInt i=0; i<finalCount; i++ )
        {
        const THandleWrapper& handle = aArray[ i ];
        RDebug::Printf(  "entry[%03d/%03d] - handle: 0x%08x, type: %d, refCount: %d", i+1, finalCount, handle.iHandle, handle.iType, handle.iRefCount );
        }

    RDebug::Printf( "CMemSpyThreadInfoHandleByContainer::GetHandlesL() - END - container: %d, finalCount: %d", ContainerType(), finalCount );
#endif
    }

















































CMemSpyThreadInfoServer::CMemSpyThreadInfoServer( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeServer, EMemSpyDriverContainerTypeServer, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoServer* CMemSpyThreadInfoServer::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoServer* self = new(ELeave) CMemSpyThreadInfoServer( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoServer::Name() const
    {
    _LIT(KName, "\tServers Running in Thread");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoServer::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }


EXPORT_C TPtrC CMemSpyThreadInfoServer::SessionType( TIpcSessionType aType )
    {
    _LIT( KUnsharable, "Unsharable" );
    _LIT( KSharable, "Sharable" );
    _LIT( KGlobalSharable, "Global Sharable" );
    //
    TPtrC pType(KNullDesC);
    switch( aType )
        {
    case EIpcSession_Unsharable:
        pType.Set( KUnsharable );
        break;
    case EIpcSession_Sharable:
        pType.Set( KSharable );
        break;
    case EIpcSession_GlobalSharable:
        pType.Set( KGlobalSharable );
        break;
    default:
        pType.Set( KMemSpyUnavailable );
        break;
        }
    //
    return pType;
    }
























CMemSpyThreadInfoSession::CMemSpyThreadInfoSession( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeSession, EMemSpyDriverContainerTypeSession, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoSession::~CMemSpyThreadInfoSession()
    {
    iServerNames.ResetAndDestroy();
    iServerNames.Close();
    }


CMemSpyThreadInfoSession* CMemSpyThreadInfoSession::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoSession* self = new(ELeave) CMemSpyThreadInfoSession( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoSession::Name() const
    {
    _LIT(KName, "\tClient <-> Server\nConnections");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoSession::Reset()
    {
    CMemSpyThreadInfoHandleByContainer::Reset();
    iServerNames.ResetAndDestroy();
    }


EXPORT_C TInt CMemSpyThreadInfoSession::ConnectionCount( const TDesC& aName ) const
    {
    TInt ret = 0;

#ifdef _DEBUG
    RDebug::Print( _L("CMemSpyThreadInfoSession::ConnectionCount() - START - aName: %S"), &aName );
#endif

    // See if we have an entry with that name...
    TInt foundIndex = 0;
    TRAP(ret, foundIndex = FindServerL(aName));
    
    if (ret == KErrNone)
        {
        // If we did, get the count
        if  ( foundIndex >=0 && foundIndex < iServerNames.Count() )
            {
            ret = iServerNames[ foundIndex ]->iCount;
            }
        }

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoSession::ConnectionCount() - END - ret: %d", ret );
#endif
    //
    return ret;
    }

TInt CMemSpyThreadInfoSession::FindServerL( const TDesC& aName ) const
    {
    TIdentityRelation<CSessionInfoEntry> comparer( CompareEntries );
    HBufC* name = aName.AllocLC();
    CSessionInfoEntry* entry = new(ELeave) CSessionInfoEntry( name );
    CleanupStack::Pop( name ); // don't destroy it as name is now owned by entry 
    CleanupStack::PushL( entry );
    const TInt foundIndex = iServerNames.Find( entry, comparer );
    User::LeaveIfError(foundIndex); // so we only return a real index
    CleanupStack::PopAndDestroy( entry );
    return foundIndex;
    }

void CMemSpyThreadInfoSession::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt /*aRefCount*/, TDes& aFullName )
    {
    // Check whether we have the item already
    TIdentityRelation<CSessionInfoEntry> comparer( CompareEntries );

    // Prepare object, just in case we don't find it...
    HBufC* name = aFullName.AllocLC();

#ifdef _DEBUG
    TBuf<KMaxName> origName; origName.Copy( aItem.iName );
    RDebug::Print( _L("CMemSpyThreadInfoSession::HandleContainerItemL() - START - handle: 0x%08x, type: %d, origName: %S, modName: %S"), aItem.iHandle, aItem.iType, &origName, name );
#else
    (void) aItem;
#endif

    CSessionInfoEntry* entry = new(ELeave) CSessionInfoEntry( name );
    CleanupStack::Pop( name );
    CleanupStack::PushL( entry );

    // Search
    const TInt foundIndex = iServerNames.Find( entry, comparer );
    if  ( foundIndex == KErrNotFound )
        {
        // Make new entry
        iServerNames.AppendL( entry );
        CleanupStack::Pop( entry );
        }
    else if ( foundIndex >= 0 )
        {
        // Existing entry, increment count
        CleanupStack::PopAndDestroy( entry );
        entry = iServerNames[ foundIndex ];
        ++entry->iCount;
        }
    else
        {
        CleanupStack::PopAndDestroy( entry );
        }
 
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoSession::HandleContainerItemL() - END - foundIndex: %d", foundIndex );
#endif
    }


void CMemSpyThreadInfoSession::HandleAllItemsLocatedL()
    {
    _LIT(KSecondLineFormatStringCount1, "1 connection");
    _LIT(KSecondLineFormatStringCountMoreThanOne, "%d connections");
    TBuf<50> buf;

    // All items have been found, now create listbox entries
    const TInt count = iServerNames.Count();
    for( TInt i=0; i<count; i++ )
        {
        CSessionInfoEntry* entry = iServerNames[ i ];

        if  ( entry->iCount == 1 )
            {
            buf.Copy( KSecondLineFormatStringCount1 );
            }
        else
            {
            buf.Format( KSecondLineFormatStringCountMoreThanOne, entry->iCount );
            }

        AddItemL( *entry->iName, buf );
        }
    }


TBool CMemSpyThreadInfoSession::CompareEntries( const CSessionInfoEntry& aLeft, const CSessionInfoEntry& aRight )
    {
    return ( aLeft.iName->CompareF( *aRight.iName ) == 0 );
    }


























CMemSpyThreadInfoSemaphore::CMemSpyThreadInfoSemaphore( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeSemaphore, EMemSpyDriverContainerTypeSemaphore, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoSemaphore* CMemSpyThreadInfoSemaphore::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoSemaphore* self = new(ELeave) CMemSpyThreadInfoSemaphore( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoSemaphore::Name() const
    {
    _LIT(KName, "\tSemaphores");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoSemaphore::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt /*aRefCount*/, TDes& aFullName )
    {
    _LIT( KFormatSpec, "Count: %d" );
    TBuf<50> buf;
    buf.AppendFormat( KFormatSpec, aItem.iCount );

    AddItemL( aFullName, buf );
    }
























CMemSpyThreadInfoMutex::CMemSpyThreadInfoMutex( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeMutex, EMemSpyDriverContainerTypeMutex, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoMutex* CMemSpyThreadInfoMutex::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoMutex* self = new(ELeave) CMemSpyThreadInfoMutex( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoMutex::Name() const
    {
    _LIT(KName, "\tMutexes");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoMutex::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt /*aRefCount*/, TDes& aFullName )
    {
    _LIT( KFormatSpec, "Count: %d" );
    TBuf<50> buf;
    buf.AppendFormat( KFormatSpec, aItem.iCount );

    AddItemL( aFullName, buf );
    }
























CMemSpyThreadInfoTimer::CMemSpyThreadInfoTimer( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeTimer, EMemSpyDriverContainerTypeTimer, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoTimer* CMemSpyThreadInfoTimer::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoTimer* self = new(ELeave) CMemSpyThreadInfoTimer( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoTimer::Name() const
    {
    _LIT(KName, "\tTimers");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoTimer::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt /*aRefCount*/, TDes& /*aFullName*/ )
    {
    // Get useful strings
    TBuf<20> state;
    GetTimerState( aItem.iTimerState, state );
    TBuf<20> type;
    GetTimerType( aItem.iTimerType, type );

    AddItemL( type, state );
    }


void CMemSpyThreadInfoTimer::GetTimerState( TMemSpyDriverTimerState aState, TDes& aBuf )
    {
    switch( aState )
        {
    default:
    case EMemSpyDriverTimerStateUnknown: 
        {
        _LIT(KStateUnknown, "Unknown");
        aBuf.Copy( KStateUnknown );
        }
        break;
    case EMemSpyDriverTimerStateIdle:
        {
        _LIT(KStateIdle, "Idle");
        aBuf.Copy( KStateIdle );
        }
        break;
    case EMemSpyDriverTimerStateWaiting: 
        {
        _LIT(KStateWaiting, "Waiting");
        aBuf.Copy( KStateWaiting );
        }
        break;
    case EMemSpyDriverTimerStateWaitHighRes: 
        {
        _LIT(KStateWaitHighRes, "Waiting, High Res.");
        aBuf.Copy( KStateWaitHighRes );
        }
        break;
        }
    }


void CMemSpyThreadInfoTimer::GetTimerType( TMemSpyDriverTimerType aType, TDes& aBuf )
    {
    switch( aType )
        {
    case EMemSpyDriverTimerTypeRelative:
        {
        _LIT( KType, "Relative" );
        aBuf.Copy( KType );
        }
        break;
    case EMemSpyDriverTimerTypeAbsolute:
        {
        _LIT( KType, "Absolute" );
        aBuf.Copy( KType );
        }
        break;
    case EMemSpyDriverTimerTypeHighRes:
        {
        _LIT( KType, "High Res." );
        aBuf.Copy( KType );
        }
        break;
    case EMemSpyDriverTimerTypeInactivity:
        {
        _LIT( KType, "Inactivity" );
        aBuf.Copy( KType );
        }
        break;
    default:
        {
        _LIT( KType, "Unknown" );
        aBuf.Copy( KType );
        }
        break;
        }
    }



















CMemSpyThreadInfoLDD::CMemSpyThreadInfoLDD( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeLDD, EMemSpyDriverContainerTypeLogicalDevice, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoLDD* CMemSpyThreadInfoLDD::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoLDD* self = new(ELeave) CMemSpyThreadInfoLDD( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoLDD::Name() const
    {
    _LIT(KName, "\tLogical Device Drivers");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoLDD::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt /*aRefCount*/, TDes& aFullName )
    {
    _LIT( KFormatSpec, "Open channels: %d" );
    TBuf<50> buf;
    buf.AppendFormat( KFormatSpec, aItem.iOpenChannels );

    AddItemL( aFullName, buf );
    }

























CMemSpyThreadInfoPDD::CMemSpyThreadInfoPDD( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypePDD, EMemSpyDriverContainerTypePhysicalDevice, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoPDD* CMemSpyThreadInfoPDD::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoPDD* self = new(ELeave) CMemSpyThreadInfoPDD( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoPDD::Name() const
    {
    _LIT(KName, "\tPhysical Device Drivers");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoPDD::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }






















CMemSpyThreadInfoLogicalChannel::CMemSpyThreadInfoLogicalChannel( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeLogicalChannel, EMemSpyDriverContainerTypeLogicalChannel, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoLogicalChannel* CMemSpyThreadInfoLogicalChannel::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoLogicalChannel* self = new(ELeave) CMemSpyThreadInfoLogicalChannel( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoLogicalChannel::Name() const
    {
    _LIT(KName, "\tLogical DD Channels");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoLogicalChannel::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }





















CMemSpyThreadInfoChangeNotifier::CMemSpyThreadInfoChangeNotifier( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeChangeNotifier, EMemSpyDriverContainerTypeChangeNotifier, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoChangeNotifier* CMemSpyThreadInfoChangeNotifier::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoChangeNotifier* self = new(ELeave) CMemSpyThreadInfoChangeNotifier( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoChangeNotifier::Name() const
    {
    _LIT(KName, "\tChange Notifiers");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoChangeNotifier::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }





















CMemSpyThreadInfoUndertaker::CMemSpyThreadInfoUndertaker( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeUndertaker, EMemSpyDriverContainerTypeUndertaker, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoUndertaker* CMemSpyThreadInfoUndertaker::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoUndertaker* self = new(ELeave) CMemSpyThreadInfoUndertaker( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoUndertaker::Name() const
    {
    _LIT(KName, "\tUndertakers");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoUndertaker::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }

















CMemSpyThreadInfoOwnedThreadHandles::CMemSpyThreadInfoOwnedThreadHandles( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeOwnedThreadHandles, EMemSpyDriverContainerTypeThread, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoOwnedThreadHandles* CMemSpyThreadInfoOwnedThreadHandles::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoOwnedThreadHandles* self = new(ELeave) CMemSpyThreadInfoOwnedThreadHandles( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoOwnedThreadHandles::Name() const
    {
    _LIT(KName, "\tHandles to other\nThreads");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoOwnedThreadHandles::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName )
    {
    const TInt bracketPosStart = aFullName.LocateF( TChar('[') );
    const TInt doubleColonPos = aFullName.FindF( _L("::" ) );
    //
    if  ( bracketPosStart >= 0 && doubleColonPos > bracketPosStart && doubleColonPos < aFullName.Length() - 2 )
        {
        // Process
        TPtrC pProcessName( aFullName.Left( bracketPosStart ) );
        HBufC* caption = HBufC::NewLC( KMaxName + 10 );
        TPtr pCaption( caption->Des() );
        pCaption.AppendFormat( _L("[%2d] %S"), aRefCount, &pProcessName );
        
        // Thread id & thread name
        TPtrC pThreadName( aFullName.Mid( doubleColonPos + 2 ) );
        HBufC* value = HBufC::NewLC( KMaxName + 10 );
        TPtr pValue( value->Des() );
        pValue.AppendFormat( _L("[%3d] %S"), aItem.iId, &pThreadName );

        // Add it & tidy up
        AddItemL( pCaption, pValue );
        CleanupStack::PopAndDestroy( 2, caption );
        }
    else
        {
        AddItemL( aFullName, KNullDesC );
        }
    }









CMemSpyThreadInfoOwnedProcessHandles::CMemSpyThreadInfoOwnedProcessHandles( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleByContainer( aContainer, EMemSpyThreadInfoItemTypeOwnedProcessHandles, EMemSpyDriverContainerTypeProcess, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoOwnedProcessHandles* CMemSpyThreadInfoOwnedProcessHandles::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoOwnedProcessHandles* self = new(ELeave) CMemSpyThreadInfoOwnedProcessHandles( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoOwnedProcessHandles::Name() const
    {
    _LIT(KName, "\tHandles to other\nProcesses");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoOwnedProcessHandles::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName )
    {
    const TInt bracketPosStart = aFullName.LocateF( TChar('[') );
    const TInt doubleColonPos = aFullName.FindF( _L("::" ) );
    //
    if  ( bracketPosStart >= 0 && doubleColonPos > bracketPosStart && doubleColonPos < aFullName.Length() - 2 )
        {
        // Process
        TPtrC pProcessName( aFullName.Left( bracketPosStart ) );
        HBufC* caption = HBufC::NewLC( KMaxName + 10 );
        TPtr pCaption( caption->Des() );
        pCaption.AppendFormat( _L("[%2d] %S"), aRefCount, &pProcessName );
        
        // Thread id & thread name
        TPtrC pThreadName( aFullName.Mid( doubleColonPos + 2 ) );
        HBufC* value = HBufC::NewLC( KMaxName + 10 );
        TPtr pValue( value->Des() );
        pValue.AppendFormat( _L("[%3d] %S"), aItem.iId, &pThreadName );

        // Add it & tidy up
        AddItemL( pCaption, pValue );
        CleanupStack::PopAndDestroy( 2, caption );
        }
    else
        {
        AddItemL( aFullName, KNullDesC );
        }
    }








































CMemSpyThreadInfoReferencedBy::CMemSpyThreadInfoReferencedBy( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aItemType, TMemSpyDriverContainerType aContainerType, TBool aAsyncConstruction )
:   CMemSpyThreadInfoHandleObjectBase( aContainer, aItemType, aContainerType, aAsyncConstruction )
    {
    }


void CMemSpyThreadInfoReferencedBy::GetHandlesL( RArray<THandleWrapper>& aArray )
    {
    aArray.Reset();
    //
    TInt r = KErrNone;
    //
    CMemSpyProcess& process = Container().Thread().Process();
    CMemSpyEngine& engine = process.Engine();
    TIdentityRelation<CMemSpyThreadInfoHandleObjectBase::THandleWrapper> finder( THandleWrapper::Match );

    // We need to either search through:
    //
    // a) all thread & process handles looking for *this thread*, or
    // b) all thread & process handles looking for *this process*
    //
    // We abuse the "container type" as a means of deciding whether it is
    // the thread or the process we are looking for.
    //
    RMemSpyDriverClient& driver = engine.Driver();
    if  ( ContainerType() == EMemSpyDriverContainerTypeProcess )
        {
        const TUint id = Container().Thread().Process().Id();
        r = driver.GetReferencesToMyProcess( id );
        }
    else if ( ContainerType() == EMemSpyDriverContainerTypeThread )
        {
        const TUint id = Container().Thread().Id();
        r = driver.GetReferencesToMyThread( id );
        }
    else
        {
        ASSERT( EFalse );
        }
    User::LeaveIfError( r );

    RMemSpyMemStreamReader stream = driver.StreamOpenL();
    CleanupClosePushL( stream );
    
    // Extract thread matches
    const TInt threadCount = stream.ReadInt32L();
    for( TInt i=0; i<threadCount; i++ )
        {
        TAny* handle = (TAny*) stream.ReadUint32L();

        // Create temporary entry that we'll use as the key in our array...
        CMemSpyThreadInfoHandleObjectBase::THandleWrapper entry( handle, EMemSpyDriverContainerTypeThread );
        
        // Find existing duplicate entry (if there is one...)
        const TInt errorOrIndex = aArray.Find( entry, finder );
	    
	    if  ( errorOrIndex == KErrNotFound )
		    {
		    // Not a duplicate handle, so keep it
		    aArray.AppendL( entry );
		    }
        else if ( errorOrIndex >= 0 )
            {
            // Increment reference count for duplicates...
            CMemSpyThreadInfoHandleObjectBase::THandleWrapper& existingEntry = aArray[ errorOrIndex ];
            ++existingEntry.iRefCount;
            }
        }
    
    // Extract process matches
    const TInt processCount = stream.ReadInt32L();
    for( TInt i=0; i<processCount; i++ )
        {
        TAny* handle = (TAny*) stream.ReadUint32L();

        // Create temporary entry that we'll use as the key in our array...
        CMemSpyThreadInfoHandleObjectBase::THandleWrapper entry( handle, EMemSpyDriverContainerTypeProcess );
        
        // Find existing duplicate entry (if there is one...)
        const TInt errorOrIndex = aArray.Find( entry, finder );
	    
	    if  ( errorOrIndex == KErrNotFound )
		    {
		    // Not a duplicate handle, so keep it
		    aArray.AppendL( entry );
		    }
        else if ( errorOrIndex >= 0 )
            {
            // Increment reference count for duplicates...
            CMemSpyThreadInfoHandleObjectBase::THandleWrapper& existingEntry = aArray[ errorOrIndex ];
            ++existingEntry.iRefCount;
            }
        }
        
    // Tidy up
    CleanupStack::PopAndDestroy( &stream );
    }




















CMemSpyThreadInfoOtherThreads::CMemSpyThreadInfoOtherThreads( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoReferencedBy( aContainer, EMemSpyThreadInfoItemTypeOtherThreads, EMemSpyDriverContainerTypeThread, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoOtherThreads* CMemSpyThreadInfoOtherThreads::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoOtherThreads* self = new(ELeave) CMemSpyThreadInfoOtherThreads( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoOtherThreads::Name() const
    {
    _LIT(KName, "\tReferences this Thread");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoOtherThreads::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }




















CMemSpyThreadInfoOtherProcesses::CMemSpyThreadInfoOtherProcesses( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoReferencedBy( aContainer, EMemSpyThreadInfoItemTypeOtherProcesses, EMemSpyDriverContainerTypeProcess, aAsyncConstruction )
    {
    }


CMemSpyThreadInfoOtherProcesses* CMemSpyThreadInfoOtherProcesses::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoOtherProcesses* self = new(ELeave) CMemSpyThreadInfoOtherProcesses( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoOtherProcesses::Name() const
    {
    _LIT(KName, "\tReferences this Process");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoOtherProcesses::HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& /*aItem*/, TInt /*aRefCount*/, TDes& aFullName )
    {
    AddItemL( aFullName, KNullDesC );
    }

















CMemSpyThreadInfoMemoryTracking::CMemSpyThreadInfoMemoryTracking( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeMemoryTracking, aAsyncConstruction ), iTotalIncludesSharedMemory( ETrue )
    {
    }


CMemSpyThreadInfoMemoryTracking::~CMemSpyThreadInfoMemoryTracking()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::~CMemSpyThreadInfoMemoryTracking() - START - this: 0x%08x, iTracker: 0x%08x", this, iTracker );
#endif
    //
    TrackingObserverRemove( *this );
    //
    delete iInfoHWM;
    delete iInfoPeak;
    delete iInfoCurrent;
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::~CMemSpyThreadInfoMemoryTracking() - END - this: 0x%08x, iTracker: 0x%08x", this, iTracker );
#endif
    }


void CMemSpyThreadInfoMemoryTracking::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::ConstructL() - START - this: 0x%08x, iTracker: 0x%08x", this, iTracker );
#endif
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    //
    const TProcessId pid = Container().Thread().Process().Id();
    iTracker = engine.HelperProcess().TrackerOrNull( pid );

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::ConstructL() - requesting observer add... - this: 0x%08x, iTracker: 0x%08x", this, iTracker );
#endif
    TrackingObserverAddL( *this );
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::ConstructL() - preparing info item... - this: 0x%08x, iTracker: 0x%08x", this, iTracker );
#endif
    if  ( iTracker )
        {
        delete iInfoCurrent;
        iInfoCurrent = NULL;
        iInfoCurrent = CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::NewLC( Container(), EFalse );
        CleanupStack::Pop( iInfoCurrent );
        //
        delete iInfoHWM;
        iInfoHWM = NULL;
        iInfoHWM = CMemSpyThreadInfoMemoryTrackingStatisticsHWM::NewLC( Container(), EFalse );
        CleanupStack::Pop( iInfoHWM );
        //
        delete iInfoPeak;
        iInfoPeak = NULL;
        iInfoPeak = CMemSpyThreadInfoMemoryTrackingStatisticsPeak::NewLC( Container(), EFalse );
        CleanupStack::Pop( iInfoPeak );
        }
    
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::ConstructL() - prepared info items - this: 0x%08x, iTracker: 0x%08x", this, iTracker );
#endif

    // Prepare items
    _LIT( KItem0, "Tracking" );
    AddItemOnOffL( KItem0, ( iTracker ) ? iTracker->AmTracking() : EFalse );

    TInt64 valCurrent( 0 );
    if  ( iTracker )
        {
        if  ( TotalIncludesSharedMemory() )
            {
            valCurrent = iTracker->InfoCurrent().TotalIncShared();
            }
        else
            {
            valCurrent = iTracker->InfoCurrent().TotalExcShared();
            }
        }
    _LIT( KItem1, "Total [Current]" );
    AddItemLongL( KItem1, valCurrent );

    TInt64 valHWM( 0 );
    if  ( iTracker )
        {
        if  ( TotalIncludesSharedMemory() )
            {
            valHWM = iTracker->InfoHWMIncShared().TotalIncShared();
            }
        else
            {
            valHWM = iTracker->InfoHWMExcShared().TotalExcShared();
            }
        }
    _LIT( KItem2, "Total [HWM]" );
    AddItemLongL( KItem2, valHWM );

    TInt64 valPeak( 0 );
    if  ( iTracker )
        {
        if  ( TotalIncludesSharedMemory() )
            {
            valPeak = iTracker->InfoPeaks().TotalIncShared();
            }
        else
            {
            valPeak = iTracker->InfoPeaks().TotalExcShared();
            }
        }
    _LIT( KItem3, "Total [Peaks]" );
    AddItemLongL( KItem3, valPeak );

    //
    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::ConstructL() - END - this: 0x%08x", this );
#endif
    }


CMemSpyThreadInfoMemoryTracking* CMemSpyThreadInfoMemoryTracking::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoMemoryTracking* self = new(ELeave) CMemSpyThreadInfoMemoryTracking( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


EXPORT_C TPtrC CMemSpyThreadInfoMemoryTracking::Name() const
    {
    _LIT(KName, "\tMemory Tracking");
    return TPtrC( KName );
    }


EXPORT_C TBool CMemSpyThreadInfoMemoryTracking::TrackingActive() const
    {
    return ( iTracker != NULL ? iTracker->AmTracking() : EFalse );
    }


EXPORT_C TBool CMemSpyThreadInfoMemoryTracking::TotalIncludesSharedMemory() const
    {
    return iTotalIncludesSharedMemory;
    }


EXPORT_C void CMemSpyThreadInfoMemoryTracking::TrackingSetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory )
    {
    iTotalIncludesSharedMemory = aIncludesSharedMemory;

    if  ( iTracker )
        {
        iInfoCurrent->SetTotalIncludesSharedMemoryL( aIncludesSharedMemory );
        iInfoHWM->SetTotalIncludesSharedMemoryL( aIncludesSharedMemory );
        iInfoPeak->SetTotalIncludesSharedMemoryL( aIncludesSharedMemory );
        
        // Update totals
        TRAP_IGNORE( UpdateCaptionsL( iTracker->InfoCurrent(), iTracker->InfoHWMIncShared(), iTracker->InfoHWMExcShared() ) );
        }
    }


EXPORT_C void CMemSpyThreadInfoMemoryTracking::TrackingStartL()
    {
    if  ( iTracker == NULL )
        {
        CMemSpyProcess& process = Container().Thread().Process();
        CMemSpyEngine& engine = process.Engine();
        iTracker = &engine.HelperProcess().TrackerL( process );

        // Make sure we are also listening to the tracker!
        TrackingObserverAddL( *this );
        //
        CMemSpyThreadInfoMemoryTrackingStatisticsCurrent* infoCurrent = CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::NewLC( Container(), EFalse );
        CMemSpyThreadInfoMemoryTrackingStatisticsHWM* infoHWM = CMemSpyThreadInfoMemoryTrackingStatisticsHWM::NewLC( Container(), EFalse );
        CMemSpyThreadInfoMemoryTrackingStatisticsPeak* infoPeak = CMemSpyThreadInfoMemoryTrackingStatisticsPeak::NewLC( Container(), EFalse );
        //
        iInfoCurrent = infoCurrent;
        iInfoHWM = infoHWM;
        iInfoPeak = infoPeak;
        //
        CleanupStack::Pop( iInfoPeak );
        CleanupStack::Pop( infoHWM );
        CleanupStack::Pop( infoCurrent );
        //
        TrackingSetTotalIncludesSharedMemoryL( iTotalIncludesSharedMemory );
        }

    iTracker->StartL();

    UpdateCaptionsL();
    }


EXPORT_C void CMemSpyThreadInfoMemoryTracking::TrackingStopL()
    {
    if  ( iTracker != NULL )
        {
        iTracker->Stop();
        }

    UpdateCaptionsL();
    }


EXPORT_C void CMemSpyThreadInfoMemoryTracking::TrackingResetHWML()
    {
    if  ( iTracker != NULL )
        {
        iTracker->ResetHWML();
        }
    }


EXPORT_C void CMemSpyThreadInfoMemoryTracking::TrackingObserverAddL( MMemSpyEngineProcessMemoryTrackerObserver& aObserver )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::TrackingObserverAddL() - START - iTracker: 0x%08x, this: 0x%08x", iTracker, this );
#endif

    if  ( iTracker != NULL )
        {
        iTracker->AddObserverL( aObserver );
        }

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::TrackingObserverAddL() - END - iTracker: 0x%08x, this: 0x%08x", iTracker, this );
#endif
    }


EXPORT_C void CMemSpyThreadInfoMemoryTracking::TrackingObserverRemove( MMemSpyEngineProcessMemoryTrackerObserver& aObserver )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::TrackingObserverRemove() - START - iTracker: 0x%08x, this: 0x%08x", iTracker, this );
#endif

    if  ( iTracker != NULL )
        {
        iTracker->RemoveObserver( aObserver );
        }

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::TrackingObserverRemove() - END - iTracker: 0x%08x, this: 0x%08x", iTracker, this );
#endif
    }


EXPORT_C MDesCArray& CMemSpyThreadInfoMemoryTracking::InfoCurrent()
    {
    return *iInfoCurrent;
    }


EXPORT_C MDesCArray& CMemSpyThreadInfoMemoryTracking::InfoHWM()
    {
    return *iInfoHWM;
    }


EXPORT_C MDesCArray& CMemSpyThreadInfoMemoryTracking::InfoPeak()
    {
    return *iInfoPeak;
    }


void CMemSpyThreadInfoMemoryTracking::HandleMemoryTrackingStartedL()
    {
    __ASSERT_ALWAYS( iTracker, MemSpyEngineUtils::Panic( EMemSpyEnginePanicTrackerNull1 ) );
    CMemSpyThreadInfoItemBase::CItem& trackingItem = Item( 0 );
    trackingItem.SetOnOffL( iTracker->AmTracking() );
    //
    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


void CMemSpyThreadInfoMemoryTracking::HandleMemoryTrackingStoppedL()
    {
    __ASSERT_ALWAYS( iTracker, MemSpyEngineUtils::Panic( EMemSpyEnginePanicTrackerNull2 ) );
    CMemSpyThreadInfoItemBase::CItem& trackingItem = Item( 0 );
    trackingItem.SetOnOffL( iTracker->AmTracking() );
    //
    Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
    }


void CMemSpyThreadInfoMemoryTracking::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& aInfoCurrent, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::HandleMemoryChangedL() - START - this: 0x%08x", this );
#endif 

    __ASSERT_ALWAYS( iTracker, MemSpyEngineUtils::Panic( EMemSpyEnginePanicTrackerNull3 ) );
    UpdateCaptionsL( aInfoCurrent, aHWMInfoIncShared, aHWMInfoExcShared );

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyThreadInfoMemoryTracking::HandleMemoryChangedL() - END - this: 0x%08x", this );
#endif 
    }


void CMemSpyThreadInfoMemoryTracking::UpdateCaptionsL()
    {
    if  ( iTracker )
        {
        UpdateCaptionsL( iTracker->InfoCurrent(), iTracker->InfoHWMIncShared(), iTracker->InfoHWMExcShared() );
        }
    }


void CMemSpyThreadInfoMemoryTracking::UpdateCaptionsL( const TMemSpyDriverProcessInspectionInfo& aInfoCurrent, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared )
    {
    if  ( iTracker )
        {
        // Update caption
        Item( 0 ).SetOnOffL( TrackingActive() );
  
        if  ( TotalIncludesSharedMemory() )
            {
            Item( 1 ).SetLongL( aInfoCurrent.TotalIncShared() );
            Item( 2 ).SetLongL( aHWMInfoIncShared.TotalIncShared() );
            Item( 3 ).SetLongL( iTracker->InfoPeaks().TotalIncShared() );
            }
        else
            {
            Item( 1 ).SetLongL( aInfoCurrent.TotalExcShared() );
            Item( 2 ).SetLongL( aHWMInfoExcShared.TotalExcShared() );
            Item( 3 ).SetLongL( iTracker->InfoPeaks().TotalExcShared() );
            }

        Container().NotifyObserverL( MMemSpyThreadInfoContainerObserver::EInfoItemChanged, Type() );
        }
    }
































CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::CMemSpyThreadInfoMemoryTrackingStatisticsCurrent( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeMemoryTrackingCurrent, aAsyncConstruction ), iTotalIncludesSharedMemory( ETrue )
    {
    }


CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::~CMemSpyThreadInfoMemoryTrackingStatisticsCurrent()
    {
    if  ( iTracker )
        {
        iTracker->RemoveObserver( *this );
        }
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    //
    if  ( iTracker )
        {
        iTracker->RemoveObserver( *this );
        }
    iTracker = &Container().Engine().HelperProcess().TrackerL( Container().Thread().Process() );
    if  ( iTracker )
        {
        iTracker->AddObserverL( *this );
        //
        _LIT( KItem1, "Stack Memory" );
        AddItemDecimalL( KItem1, iTracker->InfoCurrent().iMemoryStack );
        //
        _LIT( KItem2, "Heap Memory" );
        AddItemDecimalL( KItem2, iTracker->InfoCurrent().iMemoryHeap );
        //
        _LIT( KItem3, "Local Chunk Memory" );
        AddItemDecimalL( KItem3, iTracker->InfoCurrent().iMemoryChunkLocal );
        //
        _LIT( KItem4, "Shared Chunk Memory" );
        AddItemDecimalL( KItem4, iTracker->InfoCurrent().iMemoryChunkShared );
        //
        _LIT( KItem5, "Global Data Memory" );
        AddItemDecimalL( KItem5, iTracker->InfoCurrent().iMemoryGlobalData );
        //
        _LIT( KItem6, "Total" );
        if  ( iTotalIncludesSharedMemory )
            {
            AddItemLongL( KItem6, iTracker->InfoCurrent().TotalIncShared() );
            }
        else
            {
            AddItemLongL( KItem6, iTracker->InfoCurrent().TotalExcShared() );
            }
        }
    }


CMemSpyThreadInfoMemoryTrackingStatisticsCurrent* CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoMemoryTrackingStatisticsCurrent* self = new(ELeave) CMemSpyThreadInfoMemoryTrackingStatisticsCurrent( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


TPtrC CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::Name() const
    {
    _LIT(KName, "\tCurrent Statistics");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::SetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory )
    {
    iTotalIncludesSharedMemory = aIncludesSharedMemory;
    
    // Update totals
    HandleMemoryChangedL( iTracker->ProcessId(), iTracker->InfoCurrent(), iTracker->InfoHWMIncShared(), iTracker->InfoHWMExcShared() );
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::HandleMemoryTrackingStartedL()
    {
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::HandleMemoryTrackingStoppedL()
    {
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsCurrent::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& aInfoCurrent, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoIncShared*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoExcShared*/ )
    {
    Item( 0 ).SetDecimalL( aInfoCurrent.iMemoryStack );
    Item( 1 ).SetDecimalL( aInfoCurrent.iMemoryHeap );
    Item( 2 ).SetDecimalL( aInfoCurrent.iMemoryChunkLocal );
    Item( 3 ).SetDecimalL( aInfoCurrent.iMemoryChunkShared );
    Item( 4 ).SetDecimalL( aInfoCurrent.iMemoryGlobalData );
    //
    if  ( iTotalIncludesSharedMemory )
        {
        Item( 5 ).SetLongL( aInfoCurrent.TotalIncShared() );
        }
    else
        {
        Item( 5 ).SetLongL( aInfoCurrent.TotalExcShared() );
        }
    }



















CMemSpyThreadInfoMemoryTrackingStatisticsPeak::CMemSpyThreadInfoMemoryTrackingStatisticsPeak( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeMemoryTrackingPeak, aAsyncConstruction ), iTotalIncludesSharedMemory( ETrue )
    {
    }


CMemSpyThreadInfoMemoryTrackingStatisticsPeak::~CMemSpyThreadInfoMemoryTrackingStatisticsPeak()
    {
    if  ( iTracker )
        {
        iTracker->RemoveObserver( *this );
        }
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsPeak::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    //
    if  ( iTracker )
        {
        iTracker->RemoveObserver( *this );
        }
    iTracker = &Container().Engine().HelperProcess().TrackerL( Container().Thread().Process() );
    if  ( iTracker )
        {
        iTracker->AddObserverL( *this );
        //
        _LIT( KItem1, "Stack Memory" );
        AddItemDecimalL( KItem1, iTracker->InfoPeaks().iMemoryStack );
        //
        _LIT( KItem2, "Heap Memory" );
        AddItemDecimalL( KItem2, iTracker->InfoPeaks().iMemoryHeap );
        //
        _LIT( KItem3, "Local Chunk Memory" );
        AddItemDecimalL( KItem3, iTracker->InfoPeaks().iMemoryChunkLocal );
        //
        _LIT( KItem4, "Shared Chunk Memory" );
        AddItemDecimalL( KItem4, iTracker->InfoPeaks().iMemoryChunkShared );
        //
        _LIT( KItem5, "Global Data Memory" );
        AddItemDecimalL( KItem5, iTracker->InfoPeaks().iMemoryGlobalData );
        //
        _LIT( KItem6, "Total" );
        if  ( iTotalIncludesSharedMemory )
            {
            AddItemLongL( KItem6, iTracker->InfoPeaks().TotalIncShared() );
            }
        else
            {
            AddItemLongL( KItem6, iTracker->InfoPeaks().TotalExcShared() );
            }
        }
    }


CMemSpyThreadInfoMemoryTrackingStatisticsPeak* CMemSpyThreadInfoMemoryTrackingStatisticsPeak::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoMemoryTrackingStatisticsPeak* self = new(ELeave) CMemSpyThreadInfoMemoryTrackingStatisticsPeak( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


TPtrC CMemSpyThreadInfoMemoryTrackingStatisticsPeak::Name() const
    {
    _LIT(KName, "\tPeak Statistics");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsPeak::SetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory )
    {
    iTotalIncludesSharedMemory = aIncludesSharedMemory;
    
    // Update totals
    HandleMemoryChangedL( iTracker->ProcessId(), iTracker->InfoCurrent(), iTracker->InfoHWMIncShared(), iTracker->InfoHWMExcShared() );
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsPeak::HandleMemoryTrackingStartedL()
    {
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsPeak::HandleMemoryTrackingStoppedL()
    {
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsPeak::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& /*aInfoCurrent*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoIncShared*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoExcShared*/ )
    {
    Item( 0 ).SetDecimalL( iTracker->InfoPeaks().iMemoryStack );
    Item( 1 ).SetDecimalL( iTracker->InfoPeaks().iMemoryHeap );
    Item( 2 ).SetDecimalL( iTracker->InfoPeaks().iMemoryChunkLocal );
    Item( 3 ).SetDecimalL( iTracker->InfoPeaks().iMemoryChunkShared );
    Item( 4 ).SetDecimalL( iTracker->InfoPeaks().iMemoryGlobalData );
    //
    if ( iTotalIncludesSharedMemory )
        {
        Item( 5 ).SetLongL( iTracker->InfoPeaks().TotalIncShared() );
        }
    else
        {
        Item( 5 ).SetLongL( iTracker->InfoPeaks().TotalExcShared() );
        }
    }
















CMemSpyThreadInfoMemoryTrackingStatisticsHWM::CMemSpyThreadInfoMemoryTrackingStatisticsHWM( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
:   CMemSpyThreadInfoItemBase( aContainer, EMemSpyThreadInfoItemTypeMemoryTrackingHWM, aAsyncConstruction ), iTotalIncludesSharedMemory( ETrue )
    {
    }


CMemSpyThreadInfoMemoryTrackingStatisticsHWM::~CMemSpyThreadInfoMemoryTrackingStatisticsHWM()
    {
    if  ( iTracker )
        {
        iTracker->RemoveObserver( *this );
        }
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsHWM::ConstructL()
    {
    CMemSpyEngine& engine = Container().Thread().Process().Engine();
    //
    if  ( iTracker )
        {
        iTracker->RemoveObserver( *this );
        }
    iTracker = &Container().Engine().HelperProcess().TrackerL( Container().Thread().Process() );
    if  ( iTracker )
        {
        iTracker->AddObserverL( *this );
        //
        _LIT( KItem1, "Stack Memory" );
        _LIT( KItem2, "Heap Memory" );
        _LIT( KItem3, "Local Chunk Memory" );
        _LIT( KItem4, "Shared Chunk Memory" );
        _LIT( KItem5, "Global Data Memory" );
        _LIT( KItem6, "Total" );
        //
        if  ( iTotalIncludesSharedMemory )
            {
            AddItemDecimalL( KItem1, iTracker->InfoHWMIncShared().iMemoryStack );
            AddItemDecimalL( KItem2, iTracker->InfoHWMIncShared().iMemoryHeap );
            AddItemDecimalL( KItem3, iTracker->InfoHWMIncShared().iMemoryChunkLocal );
            AddItemDecimalL( KItem4, iTracker->InfoHWMIncShared().iMemoryChunkShared );
            AddItemDecimalL( KItem5, iTracker->InfoHWMIncShared().iMemoryGlobalData );
            AddItemLongL(    KItem6, iTracker->InfoHWMIncShared().TotalIncShared() );
            }
        else
            {
            AddItemDecimalL( KItem1, iTracker->InfoHWMExcShared().iMemoryStack );
            AddItemDecimalL( KItem2, iTracker->InfoHWMExcShared().iMemoryHeap );
            AddItemDecimalL( KItem3, iTracker->InfoHWMExcShared().iMemoryChunkLocal );
            AddItemDecimalL( KItem4, iTracker->InfoHWMExcShared().iMemoryChunkShared );
            AddItemDecimalL( KItem5, iTracker->InfoHWMExcShared().iMemoryGlobalData );
            AddItemLongL(    KItem6, iTracker->InfoHWMExcShared().TotalExcShared() );
            }
        }
    }


CMemSpyThreadInfoMemoryTrackingStatisticsHWM* CMemSpyThreadInfoMemoryTrackingStatisticsHWM::NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction )
    {
    CMemSpyThreadInfoMemoryTrackingStatisticsHWM* self = new(ELeave) CMemSpyThreadInfoMemoryTrackingStatisticsHWM( aContainer, aAsyncConstruction );
    CleanupStack::PushL( self );
    if  ( !aAsyncConstruction )
        {
        self->ConstructL();
        }
    return self;
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsHWM::SetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory )
    {
    iTotalIncludesSharedMemory = aIncludesSharedMemory;
    
    // Update totals
    HandleMemoryChangedL( iTracker->ProcessId(), iTracker->InfoCurrent(), iTracker->InfoHWMIncShared(), iTracker->InfoHWMExcShared() );
    }


TPtrC CMemSpyThreadInfoMemoryTrackingStatisticsHWM::Name() const
    {
    _LIT(KName, "\tHigh-Water-Mark Statistics");
    return TPtrC( KName );
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsHWM::HandleMemoryTrackingStartedL()
    {
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsHWM::HandleMemoryTrackingStoppedL()
    {
    }


void CMemSpyThreadInfoMemoryTrackingStatisticsHWM::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& /*aInfoCurrent*/, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared )
    {
    if  ( iTotalIncludesSharedMemory )
        {
        Item( 0 ).SetDecimalL( aHWMInfoIncShared.iMemoryStack );
        Item( 1 ).SetDecimalL( aHWMInfoIncShared.iMemoryHeap );
        Item( 2 ).SetDecimalL( aHWMInfoIncShared.iMemoryChunkLocal );
        Item( 3 ).SetDecimalL( aHWMInfoIncShared.iMemoryChunkShared );
        Item( 4 ).SetDecimalL( aHWMInfoIncShared.iMemoryGlobalData );
        Item( 5 ).SetLongL(    aHWMInfoIncShared.TotalIncShared() );
        }
    else
        {
        Item( 0 ).SetDecimalL( aHWMInfoExcShared.iMemoryStack );
        Item( 1 ).SetDecimalL( aHWMInfoExcShared.iMemoryHeap );
        Item( 2 ).SetDecimalL( aHWMInfoExcShared.iMemoryChunkLocal );
        Item( 3 ).SetDecimalL( aHWMInfoExcShared.iMemoryChunkShared );
        Item( 4 ).SetDecimalL( aHWMInfoExcShared.iMemoryGlobalData );
        Item( 5 ).SetLongL(    aHWMInfoExcShared.TotalExcShared() );
        }
    }
