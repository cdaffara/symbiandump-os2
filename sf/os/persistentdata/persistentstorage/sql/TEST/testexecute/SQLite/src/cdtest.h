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

#if (!defined __CDTEST_H__)
#define __CDTEST_H__
#include <test/testexecutestepbase.h>
#include "Te_SQL_SuiteStepBase.h"
#include "sqlfn.h"

class CSQLCDT : public CSQLFnStep
    {
public:
    CSQLCDT();
    ~CSQLCDT();
    void ResolveTestFunctionL(const TDesC&, const TInt, const TPtrC &);

    /*
     * Code driven tests (configurable).
     */
    void WriteBigTableL(const TDesC&, TInt);
    void ReadBigTableL(const TDesC&);

    void CopyCellsUsingStreamsL(const TDesC&);
    void WriteIntsToStream(const TDesC &, const TInt);
    void NearFillDisk(const TDesC &);
    void ScalarFullSelectL(const TDesC &, const TInt );
    void FilesDifferBySize(const TDesC &, const TInt );
    void SecurityPolicyCheck(const TDesC &, const TInt );
    void CollationTest(const TDesC &, const TInt );

private:
    void PrepareLargeDisk(const TDesC&, const TDriveUnit atdu, TInt64 &atowrite);
    };

#endif // CDTEST_H__
