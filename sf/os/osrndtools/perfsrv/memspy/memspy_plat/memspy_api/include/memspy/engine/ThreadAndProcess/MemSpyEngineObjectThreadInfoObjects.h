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

#ifndef MEMSPYTHREADINFOOBJECTS_H
#define MEMSPYTHREADINFOOBJECTS_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyengineobject.h>
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/engine/memspyengineprocessmemorytracker.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyThread;
class CMemSpyThreadInfoContainer;
class CMemSpyEngineChunkList;
class CMemSpyEngineCodeSegList;
class CMemSpyEngineActiveObjectArray;

// Enumerations
enum TMemSpyThreadInfoItemType
    {
    EMemSpyThreadInfoItemTypeFirst = 0,
    //
    EMemSpyThreadInfoItemTypeGeneralInfo = EMemSpyThreadInfoItemTypeFirst,
    EMemSpyThreadInfoItemTypeHeap,
    EMemSpyThreadInfoItemTypeStack,
    EMemSpyThreadInfoItemTypeChunk,
    EMemSpyThreadInfoItemTypeCodeSeg,
    EMemSpyThreadInfoItemTypeOpenFiles,
    EMemSpyThreadInfoItemTypeActiveObject,
    EMemSpyThreadInfoItemTypeOwnedThreadHandles,
    EMemSpyThreadInfoItemTypeOwnedProcessHandles,
    EMemSpyThreadInfoItemTypeServer,
    EMemSpyThreadInfoItemTypeSession,
    EMemSpyThreadInfoItemTypeSemaphore,
    EMemSpyThreadInfoItemTypeOtherThreads,
    EMemSpyThreadInfoItemTypeOtherProcesses,
    EMemSpyThreadInfoItemTypeMutex,
    EMemSpyThreadInfoItemTypeTimer,
    EMemSpyThreadInfoItemTypeLogicalChannel,
    EMemSpyThreadInfoItemTypeChangeNotifier,
    EMemSpyThreadInfoItemTypeUndertaker,
    EMemSpyThreadInfoItemTypeMessageQueue,
    EMemSpyThreadInfoItemTypeConditionalVariable,
    EMemSpyThreadInfoItemTypeLDD,
    EMemSpyThreadInfoItemTypePDD,
    EMemSpyThreadInfoItemTypeMemoryTracking,
    EMemSpyThreadInfoItemTypeMemoryTrackingCurrent,
    EMemSpyThreadInfoItemTypeMemoryTrackingHWM,
    EMemSpyThreadInfoItemTypeMemoryTrackingPeak,
    //
    EMemSpyThreadInfoItemTypeLast
    };

// Constants
const TInt KMemSpyDefaultMaxHandleCount = 128;



NONSHARABLE_CLASS( CMemSpyThreadInfoItemBase ) : public CMemSpyEngineObject, public MDesCArray
    {
protected:
    CMemSpyThreadInfoItemBase( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aType, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoItemBase();
    virtual void ConstructL() = 0;

private: // Construction support
    static TInt CallConstructL( TAny* aSelf );

public: // API
    virtual TPtrC Name() const = 0;
    IMPORT_C TMemSpyThreadInfoItemType Type() const;
    IMPORT_C virtual void RebuildL();
    inline CMemSpyThreadInfoContainer& Container() { return iContainer; }
    IMPORT_C TBool IsReady() const;
    IMPORT_C CMemSpyEngine& Engine() const;
    IMPORT_C void PrintL();

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;
    IMPORT_C TPtrC Caption(TInt aIndex ) const;
    IMPORT_C TPtrC Value(TInt aIndex ) const;

protected:
    class CItem : public CBase
        {
        public:
            static CItem* NewLC( const CItem& aCopyMe );
            static CItem* NewLC( const TDesC& aCaption );
            static CItem* NewLC( const TDesC& aCaption, const TDesC& aValue );
            static CItem* NewHexLC( const TDesC& aCaption, TUint aValue );
            static CItem* NewDecimalLC( const TDesC& aCaption, TInt aValue );
            static CItem* NewLongLC( const TDesC& aCaption, const TInt64& aValue );
            static CItem* NewYesNoLC( const TDesC& aCaption, TBool aYes );
            static CItem* NewOnOffLC( const TDesC& aCaption, TBool aOn );
            static CItem* NewPercentageLC( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue );
            ~CItem();

        private:
            CItem();
            void ConstructL( const TDesC& aCaption, const TDesC& aValue );

        public: // API
            inline const TDesC& Caption() const { return *iCaption; }
            inline const TDesC& Value() const { return *iValue; }
            inline const TDesC& Combined() const { return *iCombined; }

        public:
            void SetValueL( const TDesC& aValue );
            void SetHexL( TUint aValue );
            void SetDecimalL( TInt aValue );
            void SetLongL( const TInt64& aValue );
            void SetYesNoL( TBool aYes );
            void SetOnOffL( TBool aOn );
            void SetPercentageL( TInt aOneHundredPercentValue, TInt aValue );
            
        private: // Internal
            void UpdateCombinedL();
        
        private:
            HBufC* iCaption;
            HBufC* iValue;
            HBufC* iCombined;
        };

protected: // Internal API
    void AddItemL( const TDesC& aCaption, const TDesC& aValue );
    void AddItemHexL( const TDesC& aCaption, TUint aValue );
    void AddItemDecimalL( const TDesC& aCaption, TInt aValue );
    void AddItemLongL( const TDesC& aCaption, const TInt64& aValue );
    void AddItemYesNoL( const TDesC& aCaption, TBool aYes );
    void AddItemOnOffL( const TDesC& aCaption, TBool aOn );
    void AddItemPercentageL( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue );
    void StripProcessAndThreadNames( TDes& aText );
    void StripProcessName( TDes& aText );
    void StripThreadName( TDes& aText );
    CItem& Item( TInt aIndex );
    const CItem& Item( TInt aIndex ) const;
    virtual void Reset();

private:
    mutable CMemSpyThreadInfoContainer& iContainer;
    TBool iReady;
    TBool iIsEmpty;
    RPointerArray<CItem> iItems;
    CAsyncCallBack iCallBack;
    const TMemSpyThreadInfoItemType iType;
    };








NONSHARABLE_CLASS( CMemSpyThreadInfoGeneral ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoGeneral* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoGeneral( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // Internal methods
    void MakeRegisterListingL( RThread& aThread );
    };







NONSHARABLE_CLASS( CMemSpyThreadInfoHeap ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoHeap* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoHeap( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;
    };







NONSHARABLE_CLASS( CMemSpyThreadInfoOpenFiles ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoOpenFiles* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoOpenFiles( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;
    };






NONSHARABLE_CLASS( CMemSpyThreadInfoActiveObjects ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoActiveObjects* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoActiveObjects();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoActiveObjects( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;
    
public: // API
    inline CMemSpyEngineActiveObjectArray& Array() { return *iItems; }

private: // Member data
    CMemSpyEngineActiveObjectArray* iItems;
    };







NONSHARABLE_CLASS( CMemSpyThreadInfoStack ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoStack* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoStack( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;
    };










NONSHARABLE_CLASS( CMemSpyThreadInfoChunk ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoChunk* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoChunk();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoChunk( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;
    
public: // API
    inline CMemSpyEngineChunkList& List() { return *iList; }
    inline void NullifyList() { iList = NULL; }

private:
    CMemSpyEngineChunkList* iList;
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoCodeSeg ) : public CMemSpyThreadInfoItemBase
    {
public:
    static CMemSpyThreadInfoCodeSeg* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoCodeSeg();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoCodeSeg( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;
    
public: // API
    inline CMemSpyEngineCodeSegList& List() { return *iList; }
    inline void NullifyList() { iList = NULL; }

private: // Data members
    CMemSpyEngineCodeSegList* iList;
    };










NONSHARABLE_CLASS( CMemSpyThreadInfoHandleObjectBase ) : public CMemSpyThreadInfoItemBase
    {
public:
    ~CMemSpyThreadInfoHandleObjectBase();

protected: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoHandleObjectBase( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aItemType, TMemSpyDriverContainerType aContainerType, TBool aAsyncConstruction );
    void ConstructL();

public: // New API
    inline TMemSpyDriverContainerType ContainerType() const { return iContainerType; }
    inline TInt DetailsCount() const { return iInfoItems.Count(); }
    inline TMemSpyDriverHandleInfoGeneric& DetailsAt( TInt aIndex ) { return iInfoItems[ aIndex ]; }
    inline const TMemSpyDriverHandleInfoGeneric& DetailsAt( TInt aIndex ) const { return iInfoItems[ aIndex ]; }
    IMPORT_C TInt DetailsIndexByEntry( const TMemSpyDriverHandleInfoGeneric& aEntry ) const;
    IMPORT_C TInt DetailsIndexByHandle( TAny* aHandle ) const;

protected: // Internal class
    class THandleWrapper
        {
    public:
        inline THandleWrapper( TAny* aHandle, TMemSpyDriverContainerType aType ) : iHandle( aHandle ), iType( aType ), iRefCount( 1 ) { }

    public: // API
        static TBool Match( const THandleWrapper& aLeft, const THandleWrapper& aRight );

    public: // Data members
        TAny* iHandle;
        TMemSpyDriverContainerType iType;
        TInt iRefCount;
        };

protected: // Internal API
    virtual void GetHandlesL( RArray<THandleWrapper>& aArray ) = 0;

protected: // New internal framework API
    virtual void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName ) = 0;
    virtual void HandleAllItemsLocatedL() { }

private: // Data members
    const TMemSpyDriverContainerType iContainerType;
    RArray< TMemSpyDriverHandleInfoGeneric > iInfoItems;
    };









NONSHARABLE_CLASS( CMemSpyThreadInfoHandleByContainer ) : public CMemSpyThreadInfoHandleObjectBase
    {
protected: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoHandleByContainer( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aItemType, TMemSpyDriverContainerType aContainerType, TBool aAsyncConstruction );

protected: // From CMemSpyThreadInfoHandleObjectBase
    void GetHandlesL( RArray<THandleWrapper>& aArray );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoReferencedBy ) : public CMemSpyThreadInfoHandleObjectBase
    {
protected: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoReferencedBy( CMemSpyThreadInfoContainer& aContainer, TMemSpyThreadInfoItemType aItemType, TMemSpyDriverContainerType aContainerType, TBool aAsyncConstruction );

protected: // From CMemSpyThreadInfoHandleObjectBase
    void GetHandlesL( RArray<THandleWrapper>& aArray );
    };





























NONSHARABLE_CLASS( CMemSpyThreadInfoServer ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoServer* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoServer( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;
    IMPORT_C static TPtrC SessionType( TIpcSessionType aType );

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoSession ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoSession* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoSession();

private:
    CMemSpyThreadInfoSession( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

public: // API
    IMPORT_C TInt ConnectionCount( const TDesC& aName ) const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    void HandleAllItemsLocatedL();

protected: // From CMemSpyThreadInfoItemBase
    void Reset();

private: // Internal object
    class CSessionInfoEntry : public CBase
        {
    public:
        inline CSessionInfoEntry( HBufC* aName ) : iCount(1), iName( aName ) { }
        inline ~CSessionInfoEntry() { delete iName; }

    public:
        TInt iCount;
        HBufC* iName;
        };

private: // Internal methods
    static TBool CompareEntries( const CSessionInfoEntry& aLeft, const CSessionInfoEntry& aRight );
    TInt FindServerL( const TDesC& aName ) const;
    
private:
    RPointerArray<CSessionInfoEntry> iServerNames;
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoSemaphore ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoSemaphore* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoSemaphore( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoMutex ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoMutex* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoMutex( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoTimer ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoTimer* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoTimer( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );

private: // Internal methods
    static void GetTimerState( TMemSpyDriverTimerState aState, TDes& aBuf );
    static void GetTimerType( TMemSpyDriverTimerType aType, TDes& aBuf );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoLDD ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoLDD* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoLDD( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoPDD ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoPDD* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoPDD( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoLogicalChannel ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoLogicalChannel* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoLogicalChannel( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoChangeNotifier ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoChangeNotifier* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoChangeNotifier( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoUndertaker ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoUndertaker* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoUndertaker( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoOwnedThreadHandles ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoOwnedThreadHandles* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoOwnedThreadHandles( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoOwnedProcessHandles ) : public CMemSpyThreadInfoHandleByContainer
    {
public:
    static CMemSpyThreadInfoOwnedProcessHandles* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoOwnedProcessHandles( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };


















NONSHARABLE_CLASS( CMemSpyThreadInfoOtherThreads ) : public CMemSpyThreadInfoReferencedBy
    {
public:
    static CMemSpyThreadInfoOtherThreads* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoOtherThreads( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };



NONSHARABLE_CLASS( CMemSpyThreadInfoOtherProcesses ) : public CMemSpyThreadInfoReferencedBy
    {
public:
    static CMemSpyThreadInfoOtherProcesses* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

private:
    CMemSpyThreadInfoOtherProcesses( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;

private: // From CMemSpyThreadInfoHandleByContainer
    void HandleContainerItemL( TMemSpyDriverHandleInfoGeneric& aItem, TInt aRefCount, TDes& aFullName );
    };



















NONSHARABLE_CLASS( CMemSpyThreadInfoMemoryTrackingStatisticsCurrent ) : public CMemSpyThreadInfoItemBase, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    static CMemSpyThreadInfoMemoryTrackingStatisticsCurrent* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoMemoryTrackingStatisticsCurrent();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoMemoryTrackingStatisticsCurrent( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    TPtrC Name() const;
    
public: // API
    void SetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory );
    inline CMemSpyEngineProcessMemoryTracker& Tracker() { return *iTracker; }

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL();
    void HandleMemoryTrackingStoppedL();
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

private: // Member data
    TBool iTotalIncludesSharedMemory;
    CMemSpyEngineProcessMemoryTracker* iTracker;
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoMemoryTrackingStatisticsPeak ) : public CMemSpyThreadInfoItemBase, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    static CMemSpyThreadInfoMemoryTrackingStatisticsPeak* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoMemoryTrackingStatisticsPeak();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoMemoryTrackingStatisticsPeak( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    TPtrC Name() const;
    
public: // API
    void SetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory );
    inline CMemSpyEngineProcessMemoryTracker& Tracker() { return *iTracker; }

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL();
    void HandleMemoryTrackingStoppedL();
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

private: // Member data
    TBool iTotalIncludesSharedMemory;
    CMemSpyEngineProcessMemoryTracker* iTracker;
    };




NONSHARABLE_CLASS( CMemSpyThreadInfoMemoryTrackingStatisticsHWM ) : public CMemSpyThreadInfoItemBase, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    static CMemSpyThreadInfoMemoryTrackingStatisticsHWM* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoMemoryTrackingStatisticsHWM();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoMemoryTrackingStatisticsHWM( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    TPtrC Name() const;
    
public: // API
    void SetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory );
    inline CMemSpyEngineProcessMemoryTracker& Tracker() { return *iTracker; }

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL();
    void HandleMemoryTrackingStoppedL();
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

private: // Member data
    TBool iTotalIncludesSharedMemory;
    CMemSpyEngineProcessMemoryTracker* iTracker;
    };





NONSHARABLE_CLASS( CMemSpyThreadInfoMemoryTracking ) : public CMemSpyThreadInfoItemBase, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    static CMemSpyThreadInfoMemoryTracking* NewLC( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    ~CMemSpyThreadInfoMemoryTracking();

private: // From CMemSpyThreadInfoItemBase
    CMemSpyThreadInfoMemoryTracking( CMemSpyThreadInfoContainer& aContainer, TBool aAsyncConstruction );
    void ConstructL();

public: // From CMemSpyThreadInfoItemBase
    IMPORT_C TPtrC Name() const;
    
public: // API
    IMPORT_C TBool TrackingActive() const;
    IMPORT_C TBool TotalIncludesSharedMemory() const;
    IMPORT_C void TrackingSetTotalIncludesSharedMemoryL( TBool aIncludesSharedMemory );
    IMPORT_C void TrackingStartL();
    IMPORT_C void TrackingStopL();
    IMPORT_C void TrackingResetHWML();
    IMPORT_C void TrackingObserverAddL( MMemSpyEngineProcessMemoryTrackerObserver& aObserver );
    IMPORT_C void TrackingObserverRemove( MMemSpyEngineProcessMemoryTrackerObserver& aObserver );
    IMPORT_C MDesCArray& InfoCurrent();
    IMPORT_C MDesCArray& InfoHWM();
    IMPORT_C MDesCArray& InfoPeak();
    inline TBool TrackerExists() const { return iTracker != NULL; }

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL();
    void HandleMemoryTrackingStoppedL();
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

private: // Internal methods
    inline CMemSpyEngineProcessMemoryTracker& Tracker() { return *iTracker; }
    void UpdateCaptionsL();
    void UpdateCaptionsL( const TMemSpyDriverProcessInspectionInfo& aInfoCurrent, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

private: // Member data
    TBool iTotalIncludesSharedMemory;
    CMemSpyEngineProcessMemoryTracker* iTracker;
    CMemSpyThreadInfoMemoryTrackingStatisticsCurrent* iInfoCurrent;
    CMemSpyThreadInfoMemoryTrackingStatisticsHWM* iInfoHWM;
    CMemSpyThreadInfoMemoryTrackingStatisticsPeak* iInfoPeak;
    };




#endif
