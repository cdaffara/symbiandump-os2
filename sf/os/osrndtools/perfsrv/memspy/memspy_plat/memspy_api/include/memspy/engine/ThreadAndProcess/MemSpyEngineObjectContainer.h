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

#ifndef MEMSPYENGINEOBJECTCONTAINER_H
#define MEMSPYENGINEOBJECTCONTAINER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyenginemidwife.h>
#include <memspy/engine/memspyengineundertaker.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyThread;
class CMemSpyProcess;


NONSHARABLE_CLASS( CMemSpyEngineObjectContainer ) : public CBase, public MDesCArray, public MMemSpyEngineUndertakerObserver, public MMemSpyEngineMidwifeObserver
    {
public:
    static CMemSpyEngineObjectContainer* NewL( CMemSpyEngine& aEngine );
    static CMemSpyEngineObjectContainer* NewL( const TDesC& aFilter, CMemSpyEngine& aEngine );
    ~CMemSpyEngineObjectContainer();

private:
    CMemSpyEngineObjectContainer( CMemSpyEngine& aEngine );
    void ConstructL( const TDesC& aFilter );

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C TInt CountAll() const;

public:
    IMPORT_C void RefreshL();
    IMPORT_C void RefreshL( const TDesC& aFilter );
    IMPORT_C TBool IsAlive( TProcessId aPid ) const;
    IMPORT_C TBool IsAlive( TProcessId aPid, TThreadId aTid ) const;
    IMPORT_C CMemSpyProcess& At( TInt aIndex ) const;
    IMPORT_C CMemSpyProcess& ProcessByIdL( TProcessId aId ) const;
    IMPORT_C TInt ProcessIndexById( TProcessId aId ) const;
    IMPORT_C TInt ProcessAndThreadByThreadId( TThreadId aTid, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread ) const;
    IMPORT_C TInt ProcessAndThreadByFullName( const TDesC& aFullName, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread ) const;
    IMPORT_C TInt ProcessAndThreadByPartialName( const TDesC& aPartialName, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread ) const;

public: // API - sorting
    IMPORT_C void SortById();
    IMPORT_C void SortByName();
    IMPORT_C void SortByThreadCount();
    IMPORT_C void SortByCodeSegs();
    IMPORT_C void SortByHeapUsage();
    IMPORT_C void SortByStackUsage();

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;

private: // From MMemSpyEngineUndertakerObserver
	void ThreadIsDeadL( const TThreadId& aId, const RThread& aThread );
	void ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess );

public: // From MMemSpyEngineMidwifeObserver
	void ThreadIsBornL( const TThreadId& aId, const RThread& aThread );
	void ProcessIsBornL( const TProcessId& aId, const RProcess& aProcess );

private: // Internal methods
    void Remove( CMemSpyProcess& aProcess );
    void AppendL( CMemSpyProcess* aProcess );
    void Resort();
    void LocateProcessesL( const TDesC& aFilter );
    void DestroyProcesses( RArray< CMemSpyProcess* >& aList );
    void TryToCreateProcessAndThreadL( const TThreadId& aTid, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread );
    void TryToCreateProcessAndThreadL( const RThread& aRThread, CMemSpyProcess*& aProcess, CMemSpyThread*& aThread );
    CMemSpyProcess* CreateProcessL( const TProcessId& aId );
    TBool MoveToGarbageL( const TProcessId& aId );

private: // Comparison functions
    static TInt CompareById( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight );
    static TInt CompareByName( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight );
    static TInt CompareByThreadCount( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight );
    static TInt CompareByCodeSegs( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight );
    static TInt CompareByHeapUsage( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight );
    static TInt CompareByStackUsage( CMemSpyProcess* const & aLeft, CMemSpyProcess* const & aRight );

private: // Idle callback related
    void AsyncNotifyUiOfContainerChanges();
    static TBool NotifyUiOfContainerChanges( TAny* aSelf );

private: // Enumerations
    enum TSortType
        {
        ESortById = 0,
        ESortByName,
        ESortByThreadCount,
        ESortByCodeSegs,
        ESortByHeapUsage,
        ESortByStackUsage
        };
        
private:
    CMemSpyEngine& iEngine;
    TSortType iSortType;
    CMemSpyEngineUndertaker* iUndertaker;
    CMemSpyEngineMidwife* iMidwife;
    CIdle* iIdleNotifyContainerChanged;
    RArray< CMemSpyProcess* > iProcesses;
    RArray< CMemSpyProcess* > iGarbage;
    };


#endif