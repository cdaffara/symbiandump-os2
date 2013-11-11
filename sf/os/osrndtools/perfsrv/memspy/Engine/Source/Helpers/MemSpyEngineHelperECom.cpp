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

#include <memspy/engine/memspyenginehelperecom.h>

// System includes
#include <charconv.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>

// Literal constants
_LIT8( KMemSpyEComXmlFileMimeType, "text/xml" );
_LIT8( KMemSpyXmlSpec_MasterSection, "memspy_ecom_sections" );
_LIT8( KMemSpyXmlSpec_ChildSectionCategory, "category" );
_LIT8( KMemSpyXmlSpec_ChildSectionCategory_Attribute_Name, "name" );
_LIT8( KMemSpyXmlSpec_ChildSectionInterface, "interface" );
_LIT8( KMemSpyXmlSpec_ChildSectionInterface_Name, "name" );
_LIT8( KMemSpyXmlSpec_ChildSectionInterface_Uid, "uid" );
_LIT8( KMemSpyXmlSpec_HexPrefix, "0x" );





CMemSpyEngineHelperECom::CMemSpyEngineHelperECom( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperECom::~CMemSpyEngineHelperECom()
    {
    iCategories.ResetAndDestroy();
    iCategories.Close();
    //
    delete iParser;
    delete iConverter;
    //
    iEComSession.Close();
    REComSession::FinalClose();
    }


void CMemSpyEngineHelperECom::ConstructL()
    {
    TRAPD( err, ConstructInternalL() );
    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructL() - error: %d", err );
    err = err;
    }


void CMemSpyEngineHelperECom::ConstructInternalL()
    {
    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructInternalL() - START" );
    iEComSession = REComSession::OpenL();

    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructInternalL() - creating parser..." );
	iParser = CParser::NewL( KMemSpyEComXmlFileMimeType, *this );

    // We only convert from UTF-8 to UTF-16
    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructInternalL() - creating charconv object..." );
    iConverter = CCnvCharacterSetConverter::NewL();
    if  ( iConverter->PrepareToConvertToOrFromL( KCharacterSetIdentifierUtf8, iEngine.FsSession() ) == CCnvCharacterSetConverter::ENotAvailable )
	    {
		User::Leave(KErrNotFound);
		}

    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructInternalL() - reading xml..." );
    ConstructFromFileL();
    
    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructInternalL() - END" );
    }


void CMemSpyEngineHelperECom::ConstructFromFileL()
    {
    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructFromFileL() - START" );

    // Locate the source file
    TRAPD( err, 
        HBufC* fileName = FindEComXmlFileNameLC();
        ParseL( *fileName );
        CleanupStack::PopAndDestroy( fileName );
        );
    err = err;

    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructFromFileL() - parse error: %d", err );

    delete iConverter;
    iConverter = NULL;

    // Build captions
    const TInt categoryCount = iCategories.Count();
    for( TInt i=0; i<categoryCount; i++ )
        {
        //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructFromFileL() - building caption: %d", i );

        CMemSpyEngineEComCategory* cat = iCategories[ i ];
        cat->BuildCaptionL();
        }

    //RDebug::Printf( "CMemSpyEngineHelperECom::ConstructFromFileL() - END" );
    }


CMemSpyEngineHelperECom* CMemSpyEngineHelperECom::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperECom* self = new(ELeave) CMemSpyEngineHelperECom( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


REComSession& CMemSpyEngineHelperECom::EComSession()
    {
    return iEComSession;
    }


EXPORT_C CMemSpyEngineEComCategory& CMemSpyEngineHelperECom::At( TInt aIndex )
    {
    return *iCategories[ aIndex ];
    }


EXPORT_C TInt CMemSpyEngineHelperECom::IndexOf( const CMemSpyEngineEComCategory& aEntry )
    {
    const TInt index = iCategories.Find( &aEntry );
    return index;
    }


EXPORT_C TInt CMemSpyEngineHelperECom::MdcaCount() const
    {
    return iCategories.Count();
    }


EXPORT_C TPtrC CMemSpyEngineHelperECom::MdcaPoint( TInt aIndex ) const
    {
    CMemSpyEngineEComCategory* category = iCategories[ aIndex ];
    return category->Caption();
    }


HBufC* CMemSpyEngineHelperECom::FindEComXmlFileNameLC()
    {
    RFs& fsSession = iEngine.FsSession();
    //
    TFileName* fileName = new(ELeave) TFileName();
    CleanupStack::PushL( fileName );
    //
    fsSession.PrivatePath( *fileName );
    //
    TFindFile findFile( fsSession );
    User::LeaveIfError( findFile.FindByPath( KMemSpyEComInterfaceIdFileName, fileName ) );
    //
    HBufC* ret = findFile.File().AllocL();
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PushL( ret );
    return ret;
    }


void CMemSpyEngineHelperECom::ConvertL( const TDesC8& aInput, TDes16& aOutput )
    {
    const TInt error = iConverter->ConvertToUnicode( aOutput, aInput, iCharconvConverterState );
    User::LeaveIfError( error );
    }


void CMemSpyEngineHelperECom::ParseL( const TDesC& aFileName )
    {
	Xml::ParseL( *iParser, iEngine.FsSession(), aFileName );
    //
    const TInt error = iParserErrorCode;
    //RDebug::Printf( "CMemSpyEngineHelperECom::ParseL() - iParserErrorCode: %d", iParserErrorCode );
    iParserErrorCode = KErrNone;
    //
	User::LeaveIfError( error );
    }


void CMemSpyEngineHelperECom::OnChildSectionCategoryL( const RAttributeArray& aAttributes )
    {
    TBuf<128> name;
    TBool gotName = EFalse;
    //
    const TInt count = aAttributes.Count();
	for( TInt i=0; i<count; i++ )
		{
		const TPtrC8 attrib( aAttributes[ i ].Attribute().LocalName().DesC() );
		const TPtrC8 value( aAttributes[ i ].Value().DesC() );
        //
		if  ( attrib.CompareF( KMemSpyXmlSpec_ChildSectionCategory_Attribute_Name ) == 0 )
			{
            if  ( gotName )
                {
                User::LeaveIfError( KErrCorrupt );
                }
            else
                {
                ConvertL( value, name );
                gotName = ETrue;
                }
			}
        }
    //
    if  ( gotName )
        {
        // Make a new category
        //RDebug::Print( _L("CMemSpyEngineHelperECom::OnChildSectionCategoryL() - creating category: %S"), &name );

        CMemSpyEngineEComCategory* category = CMemSpyEngineEComCategory::NewLC( *this, name );
        iCategories.AppendL( category );
        CleanupStack::Pop( category );

        //RDebug::Print( _L("CMemSpyEngineHelperECom::OnChildSectionCategoryL() - created category: %S"), &name );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }
    }


void CMemSpyEngineHelperECom::OnChildSectionInterfaceL( const RAttributeArray& aAttributes )
    {
    TUid uid = KNullUid;
    TBuf<128> name;
    TBool gotName = EFalse;
    TBool gotUid = EFalse;
    //
    const TInt count = aAttributes.Count();
	for( TInt i=0; i<count; i++ )
		{
		const TPtrC8 attrib( aAttributes[ i ].Attribute().LocalName().DesC() );
		const TPtrC8 value( aAttributes[ i ].Value().DesC() );
        //
		if  ( attrib.CompareF( KMemSpyXmlSpec_ChildSectionInterface_Name ) == 0 )
			{
            if  ( gotName )
                {
                User::LeaveIfError( KErrCorrupt );
                }
            else
                {
                ConvertL( value, name );
                gotName = ETrue;
                }
			}
		else if ( attrib.CompareF( KMemSpyXmlSpec_ChildSectionInterface_Uid ) == 0 )
			{
            if  ( gotUid )
                {
                User::LeaveIfError( KErrCorrupt );
                }
            else
                {
                if  ( value.Length() >= 8 )
                    {
                    TRadix radix = EDecimal;
                    TPtrC8 pValue( value );
                    //
                    if  ( value.Length() == 10 && value.Left( 2 ) == KMemSpyXmlSpec_HexPrefix )
                        {
                        pValue.Set( value.Mid( 2 ) );
                        radix = EHex;
                        }
                    //
                    TUint32 uidVal = 0;
                    TLex8 lexer( pValue );
                    const TInt err = lexer.Val( uidVal, radix );
                    User::LeaveIfError( err );
                    uid.iUid = uidVal;
                    gotUid = ETrue;
                    }
                }
			}
        }
    //
    const TInt catCount = iCategories.Count();
    if  ( gotName && gotUid && catCount > 0 )
        {
        //RDebug::Print( _L("CMemSpyEngineHelperECom::OnChildSectionInterfaceL() - creating interface for %S [0x%08x]"), &name, uid.iUid );

        CMemSpyEngineEComCategory* category = iCategories[ catCount - 1 ];
        TRAPD( err, category->CreateInterfaceL( name, uid ) );
        err = err;
        //RDebug::Print( _L("CMemSpyEngineHelperECom::OnChildSectionInterfaceL() - error: %d, for %S [0x%08x]"), err, &name, uid.iUid );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }
    }


void CMemSpyEngineHelperECom::OnStartDocumentL( const RDocumentParameters& /*aDocParam*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnEndDocumentL( TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnStartElementL( const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    //
	TInt err = KErrNone;
	const TPtrC8 name( aElement.LocalName().DesC() );
    //
    if  ( name.CompareF( KMemSpyXmlSpec_MasterSection ) == 0 )
		{
		iSeenMasterSection = ETrue;
		}
    else if ( iSeenMasterSection )
        {
        if  ( name.CompareF( KMemSpyXmlSpec_ChildSectionCategory ) == 0 )
            {
            OnChildSectionCategoryL( aAttributes );
            }
        else if ( name.CompareF( KMemSpyXmlSpec_ChildSectionInterface ) == 0 )
            {
            OnChildSectionInterfaceL( aAttributes );
            }
        }
    //
    User::LeaveIfError( err );
    }


void CMemSpyEngineHelperECom::OnEndElementL( const RTagInfo& /*aElement*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnContentL( const TDesC8& /*aBytes*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnStartPrefixMappingL( const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnEndPrefixMappingL( const RString& /*aPrefix*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnSkippedEntityL( const RString& /*aName*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnProcessingInstructionL( const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyEngineHelperECom::OnError( TInt aErrorCode )
    {
    iParserErrorCode = aErrorCode;
    }


TAny* CMemSpyEngineHelperECom::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    return NULL;
    }





































CMemSpyEngineEComCategory::CMemSpyEngineEComCategory( CMemSpyEngineHelperECom& aHelper )
:   iHelper( aHelper )
    {
    }

    
CMemSpyEngineEComCategory::~CMemSpyEngineEComCategory()
    {
    delete iCaption;
    delete iName;
    iInterfaces.ResetAndDestroy();
    iInterfaces.Close();
    }


void CMemSpyEngineEComCategory::ConstructL( const TDesC& aName )
    {
    iName = aName.AllocL();
    iCaption = aName.AllocL(); // Replace later on
    }


CMemSpyEngineEComCategory* CMemSpyEngineEComCategory::NewLC( CMemSpyEngineHelperECom& aHelper, const TDesC& aName )
    {
    CMemSpyEngineEComCategory* self = new(ELeave) CMemSpyEngineEComCategory( aHelper );
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    return self;
    }


EXPORT_C CMemSpyEngineEComInterface& CMemSpyEngineEComCategory::At( TInt aIndex )
    {
    return *iInterfaces[ aIndex ];
    }


EXPORT_C TInt CMemSpyEngineEComCategory::IndexOf( const CMemSpyEngineEComInterface& aEntry )
    {
    const TInt index = iInterfaces.Find( &aEntry );
    return index;
    }


EXPORT_C TInt CMemSpyEngineEComCategory::MdcaCount() const
    {
    return iInterfaces.Count();
    }


EXPORT_C TPtrC CMemSpyEngineEComCategory::MdcaPoint( TInt aIndex ) const
    {
    CMemSpyEngineEComInterface* iface = iInterfaces[ aIndex ];
    return iface->Caption();
    }


void CMemSpyEngineEComCategory::CreateInterfaceL( const TDesC& aName, TUid aUid )
    {
    CMemSpyEngineEComInterface* iface = CMemSpyEngineEComInterface::NewLC( *this, aName, aUid );
    iInterfaces.AppendL( iface );
    CleanupStack::Pop( iface );
    }


void CMemSpyEngineEComCategory::BuildCaptionL()
    {
    delete iCaption; 
    iCaption = NULL;

    _LIT( KCaptionFormat, "\t%S\t\t%d interface" );
    iCaption = HBufC::NewL( 256 );
    TPtr pCaption( iCaption->Des() );
    pCaption.Format( KCaptionFormat, iName, iInterfaces.Count() );
    if  ( iInterfaces.Count() > 1 )
        {
        pCaption.Append( 's' );
        }
    }












CMemSpyEngineEComInterface::CMemSpyEngineEComInterface( CMemSpyEngineEComCategory& aCategory, TUid aUid )
:   iUid( aUid ), iCategory( aCategory )
    {
    }

    
CMemSpyEngineEComInterface::~CMemSpyEngineEComInterface()
    {
    delete iCaption;
    delete iName;
    iImplementations.ResetAndDestroy();
    iImplementations.Close();
    }


void CMemSpyEngineEComInterface::ConstructL( const TDesC& aName )
    {
    iName = aName.AllocL();

    // Get implementation information for this interface...
    RImplInfoPtrArray infoArray;
    CleanupResetAndDestroyPushL( infoArray );
    REComSession::ListImplementationsL( iUid, infoArray );

    // Make implementation objects for each impl that exists.
    const TInt count = infoArray.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        CImplementationInformation* info = infoArray[ i ];

        // Get info
        CMemSpyEngineEComImplementation* impl = CMemSpyEngineEComImplementation::NewLC( *this, info );

        // Now owned by impl
        infoArray.Remove( i );

        // Add to container
        iImplementations.AppendL( impl );
        CleanupStack::Pop( impl );
        }

    // Tidy up
    CleanupStack::PopAndDestroy( &infoArray );

    // Build caption
    _LIT( KCaptionFormat, "\t%S\t\t[0x%08x] [%d impl]" );
    iCaption = HBufC::NewL( 256 );
    TPtr pCaption( iCaption->Des() );
    pCaption.Format( KCaptionFormat, iName, iUid.iUid, count );
    }


CMemSpyEngineEComInterface* CMemSpyEngineEComInterface::NewLC( CMemSpyEngineEComCategory& aCategory, const TDesC& aName, TUid aUid )
    {
    CMemSpyEngineEComInterface* self = new(ELeave) CMemSpyEngineEComInterface( aCategory, aUid );
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    return self;
    }


EXPORT_C CMemSpyEngineEComImplementation& CMemSpyEngineEComInterface::At( TInt aIndex )
    {
    return *iImplementations[ aIndex ];
    }


EXPORT_C TInt CMemSpyEngineEComInterface::IndexOf( const CMemSpyEngineEComImplementation& aEntry )
    {
    const TInt index = iImplementations.Find( &aEntry );
    return index;
    }


EXPORT_C TInt CMemSpyEngineEComInterface::MdcaCount() const
    {
    return iImplementations.Count();
    }


EXPORT_C TPtrC CMemSpyEngineEComInterface::MdcaPoint( TInt aIndex ) const
    {
    CMemSpyEngineEComImplementation* impl = iImplementations[ aIndex ];
    return impl->Caption();
    }





















CMemSpyEngineEComImplementation::CMemSpyEngineEComImplementation( CMemSpyEngineEComInterface& aInterface )
:   iInterface( aInterface )
    {
    }

    
CMemSpyEngineEComImplementation::~CMemSpyEngineEComImplementation()
    {
    delete iCaption;
    delete iName;
    delete iInfoText;
    delete iInfo;
    }


void CMemSpyEngineEComImplementation::ConstructL( CImplementationInformation* aInfo )
    {
    iInfoText = new(ELeave) CDesCArrayFlat( 10 );
    TBuf< 256 > temp;

    // Make items.
    if  ( aInfo->DisplayName().Length() )
        {
        _LIT( KNameFormat, "0x%08x - %S" );
        temp.Format( KNameFormat, aInfo->ImplementationUid().iUid, &aInfo->DisplayName() );
        }
    else
        {
        _LIT( KNameFormat, "0x%08x" );
        temp.Format( KNameFormat, aInfo->ImplementationUid().iUid );
        }
    iName = temp.AllocL();

    _LIT( KItem1, "\tImpl. Uid\t\t0x%08x" );
    temp.Format( KItem1, aInfo->ImplementationUid().iUid );
    iInfoText->AppendL( temp );

    if  ( aInfo->DisplayName().Length() )
        {
        _LIT( KItem2, "\tDisplay Name\t\t%S" );
        temp.Format( KItem2, &aInfo->DisplayName() );
        }
    else
        {
        _LIT( KItem2, "\tDisplay Name\t\t[Undefined]" );
        temp.Copy( KItem2 );
        }
    iInfoText->AppendL( temp );

    _LIT( KItem3, "\tVersion\t\t%d" );
    temp.Format( KItem3, aInfo->Version() );
    iInfoText->AppendL( temp );

    HBufC* opaqueData = CopyAndCropLC( aInfo->OpaqueData() );
    if  ( opaqueData->Length() )
        {
        _LIT( KItem4, "\tOpaque Data\t\t%S" );
        temp.Format( KItem4, opaqueData );
        }
    else
        {
        _LIT( KItem4, "\tOpaque Data\t\t[Undefined]" );
        temp.Copy( KItem4 );
        }
    iInfoText->AppendL( temp );
    CleanupStack::PopAndDestroy( opaqueData );

    HBufC* dataType = CopyAndCropLC( aInfo->DataType() );
    if  ( opaqueData->Length() )
        {
        _LIT( KItem5, "\tData Type\t\t%S" );
        temp.Format( KItem5, dataType );
        }
    else
        {
        _LIT( KItem5, "\tData Type\t\t[Undefined]" );
        temp.Copy( KItem5 );
        }
    iInfoText->AppendL( temp );
    CleanupStack::PopAndDestroy( dataType );

    _LIT( KItem6, "\tDrive\t\t%S" );
    TBuf< 10 > drive;
    const TDriveNumber driveNumber = static_cast< TDriveNumber >( static_cast< TInt >( aInfo->Drive() ) );
    CMemSpyEngineHelperFileSystem::GetDriveNumberText( driveNumber, drive );
    temp.Format( KItem6, &drive );
    iInfoText->AppendL( temp );

    if  ( aInfo->Disabled() )
        {
        iInfoText->AppendL( _L("\tIs Disabled\t\tYes") );
        }
    else
        {
        iInfoText->AppendL( _L("\tIs Disabled\t\tNo") );
        }

    if  ( aInfo->RomOnly() )
        {
        iInfoText->AppendL( _L("\tROM-Only\t\tYes") );
        }
    else
        {
        iInfoText->AppendL( _L("\tROM-Only\t\tNo") );
        }
    
    if  ( aInfo->RomBased() )
        {
        iInfoText->AppendL( _L("\tROM-Based\t\tYes") );
        }
    else
        {
        iInfoText->AppendL( _L("\tROM-Based\t\tNo") );
        }
    
    // Make caption
    if  ( aInfo->DisplayName().Length() )
        {
        _LIT( KCaptionFormat, "\t0x%08x\t\t%S" );
        temp.Format( KCaptionFormat, aInfo->ImplementationUid().iUid, &aInfo->DisplayName() );
        }
    else
        {
        _LIT( KCaptionFormat, "\t0x%08x\t\t[Name Undefined]" );
        temp.Format( KCaptionFormat, aInfo->ImplementationUid().iUid );
        }
    iCaption = temp.AllocL();

    // Must do this last
    iInfo = aInfo;
    }


CMemSpyEngineEComImplementation* CMemSpyEngineEComImplementation::NewLC( CMemSpyEngineEComInterface& aInterface, CImplementationInformation* aInfo )
    {
    CMemSpyEngineEComImplementation* self = new(ELeave) CMemSpyEngineEComImplementation( aInterface );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


HBufC* CMemSpyEngineEComImplementation::CopyAndCropLC( const TDesC8& aText )
    {
    const TInt KMaxEComDataLength = 200; // Any longer and we'll crop it...
    //
    HBufC* data = HBufC::NewLC( aText.Length() );
    data->Des().Copy( aText );
    //
    if  ( data->Length() > KMaxEComDataLength )
        {
        data->Des().SetLength( KMaxEComDataLength );
        }
    //
    return data;
    }


EXPORT_C TInt CMemSpyEngineEComImplementation::MdcaCount() const
    {
    return iInfoText->MdcaCount();
    }


EXPORT_C TPtrC CMemSpyEngineEComImplementation::MdcaPoint( TInt aIndex ) const
    {
    return iInfoText->MdcaPoint( aIndex );
    }



