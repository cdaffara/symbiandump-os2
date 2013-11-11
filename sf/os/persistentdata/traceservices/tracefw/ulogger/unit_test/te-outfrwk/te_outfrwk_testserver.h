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
//

#ifndef T_OUTFRWK_SERVER_H
#define T_OUTFRWK_SERVER_H

#include <test/testexecuteserverbase.h>

/*
 * Derived class of the CTestServer
 *
 */
class CT_OutfrwkServer : public CTestServer
	{

public:
	static CT_OutfrwkServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
	};

#endif
