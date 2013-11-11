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

#ifndef MEMSPYENGINEHELPERSTACK_H
#define MEMSPYENGINEHELPERSTACK_H

// System includes
#include <e32base.h>

// Driver includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;
class CMemSpyProcess;



NONSHARABLE_CLASS( CMemSpyEngineHelperStack ) : public CBase
    {
public:
    static CMemSpyEngineHelperStack* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperStack();

private:
    CMemSpyEngineHelperStack( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void OutputStackInfoL( const CMemSpyThread& aThread );
    IMPORT_C void OutputStackInfoL( TProcessId aPid, TThreadId aTid, TDes& aLineBuffer );
    //
    IMPORT_C void OutputStackDataL( const CMemSpyThread& aThread, TMemSpyDriverDomainType aType );
    IMPORT_C void OutputStackDataL( const CMemSpyThread& aThread, TMemSpyDriverDomainType aType, TBool aEntireStack );
    //
    IMPORT_C void OutputStackInfoForDeviceL();
    IMPORT_C TInt CalculateStackSizes( const CMemSpyProcess& aProcess );

private:
    TInt CalculateStackSizesL( const CMemSpyProcess& aProcess );

private:
    CMemSpyEngine& iEngine;
    };




#endif