// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// memoryleak component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __MEMORYLEAKTESTSTEP__
#define __MEMORYLEAKTESTSTEP__

#include <test/testexecutestepbase.h>
#include <featmgr/featurecontrol.h>
#include <featmgr/featurenotifier.h>
#include "efm_featurenotifierstepbase.h"


// This tests the ReNotify function (and thus monitors possible server memory leakage)
_LIT( KMemoryLeak_ReNotifyRequestL, "MemoryLeak_ReNotifyRequestL" );
class CMemoryLeak_ReNotifyRequestLStep : public CFeatureNotifierStepBase
	{
public:
    CMemoryLeak_ReNotifyRequestLStep();
    virtual TVerdict doTestStepL(void);
    };
   
#endif
