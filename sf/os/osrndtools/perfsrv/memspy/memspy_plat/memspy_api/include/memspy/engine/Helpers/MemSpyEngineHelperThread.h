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

#ifndef MEMSPYENGINEHELPERTHREAD_H
#define MEMSPYENGINEHELPERTHREAD_H

// System includes
#include <e32base.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;



NONSHARABLE_CLASS( CMemSpyEngineHelperThread ) : public CBase
    {
public:
    static CMemSpyEngineHelperThread* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperThread();

private:
    CMemSpyEngineHelperThread( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void OutputThreadInfoL( const CMemSpyThread& aThread, TBool aDetailed );

private:
    CMemSpyEngine& iEngine;
    };




#endif