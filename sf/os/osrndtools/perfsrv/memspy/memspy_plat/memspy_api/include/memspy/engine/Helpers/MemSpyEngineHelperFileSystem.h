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

#ifndef MEMSPYENGINEHELPERFILESYSTEM_H
#define MEMSPYENGINEHELPERFILESYSTEM_H

// System includes
#include <e32base.h>
#include <f32file.h>
#include <badesca.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineChunkList;
class CMemSpyEngineChunkEntry;
class CMemSpyEngineOutputList;
class CMemSpyEngineDriveEntry;
class CMemSpyEngineDriveList;
class CMemSpyEngineOpenFileList;

NONSHARABLE_CLASS( CMemSpyEngineHelperFileSystem ) : public CBase
    {
public:
    static CMemSpyEngineHelperFileSystem* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperFileSystem();

private:
    CMemSpyEngineHelperFileSystem( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C CMemSpyEngineDriveList* DriveListL();
    IMPORT_C static TDriveNumber GetSystemDrive();
    IMPORT_C void ListOpenFilesL();
    IMPORT_C void ListOpenFilesL( const TThreadId& aThreadId );
    inline CMemSpyEngine& Engine() const { return iEngine; }

public: // API - But not exported
    CMemSpyEngineOpenFileList* ListOpenFilesLC();
    TProcessId FileServerProcessId() const;
    const CMemSpyEngineChunkEntry* IdentifyCacheChunk( const CMemSpyEngineChunkList& aList, TInt& aIndex ) const;
    static void GetDriveNumberText( TDriveNumber aDriveNumber, TDes& aText );

private: // Internal methods
    void ListOpenFilesL( const CFileList& aList, const TThreadId& aThreadId );

private:
    CMemSpyEngine& iEngine;
    TProcessId iFileServerProcessId;
    };





















NONSHARABLE_CLASS( CMemSpyEngineOpenFileListEntry ) : public CBase
    {
public:
    static CMemSpyEngineOpenFileListEntry* NewLC( const TEntry& aEntry );
    ~CMemSpyEngineOpenFileListEntry();

private:
    CMemSpyEngineOpenFileListEntry();
    void ConstructL( const TEntry& aEntry );

public:
    IMPORT_C const TDesC& FileName() const;
    IMPORT_C TInt Size() const;

public: // But not exported
    TInt UniqueFileId() const;
    void SetUniqueFileId( TInt aValue );

private: // Member data
	TUint iAtt;
    TInt iSize;
	TTime iModified;
	TUidType iType;
    HBufC* iFileName;

    // When enumerating the list of entries associated with a thread, some threads open
    // the same file multiple times. To avoid duplicates, we ensure that each file
    // has a unique identifier associated with it, so that we can distinguish instances from
    // one another.
    TInt iUniqueFileIdentifier;
    };





NONSHARABLE_CLASS( CMemSpyEngineOpenFileListForThread ) : public CBase
    {
public:
    static CMemSpyEngineOpenFileListForThread* NewLC( const TDesC& aThreadName, const TThreadId& aThreadId, const TProcessId& aProcessId, const CFileList& aList );
    ~CMemSpyEngineOpenFileListForThread();

private:
    CMemSpyEngineOpenFileListForThread( const TThreadId& aThreadId, const TProcessId& aProcessId );
    void ConstructL( const TDesC& aThreadName, const CFileList& aList );

public:
    void AddL( CMemSpyEngineOpenFileListEntry* aEntry );
    CMemSpyEngineOpenFileListEntry* EntryByFileName( const TDesC& aFileName );

public:
    IMPORT_C const TDesC& ThreadName() const;
    IMPORT_C const TThreadId& ThreadId() const;
    IMPORT_C const TProcessId& ProcessId() const;
    IMPORT_C TInt Count() const;
    IMPORT_C const CMemSpyEngineOpenFileListEntry& At( TInt aIndex ) const;

private: // Member data
    TThreadId iThreadId;
    TProcessId iProcessId;
    HBufC* iThreadName;
    RPointerArray< CMemSpyEngineOpenFileListEntry > iItems;
    };





NONSHARABLE_CLASS( CMemSpyEngineOpenFileList ) : public CBase
    {
public:
    static CMemSpyEngineOpenFileList* NewLC( CMemSpyEngine& aEngine );
    IMPORT_C ~CMemSpyEngineOpenFileList();

private:
    CMemSpyEngineOpenFileList();
    void ConstructL( CMemSpyEngine& aEngine );

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C const CMemSpyEngineOpenFileListForThread& At( TInt aIndex ) const;

private: // Data members
    RPointerArray< CMemSpyEngineOpenFileListForThread > iItems;
    };























class MMemSpyEngineDriveListObserver
    {
public: // From MMemSpyEngineDriveListObserver
    virtual void HandleDriveListChangedL( const CMemSpyEngineDriveList& aList ) = 0;
    };


NONSHARABLE_CLASS( CMemSpyEngineDriveList ) : public CActive, public MDesCArray
    {
public:
    static CMemSpyEngineDriveList* NewLC( RFs& aFsSession );
    IMPORT_C ~CMemSpyEngineDriveList();

private:
    CMemSpyEngineDriveList( RFs& aFsSession );
    void ConstructL();

public: // API
    IMPORT_C void SetObserver( MMemSpyEngineDriveListObserver* aObserver );
    IMPORT_C TInt Count() const;
    IMPORT_C CMemSpyEngineDriveEntry& At( TInt aIndex );
    IMPORT_C const CMemSpyEngineDriveEntry& At( TInt aIndex ) const;
    IMPORT_C TInt ItemIndex( const CMemSpyEngineDriveEntry& aEntry ) const;
    IMPORT_C CMemSpyEngineDriveEntry* EntryByDriveNumber( TDriveNumber aDrive );

public: // But not exported
    RFs& FsSession();

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // From CActive
    void RunL();
    void DoCancel();

private: // Internal methods
    void NotifyListChangedL();

private: // Data members
    RFs& iFsSession;
    RPointerArray< CMemSpyEngineDriveEntry > iItems;
    MMemSpyEngineDriveListObserver* iObserver;
    };











class MMemSpyEngineDriveEntryObserver
    {
public: // From MMemSpyEngineDriveEntryObserver
    virtual void HandleDriveEntryChangedL( const CMemSpyEngineDriveEntry& aEntry ) = 0;
    };


class CMemSpyEngineDriveEntry : public CActive, public MDesCArray
    {
public:
    static CMemSpyEngineDriveEntry* NewLC( CMemSpyEngineDriveList& aList, TDriveNumber aDriveNumber );
    IMPORT_C ~CMemSpyEngineDriveEntry();

private:
    CMemSpyEngineDriveEntry( CMemSpyEngineDriveList& aList, TDriveNumber aDriveNumber );
    void ConstructL();

public: // API
    IMPORT_C void SetObserver( MMemSpyEngineDriveEntryObserver* aObserver );
    inline const TDesC& Caption() const { return *iCaption; }
    inline TDriveNumber DriveNumber() const { return iDriveNumber; }
    inline const TDriveUnit& DriveUnit() const { return iDriveUnit; }

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Internal methods
    void RefreshL();
    void CreateDriveAttributesL( TDes& aItemBuf );
    void CreateMediaAttributesL( TDes& aItemBuf );
    void AddStandardFieldsL();
    void CreateCaptionL( const TDesC& aVolumeName, const TDesC& aMediaType );
    static void GetMediaTypeText( TMediaType aType, TDes& aText );
    static void GetBatteryState( TBatteryState aBatteryState, TDes& aText );
    static TDriveNumber GetSystemDrive();
    RFs& FsSession();

private: // From CActive
    void RunL();
    void DoCancel();

private: // Member data
    CMemSpyEngineDriveList& iList;
    const TDriveNumber iDriveNumber;
    const TDriveUnit iDriveUnit;
    TMediaSerialNumber iSerialNumber;
    TVolumeInfo iVolumeInfo;
    TDriveInfo iDriveInfo;
    HBufC* iCaption;
    CMemSpyEngineOutputList* iItems;
    MMemSpyEngineDriveEntryObserver* iObserver;
    };




#endif