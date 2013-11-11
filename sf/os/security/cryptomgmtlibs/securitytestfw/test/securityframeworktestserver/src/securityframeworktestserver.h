/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SECURITYFRAMEWORKTESTSERVER_H
#define SECURITYFRAMEWORKTESTSERVER_H
#include <test/testexecuteserverbase.h>

class CSecurityFrameworkTestServer : public CTestServer
	{
public:
	static CSecurityFrameworkTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	};
#endif  //SECURITYFRAMEWORKTESTSERVER_H
