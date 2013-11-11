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

#ifndef MEMSPYENGINEHELPERROM_H
#define MEMSPYENGINEHELPERROM_H

// System includes
#include <e32base.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineOutputList;


NONSHARABLE_CLASS( CMemSpyEngineHelperROM ) : public CBase
    {
public:
    static CMemSpyEngineHelperROM* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperROM();

private:
    CMemSpyEngineHelperROM( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void AddInfoL( CMemSpyEngineOutputList& aList );

private:
    CMemSpyEngine& iEngine;
    HBufC* iPlatformVersion;
    HBufC* iChecksum;
    };




#endif