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

#include <memspy/engine/memspyenginehelpercodesegment.h>

// System includes
#include <e32capability.h>
#include <f32file.h>
#include <babitflags.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>

// Constants
const TInt KMemSpyEngineMaxCodeSegmentCount = 512;

// Literal constants
_LIT( KMemSpyEngineCodeSegListOutputComma, ", " );



CMemSpyEngineHelperCodeSegment::CMemSpyEngineHelperCodeSegment( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperCodeSegment::~CMemSpyEngineHelperCodeSegment()
    {
    }


void CMemSpyEngineHelperCodeSegment::ConstructL()
    {
    }


CMemSpyEngineHelperCodeSegment* CMemSpyEngineHelperCodeSegment::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperCodeSegment* self = new(ELeave) CMemSpyEngineHelperCodeSegment( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperCodeSegment::OutputCodeSegmentsL( TUint aPid, TDes& aLine, const TDesC& aPrefix, TChar aSectionUnderlineCharacter, TBool aLowerCaseSectionHeading)
    {
	_LIT(KHeaderLC, "Code Segments");
	_LIT(KHeaderUC, "CODE SEGMENTS");

	_LIT(KFmtOverflow, "Only first %d code segments displayed");
	_LIT(KFmtMod, "%S%08X-%08X %S");

	const TInt KMaxCount = 256;
	TAny* handles[KMaxCount];
	TInt c = KMaxCount;

	TInt r = iEngine.Driver().GetCodeSegs(aPid, handles, c);
	if  ( r == KErrNone )
    	{
        if  ( c > 0 )
            {
            if  ( aLowerCaseSectionHeading )
                {
                iEngine.Sink().OutputSectionHeadingL( KHeaderLC, aSectionUnderlineCharacter );
                }
            else
                {
            	iEngine.Sink().OutputSectionHeadingL( KHeaderUC, aSectionUnderlineCharacter );
                }

        	if (c > KMaxCount)
        		{
        		c = KMaxCount;
        		aLine.Format(KFmtOverflow, c);
        		iEngine.Sink().OutputLineL( aLine );
        		}

        	TBuf<KMaxFileName> path;
        	TMemSpyDriverCodeSegInfo info;
        	for (TInt i=0; i<c; i++)
        		{
        		r = iEngine.Driver().GetCodeSegInfo(handles[i], aPid, info);
                //
        		if  ( r == KErrNone )
        			{
        			path.Copy( info.iCreateInfo.iFileName );
        			aLine.Format(KFmtMod, &aPrefix, info.iMemoryInfo.iCodeBase,info.iMemoryInfo.iCodeBase + info.iMemoryInfo.iCodeSize, &path);
        			iEngine.Sink().OutputLineL( aLine );
        			}
        		}
            }
        }
   }


EXPORT_C CMemSpyEngineCodeSegList* CMemSpyEngineHelperCodeSegment::CodeSegmentListL()
    {
    RArray<TAny*> handles( 16 );
    CleanupClosePushL( handles );

    // Get everything
    GetCodeSegmentHandlesL( handles, NULL, EFalse );
    CMemSpyEngineCodeSegList* list = ListFromHandlesLC( handles );
    //
    CleanupStack::Pop( list );
    CleanupStack::PopAndDestroy( &handles );
    //
    return list;
    }


CMemSpyEngineCodeSegList* CMemSpyEngineHelperCodeSegment::CodeSegmentListRamLoadedL()
    {
    RArray<TAny*> handles( 16 );
    CleanupClosePushL( handles );

    // Get just RAM-loaded
    GetCodeSegmentHandlesL( handles, NULL, ETrue );
    CMemSpyEngineCodeSegList* list = ListFromHandlesLC( handles );
    //
    CleanupStack::Pop( list );
    CleanupStack::PopAndDestroy( &handles );
    //
    return list;
    }


EXPORT_C CMemSpyEngineCodeSegList* CMemSpyEngineHelperCodeSegment::CodeSegmentListL( TProcessId aProcess )
    {
    TUint processId = aProcess;
    //
    RArray<TAny*> handles( 16 );
    CleanupClosePushL( handles );
    
    // Get process-specific list
    GetCodeSegmentHandlesL( handles, &processId, EFalse );
    CMemSpyEngineCodeSegList* list = ListFromHandlesLC( handles );
    //
    CleanupStack::Pop( list );
    CleanupStack::PopAndDestroy( &handles );
    //
    return list;
    }


void CMemSpyEngineHelperCodeSegment::GetCodeSegmentHandlesL( RArray<TAny*>& aHandles, TUint* aProcessId, TBool aRamOnly ) const
    {
	TAny* handles[ KMemSpyEngineMaxCodeSegmentCount ];
	TInt count = KMemSpyEngineMaxCodeSegmentCount;

	TInt r = KErrNone;
	
	if  ( aProcessId == NULL )
    	{
	    r = iEngine.Driver().GetCodeSegs( handles, count, aRamOnly );
	    }
    else
	    {
	    r = iEngine.Driver().GetCodeSegs( *aProcessId, handles, count );
	    }

	if  ( r == KErrNone )
    	{
        TInt index;
        TLinearOrder< TAny* > comparer( SortByAddress );
        
        // Remove duplicates - since we reqested code segments for all processes, there
        // might be some dupes.
        count = Min( count, KMemSpyEngineMaxCodeSegmentCount );
        for( index = 0; index < count; index++ )
            {
            TAny* handle = handles[ index ];
            const TInt error = aHandles.InsertInOrder( handle, comparer );
            //
            if  ( ! (error == KErrNone || error == KErrAlreadyExists ) )
                {
                User::Leave( error );
                }
            }
        }
    }


CMemSpyEngineCodeSegList* CMemSpyEngineHelperCodeSegment::ListFromHandlesLC( RArray<TAny*>& aHandles ) const
    {
    CMemSpyEngineCodeSegList* list = CMemSpyEngineCodeSegList::NewLC( iEngine );
    //
    TMemSpyDriverCodeSegInfo info;
    const TInt count = aHandles.Count();
    //
    for (TInt i=0; i<count; i++)
        {
        TAny* handle = aHandles[ i ];
        const TInt err = iEngine.Driver().GetCodeSegInfo( handle, 0, info );
        //
        if  ( err == KErrNone )
        	{
            // Create item
            CMemSpyEngineCodeSegEntry* entry = CMemSpyEngineCodeSegEntry::NewLC( handle, info.iSize, info.iCreateInfo, info.iMemoryInfo );
            list->AddItemL( entry );
            CleanupStack::Pop( entry );
        	}
        }
    //
    return list;
    }


TInt CMemSpyEngineHelperCodeSegment::SortByAddress( TAny* const& aLeft, TAny* const& aRight )
    {
    TInt ret = 1;
    //
    if  ( aLeft < aRight )
        {
        ret = -1;
        }
    else if ( aLeft == aRight )
        {
        ret = 0;
        }
    //
    return ret;
    }































CMemSpyEngineCodeSegList::CMemSpyEngineCodeSegList( CMemSpyEngine& aEngine )
:   CMemSpyEngineTwiddlableItemArray<CMemSpyEngineCodeSegEntry>( aEngine )
    {
    }


void CMemSpyEngineCodeSegList::ConstructL()
    {
    }


CMemSpyEngineCodeSegList* CMemSpyEngineCodeSegList::NewLC( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineCodeSegList* self = new(ELeave) CMemSpyEngineCodeSegList( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
EXPORT_C TInt CMemSpyEngineCodeSegList::IndexByHandle( TAny* aHandle ) const
    {
    TInt index = KErrNotFound;
    //
    const TInt count = Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyEngineCodeSegEntry& entry = At( i );
        if  ( entry.Handle() == aHandle )
            {
            index = i;
            break;
            }
        }
    //
    return index;
    }


EXPORT_C void CMemSpyEngineCodeSegList::SortByFileNameL()
    {
    TLinearOrder< CMemSpyEngineCodeSegEntry > comparer( CompareByFileName );
    Sort( comparer );
    }


EXPORT_C void CMemSpyEngineCodeSegList::SortByCodeSizeL()
    {
    TLinearOrder< CMemSpyEngineCodeSegEntry > comparer( CompareByCodeSize );
    Sort( comparer );
    }


EXPORT_C void CMemSpyEngineCodeSegList::SortByDataSizeL()
    {
    TLinearOrder< CMemSpyEngineCodeSegEntry > comparer( CompareByDataSize );
    Sort( comparer );
    }


EXPORT_C void CMemSpyEngineCodeSegList::SortByUidsL()
    {
    TLinearOrder< CMemSpyEngineCodeSegEntry > comparer( CompareByUid );
    Sort( comparer );
    }


EXPORT_C void CMemSpyEngineCodeSegList::ShowOnlyEntriesWithGlobalDataL()
    {
    TMemSpyEngineVisibiltyFunction< CMemSpyEngineCodeSegEntry > function( VisibilityFunctionGlobalData, NULL );
    ShowL( function );
    SortByDataSizeL();
    }


EXPORT_C void CMemSpyEngineCodeSegList::OutputDataColumnsL( CMemSpyEngine& aEngine )
    {
    HBufC* columns = HBufC::NewLC( 1500 );
    TPtr pColumns( columns->Des() );

    //
    _LIT(KCol1, "Name");
    pColumns.Append( KCol1 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol2, "Uid %d");
    pColumns.AppendFormat( KCol2, 1 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );
    pColumns.AppendFormat( KCol2, 2 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );
    pColumns.AppendFormat( KCol2, 3 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol3, "Module Version");
    pColumns.Append( KCol3 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol4, "SID");
    pColumns.Append( KCol4 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol5, "VID");
    pColumns.Append( KCol5 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol6, "Code Size");
    pColumns.Append( KCol6 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol7, "Text Size");
    pColumns.Append( KCol7 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol8, "Data Size");
    pColumns.Append( KCol8 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol9, "BSS Size");
    pColumns.Append( KCol9 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol10, "Total Data Size");
    pColumns.Append( KCol10 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol11, "Entrypoint Veneer");
    pColumns.Append( KCol11 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol12, "File Entrypoint");
    pColumns.Append( KCol12 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol13, "Dependency Count");
    pColumns.Append( KCol13 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol14, "ROM Code Load Address");
    pColumns.Append( KCol14 );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    //
    _LIT(KCol15, "Data Load Address");
    pColumns.Append( KCol15 );

    //
    _LIT(KCol16, "Capabilities...");
    pColumns.Append( KCol16 );

    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C TInt CMemSpyEngineCodeSegList::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineCodeSegList::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineCodeSegEntry& item = At( aIndex );
    return TPtrC( item.Caption() );
    }


TInt CMemSpyEngineCodeSegList::IndexByName( const TDesC& aName ) const
    {
    TInt index = KErrNotFound;
    //
    const TInt count = Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyEngineCodeSegEntry& entry = At( i );
        if  ( entry.FileName().FindF( aName ) >= 0 )
            {
            index = i;
            break;
            }
        }
    //
    return index;
    }


TInt CMemSpyEngineCodeSegList::CompareByFileName( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight )
    {
    const TInt ret = aLeft.FileName().CompareF( aRight.FileName() );
    return ret;
    }


TInt CMemSpyEngineCodeSegList::CompareByCodeSize( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight )
    {
    TInt ret = -1;
    //
    if  ( aLeft.CreateInfo().iCodeSize < aRight.CreateInfo().iCodeSize )
        {
        ret = 1;
        }
    else if ( aLeft.CreateInfo().iCodeSize == aRight.CreateInfo().iCodeSize )
        {
        ret = 0;
        }
    //
    return ret;
    }


TInt CMemSpyEngineCodeSegList::CompareByDataSize( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight )
    {
    TInt ret = -1;
    //
    if  ( aLeft.CreateInfo().iTotalDataSize < aRight.CreateInfo().iTotalDataSize )
        {
        ret = 1;
        }
    else if ( aLeft.CreateInfo().iTotalDataSize == aRight.CreateInfo().iTotalDataSize )
        {
        ret = 0;
        }
    //
    return ret;
    }


TInt CMemSpyEngineCodeSegList::CompareByUid( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight )
    {
    TInt ret = -1;
    //
    if  ( aLeft.CreateInfo().iUids.MostDerived().iUid < aRight.CreateInfo().iUids.MostDerived().iUid )
        {
        ret = 1;
        }
    else if ( aLeft.CreateInfo().iUids.MostDerived().iUid == aRight.CreateInfo().iUids.MostDerived().iUid )
        {
        ret = 0;
        }
    //
    return ret;
    }


TBool CMemSpyEngineCodeSegList::VisibilityFunctionGlobalData( const CMemSpyEngineCodeSegEntry*& aItem, TAny* /*aRune*/ )
    {
    const TBool hasGlobalData = ( aItem->CreateInfo().iTotalDataSize > 0 );
    return hasGlobalData;
    }


















CMemSpyEngineCodeSegEntry::CMemSpyEngineCodeSegEntry( TAny* aHandle, TInt aSize, const TCodeSegCreateInfo& aCreateInfo, const TProcessMemoryInfo& aMemoryInfo )
:   CDesCArrayFlat( 10 ), iHandle( aHandle ), iSize( aSize ), iCreateInfo( aCreateInfo ), iMemoryInfo( aMemoryInfo )
    {
    }


EXPORT_C CMemSpyEngineCodeSegEntry::~CMemSpyEngineCodeSegEntry()
    {
    delete iCaption;
    delete iFileName;
    }


void CMemSpyEngineCodeSegEntry::ConstructL()
    {
    iFileName = HBufC::NewL( iCreateInfo.iFileName.Length() );
    iFileName->Des().Copy( iCreateInfo.iFileName );

    // Make caption
    TParsePtrC parser( *iFileName );
    const TPtrC pFileNameWithoutPath( parser.NameAndExt() );
    TBuf<KMaxFullName + 128> item;
    //
    _LIT(KCodeSegFormat, "\t%S\t\t%S code");
    const TMemSpySizeText codeSize( MemSpyEngineUtils::FormatSizeText( iCreateInfo.iCodeSize ) );
    item.Format( KCodeSegFormat, &pFileNameWithoutPath, &codeSize );
    if  ( iCreateInfo.iDataSize > 0 )
        {
        _LIT(KCodeSegFormatAdditionalData, ", %S data");
        const TMemSpySizeText dataSize( MemSpyEngineUtils::FormatSizeText( iCreateInfo.iTotalDataSize ) );
        item.AppendFormat( KCodeSegFormatAdditionalData, &dataSize );
        }
    iCaption = item.AllocL();

    //
    _LIT(KItem0, "\tName\t\t%S");
    item.Format( KItem0, &pFileNameWithoutPath );
    AppendL( item );

    // Uids
    const TUidType uids( iCreateInfo.iUids );
    for( TInt i=0; i<KMaxCheckedUid; i++ )
        {
        _LIT(KItem1, "\tUid #%d\t\t0x%08x");
        const TUid uidValue( uids[ i ] );
        //
        item.Format( KItem1, i+1, uidValue.iUid );
        AppendL( item );
        }
    //
    if  ( iCreateInfo.iModuleVersion == KModuleVersionWild )
        {
        _LIT(KItem12, "\tModule Version\t\t[Wild]");
        AppendL( KItem12 );
        }
    else if ( iCreateInfo.iModuleVersion == KModuleVersionNull )
        {
        _LIT(KItem12, "\tModule Version\t\t[Null]");
        AppendL( KItem12 );
        }
    else
        {
        _LIT(KItem12, "\tModule Version\t\t0x%08x");
        item.Format( KItem12, iCreateInfo.iModuleVersion );
        AppendL( item );
        }

    //
    _LIT(KItem13, "\tSID\t\t0x%08x");
    item.Format( KItem13, iCreateInfo.iS.iSecureId );
    AppendL( item );

    //
    _LIT(KItem14, "\tVID\t\t0x%08x");
    item.Format( KItem14, iCreateInfo.iS.iVendorId );
    AppendL( item );

    //
    if  ( iCreateInfo.iCodeSize > 0 )
        {
        _LIT(KItem2, "\tCode Size\t\t%d");
        item.Format( KItem2, iCreateInfo.iCodeSize );
        AppendL( item );
        }

    //
    if  ( iCreateInfo.iTotalDataSize > 0 )
        {
        _LIT(KItem6, "\tTotal Data Size\t\t%d");
        item.Format( KItem6, iCreateInfo.iTotalDataSize );
        AppendL( item );
        }

    //
    if  ( iCreateInfo.iTextSize > 0 )
        {
        _LIT(KItem3, "\tText Size\t\t%d");
        item.Format( KItem3, iCreateInfo.iTextSize );
        AppendL( item );
        }

    //
    if  ( iCreateInfo.iDataSize > 0 )
        {
        _LIT(KItem4, "\tData Size\t\t%d");
        item.Format( KItem4, iCreateInfo.iDataSize );
        AppendL( item );
        }

    //
    if  ( iCreateInfo.iBssSize > 0 )
        {
        _LIT(KItem5, "\tBSS Size\t\t%d");
        item.Format( KItem5, iCreateInfo.iBssSize );
        AppendL( item );
        }

    //
    _LIT(KItem7, "\tEntrypoint Veneer\t\t0x%08x");
    item.Format( KItem7, iCreateInfo.iEntryPtVeneer );
    AppendL( item );

    //
    _LIT(KItem8, "\tFile Entrypoint\t\t0x%08x");
    item.Format( KItem8, iCreateInfo.iFileEntryPoint );
    AppendL( item );

    //
    _LIT(KItem9, "\tDependency Count\t\t%d");
    item.Format( KItem9, iCreateInfo.iDepCount );
    AppendL( item );

    //
    if  ( iCreateInfo.iCodeLoadAddress != 0 )
        {
        _LIT(KItem10, "\tROM Code Load Addr.\t\t0x%08x");
        item.Format( KItem10, iCreateInfo.iCodeLoadAddress );
        AppendL( item );
        }
    else
        {
        _LIT(KItem10, "\tROM Code Load Addr.\t\t[RAM Loaded]");
        AppendL( KItem10 );
        }

    //
    if  ( iCreateInfo.iDataLoadAddress != 0 )
        {
        _LIT(KItem11, "\tData Load Addr.\t\t0x%08x");
        item.Format( KItem11, iCreateInfo.iDataLoadAddress );
        AppendL( item );
        }

    //
    AddCapabilityItemsL();
    }


CMemSpyEngineCodeSegEntry* CMemSpyEngineCodeSegEntry::NewLC( TAny* aHandle, TInt aSize, const TCodeSegCreateInfo& aCreateInfo, const TProcessMemoryInfo& aMemoryInfo )
    {
    CMemSpyEngineCodeSegEntry* self = new(ELeave) CMemSpyEngineCodeSegEntry( aHandle, aSize, aCreateInfo, aMemoryInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TBool CMemSpyEngineCodeSegEntry::HasCapability( TCapability aCapability ) const
    {
    TBool hasCap = EFalse;
    //
    for( TInt i=0; i<SCapabilitySet::ENCapW && !hasCap; i++ )
        {
        const TUint32 capsRawValue = iCreateInfo.iS.iCaps[i];
        const TBitFlags flags( capsRawValue );
        //
        hasCap = flags.IsSet( aCapability );
        }
    //
    return hasCap;
    }


EXPORT_C void CMemSpyEngineCodeSegEntry::OutputDataL( CMemSpyEngineHelperCodeSegment& aHelper ) const
    {
    _LIT(KHexFormat, "0x%08x");

    HBufC* columns = HBufC::NewLC( 4096 );
    TPtr pColumns( columns->Des() );

    // Name
    TParsePtrC parser( *iFileName );
    const TPtrC pFileNameWithoutPath( parser.NameAndExt() );
    pColumns.Append( pFileNameWithoutPath );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Uids
    const TUidType uids( iCreateInfo.iUids );
    for( TInt i=0; i<KMaxCheckedUid; i++ )
        {
        const TUid uidValue( uids[ i ] );
        //
        pColumns.AppendFormat( KHexFormat, uidValue.iUid );
        pColumns.Append( KMemSpyEngineCodeSegListOutputComma );
        }

    // Module version
    if  ( iCreateInfo.iModuleVersion == KModuleVersionWild )
        {
        _LIT( KCaption, "Wild");
        pColumns.Append( KCaption );
        }
    else if ( iCreateInfo.iModuleVersion == KModuleVersionNull )
        {
        _LIT( KCaption, "Null");
        pColumns.Append( KCaption );
        }
    else
        {
        pColumns.AppendFormat( KHexFormat, iCreateInfo.iModuleVersion );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // SID
    pColumns.AppendFormat( KHexFormat, iCreateInfo.iS.iSecureId );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // VID
    pColumns.AppendFormat( KHexFormat, iCreateInfo.iS.iVendorId );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Code size
    if  ( iCreateInfo.iCodeSize > 0 )
        {
        pColumns.AppendNum( iCreateInfo.iCodeSize, EDecimal );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Text size
    if  ( iCreateInfo.iTextSize > 0 )
        {
        pColumns.AppendNum( iCreateInfo.iTextSize, EDecimal );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Data size
    if  ( iCreateInfo.iDataSize > 0 )
        {
        pColumns.AppendNum( iCreateInfo.iDataSize, EDecimal );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // BSS
    if  ( iCreateInfo.iBssSize > 0 )
        {
        pColumns.AppendNum( iCreateInfo.iBssSize, EDecimal );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Total data size
    if  ( iCreateInfo.iTotalDataSize > 0 )
        {
        pColumns.AppendNum( iCreateInfo.iTotalDataSize, EDecimal );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Entrypoint veneer
    pColumns.AppendFormat( KHexFormat, iCreateInfo.iEntryPtVeneer );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // File Entrypoint
    pColumns.AppendFormat( KHexFormat, iCreateInfo.iFileEntryPoint );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Dependency Count
    pColumns.AppendNum( iCreateInfo.iDepCount, EDecimal );
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // ROM Code Load Address
    if  ( iCreateInfo.iCodeLoadAddress != 0 )
        {
        pColumns.AppendFormat( KHexFormat, iCreateInfo.iCodeLoadAddress );
        }
    else
        {
        _LIT(KCaption, "N.A. - RAM Loaded");
        pColumns.Append( KCaption );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Data Load Address
    if  ( iCreateInfo.iDataLoadAddress != 0 )
        {
        pColumns.AppendFormat( KHexFormat, iCreateInfo.iDataLoadAddress );
        }
    pColumns.Append( KMemSpyEngineCodeSegListOutputComma );

    // Capabilities
    CDesCArray* capabilities = CapabilityStringsLC();
    const TInt count = capabilities->Count();
    //
    for( TInt j=0; j<count; j++ )
        {
        const TPtrC capabilityName( (*capabilities)[ j ] );
        //
        pColumns.Append( capabilityName );
        if  ( j < count-1 )
            {
            pColumns.Append( KMemSpyEngineCodeSegListOutputComma );
            }
        }
    //
    CleanupStack::PopAndDestroy( capabilities );

    aHelper.Engine().Sink().OutputLineL( pColumns );

    CleanupStack::PopAndDestroy( columns );
    }


void CMemSpyEngineCodeSegEntry::AddCapabilityItemsL()
    {
    _LIT(KCapFormat, "\tCapability #%3d\t\t%S");
    TBuf<128> item;
    //
    CDesCArray* capabilities = CapabilityStringsLC();
    const TInt count = capabilities->Count();
    //
    for( TInt i=0; i<count; i++ )
        {
        const TPtrC capabilityName( (*capabilities)[ i ] );
        item.Format( KCapFormat, i+1, &capabilityName );
        AppendL( item );
        }
    //
    CleanupStack::PopAndDestroy( capabilities );
    }


CDesCArray* CMemSpyEngineCodeSegEntry::CapabilityStringsLC() const
    {
    CDesCArrayFlat* array = new(ELeave) CDesCArrayFlat( ECapability_Limit );
    CleanupStack::PushL( array );
    //
    const TInt KCapabilityCountPer32Bits = 32;
    //
    for( TInt i=0; i<SCapabilitySet::ENCapW; i++ )
        {
        const TUint32 caps = iCreateInfo.iS.iCaps[i];
        //
        AddCapabilitiesL( caps, i * KCapabilityCountPer32Bits, *array );
        }
    //
    return array;
    }


void CMemSpyEngineCodeSegEntry::AddCapabilitiesL( TUint32 aCaps, TInt aCapCount, CDesCArray& aArray ) const
    {
    TBitFlags flags( aCaps );
    TBuf<128> capabilityName;
    //
    for( TInt i=aCapCount; i<aCapCount + 32 && i<ECapability_Limit; i++ )
        {
        const TBool isSet = flags.IsSet( i );
        //
        if  ( isSet )
            {
            // Get capability name
            const TCapability capability = static_cast< TCapability >( i );
            MemSpyEngineUtils::GetCapabilityName( capabilityName, capability );

            // Make a capability entry for this item
            aArray.AppendL( capabilityName );
            }
        }
    }














