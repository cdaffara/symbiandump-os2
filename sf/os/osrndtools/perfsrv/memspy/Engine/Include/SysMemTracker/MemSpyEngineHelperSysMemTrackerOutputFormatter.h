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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKEROUTPUTFORMATTER_H
#define MEMSPYENGINEHELPERSYSMEMTRACKEROUTPUTFORMATTER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>

// Classes referenced
class CMemSpyEngine;
class TMemSpyEngineHelperSysMemTrackerConfig;
class CMemSpyEngineHelperSysMemTrackerCycle;



NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerOutputFormatter ) : public CBase
    {
public:
    static CMemSpyEngineHelperSysMemTrackerOutputFormatter* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperSysMemTrackerOutputFormatter();

private:
    CMemSpyEngineHelperSysMemTrackerOutputFormatter( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    void SetConfig( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    void OutputL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    inline const TMemSpyEngineHelperSysMemTrackerConfig& Config() const { return iConfig; }

private: // Internal methods
    void OutputOverallHeaderL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputOverallFooterL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    static TBool HaveTypesChanged( TInt aLeft, TInt aRight );

private:
    CMemSpyEngine& iEngine;
    TMemSpyEngineHelperSysMemTrackerConfig iConfig;
    };



#endif