// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Feature Registry test step base class - utilities for derived test steps.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TE_FEATREG_STEPBASE_H
#define TE_FEATREG_STEPBASE_H

#include <test/testexecutestepbase.h>

class CTe_FeatReg_StepBase : public CTestStep
	{
public:
	CTe_FeatReg_StepBase(){}
	TInt RunFeatRegSetup();
	TInt RunMakeTestConfig(const TDesC& aConfigName);
	};

#endif
