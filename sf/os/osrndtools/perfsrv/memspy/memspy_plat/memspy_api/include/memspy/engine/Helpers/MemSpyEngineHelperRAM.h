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

#ifndef MEMSPYENGINEHELPERRAM_H
#define MEMSPYENGINEHELPERRAM_H

// System includes
#include <e32base.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineOutputList;


NONSHARABLE_CLASS( CMemSpyEngineHelperRAM ) : public CBase
    {
public:
    static CMemSpyEngineHelperRAM* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperRAM();

private:
    CMemSpyEngineHelperRAM( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C TBool IsAknIconCacheConfigurable() const;

    /** Returns RAM change value */
    IMPORT_C TInt64 SetAknIconCacheStatusL( TBool aEnabled );

private: // Internal methods
    void CheckIfAknIconCacheCanBeConfiguredL();
    TInt SetAknIconStatus( TBool aEnabled );

private:
    CMemSpyEngine& iEngine;
    TBool iIsAknIconCacheConfigurable;
    };




#endif