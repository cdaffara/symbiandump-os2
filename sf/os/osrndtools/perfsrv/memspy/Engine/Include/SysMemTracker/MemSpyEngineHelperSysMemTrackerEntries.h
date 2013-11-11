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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRIES_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRIES_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyenginehelpersysmemtrackerenums.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyThread;
class CMemSpyProcess;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;

// Literal constants
_LIT( KMemSpySWMTThreadNotFound, "Thread Not Found" );


NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerEntry ) : public CBase
    {
protected:
    CMemSpyEngineHelperSysMemTrackerEntry( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TMemSpyEngineSysMemTrackerType aType );

public: // API
    void HandleNewCycleL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

public: // API - framework 
    virtual TUint64 Key() const;
    virtual void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );

protected: // API - framework, internal
    virtual TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;
    virtual void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    virtual void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

public: // API - inline
    inline TMemSpyEngineSysMemTrackerType Type() const { return iType; }
    inline TUint8 Attributes() const { return iAttributes; }
    //
    inline TThreadId ThreadId() const { return iThreadId; }
    inline void SetThread( TThreadId aThreadId ) { iThreadId = aThreadId; }
    //
    inline TProcessId ProcessId() const { return iProcessId; }
    inline void SetProcess( TProcessId aProcessId ) { iProcessId = aProcessId; }
    //
    inline TUint32 Handle() const { return iHandle; }
    inline void SetHandle( TAny* aHandle ) { iHandle = (TUint32) aHandle; }
    inline void SetHandle( TUint32 aHandle ) { iHandle = aHandle; }
    //
    inline const TTime& LastUpdateTime() const { return iLastUpdateTime; }
    
public: // Attributes
    inline TBool IsNew() const { return iAttributes & EMemSpyEngineSysMemTrackerEntryAttributeIsNew; }
    inline void SetNew( TBool aNew );
    //
    inline TBool IsDead() const { return !IsAlive(); }
    inline void SetDead() { iAttributes &= ~EMemSpyEngineSysMemTrackerEntryAttributeIsAlive; }
    //
    inline TBool IsAlive() const { return iAttributes & EMemSpyEngineSysMemTrackerEntryAttributeIsAlive; }
    inline void SetAlive();

protected: // Internal methods
    CMemSpyEngine& Engine();
    void UpdateTime();

private: // Data members
    CMemSpyEngineHelperSysMemTrackerImp& iTracker;
    const TMemSpyEngineSysMemTrackerType iType;
    TUint32 iThreadId;
    TUint32 iProcessId;
    TUint8 iAttributes;
    TUint32 iHandle;
    TTime iLastUpdateTime;
    };





// Helper class used when searching for entries
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerEntryWithSuppliedKey ) : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    inline CMemSpyEngineHelperSysMemTrackerEntryWithSuppliedKey( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TUint64& aKey )
    : CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeCount ), iKey( aKey )
        { }

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const { return iKey; }

private: // Data members
    const TUint64 iKey;
    };





inline void CMemSpyEngineHelperSysMemTrackerEntry::SetNew( TBool aNew )
    {
    if  ( aNew )
        {
        iAttributes |= EMemSpyEngineSysMemTrackerEntryAttributeIsNew;
        }
    else
        {
        iAttributes &= ~EMemSpyEngineSysMemTrackerEntryAttributeIsNew;
        }
    }


inline void CMemSpyEngineHelperSysMemTrackerEntry::SetAlive()
    {
    iAttributes |= EMemSpyEngineSysMemTrackerEntryAttributeIsAlive;
    UpdateTime();
    }


#endif