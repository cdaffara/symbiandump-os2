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

#ifndef MEMSPYENGINEOBJECTPROCESS_H
#define MEMSPYENGINEOBJECTPROCESS_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;
class TMemSpyDriverProcessInfo;


NONSHARABLE_CLASS( CMemSpyProcess ) : public CMemSpyEngineObject, public MDesCArray
    {
public:
    IMPORT_C static CMemSpyProcess* NewL( const CMemSpyProcess& aCopyMe );
    static CMemSpyProcess* NewL( TProcessId aId, CMemSpyEngine& aEngine );
    static CMemSpyProcess* NewLC( TProcessId aId, CMemSpyEngine& aEngine );

private:
    ~CMemSpyProcess();

public: // From CMemSpyEngineObject
    IMPORT_C void Open();
    IMPORT_C void Close();

private:
    CMemSpyProcess( TProcessId aId, CMemSpyEngine& aEngine );
    void ConstructL();

public: // API - access
    inline CMemSpyEngine& Engine() const { return iEngine; }
    inline const TDesC& NameForListBox() const { return *iName; }
    inline TProcessId Id() const { return iId; }
    inline TInt Count() const { return iThreads.Count(); }
    
public: // API - query
    IMPORT_C TPtrC Name() const;
    IMPORT_C CMemSpyThread& At( TInt aIndex ) const;
    IMPORT_C TInt ThreadIndexById( TThreadId aId ) const;
    IMPORT_C CMemSpyThread& ThreadByIdL( TThreadId aId ) const;
    IMPORT_C TBool IsSystemPermanent() const;
    IMPORT_C TBool IsSystemCritical() const;
    IMPORT_C TBool IsDead() const;
    IMPORT_C TUint32 SID() const;
    IMPORT_C TUint32 VID() const;
    IMPORT_C TProcessPriority Priority() const;
    IMPORT_C TExitCategoryName ExitCategory() const;
    IMPORT_C TInt ExitReason() const;
    IMPORT_C TExitType ExitType() const;
    
public: // API - misc
    IMPORT_C void KillL();
    IMPORT_C void TerminateL();
    IMPORT_C void PanicL();

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;

public: // New API functions:
    IMPORT_C TUidType UIDs() const;
    IMPORT_C SCapabilitySet Capabilities() const;

public: // API - but not exported
    void GetFileName( TFileName& aFileName );
    void RefreshL();
    void RefreshL( const RProcess& aProcess );
    void SetDeadL();
    void SetDeadL( const RProcess& aProcess );
    void FullName( TDes& aFullName ) const;

public: // API - utility functions
    static void AppendPriority( TDes& aDes, TProcessPriority aPriority );

public: // Called by process container
    void HandleThreadIsBornL( const TThreadId& aId );

private: // Internal methods
    void LocateThreadsL( RProcess& aProcess );
    void BuildFormattedNameL();
    void CloseAllThreads();
    HBufC* GetProcessNameLC( const RProcess& aProcessOrNull, TBool& aProcessNameIncludesExeSuffix ) const;

private: // Internal enumerations
    enum TFlags
        {
        EFlagsNone = 0,
        EFlagsIncludedExecutableWithinName = 1
        };

private: // Data members
    const TProcessId iId;
    CMemSpyEngine& iEngine;
    TProcessPriority iPriority;
    TExitCategoryName iExitCategory;
    TInt iExitReason;
    TExitType iExitType;
    HBufC* iName;
    RArray< CMemSpyThread* > iThreads;
    TMemSpyDriverProcessInfo* iInfo;
    TUint32 iFlags;
    };


#endif
