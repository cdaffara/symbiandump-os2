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

#ifndef MEMSPYENGINEHELPERCHUNK_H
#define MEMSPYENGINEHELPERCHUNK_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyProcess;
class CMemSpyEngine;
class CMemSpyEngineChunkEntry;
class CMemSpyEngineChunkList;
class CMemSpyEngineOutputList;
class TMemSpyDriverChunkInfo;


NONSHARABLE_CLASS( CMemSpyEngineHelperChunk ) : public CBase
    {
public:
    static CMemSpyEngineHelperChunk* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperChunk();

private:
    CMemSpyEngineHelperChunk( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void OutputChunkInfoForThreadL( const CMemSpyThread& aThread );
    IMPORT_C void OutputChunkInfoForThreadL( TThreadId aTid );
    IMPORT_C void OutputChunkInfoForThreadL( TThreadId aTid, TDes& aLine );
    //
    IMPORT_C void OutputChunkInfoForProcessL( const CMemSpyProcess& aProcess );
    IMPORT_C void OutputChunkInfoForProcessL( TProcessId aPid );
    IMPORT_C void OutputChunkInfoForProcessL( TProcessId aPid, TDes& aLine );
    //
    IMPORT_C CMemSpyEngineChunkList* ListL();
    IMPORT_C CMemSpyEngineChunkList* ListForThreadL( TThreadId aTid );
    IMPORT_C CMemSpyEngineChunkList* ListForProcessL( TProcessId aPid );

public:
    inline CMemSpyEngine& Engine() const { return iEngine; }

private: // Internal enumerations
    enum TType
        {
        EThread = 0,
        EProcess,
        EAll
        };

private: // Internal methods
    void DoOutputChunkInfoForObjectL( TUint aId, TDes& aLineBuffer, TType aType );
    void GetChunkHandlesL( RArray<TAny*>& aHandles, TType aType, TUint aId = 0 );
    CMemSpyEngineChunkList* CreateListFromHandlesL( const RArray<TAny*>& aHandles );

private:
    CMemSpyEngine& iEngine;
    };






NONSHARABLE_CLASS( CMemSpyEngineChunkList ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineChunkList* NewLC( CMemSpyEngine& aEngine );
    IMPORT_C ~CMemSpyEngineChunkList();

private:
    CMemSpyEngineChunkList( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C CMemSpyEngineChunkEntry& At( TInt aIndex );
    IMPORT_C TInt ItemIndex( const CMemSpyEngineChunkEntry& aEntry ) const;
    IMPORT_C TBool EntryExists( TAny* aHandle ) const;
    IMPORT_C const CMemSpyEngineChunkEntry& At( TInt aIndex ) const;
    IMPORT_C void SortByNameL();
    IMPORT_C void SortBySizeL();
    IMPORT_C static void OutputDataColumnsL( CMemSpyEngine& aEngine );

public: // But not exported
    void AddItemL( CMemSpyEngineChunkEntry* aItem );
    void Remove( TInt aIndex );
    void RemoveByHandle( TAny* aChunkHandle );
    TInt ItemIndexByProcessId( TUint aPid ) const;
    TInt ItemIndexByProcessId( TUint aPid, TInt aStartIndex ) const;
    TInt ItemIndexByProcessId( TUint aPid, TMemSpyDriverChunkType aType ) const;
    TInt ItemIndexByChunkHandle( TAny* aHandle ) const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Internal methods
    static TInt CompareByName( const CMemSpyEngineChunkEntry& aLeft, const CMemSpyEngineChunkEntry& aRight );
    static TInt CompareBySize( const CMemSpyEngineChunkEntry& aLeft, const CMemSpyEngineChunkEntry& aRight );

private: // Data members
    CMemSpyEngine& iEngine;
    RPointerArray<CMemSpyEngineChunkEntry> iItems;
    };





NONSHARABLE_CLASS( CMemSpyEngineChunkEntry ) : public CBase, public MDesCArray
    {
public: // Enumerations - see DChunk::TChunkAttributes
    enum TChunkAttributes
        {
		ENormal			=0x00,
		EDoubleEnded	=0x01,
		EDisconnected	=0x02,
		EConstructed	=0x04,
		EMemoryNotOwned	=0x08
        };

    public:
    static CMemSpyEngineChunkEntry* NewLC( CMemSpyEngine& aEngine, const TMemSpyDriverChunkInfo& aInfo );
    ~CMemSpyEngineChunkEntry();

private:
    CMemSpyEngineChunkEntry( CMemSpyEngine& aEngine );
    void ConstructL( const TMemSpyDriverChunkInfo& aInfo );

public: // API
    inline const TDesC& Caption() const { return *iCaption; }

public: // API - chunk info
    const TDesC& Name() const;
    TAny* Handle() const;
    TInt Size() const;
    TInt MaxSize() const;
    TAny* BaseAddress() const;
    TAny* UpperAddress() const;
    TUint OwnerId() const;

public: // API
    IMPORT_C void OwnerName( TDes& aDes ) const;
    IMPORT_C void AppendOwnerName( TDes& aDes ) const;
    IMPORT_C void OutputDataL( CMemSpyEngineHelperChunk& aHelper ) const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

public: // But not exported
    TMemSpyDriverChunkInfo& Info();    
    const TMemSpyDriverChunkInfo& Info() const;    

private: // Member data
    CMemSpyEngine& iEngine;
    TMemSpyDriverChunkInfo* iInfo;
    HBufC* iCaption;
    CMemSpyEngineOutputList* iList;
    };




#endif