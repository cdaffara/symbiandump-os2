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

#ifndef MEMSPYENGINEOBJECTTHREAD_H
#define MEMSPYENGINEOBJECTTHREAD_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyProcess;
class CMemSpyThreadInfoContainer;


NONSHARABLE_CLASS( CMemSpyThread ) : public CMemSpyEngineObject
    {
public:
    static CMemSpyThread* NewL( TThreadId aId, CMemSpyProcess& aProcess );
    static CMemSpyThread* NewLC( TThreadId aId, CMemSpyProcess& aProcess );

private:
    ~CMemSpyThread();

public: // From CMemSpyEngineObject
    IMPORT_C void Open();
    IMPORT_C void Close();

private:
    CMemSpyThread( TThreadId aId, CMemSpyProcess& aProcess );
    void ConstructL();

public: // API - query
    IMPORT_C TPtrC Name() const;
    IMPORT_C TFullName FullName() const;
    IMPORT_C CMemSpyThreadInfoContainer& InfoContainerL();
    IMPORT_C CMemSpyThreadInfoContainer& InfoContainerForceSyncronousConstructionL();
    IMPORT_C TBool IsSystemPermanent() const;
    IMPORT_C TBool IsSystemCritical() const;
    IMPORT_C TBool IsDead() const;
    IMPORT_C TThreadPriority Priority() const;

public: // API - misc
    IMPORT_C void KillL();
    IMPORT_C void TerminateL();
    IMPORT_C void PanicL();
    IMPORT_C void SetPriorityL( TThreadPriority aPriority );

public: // API - access
    inline CMemSpyProcess& Process() { return *iProcess; }
    inline const CMemSpyProcess& Process() const { return *iProcess; }
    inline TThreadId Id() const { return iId; }
    inline const TDesC& NameForListBox() const { return *iName; }
    inline TBool InfoContainerReady() const { return iInfoContainer != NULL; }

public: // API - but not exported
    CMemSpyEngine& Engine() const;
    void OpenLC( RThread& aThread );
    TInt Open( RThread& aThread );
    void RefreshL();
    void RefreshL( const RThread& aThread );
    void SetDeadL();
    void SetDeadL( const RThread& aThread );
    void FullName( TDes& aName ) const;

public: // API - utility functions - used by process also
    static void AppendPriority( TDes& aDes, TThreadPriority aPriority );
    static void AppendExitType( TDes& aDes, TExitType aType );
    static void AppendExitInfo( TDes& aDes, TExitType aType, TInt aExitReason, const TDesC& aExitCategory );

private: // Internal methods
    HBufC* GetThreadNameLC( const RThread& aThreadOrNull ) const;

private: // Data members
    TThreadId iId;
    CMemSpyProcess* iProcess;
    HBufC* iName;
    CMemSpyThreadInfoContainer* iInfoContainer;

    // RThread attributes
    TThreadPriority iPriority;
    TExitCategoryName iExitCategory;
    TExitType iExitType;
    TInt iExitReason;
    TUint32 iFlags;
    };




#endif
