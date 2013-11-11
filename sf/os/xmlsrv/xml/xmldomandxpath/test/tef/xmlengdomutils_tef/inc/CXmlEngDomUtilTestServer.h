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
 @file CXmlEngDomUtilTestServer.h
 @internalTechnology
*/
#ifndef CXMLENGDOMUTILTESTSERVER_H_
#define CXMLENGDOMUTILTESTSERVER_H_

#include <test/testexecuteserverbase.h>


_LIT(KServerName, "xmlengdomutils_tef");


class CXmlEngDomUtilTestServer : public CTestServer
{
public:
	static CXmlEngDomUtilTestServer* NewL();
	void ConstructL(const TDesC& aName);
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
};

#endif /*CXMLENGDOMUTILTESTSERVER_H_*/
