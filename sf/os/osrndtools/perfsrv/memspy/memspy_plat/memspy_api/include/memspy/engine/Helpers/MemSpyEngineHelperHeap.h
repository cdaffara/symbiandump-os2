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

#ifndef MEMSPYENGINEHELPERHEAP_H
#define MEMSPYENGINEHELPERHEAP_H

// System includes
#include <e32base.h>
#include <s32strm.h>
#include <bamdesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// MemSpyClient includes
#include <memspy/engine/memspyheapdata.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;
class CMemSpyProcess;
class CMemSpyEngineOutputList;


NONSHARABLE_CLASS( CMemSpyEngineHelperHeap ) : public CBase
    {
public:
    static CMemSpyEngineHelperHeap* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperHeap();

private:
    CMemSpyEngineHelperHeap( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API - User Heap 
    IMPORT_C void GetHeapInfoUserL( const TProcessId& aProcess, const TThreadId& aThread, TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverCell>* aCells = NULL );
	IMPORT_C void GetHeapInfoUserL(const TProcessId& aProcess, const TThreadId& aThread, TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverCell>* aCells, TBool aCollectAllocatedCellsAsWellAsFree);
    IMPORT_C void GetHeapInfoUserL( const CMemSpyProcess& aProcess, RArray<TMemSpyHeapInfo >& aInfos );
    IMPORT_C void OutputHeapInfoUserL( const CMemSpyThread& aThread );
    IMPORT_C void OutputHeapDataUserL( const CMemSpyThread& aThread );
    IMPORT_C void OutputHeapDataUserL( const TProcessId& aPid, const TThreadId& aTid, const TDesC& aThreadName, const TMemSpyHeapInfo& aInfo, const RArray<TMemSpyDriverFreeCell>* aFreeCells = NULL );
    IMPORT_C void OutputCellListingUserL( const CMemSpyThread& aThread );

public: // API - Kernel Heap
    IMPORT_C void GetHeapInfoKernelL( TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverFreeCell>* aFreeCells, TBool aCollectAllocatedCellsAsWellAsFree );
    IMPORT_C void GetHeapInfoKernelL( TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverFreeCell>* aFreeCells = NULL );
    IMPORT_C void OutputHeapInfoKernelL();
    IMPORT_C void OutputHeapDataKernelL();

public: // API - Common
    IMPORT_C void OutputHeapInfoL( const TMemSpyHeapInfo& aInfo, const TDesC& aThreadName, const RArray<TMemSpyDriverCell>* aCells = NULL );
    IMPORT_C void OutputHeapInfoForDeviceL( TBool aIncludeKernel = ETrue );
    IMPORT_C CMemSpyEngineOutputList* NewHeapSummaryShortLC( const TMemSpyHeapInfo& aInfo );
    IMPORT_C CMemSpyEngineOutputList* NewHeapSummaryExtendedLC( const TMemSpyHeapInfo& aInfo, const RArray<TMemSpyDriverCell>* aCells = NULL );
    //cigasto    
    IMPORT_C TMemSpyHeapData NewHeapRawInfo( const TMemSpyHeapInfo& aInfo ); //not formatted heap info

public: // But not exported
    void GetHeapInfoKernelL( TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverFreeCell>* aFreeCells, TBool aCollectAllocatedCellsAsWellAsFree, TBool aUseKernelCopy = EFalse );
    void OutputHeapDataUserL( const CMemSpyThread& aThread, TBool aCreateDataStream );
    void OutputHeapDataUserL( const TProcessId& aPid, const TThreadId& aTid, const TDesC& aThreadName, const TMemSpyHeapInfo& aInfo, TBool aCreateDataStream, const RArray<TMemSpyDriverFreeCell>* aFreeCells );
    void OutputHeapDataKernelL( TBool aCreateDataStream );
    void OutputHeapDataKernelL(const TMemSpyHeapInfo& aHeapInfo, TBool aCreateDataStream, const RArray<TMemSpyDriverCell>* aCells);

private:
    static TUint DescriptorAsDWORD( const TDesC8& aItem );
    void AppendMetaDataL( const TMemSpyHeapInfo& aInfo, CMemSpyEngineOutputList& aList );
    void AppendStatisticsL( const TMemSpyHeapInfo& aInfo, CMemSpyEngineOutputList& aList );
	void AppendCellsL(const RArray<TMemSpyDriverCell>& aCells, CMemSpyEngineOutputList& aList);
    void OutputCSVEntryL( TInt aIndex, const TMemSpyHeapInfo& aInfo, const TDesC& aThreadName, const TDesC& aProcessName );
    void UpdateSharedHeapInfoL( const TProcessId& aProcess, const TThreadId& aThread, TMemSpyHeapInfo& aInfo );
    static void CleanupHeapDataKernel( TAny* aPtr );
    
private:
    CMemSpyEngine& iEngine;
    };




#endif
