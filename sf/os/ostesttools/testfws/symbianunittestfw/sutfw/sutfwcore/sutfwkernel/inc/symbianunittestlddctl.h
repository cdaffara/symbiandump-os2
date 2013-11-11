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

#ifndef __SYMBIANUNITTESTLDDCTL_H
#define __SYMBIANUNITTESTLDDCTL_H

#include <e32base.h>
#include "symbianunittestlddif.h"
#include "symbianunittestresult.h"
#include "symbianunittestobserver.h"


_LIT(KKInstallLddName,"symbianunittest");


class CSymbianUnitTestLddCtl : public CBase
{
    public: // Constructors and destructor
    
        static CSymbianUnitTestLddCtl* NewLC(const TDesC& aDriverName);
        static CSymbianUnitTestLddCtl* NewL(const TDesC& aDriverName);
        ~CSymbianUnitTestLddCtl();
    
    public: // New functions
        TInt LoadDriver();
        TInt ExecuteLddTests(
                MSymbianUnitTestObserver& aObserver,
                CSymbianUnitTestResult& aResult);
        TInt FreeDriver();
        TInt GetTestCaseNames(CDesCArray& aTestCaseNames);
        
    private: // Constructors
        CSymbianUnitTestLddCtl();
        void ConstructL(const TDesC& aDriverName);
            
    private: // Data
        
            HBufC* iDriverName;
            RSymbianUnitTest iLddconn;
};

#endif __SYMBIANUNITTESTLDDCTL_H
