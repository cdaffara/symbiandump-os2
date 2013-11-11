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
#if (!defined __REPOSITORYFN_H__)
#define __REPOSITORYFN_H__
#include <test/testexecutestepbase.h>
#include "Te_centrep_SWISuiteStepBase.h"
#include "createcheckcommon.h"

class CRepositoryFnStep : public CTe_centrep_SWISuiteStepBase
    {
public:
    CRepositoryFnStep();
    ~CRepositoryFnStep();
    virtual TVerdict doTestStepL();
    void ModifyRepositoryIntegerSettings(CRepository *, TInt );
    void ModifyRepositoryRealSettings(CRepository *, TInt );
    void ModifyRepositoryStringSettings(CRepository *, TInt );
    int ErrorToNum(TDesC & );
    int FuncToNum(TDesC & );
private:
    enum ReposFns { fn_set, fn_create, fn_delete, fn_reset, unknown_func };
    };

_LIT(KRepositoryFn,   "RepositoryFn"  );

// This lot for turning strings in config files into numbers in 'ErrorToNum'
_LIT(RFN_KErrNone,                "KErrNone"  );
_LIT(RFN_KErrNotFound,            "KErrNotFound"  );
_LIT(RFN_KErrNotSupported,        "KErrNotSupported"  );
_LIT(RFN_KErrAbort,               "KErrAbort"  );
_LIT(RFN_KErrLocked,              "KErrLocked"  );
_LIT(RFN_KErrOverflow,            "KErrOverflow"  );
_LIT(RFN_KErrPermissionDenied,     "KErrPermissionDenied"  );
_LIT(RFN_KErrAlreadyExists,       "KErrAlreadyExists"  );
_LIT(RFN_KErrArgument,            "KErrArgument"  );
_LIT(RFN_KMaxBinaryLength,        "KMaxBinaryLength"  );
_LIT(RFN_KMaxUnicodeStringLength, "KMaxUnicodeStringLength"  );

// Functions that can be performed on Repository settings.
_LIT(RFN_Set,    "set");
_LIT(RFN_Create, "create");
_LIT(RFN_Delete, "delete");
_LIT(RFN_Reset,  "reset");
#endif // __REPOSITORYFN_H__
