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

#ifndef MEMSPYENGINEHELPERCONDVAR_H
#define MEMSPYENGINEHELPERCONDVAR_H

// System includes
#include <e32base.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;



NONSHARABLE_CLASS( CMemSpyEngineHelperCondVar ) : public CBase
    {
public:
    static CMemSpyEngineHelperCondVar* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperCondVar();

private:
    CMemSpyEngineHelperCondVar( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void GetCondVarSuspendedThreadsL( const TMemSpyDriverHandleInfoGeneric& aCondVarDetails, RArray<TMemSpyDriverCondVarSuspendedThreadInfo>& aThreads );

private:
    CMemSpyEngine& iEngine;
    };




#endif
