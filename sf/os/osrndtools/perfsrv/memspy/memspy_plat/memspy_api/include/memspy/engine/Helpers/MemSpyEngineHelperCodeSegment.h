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

#ifndef MEMSPYENGINEHELPERCODESEGMENT_H
#define MEMSPYENGINEHELPERCODESEGMENT_H

// System includes
#include <e32base.h>
#include <e32svr.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyenginetwiddlableitemarray.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineCodeSegEntry;
class CMemSpyEngineCodeSegList;




NONSHARABLE_CLASS( CMemSpyEngineHelperCodeSegment ) : public CBase
    {
public:
    static CMemSpyEngineHelperCodeSegment* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperCodeSegment();

private:
    CMemSpyEngineHelperCodeSegment( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void OutputCodeSegmentsL( TUint aPid, TDes& aLine, const TDesC& aPrefix, TChar aSectionUnderlineCharacter = '=', TBool aLowerCaseSectionHeading = EFalse );
    IMPORT_C CMemSpyEngineCodeSegList* CodeSegmentListL();
    IMPORT_C CMemSpyEngineCodeSegList* CodeSegmentListL( TProcessId aProcess );

public: // API - but not exported
    CMemSpyEngineCodeSegList* CodeSegmentListRamLoadedL();
    inline CMemSpyEngine& Engine() const { return iEngine; }

private: // Internal 
    void GetCodeSegmentHandlesL( RArray<TAny*>& aHandles, TUint* aProcessId = NULL, TBool aRamOnly = EFalse ) const;
    CMemSpyEngineCodeSegList* ListFromHandlesLC( RArray<TAny*>& aHandles ) const;
    static TInt SortByAddress( TAny* const& aLeft, TAny* const& aRight );

private:
    CMemSpyEngine& iEngine;
    };





NONSHARABLE_CLASS( CMemSpyEngineCodeSegList ) : public CMemSpyEngineTwiddlableItemArray<CMemSpyEngineCodeSegEntry>, public MDesCArray
    {
public:
    static CMemSpyEngineCodeSegList* NewLC( CMemSpyEngine& aEngine );

private:
    CMemSpyEngineCodeSegList( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C TInt IndexByHandle( TAny* aHandle ) const;
    IMPORT_C void SortByFileNameL();
    IMPORT_C void SortByCodeSizeL();
    IMPORT_C void SortByDataSizeL();
    IMPORT_C void SortByUidsL();
    IMPORT_C void ShowOnlyEntriesWithGlobalDataL();
    IMPORT_C static void OutputDataColumnsL( CMemSpyEngine& aEngine );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

public: // API - but not exported
    TInt IndexByName( const TDesC& aName ) const;

private: // Internal methods
    static TInt CompareByFileName( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight );
    static TInt CompareByCodeSize( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight );
    static TInt CompareByDataSize( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight );
    static TInt CompareByUid( const CMemSpyEngineCodeSegEntry& aLeft, const CMemSpyEngineCodeSegEntry& aRight );
    static TBool VisibilityFunctionGlobalData( const CMemSpyEngineCodeSegEntry*& aItem, TAny* aRune );
    };





NONSHARABLE_CLASS( CMemSpyEngineCodeSegEntry ) : public CDesCArrayFlat
    {
public:
    static CMemSpyEngineCodeSegEntry* NewLC( TAny* aHandle, TInt aSize, const TCodeSegCreateInfo& aCreateInfo, const TProcessMemoryInfo& aMemoryInfo );
    IMPORT_C ~CMemSpyEngineCodeSegEntry();

private:
    CMemSpyEngineCodeSegEntry( TAny* aHandle, TInt aSize, const TCodeSegCreateInfo& aCreateInfo, const TProcessMemoryInfo& aMemoryInfo );
    void ConstructL();

public: // API
    inline TAny* Handle() const { return iHandle; }
    inline TInt Size() const { return iSize; }
    inline const TDesC& Caption() const { return *iCaption; }
    inline const TDesC& FileName() const { return *iFileName; }
    inline const TCodeSegCreateInfo& CreateInfo() const { return iCreateInfo; }
    inline const TProcessMemoryInfo& MemoryInfo() const { return iMemoryInfo; }

public: // API
    IMPORT_C TBool HasCapability( TCapability aCapability ) const;
    IMPORT_C void OutputDataL( CMemSpyEngineHelperCodeSegment& aHelper ) const;

private: // Internal methods
    void AddCapabilityItemsL();
    CDesCArray* CapabilityStringsLC() const;
    void AddCapabilitiesL( TUint32 aCaps, TInt aCapCount, CDesCArray& aArray ) const;

private: // Member data
    TAny* iHandle;
    TInt iSize;
    HBufC* iCaption;
    HBufC* iFileName;
    TCodeSegCreateInfo iCreateInfo;
    TProcessMemoryInfo iMemoryInfo;
    };





#endif
