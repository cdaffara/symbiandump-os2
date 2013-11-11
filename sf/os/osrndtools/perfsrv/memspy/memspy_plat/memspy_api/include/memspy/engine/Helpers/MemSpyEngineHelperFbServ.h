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

#ifndef MEMSPYENGINEHELPERFBSERV_H
#define MEMSPYENGINEHELPERFBSERV_H

// System includes
#include <e32base.h>
#include <badesca.h>
#include <fbs.h>
#include <bitmap.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyThread;
class TMemSpyDriverChunkInfo;
class TMemSpyEngineFBServBitmapInfo;

// Constants
const TInt KMemSpyEngineDefaultBitmapCreationBatchSize = 4; // Create 4 bitmaps (asynchronously) at a time




NONSHARABLE_CLASS( CMemSpyEngineHelperFbServ ) : public CBase
    {
public: // Construct/destruct
    static CMemSpyEngineHelperFbServ* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperFbServ();

private: // Internal construction
    CMemSpyEngineHelperFbServ( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void GetArrayOfBitmapHandlesL( RArray<TInt>& aHandles );

public: // API - but not exported
    void GetArrayOfBitmapHandlesL( RArray<TInt>& aHandles, TInt& aBitmapConCount, TInt& aFontConCount );
    static TBool IsLargeBitmapChunk( const TMemSpyDriverChunkInfo& aChunkInfo );
    static TBool IsSharedBitmapChunk( const TMemSpyDriverChunkInfo& aChunkInfo );
    static void GetBitmapInfoL( TInt aHandle, TMemSpyEngineFBServBitmapInfo& aInfo );
    static CFbsBitmap* GetBitmapInfoLC( TInt aHandle, TMemSpyEngineFBServBitmapInfo& aInfo );

private: // Internal structures
    class CBitmapObject : public CObject
        {
    public:
        CBitmapObject* iThisPointer;
    public:
        CBase* iTop; // actually CFbTop*
        CBitwiseBitmap* iAddressPointer;
        TInt iHandle;
        CBitmapObject* iCleanBitmap;
        };

private: // Internal methods
    CMemSpyThread& IdentifyFbServThreadL();
    HBufC8* LocateCFbTopHeapCellDataLC( CMemSpyThread& aFbServThread, TAny*& aCellAddress );
    HBufC8* LocateBitmapArrayHeapCellDataLC( TAny*& aArrayCellAddress, TInt aArrayAllocCount );
    void ReadCObjectConInfoL( TAny* aCellAddress, RArray<TAny*>& aContainerObjects, TInt& aCount, TInt& aAllocated );
    static void ParseCellDataAndExtractHandlesL( const TDesC8& aData, RArray<TInt>& aHandles, TInt aArrayEntryCount );
    static TBool VerifyCorrectHeapCellL( const TDesC8& aData, TAny* aCellAddress, TAny* aPayloadAddress, TUint aHeapStartingAddress, TUint aHeapMaxSize );
    static TUint OffsetToCObjectConBitmapCon();
    static TUint OffsetToCObjectConFontCon();
    static TUint OffsetToBitmapHandleArray();
    CBitmapObject* GetBitmapObjectLC( TAny* aAddress );

private: // Data members
    CMemSpyEngine& iEngine;
    };








NONSHARABLE_CLASS( CMemSpyEngineFbServBitmap ) : public CDesCArrayFlat
    {
public: // Construct/destruct
    IMPORT_C static CMemSpyEngineFbServBitmap* NewLC( TInt aHandle );
    IMPORT_C ~CMemSpyEngineFbServBitmap();

private: // Internal construction
    CMemSpyEngineFbServBitmap();
    void ConstructL( TInt aHandle );

public: // API
    TInt BitmapSizeInBytes() const;
    IMPORT_C const TDesC& Caption() const;
    IMPORT_C CFbsBitmap& Bitmap();
    IMPORT_C const CFbsBitmap& Bitmap() const;
    IMPORT_C TInt Handle() const;
    IMPORT_C void OutputDataL( CMemSpyEngine& aEngine ) const;
    IMPORT_C static void OutputDataColumnsL( CMemSpyEngine& aEngine );
    IMPORT_C void GetExportableFileNameL( TDes& aAppendName ) const;

public: // But not exported
    static void GetExportableFileNameL( const TMemSpyEngineFBServBitmapInfo& aInfo, TDes& aAppendName );

private: // Internal
    void PrepareItemsL();

private: 
    static void GetFileCompressionTypeAsString( TBitmapfileCompression aType, TDes& aString );

private: // Data members
    CFbsBitmap* iBitmap;
    HBufC* iCaption;
    };










class MMemSpyEngineFbSerbBitmapArrayObserver
    {
public:
    enum TEvent
        {
        EBitmapItemsCreated = 0,
        EBitmapArrayConstructionComplete
        };

public: // From MMemSpyEngineFbSerbBitmapArrayObserver
    virtual void HandleFbServBitmapArrayEventL( TEvent aEvent ) = 0;
    };












NONSHARABLE_CLASS( CMemSpyEngineFbServBitmapArray ) : public CActive, public MDesCArray
    {
public: // Construct/destruct
    IMPORT_C static CMemSpyEngineFbServBitmapArray* NewL( const RArray<TInt>& aBitmapHandles );
    IMPORT_C static CMemSpyEngineFbServBitmapArray* NewL( TInt aPriority, const RArray<TInt>& aBitmapHandles, MMemSpyEngineFbSerbBitmapArrayObserver& aObserver, TInt aGranularity = KMemSpyEngineDefaultBitmapCreationBatchSize );
    IMPORT_C ~CMemSpyEngineFbServBitmapArray();

private: // Internal construction
    CMemSpyEngineFbServBitmapArray( const RArray<TInt>& aBitmapHandles );
    CMemSpyEngineFbServBitmapArray( TInt aPriority, const RArray<TInt>& aBitmapHandles, MMemSpyEngineFbSerbBitmapArrayObserver& aObserver, TInt aGranularity );
    void ConstructL();

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C CMemSpyEngineFbServBitmap& At( TInt aIndex );
    IMPORT_C const CMemSpyEngineFbServBitmap& At( TInt aIndex ) const;
    IMPORT_C CMemSpyEngineFbServBitmap& BitmapByHandleL( TInt aHandle );
    IMPORT_C TInt BitmapIndexByHandle( TInt aHandle ) const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private: // Internal
    void CompleteSelf();
    void CreateBitmapL( TInt aHandle );
    void SortBySizeL();
    static TInt CompareBySize( const CMemSpyEngineFbServBitmap& aLeft, const CMemSpyEngineFbServBitmap& aRight );

private: // Data members
    const RArray<TInt>& iBitmapHandles;
    MMemSpyEngineFbSerbBitmapArrayObserver* iObserver;
    const TInt iGranularity;

    // Transient
    TInt iIndex;

    // Owned
    RPointerArray< CMemSpyEngineFbServBitmap > iBitmaps;
    };







class TMemSpyEngineFBServBitmapInfo
    {
public:
    inline TMemSpyEngineFBServBitmapInfo()
        : iHandle( 0 ), iSizeInPixels( 0, 0 ), iCompressionType( ENoBitmapCompression ),
        iSizeInBytes( 0 ), iDisplayMode( 0 ), iBitsPerPixel( 0 ), iFlags( TMemSpyEngineFBServBitmapInfo::EFlagsNone )
        {}

public: // Enumerations
    enum TFlags
        {
        EFlagsNone = 0,
        EFlagsIsRomBitmap = 1,
        EFlagsIsMonochrome = 2,
        EFlagsIsLarge = 4,
        EFlagsIsCompressedInRam = 8,
        };

public: // Data members
    TInt iHandle;
    TSize iSizeInPixels;
    TBitmapfileCompression iCompressionType;
    TInt iSizeInBytes;
    TInt8 iDisplayMode;
    TInt8 iBitsPerPixel;
    TUint16 iFlags;
    TInt iColor;
    };


#endif