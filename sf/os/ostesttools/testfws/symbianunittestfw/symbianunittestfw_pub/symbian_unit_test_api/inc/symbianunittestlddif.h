// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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

#ifndef __SYMBIANUNITTESTLDDIF_H
#define __SYMBIANUNITTESTLDDIF_H

#include <e32cmn.h>

class RSymbianUnitTest : public RBusLogicalChannel
{
public:
	enum TControl
         {
         ETESTCOUNT=1,
         EEXECUTETESTCASE=2,
         ETESTCASENAME=3
         };
	
	TInt GetTestCaseCount(TInt* a1);
	TInt ExecTestCase(TInt* a1, TAny* a2);
	TInt GetTestCaseName(TInt* a1, TAny* a2);
	TInt Open(const TDesC& aName);
};

typedef TBuf8<80> TTestName;
typedef TBuf8<256> TFAILUREMESSAGE;
typedef TBuf8<256> TFILENAME;

class TSUTTestCaseResult
    {
    public: TSUTTestCaseResult() : iRetCode (0), iLineNumber (0)
        {
        iTestName.Zero();
        iFailureMessage.Zero();
        iFileName.Zero();
        }
    public:
        TInt iRetCode;
        TInt iLineNumber;
        TTestName iTestName;
        TFAILUREMESSAGE iFailureMessage;
        TFILENAME iFileName;
    };


#endif __SYMBIANUNITTESTLDDIF_H
