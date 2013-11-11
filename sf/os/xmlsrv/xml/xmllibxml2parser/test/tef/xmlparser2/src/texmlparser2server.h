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
 @file texmlparser2server.h
 @internalTechnology
*/

#ifndef _TEF_XMLPARSER2SERVER_H_
#define _TEF_XMLPARSER2SERVER_H_

#include <test/testexecuteserverbase.h>
#include <e32debug.h>

/** Test server to implement the pure virtual CreateTestStep
 method.
 */
_LIT(KServerName, "texmlparser2");

class CXmlParser2TestServer : public CTestServer
	{
public:
	static CXmlParser2TestServer* NewL();
	void ConstructL(const TDesC& aName);
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
