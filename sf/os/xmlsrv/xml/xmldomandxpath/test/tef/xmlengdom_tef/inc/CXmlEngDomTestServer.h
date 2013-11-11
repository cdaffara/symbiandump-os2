// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CXmlEngDomTestServer.h
 @internalTechnology
*/
#ifndef CXMLENGDOMTESTSERVER_H_
#define CXMLENGDOMTESTSERVER_H_

#include <test/testexecuteserverbase.h>


/** Test server to implement the pure virtual CreateTestStep
 method.
 */
_LIT(KServerName, "xmlengdom_tef");


class CXmlEngDomTestServer : public CTestServer
{
public:
	static CXmlEngDomTestServer* NewL();
	void ConstructL(const TDesC& aName);
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
};

#endif /*CXMLENGDOMTESTSERVER_H_*/
