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

#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>

// System includes
#include <e32svr.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>



CMemSpyThreadInfoContainer::CMemSpyThreadInfoContainer( CMemSpyThread& aThread )
:   CMemSpyEngineObject( aThread ), iThread( &aThread )
    {
    }


CMemSpyThreadInfoContainer::~CMemSpyThreadInfoContainer()
    {
    CloseAllInfoItems();
    //
    iItems.Close();
    iObservers.Close();
    }


void CMemSpyThreadInfoContainer::ConstructItemByTypeL( TBool aAsync, TMemSpyThreadInfoItemType aType )
    {
    CMemSpyThreadInfoItemBase* item = NULL;
    //
    switch( aType )
        {
    case EMemSpyThreadInfoItemTypeGeneralInfo:
        item = CMemSpyThreadInfoGeneral::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeHeap:
        item = CMemSpyThreadInfoHeap::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeStack:
        item = CMemSpyThreadInfoStack::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeChunk:
        item = CMemSpyThreadInfoChunk::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeCodeSeg:
        item = CMemSpyThreadInfoCodeSeg::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeOpenFiles:
        item = CMemSpyThreadInfoOpenFiles::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeActiveObject:
        item = CMemSpyThreadInfoActiveObjects::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeServer:
        item = CMemSpyThreadInfoServer::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeSession:
        item = CMemSpyThreadInfoSession::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeSemaphore:
        item = CMemSpyThreadInfoSemaphore::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeMutex:
        item = CMemSpyThreadInfoMutex::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeTimer:
        item = CMemSpyThreadInfoTimer::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeLDD:
        item = CMemSpyThreadInfoLDD::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypePDD:
        item = CMemSpyThreadInfoPDD::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeLogicalChannel:
        item = CMemSpyThreadInfoLogicalChannel::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeChangeNotifier:
        item = CMemSpyThreadInfoChangeNotifier::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeUndertaker:
        item = CMemSpyThreadInfoUndertaker::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeOwnedThreadHandles:
        item = CMemSpyThreadInfoOwnedThreadHandles::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeOwnedProcessHandles:
        item = CMemSpyThreadInfoOwnedProcessHandles::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeOtherThreads:
        item = CMemSpyThreadInfoOtherThreads::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeOtherProcesses:
        item = CMemSpyThreadInfoOtherProcesses::NewLC( *this, aAsync );
        break;
    case EMemSpyThreadInfoItemTypeMemoryTracking:
        item = CMemSpyThreadInfoMemoryTracking::NewLC( *this, aAsync );
        break;
    default:
    case EMemSpyThreadInfoItemTypeMessageQueue:
    case EMemSpyThreadInfoItemTypeConditionalVariable:
        break;
        }
    //
    if  ( item )
        {
        iItems.AppendL( item );
        CleanupStack::Pop( item );
        }
    }


void CMemSpyThreadInfoContainer::ConstructL( TBool aAsync )
    {
    for( TInt type = EMemSpyThreadInfoItemTypeFirst; type<EMemSpyThreadInfoItemTypeLast; type++ )
        {
        const TMemSpyThreadInfoItemType realType = static_cast< TMemSpyThreadInfoItemType >( type );
        ConstructItemByTypeL( aAsync, realType );
        }
    }


CMemSpyThreadInfoContainer* CMemSpyThreadInfoContainer::NewL( CMemSpyThread& aThread, TBool aAsync )
    {
    CMemSpyThreadInfoContainer* self = CMemSpyThreadInfoContainer::NewLC( aThread, aAsync );
    CleanupStack::Pop( self );
    return self;
    }


CMemSpyThreadInfoContainer* CMemSpyThreadInfoContainer::NewLC( CMemSpyThread& aThread, TBool aAsync )
    {
    CMemSpyThreadInfoContainer* self = new(ELeave) CMemSpyThreadInfoContainer( aThread );
    CleanupStack::PushL( self );
    self->ConstructL( aAsync );
    return self;
    }


CMemSpyThreadInfoContainer* CMemSpyThreadInfoContainer::NewLC( CMemSpyThread& aThread, TMemSpyThreadInfoItemType aSpecificType )
    {
    CMemSpyThreadInfoContainer* self = new(ELeave) CMemSpyThreadInfoContainer( aThread );
    CleanupStack::PushL( self );
    self->ConstructItemByTypeL( EFalse, aSpecificType );
    return self;
    }


void CMemSpyThreadInfoContainer::AddItemL( TMemSpyThreadInfoItemType aType )
    {
    const TInt index = InfoItemIndexByType( aType );
    if  ( index == KErrNotFound )
        {
        ConstructItemByTypeL( EFalse, aType );
        }
    }


EXPORT_C void CMemSpyThreadInfoContainer::Open()
    {
    if  ( !OpenOrCloseInProgress() )
        {
        SetOpenOrCloseInProgress( ETrue );
        CMemSpyEngineObject::Open();
        SetOpenOrCloseInProgress( EFalse );
        }
    }


EXPORT_C void CMemSpyThreadInfoContainer::Close()
    {
    if  ( !OpenOrCloseInProgress() )
        {
        SetOpenOrCloseInProgress( ETrue );
        CMemSpyEngineObject::Close();
        SetOpenOrCloseInProgress( EFalse );
        }
    }


EXPORT_C void CMemSpyThreadInfoContainer::PrintL()
    {
    _LIT( KMemSpyFolder, "ThreadInfo" );
    _LIT( KMemSpyContext, "ThreadInfo - %S" );
    //
    CMemSpyEngine& engine = Engine();
    CMemSpyEngineOutputSink& sink = engine.Sink();
    //
    TFullName fullName( iThread->FullName() );
    HBufC* context = HBufC::NewLC( KMaxFileName * 2 );
    TPtr pContext( context->Des() );
    pContext.Format( KMemSpyContext, &fullName );
    sink.DataStreamBeginL( pContext, KMemSpyFolder );
    CleanupStack::PopAndDestroy( context );
    //
    sink.OutputSectionHeadingL( fullName, TChar('=') );
    sink.OutputBlankLineL();
    //
    const TInt count = iItems.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyThreadInfoItemBase* item = iItems[ i ];
        item->PrintL();
        }
    //
    sink.OutputBlankLineL();
    sink.DataStreamEndL();
    }


EXPORT_C TInt CMemSpyThreadInfoContainer::MdcaCount() const
    {
    return iItems.Count();
    }


EXPORT_C TPtrC CMemSpyThreadInfoContainer::MdcaPoint( TInt aIndex ) const
    {
    return iItems[ aIndex ]->Name();
    }


EXPORT_C CMemSpyEngine& CMemSpyThreadInfoContainer::Engine() const
    {
    return iThread->Engine();
    }


EXPORT_C CMemSpyThreadInfoItemBase& CMemSpyThreadInfoContainer::Item( TInt aIndex )
    {
    return *iItems[ aIndex ];
    }


EXPORT_C CMemSpyThreadInfoItemBase& CMemSpyThreadInfoContainer::Item( TMemSpyThreadInfoItemType aType )
    {
    const TInt index = InfoItemIndexByType( aType );
    CMemSpyThreadInfoItemBase* ret = iItems[ index ];
    __ASSERT_ALWAYS( ret != NULL, User::Invariant() );
    return *ret;
    }


EXPORT_C TInt CMemSpyThreadInfoContainer::InfoItemIndexByType( TMemSpyThreadInfoItemType aType )
    {
    TInt index = KErrNotFound;
    //
    const TInt count = iItems.Count();
    for(TInt i=0; i<count; i++)
        {
        CMemSpyThreadInfoItemBase* item = iItems[ i ];
        if  ( item->Type() == aType )
            {
            index = i;
            break;
            }
        }
    //
    return index;
    }


EXPORT_C void CMemSpyThreadInfoContainer::ObserverAddL( MMemSpyThreadInfoContainerObserver& aObserver )
    {
    const TInt count = iObservers.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        MMemSpyThreadInfoContainerObserver* observer = iObservers[ i ];
        if  ( observer == &aObserver )
            {
            return;
            }
        }

    iObservers.AppendL( &aObserver );
    }


EXPORT_C void CMemSpyThreadInfoContainer::ObserverRemove( MMemSpyThreadInfoContainerObserver& aObserver )
    {
    const TInt count = iObservers.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        MMemSpyThreadInfoContainerObserver* observer = iObservers[ i ];
        if  ( observer == &aObserver )
            {
            iObservers.Remove( i );
            break;
            }
        }
    }


void CMemSpyThreadInfoContainer::NotifyObserverL( MMemSpyThreadInfoContainerObserver::TEvent aEvent, TMemSpyThreadInfoItemType aType )
    {
    if  ( aEvent == MMemSpyThreadInfoContainerObserver::EInfoItemDestroyed )
        {
        // Make sure we remove dead item
        const TInt index = InfoItemIndexByType( aType );
        if  ( index >= 0 )
            {
            iItems.Remove( index );
            }
        }
        
    const TInt count = iObservers.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        MMemSpyThreadInfoContainerObserver* observer = iObservers[ i ];
        observer->HandleMemSpyEngineInfoContainerEventL( aEvent, aType );
        }
    }


void CMemSpyThreadInfoContainer::OpenAllInfoItems()
    {
    const TInt count = iItems.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        CMemSpyThreadInfoItemBase* item = iItems[ i ];
        item->Open();
        }
    }


void CMemSpyThreadInfoContainer::CloseAllInfoItems()
    {
    const TInt count = iItems.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        CMemSpyThreadInfoItemBase* item = iItems[ i ];
        item->Close();
        }
    }

