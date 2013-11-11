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

#ifndef MEMSPYENGINEHELPERACTIVEOBJECT_H
#define MEMSPYENGINEHELPERACTIVEOBJECT_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;





NONSHARABLE_CLASS( CMemSpyEngineActiveObject ) : public CDesCArrayFlat
    {
public:
	enum TRequestStatusFlags
		{
		EActive				= 1,  //bit0
		ERequestPending		= 2,  //bit1
		};

public: // Construct / destruct
    static CMemSpyEngineActiveObject* NewLC( TAny* aAddress, TAny* aVTable, TInt aPriority, TInt aRSValue, TUint aRSFlags, TAny* aNextAOAddress, TAny* aPrevAOAddress, CMemSpyEngine& aEngine );
    IMPORT_C ~CMemSpyEngineActiveObject();
    
private: // Internal construct
    CMemSpyEngineActiveObject( TAny* aAddress, TAny* aVTable, TInt aPriority, TInt aRSValue, TUint aRSFlags, TAny* aNextAOAddress, TAny* aPrevAOAddress );
    void ConstructL( CMemSpyEngine& aEngine );
    
public:
    inline TAny* Address() const { return iAddress; }
    inline TAny* VTable() const { return iVTable; }
    inline TAny* NextAOAddress() const { return iNextAOAddress; }
    inline TAny* PrevAOAddress() const { return iPrevAOAddress; }
    inline TInt RequestStatusValue() const { return iRequestStatusValue; }
    inline TUint RequestStatusFlags() const { return iRequestStatusFlags; }
    inline const TDesC& Caption() const { return *iCaption; }
    inline TInt Priority() const { return iPriority; }
    IMPORT_C TBool IsActive() const;
    IMPORT_C TBool IsAddedToScheduler() const;
    IMPORT_C TBool RequestIsPending() const;
    IMPORT_C void OutputDataL( CMemSpyEngine& aEngine ) const;

public: // But not exported
    void SetPriority( TInt aPriority );
    void SetVTable( TAny* aVTable );
    void SetRequestStatusValues( TInt aStatus, TUint aFlags );

private: // Internal methods
    static TPtrC YesNoValue( TBool aValue );

private: // Data members
    TAny* iAddress;
    TAny* iVTable;
    TInt iPriority;
    TInt iRequestStatusValue;
    TUint iRequestStatusFlags;
    TAny* iNextAOAddress;
    TAny* iPrevAOAddress;
    //
    HBufC* iCaption;
    };



// CActive

NONSHARABLE_CLASS( CMemSpyEngineActiveObjectArray ) : public CBase, public MDesCArray
    {
public: // Construct/destruct
    static CMemSpyEngineActiveObjectArray* NewLC();
    IMPORT_C ~CMemSpyEngineActiveObjectArray();

private: // Internal construction
    CMemSpyEngineActiveObjectArray();
    void ConstructL();

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C CMemSpyEngineActiveObject& At( TInt aIndex );
    IMPORT_C const CMemSpyEngineActiveObject& At( TInt aIndex ) const;
    IMPORT_C CMemSpyEngineActiveObject& ObjectByAddressL( TAny* aAddress );
    IMPORT_C TInt ObjectIndexByAddress( TAny* aAddress ) const;
    IMPORT_C static void OutputDataColumnsL( CMemSpyEngine& aEngine );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

public: // But not exported
    void AddItemL( CMemSpyEngineActiveObject* aItem );
    void InsertL( CMemSpyEngineActiveObject* aItem, TInt aIndex );

private: // Internal methods
    void BuildHeaderCaptionL();

private: // Data members
    HBufC* iHeader;
    RPointerArray< CMemSpyEngineActiveObject > iObjects;
    };





NONSHARABLE_CLASS( CMemSpyEngineHelperActiveObject ) : public CBase
    {
public:
    static CMemSpyEngineHelperActiveObject* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperActiveObject();

private:
    CMemSpyEngineHelperActiveObject( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C CMemSpyEngineActiveObjectArray* ActiveObjectListL( const CMemSpyThread& aThread );

private: // Internal methods
    HBufC8* SchedulerHeapCellDataLC( TAny*& aCellAddress, TThreadId aThreadId );
    void ExtractActiveObjectAddressesL( TAny* aSchedulerCellAddress, const TDesC8& aSchedulerCellData, CMemSpyEngineActiveObjectArray& aArray );
    TAny* ReadActiveObjectDataL( TAny* aCellAddress, CMemSpyEngineActiveObjectArray& aArray );
    static void CleanupHeapWalk( TAny* aSelf );
    TAny* ConvertAddressToRealHeapCellAddressL( TAny* aAddress );

private:
    CMemSpyEngine& iEngine;
    TMemSpyHeapInfo iHeapInfo;
    };




#endif