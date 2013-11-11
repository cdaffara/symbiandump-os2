/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __T_FatCharsetConvServer_H__
#define __T_FatCharsetConvServer_H__

//  Include Files

#include <test/testexecuteserverbase.h>

class CT_FatCharsetConvServer : public CTestServer
    {
public:
    static CT_FatCharsetConvServer* NewL();
//from  CTestServer
    virtual CTestStep* CreateTestStep(const TDesC& aStepName);
    };

#endif  // __T_FatCharsetConvServer_H__

