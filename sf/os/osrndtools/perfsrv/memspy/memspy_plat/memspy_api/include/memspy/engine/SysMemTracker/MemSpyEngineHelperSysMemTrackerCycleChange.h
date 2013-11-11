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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERCYCLECHANGE_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERCYCLECHANGE_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyenginehelpersysmemtrackerenums.h> 

// Classes referenced
class CMemSpyEngineOutputSink;
class CMemSpyEngineOutputList;
class CMemSpyEngineHelperSysMemTrackerCycle;
typedef TBuf<6> TMemSpySWMTTypeName;

/**
 * A change descriptor object represents precisely one system-wide change that was detected
 * during any given cycle.
 *
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChange ) : public CBase
    {
public:
    ~CMemSpyEngineHelperSysMemTrackerCycleChange();

protected:
    CMemSpyEngineHelperSysMemTrackerCycleChange( TUint8 aAttributes );
    void BaseConstructL();

public: // API - framework
    virtual TMemSpyEngineSysMemTrackerType Type() const = 0;
    virtual void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle ) = 0;
    virtual void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle ) = 0;
    virtual void OutputDataL( CMemSpyEngineOutputSink& /*aSink*/, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ ) { };

protected: // Internal methods
    void FormatType( TMemSpySWMTTypeName& aBuffer ) const;
    void FormatAttributes( TDes& aBuffer ) const;

private: // Data members
    const TUint8 iAttributes;
    };


#endif