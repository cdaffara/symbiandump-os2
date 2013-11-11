// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#if (!defined __TEF_EFM_BURSUITESERVER_H__)
#define __TEF_EFM_BURSUITESERVER_H__

#include <test/testexecuteserverbase.h>

/**
 * New test server for the BUR suite of tests
 */
class CTefEfmBURSuite : public CTestServer
	{
public:
	static CTefEfmBURSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	};

#endif
