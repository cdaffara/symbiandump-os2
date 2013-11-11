/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file IPSuiteServer.h
*/
// __EDIT_ME__ Create your own class definition based on this
#if (!defined __IP_SUITE_SERVER_H__)
#define __IP_SUITE_SERVER_H__
#include <test/testexecuteserverbase.h>

class CDemoIPSuite : public CTestServer
	{
public:
	static CDemoIPSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
	};

#endif
