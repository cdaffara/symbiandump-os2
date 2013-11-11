/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
*/
#if (!defined __REPOSCHECK_STEP_H__)
#define __REPOSCHECK_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_centrep_SWISuiteStepBase.h"
#include "createcheckcommon.h"

class CRepositoryCheckStep : public CTe_centrep_SWISuiteStepBase
    {
public:
    CRepositoryCheckStep();
    ~CRepositoryCheckStep();
    virtual TVerdict doTestStepL();
    // probably shouldn't be void..
    void CheckRepositoryIntegerSettings(CRepository *, TInt);
    void CheckRepositoryRealSettings(CRepository *, TInt);
    void CheckRepositoryStringSettings(CRepository *, TInt);

private:
    static const int ipauseincrement=950000;
    // This exists in srvdefs.h in the centrep code but we can't reach that.
    static const int KDefaultEvictionTimeout=120000000; //505000;
    };

_LIT(KRepositoryCheck,     "RepositoryCheck"  );
_LIT(KRepositoryAbsent,    "NumAbsentSettings");
_LIT(KAbsentPrefix,        "Absent" );
_LIT(KRepositoryPresent,   "RepositoryPresent");
_LIT(KExpectedNumSettings, "ExpectedNumSettings");
_LIT(KPause,               "Pause");

#endif // __REPOSCHECK_STEP_H__
