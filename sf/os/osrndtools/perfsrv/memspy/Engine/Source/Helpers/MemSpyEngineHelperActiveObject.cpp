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

#include <memspy/engine/memspyenginehelperactiveobject.h>

// System includes
#include <s32mem.h>
#include <e32svr.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyenginehelperheap.h>

// Driver includes
#include <memspy/driver/memspydriverconstants.h>

// Literal constants
_LIT( KMemSpyEngineAOOutputComma, ", " );



CMemSpyEngineHelperActiveObject::CMemSpyEngineHelperActiveObject( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperActiveObject::~CMemSpyEngineHelperActiveObject()
    {
    }


void CMemSpyEngineHelperActiveObject::ConstructL()
    {
    }


CMemSpyEngineHelperActiveObject* CMemSpyEngineHelperActiveObject::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperActiveObject* self = new(ELeave) CMemSpyEngineHelperActiveObject( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CMemSpyEngineActiveObjectArray* CMemSpyEngineHelperActiveObject::ActiveObjectListL( const CMemSpyThread& aThread )
    {
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - START");
    CMemSpyEngineActiveObjectArray* array = CMemSpyEngineActiveObjectArray::NewLC();

    // Is the thread's process already suspended? If not, we need to do it now.
    const TProcessId parentProcessId( aThread.Process().Id() );
    const TBool isSuspended = ( iEngine.SuspendedProcessId() == parentProcessId );
    if  ( !isSuspended )
        {
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - suspending process");
        iEngine.ProcessSuspendLC( parentProcessId );
        }

    // Push a cleanup item to close the heap walk in case of leaves
    CleanupStack::PushL( TCleanupItem( CleanupHeapWalk, this ) );

    // Get the thread info
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - getting thread info...");
    TMemSpyDriverThreadInfo threadInfo;
    TInt err = iEngine.Driver().GetThreadInfo( aThread.Id(), threadInfo );
    User::LeaveIfError( err );
    TAny* scheduler = threadInfo.iScheduler;

#if defined( _DEBUG ) && !defined( __WINS__ )
    iEngine.HelperHeap().OutputCellListingUserL( aThread );
#endif

    // Get the heap info - we need this for verification purposes
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - getting heap info...");
    err = iEngine.Driver().GetHeapInfoUser( iHeapInfo, aThread.Id() );
    User::LeaveIfError( err );
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - allocated cell header length is: %d", iHeapInfo.iHeapCellHeaderLengthAllocated);

    // Do we have a ROM-based scheduler pointer?
    if  ( scheduler != NULL && iHeapInfo.Type() != TMemSpyHeapInfo::ETypeUnknown )
        {
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - scheduler: 0x%08x", scheduler);

        // Let's try to get the scheduler's heap cell...
        HBufC8* data = SchedulerHeapCellDataLC( scheduler, aThread.Id() );
        
        // Try to extract the active object addresses
        ExtractActiveObjectAddressesL( scheduler, *data, *array );
        CleanupStack::PopAndDestroy( data );
        }

    // Tidy up
    CleanupStack::PopAndDestroy(this); // heap walk cleanup item
    if  ( !isSuspended )
        {
        CleanupStack::PopAndDestroy(); // will resume the process we suspended earlier
        }
    //
    CleanupStack::Pop( array );
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ActiveObjectListL() - END");
    return array;
    }


HBufC8* CMemSpyEngineHelperActiveObject::SchedulerHeapCellDataLC( TAny*& aCellAddress, TThreadId aThreadId )
    {
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::SchedulerHeapCellDataLC() - START - aCellAddress: 0x%08x, aThread: %d", aCellAddress, I64INT(aThreadId.Id()));

    // This is what we'll return, if we find it...
    HBufC8* heapCellData = NULL;

    // Now walk the heap!
    TInt err = iEngine.Driver().WalkHeapInit( aThreadId );
    User::LeaveIfError( err );

    // Now we can try to find the cell's info
    TMemSpyDriverCellType cellType;
    TInt cellLength;
    TInt cellNestingLevel;
    TInt cellAllocationNumber;
    TInt cellHeaderSize;
    TAny* cellPayloadAddress;
    //
    err = iEngine.Driver().WalkHeapGetCellInfo( aCellAddress, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::SchedulerHeapCellDataLC() - err: %d, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, cellLength, cellAllocationNumber, cellType);
    User::LeaveIfError( err );
    
	if (cellType & EMemSpyDriverAllocatedCellMask)
        {
        const TInt payloadLength = cellLength;
        HBufC8* data = HBufC8::NewLC( payloadLength );
        TPtr8 pData( data->Des() );
        //
        err = iEngine.Driver().WalkHeapReadCellData( aCellAddress, pData, payloadLength );
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::SchedulerHeapCellDataLC() - data fetch returned error: %d", err);
        User::LeaveIfError( err );
        heapCellData = data;
        CleanupStack::Pop( data );
        }
    //
    if  ( heapCellData == NULL )
        {
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::SchedulerHeapCellDataLC() - END - didn't find the right cell => KErrNotFound");
        User::Leave( KErrNotFound );
        }
    //
    CleanupStack::PushL( heapCellData );

    //RDebug::Printf("CMemSpyEngineHelperActiveObject::SchedulerHeapCellDataLC() - END - everything okay, cell is: 0x%08x", aCellAddress);
    return heapCellData;
    }


void CMemSpyEngineHelperActiveObject::ExtractActiveObjectAddressesL( TAny* aSchedulerCellAddress, const TDesC8& aSchedulerCellData, CMemSpyEngineActiveObjectArray& aArray )
    {
    // Create read stream
    RDesReadStream stream( aSchedulerCellData );
    CleanupClosePushL( stream );

    // First item is vtable
    TUint address = stream.ReadUint32L();
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ExtractActiveObjectAddressesL() - vtable: 0x%08x", address );
    (void) address;

    // Next item is CActiveScheduler::iStack - which we'll skip, because it might be a stack address
    // I suppose we could validate this against the thread's stack address range, but can't be bothered
    // at the moment.
    address = stream.ReadUint32L();
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ExtractActiveObjectAddressesL() - got CActiveScheduler::iStack as: 0x%08x", address);
    (void) address;

    // Then comes CActiveScheduler::iActiveQ - this is what we are interested in.... 
    //
    //  class TPriQue : public TDblQueBase
    //        [TDblQueBase::iHead] - this just derives from TDblQueLinkBase and doesn't have any direct data members
    //            class TDblQueLink : public TDblQueLinkBase
    //                [ptr]  TDblQueLinkBase::iNext*
    //                [ptr]  TDblQueLinkBase::iPrev*
    // [TInt] TDblQueBase::iOffset
    __ASSERT_COMPILE( sizeof( TDblQueLinkBase* ) == sizeof(TUint) );
    __ASSERT_COMPILE( sizeof( TInt ) == 4 );

    // Get read offset so that we know the starting address of the queue
    const TStreamPos pos = stream.Source()->TellL(MStreamBuf::ERead);
#ifdef __WINS__
    const TAny* terminatingQueueAddress = (TAny*) (TUint(aSchedulerCellAddress) + pos.Offset());
#else
    const TAny* terminatingQueueAddress = (TAny*) aSchedulerCellAddress;
#endif

    const TAny* queueNext = (TAny*) stream.ReadUint32L();
    const TAny* queuePrev = (TAny*) stream.ReadUint32L();
    const TUint queueItemOffset = stream.ReadUint32L();
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ExtractActiveObjectAddressesL() - queueNext: 0x%08x, queuePrev: 0x%08x, queueItemOffset: %d, pos: %d, terminatingQueueAddress: 0x%08x", queueNext, queuePrev, queueItemOffset, pos.Offset(), terminatingQueueAddress);
    (void) queuePrev;
    CleanupStack::PopAndDestroy( &stream );
        
    // Iterate through the active objects
    if  ( queueNext != NULL )
        {
        TAny* realNextCellHeapCell = NULL;
        TAny* calculatedCellAddress = ((TAny*) (TUint(queueNext) - queueItemOffset));

        while( !( calculatedCellAddress == NULL || calculatedCellAddress == terminatingQueueAddress || realNextCellHeapCell == terminatingQueueAddress ) )
            {
            // Create an active object for this cell
            TAny* nextCell = ReadActiveObjectDataL( calculatedCellAddress, aArray );
            
            // Work out next cell address
            calculatedCellAddress = ((TAny*) ( TUint( nextCell ) - queueItemOffset ) );
            //RDebug::Printf("CMemSpyEngineHelperActiveObject::ExtractActiveObjectAddressesL() - calculatedCellAddress: 0x%08x, terminatingQueueAddress: 0x%08x", calculatedCellAddress, terminatingQueueAddress);

            // Identify the next cell address
            realNextCellHeapCell = ConvertAddressToRealHeapCellAddressL( nextCell );
            }
        }
    }


TAny* CMemSpyEngineHelperActiveObject::ConvertAddressToRealHeapCellAddressL( TAny* aAddress )
    {
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ConvertAddressToRealHeapCellAddressL() - START - aAddress: 0x%08x", aAddress);

    TMemSpyDriverCellType cellType;
    TInt cellLength;
    TInt cellNestingLevel;
    TInt cellAllocationNumber;
    TInt cellHeaderSize;
    TAny* cellPayloadAddress;

    TInt err = iEngine.Driver().WalkHeapGetCellInfo( aAddress, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ConvertAddressToRealHeapCellAddressL() - END - err: %d, realCellAddress: 0x%08x, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, aAddress, cellLength, cellAllocationNumber, cellType);
    User::LeaveIfError( err );

    return aAddress;
    }


TAny* CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL( TAny* aCellAddress, CMemSpyEngineActiveObjectArray& aArray )
    {
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - START");

    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.cellAddress:     0x%08x", aCellAddress);
    TAny* nextCellAddress = NULL;
    
    TMemSpyDriverCellType cellType;
    TInt cellLength;
    TInt cellNestingLevel;
    TInt cellAllocationNumber;
    TInt cellHeaderSize;
    TAny* cellPayloadAddress;

    // Make a separate copy of the cell address - calling GetCellInfo may well result in the address being
    // changed in order to match the real starting address of a *heap cell*.
    TAny* requestedCellAddress = aCellAddress;
    TInt err = iEngine.Driver().WalkHeapGetCellInfo( requestedCellAddress, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - err: %d, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, cellLength, cellAllocationNumber, cellType);
    User::LeaveIfError( err );
    
    if (cellType & EMemSpyDriverAllocatedCellMask)
        {
        const TInt payloadLength = cellLength;
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - payloadLength: %d", payloadLength);

        // const TInt payloadLength = Max( 512, cellLength - iHeapInfo.iHeapCellHeaderLengthAllocated ); // Prevent negative payload lengths?
        CBufFlat* data = CBufFlat::NewL( payloadLength );
        CleanupStack::PushL( data );
        data->ResizeL( payloadLength );
        TPtr8 pData( data->Ptr( 0 ) );
        //
        err = iEngine.Driver().WalkHeapReadCellData( requestedCellAddress, pData, payloadLength );
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.heapCellAddress: 0x%08x (err: %d)", requestedCellAddress, err);
        User::LeaveIfError( err );

        // If an object is embedded directly within a class, for example
        //
        // class CSomething : public CBase
        //   {
        //   CIdle iEmbeddedIdler;
        //   }
        //
        // then aCellAddress actually points to somewhere *within* a heap cell, not to the actual starting address of
        // the heap cell itself. We must take this into account when parsing the heap cell data (i.e. the bit of the cell we
        // are interested in starts part way through the cell data).
        TInt cellOffset = TUint32( aCellAddress ) - TUint32( requestedCellAddress );
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.cellOffset:      %d (ignoring cell header)", cellOffset);
        cellOffset -= cellHeaderSize;
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.cellOffset:      %d (adjusted for cell header)", cellOffset);

        // Got the cell data for the active object. Let's parse it.
        RBufReadStream stream( *data, cellOffset );
        CleanupClosePushL( stream );

        // First item should be vTable
        TAny* vTable = (TAny*) stream.ReadUint32L();
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.vTable:          0x%08x", vTable );

        // Next item should be the request status. First the iStatus, then the iFlags
        const TInt requestStatusValue = stream.ReadInt32L();
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.rsVal:           %10d", requestStatusValue );
        const TUint requestStatusFlags = stream.ReadUint32L();
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.rsFlags:         0x%02x", requestStatusFlags );

        // Next comes the baseclass for the link - TDblQueLinkBase
        TAny* nextEntryAddress = (TAny*) stream.ReadUint32L();
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.iLink.Next:      0x%08x", nextEntryAddress );
        TAny* prevEntryAddress = (TAny*) stream.ReadUint32L();
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.iLink.Prev:      0x%08x", prevEntryAddress );

        // Next comes the TPriQueueLink itself
        const TInt priority = stream.ReadInt32L();
        //RDebug::Printf("CMemSpyEngineHelperActiveObject::ReadActiveObjectDataL() - AO.iLink.Pri:       %d", priority );
        
        // Done - save object & tidy up
        CMemSpyEngineActiveObject* object = CMemSpyEngineActiveObject::NewLC( aCellAddress, vTable, priority, requestStatusValue, requestStatusFlags, nextEntryAddress, prevEntryAddress, iEngine );
        aArray.AddItemL( object );
        CleanupStack::Pop( object );
        CleanupStack::PopAndDestroy( 2, data ); // stream & data
        
        nextCellAddress = (TAny*) nextEntryAddress;
        //RDebug::Printf(" ");
        }

    return nextCellAddress;
    }


void CMemSpyEngineHelperActiveObject::CleanupHeapWalk( TAny* aSelf )
    {
    CMemSpyEngineHelperActiveObject* self = reinterpret_cast< CMemSpyEngineHelperActiveObject* >( aSelf );
    self->iEngine.Driver().WalkHeapClose();
    }

































CMemSpyEngineActiveObjectArray::CMemSpyEngineActiveObjectArray()
    {
    }


EXPORT_C CMemSpyEngineActiveObjectArray::~CMemSpyEngineActiveObjectArray()
    {
    delete iHeader;
    iObjects.ResetAndDestroy();
    iObjects.Close();
    }


void CMemSpyEngineActiveObjectArray::ConstructL()
    {
    }


CMemSpyEngineActiveObjectArray* CMemSpyEngineActiveObjectArray::NewLC(  )
    {
    CMemSpyEngineActiveObjectArray* self = new(ELeave) CMemSpyEngineActiveObjectArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TInt CMemSpyEngineActiveObjectArray::Count() const
    {
    return iObjects.Count();
    }


EXPORT_C CMemSpyEngineActiveObject& CMemSpyEngineActiveObjectArray::At( TInt aIndex )
    {
    return *iObjects[ aIndex ];
    }


EXPORT_C const CMemSpyEngineActiveObject& CMemSpyEngineActiveObjectArray::At( TInt aIndex ) const
    {
    return *iObjects[ aIndex ];
    }


EXPORT_C CMemSpyEngineActiveObject& CMemSpyEngineActiveObjectArray::ObjectByAddressL( TAny* aAddress )
    {
    const TInt index = ObjectIndexByAddress( aAddress );
    User::LeaveIfError( index );
    CMemSpyEngineActiveObject& ret = At( index );
    return ret;
    }


EXPORT_C TInt CMemSpyEngineActiveObjectArray::ObjectIndexByAddress( TAny* aAddress ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineActiveObject& object = At( i );
        //
        if  ( object.Address() == aAddress )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineActiveObjectArray::OutputDataColumnsL( CMemSpyEngine& aEngine )
    {
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );
    //
    _LIT(KCol1, "Address");
    pColumns.Append( KCol1 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol3, "Priority");
    pColumns.Append( KCol3 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol4, "Is Active");
    pColumns.Append( KCol4 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol5, "Request Pending");
    pColumns.Append( KCol5 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol6, "Status Value");
    pColumns.Append( KCol6 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol7, "Status Flags");
    pColumns.Append( KCol7 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol8, "vTable Address");
    pColumns.Append( KCol8 );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    _LIT(KCol9, "vTable for Symbolic Lookup");
    pColumns.Append( KCol9 );
    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C TInt CMemSpyEngineActiveObjectArray::MdcaCount() const
    {
    TInt count = Count();
    //
    if  ( count > 0 )
        {
        ++count;
        }
    //
    return count;
    }


EXPORT_C TPtrC CMemSpyEngineActiveObjectArray::MdcaPoint( TInt aIndex ) const
    {
    TPtrC ret( *iHeader );
    //
    if  ( aIndex > 0 )
        {
        const CMemSpyEngineActiveObject& object = At( aIndex - 1 );
        ret.Set( object.Caption() );
        }
    //
    return ret;
    }


void CMemSpyEngineActiveObjectArray::AddItemL( CMemSpyEngineActiveObject* aItem )
    {
    iObjects.AppendL( aItem );
    BuildHeaderCaptionL();
    }


void CMemSpyEngineActiveObjectArray::InsertL( CMemSpyEngineActiveObject* aItem, TInt aIndex )
    {
    iObjects.InsertL( aItem, aIndex );
    BuildHeaderCaptionL();
    }


void CMemSpyEngineActiveObjectArray::BuildHeaderCaptionL()
    {
    const TInt KHeaderLength = 100;
    //
    if  ( !iHeader )
        {
        iHeader = HBufC::NewL( KHeaderLength );
        }
    //
    TPtr pHeader( iHeader->Des() );
    //
    _LIT(KCaption, "\tNumber of AO\'s\t\t%d");
    pHeader.Format( KCaption, Count() );
    }

























CMemSpyEngineActiveObject::CMemSpyEngineActiveObject( TAny* aAddress, TAny* aVTable, TInt aPriority, TInt aRSValue, TUint aRSFlags, TAny* aNextAOAddress, TAny* aPrevAOAddress )
:   CDesCArrayFlat(6), iAddress( aAddress ), iVTable( aVTable ), iPriority( aPriority ), iRequestStatusValue( aRSValue ), iRequestStatusFlags( aRSFlags ), iNextAOAddress( aNextAOAddress ), iPrevAOAddress( aPrevAOAddress )
    {
    }


EXPORT_C CMemSpyEngineActiveObject::~CMemSpyEngineActiveObject()
    {
    delete iCaption;
    }


void CMemSpyEngineActiveObject::ConstructL( CMemSpyEngine& /*aEngine*/ )
    {
    TBuf<256> item;

    _LIT(KBasicFormat, "\t0x%08x\t\t");
    item.Format( KBasicFormat, VTable() );

    // Add modifiers
    _LIT( KModifiers, "%d" );
    _LIT( KBoxedCharFormat, " [%c]" );
    item.AppendFormat( KModifiers, RequestStatusValue() );
    if  ( IsActive() )
        {
        item.AppendFormat( KBoxedCharFormat, 'A' );
        }
    if  ( RequestIsPending() )
        {
        item.AppendFormat( KBoxedCharFormat, 'P' );
        }
    iCaption = item.AllocL();

    // Listbox items
    TPtrC value;

    // Address
    _LIT(KCaption1, "\tAddress\t\t0x%08x");
    item.Format( KCaption1, iAddress );
    AppendL( item );

    // vTable
    _LIT(KCaption2, "\tVTable\t\t0x%08x");
    item.Format( KCaption2, iVTable );
    AppendL( item );

    //
    _LIT(KCaption3, "\tStatus Value\t\t%d");
    item.Format( KCaption3, iRequestStatusValue );
    AppendL( item );

    //
    _LIT(KCaption5, "\tIs Active\t\t%S");
    value.Set( YesNoValue( IsActive() ) );
    item.Format( KCaption5, &value );
    AppendL( item );

    //
    _LIT(KCaption6, "\tRequest Pending\t\t%S");
    value.Set( YesNoValue( RequestIsPending() ) );
    item.Format( KCaption6, &value );
    AppendL( item );

    //
    _LIT(KCaption4, "\tPriority\t\t%d");
    item.Format( KCaption4, iPriority );
    AppendL( item );
    }


CMemSpyEngineActiveObject* CMemSpyEngineActiveObject::NewLC( TAny* aAddress, TAny* aVTable, TInt aPriority, TInt aRSValue, TUint aRSFlags, TAny* aNextAOAddress, TAny* aPrevAOAddress, CMemSpyEngine& aEngine )
    {
    CMemSpyEngineActiveObject* self = new(ELeave) CMemSpyEngineActiveObject( aAddress, aVTable, aPriority, aRSValue, aRSFlags, aNextAOAddress, aPrevAOAddress );
    CleanupStack::PushL( self );
    self->ConstructL( aEngine );
    return self;
    }


EXPORT_C TBool CMemSpyEngineActiveObject::IsActive() const
    {
    return ( iRequestStatusFlags & CMemSpyEngineActiveObject::EActive );
    }


EXPORT_C TBool CMemSpyEngineActiveObject::IsAddedToScheduler() const
    {
    return ( iNextAOAddress != NULL );
    }


EXPORT_C TBool CMemSpyEngineActiveObject::RequestIsPending() const
    {
    return ( iRequestStatusFlags & CMemSpyEngineActiveObject::ERequestPending );
    }


EXPORT_C void CMemSpyEngineActiveObject::OutputDataL( CMemSpyEngine& aEngine ) const
    {
    _LIT(KMemSpyEngineAOOutputHex, "0x%08x");
    _LIT(KMemSpyEngineAOOutputDecimal, "%d");
    _LIT(KMemSpyEngineAOOutputDecimalFixed10, "%10d");
    _LIT(KMemSpyEngineAOOutputString, "%S");
    _LIT(KMemSpyEngineAOOutputVTable, "vTable: 0x%08x");
    //
    TPtrC yesNoValue( KNullDesC );
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );
    //
    pColumns.AppendFormat( KMemSpyEngineAOOutputHex, Address() );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineAOOutputDecimal, Priority() );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    yesNoValue.Set( YesNoValue( IsActive() ) );
    pColumns.AppendFormat( KMemSpyEngineAOOutputString, &yesNoValue );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    yesNoValue.Set( YesNoValue( RequestIsPending() ) );
    pColumns.AppendFormat( KMemSpyEngineAOOutputString, &yesNoValue );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineAOOutputDecimalFixed10, RequestStatusValue() );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineAOOutputDecimal, RequestStatusFlags() );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineAOOutputHex, VTable() );
    pColumns.Append( KMemSpyEngineAOOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineAOOutputVTable, VTable() );
    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


TPtrC CMemSpyEngineActiveObject::YesNoValue( TBool aValue )
    {
    _LIT(KYesString, "Yes");
    _LIT(KNoString, "No");
    //
    TPtrC pRet( KNoString );
    if  ( aValue )
        {
        pRet.Set( KYesString );
        }
    //
    return pRet;
    }

