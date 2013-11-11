/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file def130695_SuiteServer.h
 @internalTechnology
*/

#if (!defined ___DEF130695__SERVER_H__)
#define ___DEF130695__SERVER_H__
#include <test/testexecuteserverbase.h>


class C_def130695_Suite : public CTestServer
	{
public:
	static C_def130695_Suite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);


private:
	};

#endif
