/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @file
*/
#ifndef __SERVER_H__
#define __SERVER_H__

#include <test/testexecuteserverbase.h>

struct TScriptTests;

namespace TestExecuteInterface
	{
	class CServer : public ::CTestServer
		{
	public:
		CServer(TScriptTests* aTestTypes);
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	private:
		TScriptTests* iTestTypes;
		};
	}

#endif
