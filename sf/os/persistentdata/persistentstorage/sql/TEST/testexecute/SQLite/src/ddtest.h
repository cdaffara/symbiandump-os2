// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if (!defined __DDTEST_H__)
#define __DDTEST_H__
#include <test/testexecutestepbase.h>
#include "Te_SQL_SuiteStepBase.h"
#include "sqlfn.h"

class CSQLDDT : public CSQLFnStep
    {
public:
    void ResolveTestFunctionL(const TDesC&, const TInt, const TPtrC &);
    CSQLDDT();
    ~CSQLDDT();
    };

#endif // DDTEST_H__
