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

#ifndef MEMSPYENGINEOUTPUTSINK_H
#define MEMSPYENGINEOUTPUTSINK_H

// System includes
#include <e32base.h>
#include <bamdesca.h>

// User includes
#include <memspy/engine/memspyengineoutputsinktype.h>

// Classes referenced
class CMemSpyEngine;

// Constants
const TBool KMemSpyEngineSinkDoNotCreateOwnDataStream = EFalse;

// Literal constants
_LIT( KMemSpySinkTagClose, "/" );


/** 
 * Context is like additional data that is appended to the file name.
 * Folder is used to formulate a subdirectory within the main memspy output folder
 * Extension is the file extension
 * aUseFileTimeStamp, if true, will cause all output files (irrespective of the folder) to include a timestamp prefix
 * aFolderTimeStamp, if specified, will cause the parent folder to also have a timestamp prepended to it
 */
NONSHARABLE_CLASS( CMemSpyEngineSinkMetaData ) : public CBase
    {
public:
    IMPORT_C static CMemSpyEngineSinkMetaData* NewL();
    IMPORT_C static CMemSpyEngineSinkMetaData* NewL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp );
    IMPORT_C static CMemSpyEngineSinkMetaData* NewL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp, const TTime& aFolderTimeStamp );
    IMPORT_C static CMemSpyEngineSinkMetaData* NewL( const TDesC& aRoot, const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp );
    IMPORT_C static CMemSpyEngineSinkMetaData* NewL( const TDesC& aRoot, const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseFileTimeStamp, const TTime& aFolderTimeStamp );
    IMPORT_C ~CMemSpyEngineSinkMetaData();

private:
    CMemSpyEngineSinkMetaData( TBool aOverwrite, TBool aUseTimeStamp );
    void ConstructL( const TDesC& aRoot, const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, const TTime& aFolderTime );

public: // Access
    inline const TDesC& Root() const { return *iRoot; }
    inline const TDesC& Context() const { return *iContext; }
    inline const TDesC& Folder() const { return *iFolder; }
    inline const TDesC& Extension() const { return *iExtension; }
    inline const TDesC& FolderTimeStamp() const { return *iFolderTimeStamp; }
    inline const TBool Overwrite() const { return iOverwrite; }
    inline const TBool UseFileTimeStamp() const { return iUseFileTimeStamp; }

private: // Data members
    HBufC* iRoot;
    HBufC* iContext;
    HBufC* iFolder;
    HBufC* iExtension;
    HBufC* iFolderTimeStamp;
    TBool iOverwrite;
    TBool iUseFileTimeStamp;
    };



NONSHARABLE_CLASS( CMemSpyEngineOutputSink ) : public CBase
    {
protected:
    CMemSpyEngineOutputSink( CMemSpyEngine& aEngine );
    void BaseConstructL();

public:
    ~CMemSpyEngineOutputSink();

public:
    virtual TMemSpySinkType Type() const = 0;

public:
    IMPORT_C void DataStreamTimeStampBeginL( const TTime& aTime );
    IMPORT_C void DataStreamTimeStampEnd();
    IMPORT_C void DataStreamBeginL( const TDesC& aContext );
    IMPORT_C void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder );
    virtual void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension ) = 0;
    virtual void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite ) = 0;
    virtual void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseTimeStamp ) = 0;
    virtual void DataStreamEndL() = 0;

public:
    virtual void ProcessSuspendedL( TProcessId aId );
    virtual void ProcessResumed( TProcessId aId );

public:
    IMPORT_C void OutputPrefixSetLC( const TDesC& aPrefix );
    IMPORT_C void OutputPrefixSetFormattedLC( TRefByValue<const TDesC> aFormat, ... );
    IMPORT_C void OutputPrefixClear();

public:
    IMPORT_C void OutputRawL( const TDesC8& aData );
    IMPORT_C void OutputLineL( const TDesC& aLine, TBool aIncludePrefix = ETrue );
    IMPORT_C void OutputLineFormattedL( TRefByValue<const TDesC> aFormat, ... );
    IMPORT_C void OutputBlankLineL();
    IMPORT_C void OutputSectionHeadingL( const TDesC& aCaption, TChar aUnderlineCharacter );

public:
    IMPORT_C void OutputItemAndValueL( const TDesC& aItem, const TDesC& aValue );
    IMPORT_C void OutputItemAndValueL( const TDesC& aItem, TUint aValue, TBool aHex = EFalse );

public:
    IMPORT_C void OutputBinaryDataL( const TDesC& aFormat, const TUint8* aBufferAddress, const TUint8* aDisplayStartAddress, TInt aLength );
    IMPORT_C void OutputBinaryDataCompressedL( const TDesC& aFormat, const TUint8* aBufferAddress, const TUint8* aDisplayStartAddress, TInt aLength );

public: // But not exported
    TPtr& FormatBuffer();
    CMemSpyEngine& Engine();

protected:
    virtual void DoOutputLineL( const TDesC& aLine ) = 0;
    virtual void DoOutputRawL( const TDesC8& aData ) = 0;
    virtual TBool IsPrefixAllowed( const TDesC& aPrefix );

protected:
    static void ClearPrefix( TAny* aSelf );
    const TDesC& DataStreamTimeStamp() const;

protected:
    CMemSpyEngine& iEngine;

private:
    HBufC* iFormatBuffer;
    HBufC* iLocalBuffer;
    HBufC* iPrefix;
    TPtr iFormatBufferPointer;
    HBufC* iDataStreamTimeStampSpecifier;
    };




#endif
