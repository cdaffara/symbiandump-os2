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
 @file CXmlEngDomNodeSetTestServer.h
 @internalTechnology
*/

#ifndef CXMLENGDOMNODESETTESTSERVER_H_
#define CXMLENGDOMNODESETTESTSERVER_H_

#include <test/testexecuteserverbase.h>


_LIT(KServerName, "xmlengdomnodeset_tef");


class CXmlEngDomNodeSetTestServer : public CTestServer
{
public:
	static CXmlEngDomNodeSetTestServer* NewL();
	void ConstructL(const TDesC& aName);
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
};

#endif /*CXMLENGDOMNODESETTESTSERVER_H_*/
