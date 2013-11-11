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
// te_efm_suiteserver.h
// 
//

#if (!defined __TE_EFM_SERVER_H__)
#define __TE_EFM_SERVER_H__
#include <test/testexecuteserverbase.h>
#include "efmmain.h"


class CTe_EFM_Suite : public CTestServer
    {
public:
    static CTe_EFM_Suite* NewL();
    // Base class pure virtual override
    virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
    };

#endif //__TE_EFM_SERVER_H__
