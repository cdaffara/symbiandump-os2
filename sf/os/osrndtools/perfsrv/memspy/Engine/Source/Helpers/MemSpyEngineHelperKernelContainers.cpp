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

#include<memspy/engine/memspyenginehelperkernelcontainers.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// Constants
const TInt KMaxGenericHandleCount = 1024 * 2;
_LIT( KMemSpyMarkerKernelContainer, "<%SMEMSPY_KERNEL_CONTAINER_%02d>" );
_LIT( KMemSpyMarkerKernelContainersAll, "<%SMEMSPY_KERNEL_CONTAINERS>" );



CMemSpyEngineHelperKernelContainers::CMemSpyEngineHelperKernelContainers( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperKernelContainers::~CMemSpyEngineHelperKernelContainers()
    {
    }


void CMemSpyEngineHelperKernelContainers::ConstructL()
    {
    }


CMemSpyEngineHelperKernelContainers* CMemSpyEngineHelperKernelContainers::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperKernelContainers* self = new(ELeave) CMemSpyEngineHelperKernelContainers( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CMemSpyEngineGenericKernelObjectList* CMemSpyEngineHelperKernelContainers::ObjectsForSpecificContainerL( TMemSpyDriverContainerType aForContainer )
    {
    CMemSpyEngineGenericKernelObjectList* list = CMemSpyEngineGenericKernelObjectList::NewLC( aForContainer, iEngine );
    AddObjectsFromContainerL( *list, aForContainer );
    CleanupStack::Pop( list );
    return list;
    }


EXPORT_C CMemSpyEngineGenericKernelObjectContainer* CMemSpyEngineHelperKernelContainers::ObjectsAllL()
    {
    CMemSpyEngineGenericKernelObjectContainer* container = CMemSpyEngineGenericKernelObjectContainer::NewLC();
    //
    for( TInt i=EMemSpyDriverContainerTypeFirst; i<=EMemSpyDriverContainerTypeLast; i++ )
        {
        const TMemSpyDriverContainerType type = static_cast< TMemSpyDriverContainerType >( i );
        //
        CMemSpyEngineGenericKernelObjectList* list = CMemSpyEngineGenericKernelObjectList::NewLC( type, iEngine );
        AddObjectsFromContainerL( *list, type );
        container->AddListL( list );
        CleanupStack::Pop( list );
        }
    //
    CleanupStack::Pop( container );
    return container;
    }


CMemSpyEngineGenericKernelObjectContainer* CMemSpyEngineHelperKernelContainers::ObjectsAllLightweightL()
    {
    CMemSpyEngineGenericKernelObjectContainer* container = CMemSpyEngineGenericKernelObjectContainer::NewLC();
    //
    for( TInt i=EMemSpyDriverContainerTypeFirst; i<=EMemSpyDriverContainerTypeLast; i++ )
        {
        const TMemSpyDriverContainerType type = static_cast< TMemSpyDriverContainerType >( i );
        //
        CMemSpyEngineGenericKernelObjectList* list = CMemSpyEngineGenericKernelObjectList::NewLC( type, iEngine );
        AddObjectsFromContainerL( *list, type, EFalse );
        container->AddListL( list );
        CleanupStack::Pop( list );
        }
    //
    CleanupStack::Pop( container );
    return container;
    }


TMemSpyDriverContainerType CMemSpyEngineHelperKernelContainers::MapToType( TObjectType aType )
    {
    TMemSpyDriverContainerType ret = EMemSpyDriverContainerTypeUnknown;
    //
    if ( aType >= EMemSpyDriverContainerTypeFirst && aType <= EMemSpyDriverContainerTypeLast )
        {
        ret = (TMemSpyDriverContainerType) aType;
        }
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineHelperKernelContainers::OutputL( const CMemSpyEngineGenericKernelObjectContainer& aContainer ) const
    {
    aContainer.OutputL( iEngine.Sink() );
    }


CMemSpyEngine& CMemSpyEngineHelperKernelContainers::Engine() const
    {
    return iEngine;
    }


void CMemSpyEngineHelperKernelContainers::AddObjectsFromContainerL( CMemSpyEngineGenericKernelObjectList& aList, TMemSpyDriverContainerType aContainer, TBool aGetInfo )
    {
	TAny* handles[KMaxGenericHandleCount];
	TInt c = KMaxGenericHandleCount;

	TInt r = iEngine.Driver().GetContainerHandles( aContainer, handles, c );
	if  ( r == KErrNone )
    	{
        if  ( c > 0 )
            {
        	if (c > KMaxGenericHandleCount)
        		{
        		c = KMaxGenericHandleCount;
        		}

        	TMemSpyDriverHandleInfoGeneric info;
        	for (TInt i=0; i<c && r == KErrNone; i++)
        		{
                if  ( aGetInfo )
                    {
        		    r = iEngine.Driver().GetGenericHandleInfo( KMemSpyDriverEnumerateContainerHandles, aContainer, handles[i], info );
        		    if  ( r == KErrNone )
        			    {
        			    aList.AddItemL( info );
        			    }
                    }
                else
                    {
                    aList.AddItemL( handles[ i ] );
                    }
        		}
            }
        }
    }































CMemSpyEngineGenericKernelObjectList::CMemSpyEngineGenericKernelObjectList( TMemSpyDriverContainerType aType )
:   iType( aType ), iItems( 25 ), iHandles( 25 )
    {
    }


EXPORT_C CMemSpyEngineGenericKernelObjectList::~CMemSpyEngineGenericKernelObjectList()
    {
    delete iName;
    iHandles.Close();
    iItems.Close();
    }


void CMemSpyEngineGenericKernelObjectList::ConstructL( CMemSpyEngine& aEngine )
    {
    iName = HBufC::NewL( KMaxFullName );
    iSize = aEngine.Driver().GetApproximateKernelObjectSize( iType );
    UpdateNameL();
    }


CMemSpyEngineGenericKernelObjectList* CMemSpyEngineGenericKernelObjectList::NewLC( TMemSpyDriverContainerType aType, CMemSpyEngine& aEngine )
    {
    CMemSpyEngineGenericKernelObjectList* self = new(ELeave) CMemSpyEngineGenericKernelObjectList( aType );
    CleanupStack::PushL( self );
    self->ConstructL( aEngine );
    return self;
    }


EXPORT_C TInt CMemSpyEngineGenericKernelObjectList::Count() const
    {
    return iHandles.Count();
    }


EXPORT_C const TMemSpyDriverHandleInfoGeneric& CMemSpyEngineGenericKernelObjectList::At( TInt aIndex ) const
    {
    return iItems[ aIndex ];
    }


EXPORT_C TMemSpyDriverContainerType CMemSpyEngineGenericKernelObjectList::Type() const
    {
    return iType;
    }

EXPORT_C TInt CMemSpyEngineGenericKernelObjectList::Size() const
    {
    return iSize;
    }

EXPORT_C TInt CMemSpyEngineGenericKernelObjectList::ItemsCount() const
	{
	return iItems.Count();
	}
    

EXPORT_C TPtrC CMemSpyEngineGenericKernelObjectList::TypeAsString( TMemSpyDriverContainerType aType )
    {
    _LIT( KTypeUnknown, "Unknown Type" );
    _LIT( KTypeThread, "Thread" );
    _LIT( KTypeProcess, "Process" );
    _LIT( KTypeChunk, "Chunk" );
    _LIT( KTypeLibrary, "Library" );
    _LIT( KTypeSemaphore, "Semaphore" );
    _LIT( KTypeMutex, "Mutex" );
    _LIT( KTypeTimer, "Timer" );
    _LIT( KTypeServer, "Server" );
    _LIT( KTypeSession, "Session" );
    _LIT( KTypeLogicalDevice, "Logical Device" );
    _LIT( KTypePhysicalDevice, "Physical Device" );
    _LIT( KTypeLogicalChannel, "Logical Channel" );
    _LIT( KTypeChangeNotifier, "Change Notifier" );
    _LIT( KTypeUndertaker, "Undertaker" );
    _LIT( KTypeMsgQueue, "Msg. Queue" );
    _LIT( KTypePropertyRef, "Property Ref." );
    _LIT( KTypeCondVar, "Conditional Var." );
     //
    TPtrC pType( KTypeUnknown );
    //
    switch( aType )
        {
    case EMemSpyDriverContainerTypeThread:
        pType.Set( KTypeThread );
        break;
    case EMemSpyDriverContainerTypeProcess:
        pType.Set( KTypeProcess );
        break;
    case EMemSpyDriverContainerTypeChunk:
        pType.Set( KTypeChunk );
        break;
    case EMemSpyDriverContainerTypeLibrary:
        pType.Set( KTypeLibrary );
        break;
    case EMemSpyDriverContainerTypeSemaphore:
        pType.Set( KTypeSemaphore );
        break;
    case EMemSpyDriverContainerTypeMutex:
        pType.Set( KTypeMutex );
        break;
    case EMemSpyDriverContainerTypeTimer:
        pType.Set( KTypeTimer );
        break;
    case EMemSpyDriverContainerTypeServer:
        pType.Set( KTypeServer );
        break;
    case EMemSpyDriverContainerTypeSession:
        pType.Set( KTypeSession );
        break;
    case EMemSpyDriverContainerTypeLogicalDevice:
        pType.Set( KTypeLogicalDevice );
        break;
    case EMemSpyDriverContainerTypePhysicalDevice:
        pType.Set( KTypePhysicalDevice );
        break;
    case EMemSpyDriverContainerTypeLogicalChannel:
        pType.Set( KTypeLogicalChannel );
        break;
    case EMemSpyDriverContainerTypeChangeNotifier:
        pType.Set( KTypeChangeNotifier );
        break;
    case EMemSpyDriverContainerTypeUndertaker:
        pType.Set( KTypeUndertaker );
        break;
    case EMemSpyDriverContainerTypeMsgQueue:
        pType.Set( KTypeMsgQueue );
        break;
    case EMemSpyDriverContainerTypePropertyRef:
        pType.Set( KTypePropertyRef );
        break;
    case EMemSpyDriverContainerTypeCondVar:
        pType.Set( KTypeCondVar );
        break;
        
    default:
        break;
        }
    //
    return pType;
    }


EXPORT_C TPtrC CMemSpyEngineGenericKernelObjectList::Name() const
    {
    return TPtrC( *iName );
    }


EXPORT_C void CMemSpyEngineGenericKernelObjectList::OutputL( CMemSpyEngineOutputSink& aSink ) const
    {
    // Start marker
    aSink.OutputLineFormattedL( KMemSpyMarkerKernelContainer, &KNullDesC, iType );

    TBuf<70> itemPrefix;
    TBuf<KMaxFullName + 128> buf;
    const TInt entryCount = Count();
    const TInt64 size( iSize * entryCount );
    const TMemSpySizeText sizeText( MemSpyEngineUtils::FormatSizeText( size, 0 ) );
    //    
    _LIT(KContainerTitle, "CONTAINER \'%S\' => %d item(s), %S");
    const TPtrC pType( TypeAsString( iType ) );
    buf.Format(KContainerTitle, &pType, entryCount, &sizeText);
    aSink.OutputSectionHeadingL( buf, '=' );
    //
    for( TInt entryIndex=0; entryIndex<entryCount; entryIndex++ )
        {
        const TMemSpyDriverHandleInfoGeneric& entry = At( entryIndex );
        //
        _LIT( KItemPrefixFormat, "[KernCont] {%04d} 0x%08x" );
        itemPrefix.Format( KItemPrefixFormat, entryIndex, entry.iHandle );

        buf.Copy( entry.iName );

        aSink.OutputItemAndValueL( itemPrefix, buf );
        }

    // End marker
    aSink.OutputBlankLineL();
    aSink.OutputLineFormattedL( KMemSpyMarkerKernelContainer, &KMemSpySinkTagClose, iType );
    aSink.OutputBlankLineL();
    }


TAny* CMemSpyEngineGenericKernelObjectList::HandleAt( TInt aIndex ) const
    {
    return iHandles[ aIndex ];
    }


void CMemSpyEngineGenericKernelObjectList::AddItemL( TAny* aHandle )
    {
    iHandles.InsertInAddressOrderL( aHandle );
    }


void CMemSpyEngineGenericKernelObjectList::AddItemL( const TMemSpyDriverHandleInfoGeneric& aItem )
    {
    AddItemL( aItem.iHandle );
    iItems.AppendL( aItem );
    //
    //UpdateNameL();
    }


void CMemSpyEngineGenericKernelObjectList::UpdateNameL()
    {
    const TInt count = iItems.Count();
    const TInt64 size( iSize * count );
    const TMemSpySizeText sizeText( MemSpyEngineUtils::FormatSizeText( size, 0 ) );
    //
    _LIT(KNameFormat, "\t%S\t\t%d item");
    TPtr pName( iName->Des() );
    //
    const TPtrC pType( TypeAsString( iType ) );
    //
    pName.Format( KNameFormat, &pType, count );
    //
    if  ( count == 0 || count > 1 )
        {
        pName.Append( _L("s") );
        }

    pName.AppendFormat( _L(", %S"), &sizeText );
    }

























CMemSpyEngineGenericKernelObjectContainer::CMemSpyEngineGenericKernelObjectContainer()
:   iItems( 25 )
    {
    }


EXPORT_C CMemSpyEngineGenericKernelObjectContainer::~CMemSpyEngineGenericKernelObjectContainer()
    {
    iItems.ResetAndDestroy();
    iItems.Close();
    }


void CMemSpyEngineGenericKernelObjectContainer::ConstructL()
    {
    }


CMemSpyEngineGenericKernelObjectContainer* CMemSpyEngineGenericKernelObjectContainer::NewLC()
    {
    CMemSpyEngineGenericKernelObjectContainer* self = new(ELeave) CMemSpyEngineGenericKernelObjectContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TInt CMemSpyEngineGenericKernelObjectContainer::Count() const
    {
    return iItems.Count();
    }


EXPORT_C const CMemSpyEngineGenericKernelObjectList& CMemSpyEngineGenericKernelObjectContainer::At( TInt aIndex ) const
    {
    return *iItems[ aIndex ];
    }


EXPORT_C void CMemSpyEngineGenericKernelObjectContainer::OutputL( CMemSpyEngineOutputSink& aSink ) const
    {
    // Start marker
    aSink.OutputLineFormattedL( KMemSpyMarkerKernelContainersAll, &KNullDesC );

    const TInt listCount= Count();
    for( TInt listIndex=0; listIndex<listCount; listIndex++ )
        {
        const CMemSpyEngineGenericKernelObjectList& list = At( listIndex );
        list.OutputL( aSink );
        }

    // End marker
    aSink.OutputLineFormattedL( KMemSpyMarkerKernelContainersAll, &KMemSpySinkTagClose );
    }


void CMemSpyEngineGenericKernelObjectContainer::AddListL( CMemSpyEngineGenericKernelObjectList* aList )
    {
    iItems.AppendL( aList );
    }


EXPORT_C TInt CMemSpyEngineGenericKernelObjectContainer::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineGenericKernelObjectContainer::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineGenericKernelObjectList& item = At( aIndex );
    return item.Name();
    }
















