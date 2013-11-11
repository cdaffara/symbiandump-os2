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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYWINDOWSERVER_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYWINDOWSERVER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include "MemSpyEngineHelperSysMemTrackerEntries.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>
#include <memspy/engine/memspyenginehelperwindowserver.h>

// Classes referenced
class CMemSpyEngineOutputSink;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;
class TMemSpyEngineWindowGroupDetails;



class CMemSpyEngineHelperSysMemTrackerEntryWindowServer : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryWindowServer* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyEngineWindowGroupDetails& aWindowGroupDetails );
    ~CMemSpyEngineHelperSysMemTrackerEntryWindowServer();

private:
    CMemSpyEngineHelperSysMemTrackerEntryWindowServer( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyEngineWindowGroupDetails& aWindowGroupDetails );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );

private: // Internal methods

private: // Data members
    TMemSpyEngineWindowGroupDetails iCurrentWindowGroupDetails;
    TMemSpyEngineWindowGroupDetails iLastWindowGroupDetails;
    };







/**
 * Change descriptor associated with window group changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    enum TMemSpyWindowServerEvent
        {
        EMemSpyWindowServerEventNoEvent,
        EMemSpyWindowServerEventNameChanged,
        EMemSpyWindowServerEventFocusGained,
        EMemSpyWindowServerEventFocusLost
        };
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* NewLC( TUint8 aAttribs, const TMemSpyEngineWindowGroupDetails aCurrent, TMemSpyWindowServerEvent aEvent = EMemSpyWindowServerEventNoEvent );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup( TUint8 aAttrib, const TMemSpyEngineWindowGroupDetails aCurrent, TMemSpyWindowServerEvent aEvent );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
   
private: // Data members
    TMemSpyEngineWindowGroupDetails iCurrentWindowGroupDetails;
    TMemSpyWindowServerEvent iEvent;
    };



#endif
