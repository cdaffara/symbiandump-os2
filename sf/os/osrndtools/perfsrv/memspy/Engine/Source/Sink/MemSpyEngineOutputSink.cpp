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

#include <memspy/engine/memspyengineoutputsink.h>

// User includes
#include <memspy/engine/memspyengineutils.h>

// Constants
const TInt KMemSpyItemAndValueAlignmentAmount = 30;
const TInt KMemSpyNumericFormatBufferSize = 20;

// Literal constants
_LIT( KMemSpyNumericHexFormat, "0x%08x" );
_LIT( KMemSpyNumericDecFormat, "%d" );
_LIT( KMemSpyDataStreamFolderNameFormatSpec, "%04d%02d%02d - %02dh %02dm %02ds" );


CMemSpyEngineOutputSink::CMemSpyEngineOutputSink( CMemSpyEngine& aEngine )
:   iEngine( aEngine ), iFormatBufferPointer( NULL, 0 )
    {
    }


CMemSpyEngineOutputSink::~CMemSpyEngineOutputSink()
    {
    delete iFormatBuffer;
    delete iPrefix;
    delete iLocalBuffer;
    delete iDataStreamTimeStampSpecifier;
    }


void CMemSpyEngineOutputSink::BaseConstructL()
    {
    iFormatBuffer = HBufC::NewL( 1024 );
    iFormatBufferPointer.Set( iFormatBuffer->Des() );
    iPrefix = HBufC::NewL( 2048 );
    iLocalBuffer = HBufC::NewL( 2048 );
    }


EXPORT_C void CMemSpyEngineOutputSink::DataStreamTimeStampBeginL( const TTime& aTime )
    {
    const TDateTime dt( aTime.DateTime() );

    // Build it up...
    HBufC* spec = HBufC::NewL( KMaxFileName );
    TPtr pName( spec->Des() );
    pName.Format( KMemSpyDataStreamFolderNameFormatSpec, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
    
    DataStreamTimeStampEnd();
    iDataStreamTimeStampSpecifier = spec;
    }


EXPORT_C void CMemSpyEngineOutputSink::DataStreamTimeStampEnd()
    {
    delete iDataStreamTimeStampSpecifier;
    iDataStreamTimeStampSpecifier = NULL;
    }


EXPORT_C void CMemSpyEngineOutputSink::DataStreamBeginL( const TDesC& aContext )
    {
    DataStreamBeginL( aContext, KNullDesC );
    }


EXPORT_C void CMemSpyEngineOutputSink::DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder )
    {
    DataStreamBeginL( aContext, aFolder, KNullDesC );
    }


void CMemSpyEngineOutputSink::ProcessSuspendedL( TProcessId /*aId*/ )
    {
    }


void CMemSpyEngineOutputSink::ProcessResumed( TProcessId /*aId*/ )
    {
    }


TBool CMemSpyEngineOutputSink::IsPrefixAllowed( const TDesC& /*aPrefix*/ )
    {
    return ETrue;
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputPrefixSetLC( const TDesC& aPrefix )
    {
    __ASSERT_ALWAYS( aPrefix.Length() <= iPrefix->Des().MaxLength(), User::Invariant() );
    TPtr pPrefix( iPrefix->Des() );
    pPrefix.Zero();

    if  ( IsPrefixAllowed( aPrefix ) )
        {
        pPrefix.Copy( aPrefix );
        }

    CleanupStack::PushL( TCleanupItem( ClearPrefix, this ) );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputPrefixSetFormattedLC( TRefByValue<const TDesC> aFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aFormat);
    //
	iFormatBufferPointer.Zero();
    iFormatBufferPointer.FormatList( aFormat, list );
    //
    OutputPrefixSetLC( iFormatBufferPointer );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputPrefixClear()
    {
    iPrefix->Des().Zero();
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputRawL( const TDesC8& aData )
    {
    DoOutputRawL( aData );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputLineL( const TDesC& aLine, TBool aIncludePrefix )
    {
    TPtr pBuffer( iLocalBuffer->Des() );
    if  ( aIncludePrefix )
        {
        pBuffer.Copy( *iPrefix );
        }
    else
        {
        pBuffer.Zero();
        }

    pBuffer.Append( aLine );
    DoOutputLineL( pBuffer );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputLineFormattedL( TRefByValue<const TDesC> aFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aFormat);

	iFormatBufferPointer.Zero();
    iFormatBufferPointer.FormatList( aFormat, list );
    //
    OutputLineL( iFormatBufferPointer );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputBlankLineL()
    {
    DoOutputLineL( KNullDesC );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputSectionHeadingL( const TDesC& aCaption, TChar aUnderlineCharacter )
    {
    OutputBlankLineL();
    OutputLineL( aCaption );
    //
    HBufC* underline = HBufC::NewLC( aCaption.Length() );
    TPtr pUnderline( underline->Des() );
    pUnderline.Fill( aUnderlineCharacter, aCaption.Length());
    OutputLineL( *underline );
    CleanupStack::PopAndDestroy( underline );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputItemAndValueL( const TDesC& aItem, const TDesC& aValue )
    {
    FormatBuffer().Zero();
    FormatBuffer().Append( aItem );
    FormatBuffer().Append( ':' );

    const TInt padAmount = KMemSpyItemAndValueAlignmentAmount - aItem.Length();
 
    FormatBuffer().AppendFill(' ', padAmount);
    FormatBuffer().Append( ' ' );
    FormatBuffer().Append( aValue );

    OutputLineL( FormatBuffer() );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputItemAndValueL( const TDesC& aItem, TUint aValue, TBool aHex )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    //
    if  ( aHex )
        {
        val.Format( KMemSpyNumericHexFormat, aValue );
        }
    else
        {
        val.Format( KMemSpyNumericDecFormat, aValue );
        }
    //
    OutputItemAndValueL( aItem, val );
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputBinaryDataL( const TDesC& aFormat, const TUint8* aBufferAddress, const TUint8* aDisplayStartAddress, TInt aLength )
    {
    _LIT(KAddresPostfix, ": ");
    _LIT(KDoubleSpace, "  ");
    _LIT(KSingleSpace, " ");

    const TInt maxLen = aLength;
    TInt len = aLength;
    const TUint8* pDataAddr = aBufferAddress;

	TBuf<81> out;
	TBuf<20> ascii;
	TInt offset = 0;
	const TUint8* a = pDataAddr;
    const TUint8* displayAddress = aDisplayStartAddress;
    //
	while(len>0)
		{
		out.Zero();
		ascii.Zero();
		out.AppendNumFixedWidth((TUint) displayAddress, EHex, 8);
		out.Append(KAddresPostfix);

        TUint b;
		for (b=0; b<16; b++)
			{
            TUint8 c = ' ';
            if	((pDataAddr + offset + b) < pDataAddr + maxLen)
	            {
	            c = *(pDataAddr + offset + b);
				out.AppendNumFixedWidth(c, EHex, 2);
	            }
            else
	            {
				out.Append(KDoubleSpace);
	            }
			out.Append( KSingleSpace );
			if (c<=0x20 || c == 0x27 || c>=0x7f || c=='%')
				c=0x2e;
			ascii.Append(TChar(c));
			}
		out.Append(ascii);
        out.ZeroTerminate();

        FormatBuffer().Format( aFormat, &out );
        OutputLineL( FormatBuffer() );

        displayAddress += 16;
        a += 16;
		offset += 16;
		len -= 16;
        }
    }


EXPORT_C void CMemSpyEngineOutputSink::OutputBinaryDataCompressedL( const TDesC& aFormat, const TUint8* aBufferAddress, const TUint8* aDisplayStartAddress, TInt aLength )
    {
    _LIT(KAddresPostfix, ": ");
    _LIT(KDoubleSpace, "  ");

    const TInt maxLen = aLength;
    TInt len = aLength;
    const TUint8* pDataAddr = aBufferAddress;

	TBuf<81> out;
	TInt offset = 0;
	const TUint8* a = pDataAddr;
    const TUint8* displayAddress = aDisplayStartAddress;
    //
	while(len>0)
		{
		out.Zero();
		out.AppendNumFixedWidth((TUint) displayAddress, EHex, 8);
		out.Append(KAddresPostfix);

        TUint b;
		for (b=0; b<16; b++)
			{
            TUint8 c = ' ';
            if	((pDataAddr + offset + b) < pDataAddr + maxLen)
	            {
	            c = *(pDataAddr + offset + b);
				out.AppendNumFixedWidth(c, EHex, 2);
	            }
            else
	            {
				out.Append(KDoubleSpace);
	            }
			if (c<=0x20 || c>=0x7f || c=='%')
				c=0x2e;
			}
        out.ZeroTerminate();

        FormatBuffer().Format( aFormat, &out );
        OutputLineL( FormatBuffer() );

        displayAddress += 16;
        a += 16;
		offset += 16;
		len -= 16;
        }
    }


TPtr& CMemSpyEngineOutputSink::FormatBuffer()
    {
    return iFormatBufferPointer;
    }


CMemSpyEngine& CMemSpyEngineOutputSink::Engine()
    {
    return iEngine;
    }


void CMemSpyEngineOutputSink::ClearPrefix( TAny* aSelf )
    {
    CMemSpyEngineOutputSink* self = reinterpret_cast< CMemSpyEngineOutputSink* >( aSelf );
    self->OutputPrefixClear();
    }


const TDesC& CMemSpyEngineOutputSink::DataStreamTimeStamp() const
    {
    if  ( iDataStreamTimeStampSpecifier )
        {
        return *iDataStreamTimeStampSpecifier;
        }

    return KNullDesC;
    }

















CMemSpyEngineSinkMetaData::CMemSpyEngineSinkMetaData( TBool aOverwrite, TBool aUseFileTimeStamp )
:   iOverwrite( aOverwrite ), iUseFileTimeStamp( aUseFileTimeStamp )
    {
    }


EXPORT_C CMemSpyEngineSinkMetaData::~CMemSpyEngineSinkMetaData()
    {
	delete iRoot;
    delete iContext;
    delete iFolder;
    delete iExtension;
    delete iFolderTimeStamp;
    }


void CMemSpyEngineSinkMetaData::ConstructL( const TDesC& aRoot, const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, const TTime& aFolderTime )
    {
	iRoot = aRoot.AllocL();
    iContext = aContext.AllocL();
    iFolder = aFolder.AllocL();
    iExtension = aExtension.AllocL();

    const TDateTime dt = aFolderTime.DateTime();
    HBufC* spec = HBufC::NewLC( KMaxFileName );
    TPtr pName( spec->Des() );
    pName.Format( KMemSpyDataStreamFolderNameFormatSpec, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
    iFolderTimeStamp = pName.AllocL();
    CleanupStack::PopAndDestroy( spec );
    }


EXPORT_C CMemSpyEngineSinkMetaData* CMemSpyEngineSinkMetaData::NewL()
    {
    return CMemSpyEngineSinkMetaData::NewL( KNullDesC, KNullDesC, KNullDesC, ETrue, ETrue );
    }

EXPORT_C CMemSpyEngineSinkMetaData* CMemSpyEngineSinkMetaData::NewL(  const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp )
	{
	return NewL( KNullDesC, aContext, aFolder, aExtension, aOverwrite, aUseFileTimeStamp );
	}

EXPORT_C CMemSpyEngineSinkMetaData* CMemSpyEngineSinkMetaData::NewL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp, const TTime& aFolderTimeStamp )
    {
    return NewL( KNullDesC, aContext, aFolder, aExtension, aOverwrite, aUseFileTimeStamp, aFolderTimeStamp );
    }

EXPORT_C CMemSpyEngineSinkMetaData* CMemSpyEngineSinkMetaData::NewL( const TDesC& aRoot, const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp )
    {
    // Create a dummy time, we'll clear it after ConstructL() returns...
    TTime now; now.HomeTime();

    CMemSpyEngineSinkMetaData* self = new(ELeave) CMemSpyEngineSinkMetaData( aOverwrite, aUseFileTimeStamp );
    CleanupStack::PushL( self );
    self->ConstructL( aRoot, aContext, aFolder, aExtension, now );
    CleanupStack::Pop( self );

    // Clear folder time stamp
    self->iFolderTimeStamp->Des().Zero();
    return self;
    }

EXPORT_C CMemSpyEngineSinkMetaData* CMemSpyEngineSinkMetaData::NewL( const TDesC& aRoot, const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp, const TTime& aFolderTimeStamp )
    {
    CMemSpyEngineSinkMetaData* self = new(ELeave) CMemSpyEngineSinkMetaData( aOverwrite, aUseFileTimeStamp );
    CleanupStack::PushL( self );
    self->ConstructL( aRoot, aContext, aFolder, aExtension, aFolderTimeStamp );
    CleanupStack::Pop( self );
    return self;
    }
