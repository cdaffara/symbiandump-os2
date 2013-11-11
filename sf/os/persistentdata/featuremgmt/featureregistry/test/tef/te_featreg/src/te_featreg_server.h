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
// Feature Registry TEF test server.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TE_FEATREG_SERVER_H
#define TE_FEATREG_SERVER_H

#include <test/testexecuteserverbase.h>

class CTe_FeatReg_Server : public CTestServer
	{
public:
	static CTe_FeatReg_Server* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	};

#endif
