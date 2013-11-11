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

#include <memspy/engine/memspyenginehelperchunk.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>

// Constants
const TInt KMemSpyEngineMaxChunkCount = 768;

// Literal constants
_LIT( KMemSpyEngineChunkListOutputComma, ", " );



CMemSpyEngineHelperChunk::CMemSpyEngineHelperChunk( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperChunk::~CMemSpyEngineHelperChunk()
    {
    }


void CMemSpyEngineHelperChunk::ConstructL()
    {
    }


CMemSpyEngineHelperChunk* CMemSpyEngineHelperChunk::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperChunk* self = new(ELeave) CMemSpyEngineHelperChunk( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperChunk::OutputChunkInfoForThreadL( const CMemSpyThread& aThread )
    {
    OutputChunkInfoForThreadL( aThread.Id() );
    }


EXPORT_C void CMemSpyEngineHelperChunk::OutputChunkInfoForThreadL( TThreadId aTid )
    {
    TBuf<512> lineBuffer;
    OutputChunkInfoForThreadL( aTid, lineBuffer );
    }


EXPORT_C void CMemSpyEngineHelperChunk::OutputChunkInfoForThreadL( TThreadId aTid, TDes& aLineBuffer )
    {
    DoOutputChunkInfoForObjectL( aTid, aLineBuffer, EThread );
    }


EXPORT_C void CMemSpyEngineHelperChunk::OutputChunkInfoForProcessL( const CMemSpyProcess& aProcess )
    {
    OutputChunkInfoForProcessL( aProcess.Id() );
    }


EXPORT_C void CMemSpyEngineHelperChunk::OutputChunkInfoForProcessL( TProcessId aPid )
    {
    TBuf<512> lineBuffer;
    OutputChunkInfoForProcessL( aPid, lineBuffer );
    }


EXPORT_C void CMemSpyEngineHelperChunk::OutputChunkInfoForProcessL( TProcessId aPid, TDes& aLineBuffer )
    {
    DoOutputChunkInfoForObjectL( aPid, aLineBuffer, EProcess );
    }


EXPORT_C CMemSpyEngineChunkList* CMemSpyEngineHelperChunk::ListL()
    {
    RArray<TAny*> handles( 128 );
    CleanupClosePushL( handles );
    //
    GetChunkHandlesL( handles, EAll );
    CMemSpyEngineChunkList* list = CreateListFromHandlesL( handles );
    //
    CleanupStack::PopAndDestroy( &handles );
    return list;
    }


EXPORT_C CMemSpyEngineChunkList* CMemSpyEngineHelperChunk::ListForThreadL( TThreadId aTid )
    {
    RArray<TAny*> handles( 128 );
    CleanupClosePushL( handles );
    //
    GetChunkHandlesL( handles, EThread, aTid );
    CMemSpyEngineChunkList* list = CreateListFromHandlesL( handles );
    //
    CleanupStack::PopAndDestroy( &handles );
    return list;
    }


EXPORT_C CMemSpyEngineChunkList* CMemSpyEngineHelperChunk::ListForProcessL( TProcessId aPid )
    {
    RArray<TAny*> handles( 128 );
    CleanupClosePushL( handles );
    //
    GetChunkHandlesL( handles, EProcess, aPid );
    CMemSpyEngineChunkList* list = CreateListFromHandlesL( handles );
    //
    CleanupStack::PopAndDestroy( &handles );
    return list;
    }


void CMemSpyEngineHelperChunk::DoOutputChunkInfoForObjectL( TUint aId, TDes& aLineBuffer, TType aType )
    {
    TFullName ownerName;
	//
    RArray<TAny*> handles( 128 );
    CleanupClosePushL( handles );
    //
    GetChunkHandlesL( handles, aType, aId );
    CMemSpyEngineChunkList* list = CreateListFromHandlesL( handles );
    //
    CleanupStack::PopAndDestroy( &handles );
    CleanupStack::PushL( list );
    //
    const TInt count = list->Count();
    for (TInt i=0; i<count; i++)
    	{
        const CMemSpyEngineChunkEntry& entry = list->At( i );
        //
        _LIT(KLine1, "Name");
        iEngine.Sink().OutputItemAndValueL( KLine1, entry.Name() );
        //
        _LIT(KLine2, "Owner");
        entry.OwnerName( ownerName );
        iEngine.Sink().OutputItemAndValueL( KLine2, ownerName );
        //
        _LIT(KLine3, "Address");
        _LIT(KLine3Format, "0x%08x - 0x%08x");
        aLineBuffer.Format(KLine3Format, entry.BaseAddress(), entry.UpperAddress() );
        iEngine.Sink().OutputItemAndValueL( KLine3, aLineBuffer );
        //
        _LIT(KLine4, "Size (max)");
        _LIT(KLine4Format, "%d (%d)");
        aLineBuffer.Format(KLine4Format, entry.Size(), entry.MaxSize());
        iEngine.Sink().OutputItemAndValueL( KLine4, aLineBuffer );
        //
        if  ( i < count - 1 )
            {
            iEngine.Sink().OutputBlankLineL();
    		}
        }
    //
    CleanupStack::PopAndDestroy( list );
    }


void CMemSpyEngineHelperChunk::GetChunkHandlesL( RArray<TAny*>& aHandles, TType aType, TUint aId )
    {
	TAny* handles[ KMemSpyEngineMaxChunkCount ];
	TInt count = KMemSpyEngineMaxChunkCount;

    TInt r = 0;
    //
    switch( aType )
        {
    case EThread:
        r = iEngine.Driver().GetChunkHandlesForThread( aId, handles, count );
        break;
    case EProcess:
        r = iEngine.Driver().GetChunkHandlesForProcess( aId, handles, count );
        break;
    default:
    case EAll:
        r = iEngine.Driver().GetChunkHandles( handles, count );
        break;
        }
    //
    if  ( r == KErrNone )
    	{
        count = Min( count, KMemSpyEngineMaxChunkCount );
        for( TInt index = 0; index < count; index++ )
            {
            TAny* handle = handles[ index ];

            if  ( aHandles.Find( handle ) == KErrNotFound )
                {
                aHandles.AppendL( handle );
                }
            }
        }
    }


CMemSpyEngineChunkList* CMemSpyEngineHelperChunk::CreateListFromHandlesL( const RArray<TAny*>& aHandles )
    {
    CMemSpyEngineChunkList* list = CMemSpyEngineChunkList::NewLC( iEngine );
    //
    TMemSpyDriverChunkInfo info;
    const TInt count = aHandles.Count();
    //
    for( TInt i=0; i<count; i++ )
        {
        TAny* handle = aHandles[ i ];
        //
    	const TInt error = iEngine.Driver().GetChunkInfo( handle, info );
        if  ( error == KErrNone )
            {
            CMemSpyEngineChunkEntry* entry = CMemSpyEngineChunkEntry::NewLC( iEngine, info );
            list->AddItemL( entry );
            CleanupStack::Pop( entry );
            }
        }
    //
    list->SortBySizeL();
    //
    CleanupStack::Pop( list );
    return list;
    }













































CMemSpyEngineChunkList::CMemSpyEngineChunkList( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }


EXPORT_C CMemSpyEngineChunkList::~CMemSpyEngineChunkList()
    {
    iItems.ResetAndDestroy();
    iItems.Close();
    }


void CMemSpyEngineChunkList::ConstructL()
    {
    }


CMemSpyEngineChunkList* CMemSpyEngineChunkList::NewLC( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineChunkList* self = new(ELeave) CMemSpyEngineChunkList( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TInt CMemSpyEngineChunkList::Count() const
    {
    return iItems.Count();
    }


EXPORT_C CMemSpyEngineChunkEntry& CMemSpyEngineChunkList::At( TInt aIndex )
    {
    return *iItems[ aIndex ];
    }


EXPORT_C const CMemSpyEngineChunkEntry& CMemSpyEngineChunkList::At( TInt aIndex ) const
    {
    return *iItems[ aIndex ];
    }


EXPORT_C void CMemSpyEngineChunkList::SortByNameL()
    {
    TLinearOrder< CMemSpyEngineChunkEntry > comparer( CompareByName );
    iItems.Sort( comparer );
    }


EXPORT_C void CMemSpyEngineChunkList::SortBySizeL()
    {
    TLinearOrder< CMemSpyEngineChunkEntry > comparer( CompareBySize );
    iItems.Sort( comparer );
    }


EXPORT_C TInt CMemSpyEngineChunkList::ItemIndex( const CMemSpyEngineChunkEntry& aEntry ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineChunkEntry* item = iItems[ i ];
        //
        if  ( item == &aEntry )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineChunkList::OutputDataColumnsL( CMemSpyEngine& aEngine )
    {
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );

    //
    _LIT(KCol1, "Name");
    pColumns.Append( KCol1 );
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    //
    _LIT(KCol2, "Owning Process");
    pColumns.Append( KCol2 );
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    //
    _LIT(KCol3, "Size");
    pColumns.Append( KCol3 );
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    //
    _LIT(KCol4, "Max. Size");
    pColumns.Append( KCol4 );
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    //
    _LIT(KCol5, "Address");
    pColumns.Append( KCol5 );

    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C TBool CMemSpyEngineChunkList::EntryExists( TAny* aHandle ) const
    {
    TBool ret = EFalse;
    //
    const TInt count = Count();
    for( TInt i=0; i<count && !ret; i++ )
        {
        const CMemSpyEngineChunkEntry* item = iItems[ i ];
        ret = ( item->Handle() == aHandle );
        }
    //
    return ret;
    }


void CMemSpyEngineChunkList::AddItemL( CMemSpyEngineChunkEntry* aItem )
    {
    iItems.AppendL( aItem );
    }


void CMemSpyEngineChunkList::Remove( TInt aIndex )
    {
    CMemSpyEngineChunkEntry* item = iItems[ aIndex ];
    delete item;
    iItems.Remove( aIndex );
    }


void CMemSpyEngineChunkList::RemoveByHandle( TAny* aChunkHandle )
    {
    const TInt index = ItemIndexByChunkHandle( aChunkHandle );
    if  ( index >= 0 )
        {
        Remove( index );
        }
    }


TInt CMemSpyEngineChunkList::ItemIndexByProcessId( TUint aPid ) const
    {
    const TInt ret = ItemIndexByProcessId( aPid, 0 );
    return ret;
    }


TInt CMemSpyEngineChunkList::ItemIndexByProcessId( TUint aPid, TInt aStartIndex ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=aStartIndex; i<count; i++ )
        {
        const CMemSpyEngineChunkEntry* item = iItems[ i ];
        //
        if  ( item->OwnerId() == aPid )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


TInt CMemSpyEngineChunkList::ItemIndexByProcessId( TUint aPid, TMemSpyDriverChunkType aType ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineChunkEntry* item = iItems[ i ];
        //
        if  ( item->OwnerId() == aPid && item->Info().iType == aType )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


TInt CMemSpyEngineChunkList::ItemIndexByChunkHandle( TAny* aHandle ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineChunkEntry* item = iItems[ i ];
        //
        if  ( item->Handle() == aHandle )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


EXPORT_C TInt CMemSpyEngineChunkList::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineChunkList::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineChunkEntry& item = At( aIndex );
    return TPtrC( item.Caption() );
    }


TInt CMemSpyEngineChunkList::CompareByName( const CMemSpyEngineChunkEntry& aLeft, const CMemSpyEngineChunkEntry& aRight )
    {
    const TInt ret = aLeft.Name().CompareF( aRight.Name() );
    return ret;
    }


TInt CMemSpyEngineChunkList::CompareBySize( const CMemSpyEngineChunkEntry& aLeft, const CMemSpyEngineChunkEntry& aRight )
    {
    TInt ret = -1;
    //
    if  ( aLeft.Size() < aRight.Size() )
        {
        ret = 1;
        }
    else if ( aLeft.Size() == aRight.Size() )
        {
        ret = 0;
        }
    //
    return ret;
    }


















CMemSpyEngineChunkEntry::CMemSpyEngineChunkEntry( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }


CMemSpyEngineChunkEntry::~CMemSpyEngineChunkEntry()
    {
    delete iCaption;
    delete iInfo;
    delete iList;
    }


void CMemSpyEngineChunkEntry::ConstructL( const TMemSpyDriverChunkInfo& aInfo )
    {
    // Copy info
    iInfo = new(ELeave) TMemSpyDriverChunkInfo();
    *iInfo = aInfo;

    // Make caption
    TBuf<KMaxFullName+128> item;
    _LIT(KCaptionFormat, "\t%S\t\t%d");
    item.Format( KCaptionFormat, &Name(), Size() );
    iCaption = item.AllocL();

    // Make other items
    iList = CMemSpyEngineOutputList::NewL();

    _LIT( KEntry0, "Name");
    iList->AddItemL( KEntry0, Name() );

    _LIT( KEntryType, "Type" );
    switch( aInfo.iType )
        {
    default:
    case EMemSpyDriverChunkTypeUnknown:
        iList->AddItemL( KEntryType, _L("Unknown") );
        break;
    case EMemSpyDriverChunkTypeHeap:
        iList->AddItemL( KEntryType, _L("Heap") );
        break;
    case EMemSpyDriverChunkTypeHeapKernel:
        iList->AddItemL( KEntryType, _L("Kernel Heap") );
        break;
    case EMemSpyDriverChunkTypeStackAndProcessGlobalData:
        iList->AddItemL( KEntryType, _L("Stack and Process Global Data") );
        break;
    case EMemSpyDriverChunkTypeStackKernel:
        iList->AddItemL( KEntryType, _L("Kernel Stack") );
        break;
    case EMemSpyDriverChunkTypeGlobalData:
        iList->AddItemL( KEntryType, _L("Global Data") );
        break;
    case EMemSpyDriverChunkTypeCode:
        iList->AddItemL( KEntryType, _L("Code") );
        break;
    case EMemSpyDriverChunkTypeCodeGlobal:
        iList->AddItemL( KEntryType, _L("Global Code") );
        break;
    case EMemSpyDriverChunkTypeCodeSelfModifiable:
        iList->AddItemL( KEntryType, _L("Self Modifiable Code") );
        break;
    case EMemSpyDriverChunkTypeLocal:
        iList->AddItemL( KEntryType, _L("Local") );
        break;
    case EMemSpyDriverChunkTypeGlobal:
        iList->AddItemL( KEntryType, _L("Global") );
        break;
    case EMemSpyDriverChunkTypeRamDrive:
        iList->AddItemL( KEntryType, _L("RAM Drive") );
        break;
        }

    _LIT( KEntry1, "Owning Process");
    OwnerName( item );
    iList->AddItemL( KEntry1, item );

    _LIT( KEntry2, "Address" );
    _LIT( KEntry2Format, "0x%08x - 0x%08x" );
    item.Format( KEntry2Format, BaseAddress(), UpperAddress() );
    iList->AddItemL( KEntry2, item );

    _LIT( KEntry3, "Size");
    iList->AddItemL( KEntry3, Size() );

    _LIT( KEntry4, "Max. Size");
    iList->AddItemL( KEntry4, MaxSize() );

    _LIT( KEntryAttributeFormat, "Attribute %d");
    TInt attribNum = 0;
    //
    if  ( aInfo.iAttributes & ENormal )
        {
        item.Format( KEntryAttributeFormat, ++attribNum );
        iList->AddItemL( item, _L("Normal") );
        }
    if  ( aInfo.iAttributes & EDoubleEnded )
        {
        item.Format( KEntryAttributeFormat, ++attribNum );
        iList->AddItemL( item, _L("Double Ended") );
        }
    if  ( aInfo.iAttributes & EDisconnected )
        {
        item.Format( KEntryAttributeFormat, ++attribNum );
        iList->AddItemL( item, _L("Disconnected") );
        }
    if  ( aInfo.iAttributes & EConstructed )
        {
        item.Format( KEntryAttributeFormat, ++attribNum );
        iList->AddItemL( item, _L("Constructed") );
        }
    if  ( aInfo.iAttributes & EMemoryNotOwned )
        {
        item.Format( KEntryAttributeFormat, ++attribNum );
        iList->AddItemL( item, _L("Memory Not Owned") );
        }
    }


CMemSpyEngineChunkEntry* CMemSpyEngineChunkEntry::NewLC( CMemSpyEngine& aEngine, const TMemSpyDriverChunkInfo& aInfo )
    {
    CMemSpyEngineChunkEntry* self = new(ELeave) CMemSpyEngineChunkEntry( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


const TDesC& CMemSpyEngineChunkEntry::Name() const
    {
    return iInfo->iName;
    }


TAny* CMemSpyEngineChunkEntry::Handle() const 
    {
    return iInfo->iHandle;
    }


TInt CMemSpyEngineChunkEntry::Size() const
    {
    return iInfo->iSize;
    }


TInt CMemSpyEngineChunkEntry::MaxSize() const
    {
    return iInfo->iMaxSize;
    }


TAny* CMemSpyEngineChunkEntry::BaseAddress() const
    { 
    return iInfo->iBaseAddress;
    }


TAny* CMemSpyEngineChunkEntry::UpperAddress() const
    {
    return (TAny*) (TUint(BaseAddress()) + TUint(Size()));
    }


TUint CMemSpyEngineChunkEntry::OwnerId() const
    {
    return iInfo->iOwnerId;
    }


EXPORT_C void CMemSpyEngineChunkEntry::OwnerName( TDes& aDes ) const
    {
    aDes.Zero();
    AppendOwnerName( aDes );
    }


EXPORT_C void CMemSpyEngineChunkEntry::AppendOwnerName( TDes& aDes ) const
    {
    RProcess owner;
    const TInt error = owner.Open( (TProcessId) OwnerId() );
    if  ( error == KErrNone )
        {
        aDes.Append( owner.FullName() );
        owner.Close();
        }
    }


EXPORT_C void CMemSpyEngineChunkEntry::OutputDataL( CMemSpyEngineHelperChunk& aHelper ) const
    {
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );

    // Name
    pColumns.Copy( Name() );
    MemSpyEngineUtils::TextAfterDoubleColon( pColumns );
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    // Owning Process
    AppendOwnerName( pColumns );
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    // Size
    pColumns.AppendNum( Size(), EDecimal ); 
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    // Max. Size
    pColumns.AppendNum( MaxSize(), EDecimal ); 
    pColumns.Append( KMemSpyEngineChunkListOutputComma );

    // Address
    _LIT( KAddressFormat, "0x%08x - 0x%08x");
    pColumns.AppendFormat( KAddressFormat, BaseAddress(), UpperAddress() );

    //
    aHelper.Engine().Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C TInt CMemSpyEngineChunkEntry::MdcaCount() const
    {
    return iList->MdcaCount();
    }


EXPORT_C TPtrC CMemSpyEngineChunkEntry::MdcaPoint( TInt aIndex ) const
    {
    return iList->MdcaPoint( aIndex );
    }


TMemSpyDriverChunkInfo& CMemSpyEngineChunkEntry::Info()
    {
    return *iInfo;
    }


const TMemSpyDriverChunkInfo& CMemSpyEngineChunkEntry::Info() const
    {
    return *iInfo;
    }


