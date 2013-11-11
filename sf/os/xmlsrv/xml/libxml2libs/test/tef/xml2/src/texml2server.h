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
 @file texml2server.h
 @internalTechnology
*/

#ifndef _TEF_XML2SERVER_H_
#define _TEF_XML2SERVER_H_

#include <test/testexecuteserverbase.h>
#include <e32debug.h>

//Server name - same as the executable name mentioned in mmp
_LIT(KServerName, "texml2");

/**
 * Server to test the APIs in xmlengineutils.
 * APIs tested are:
 * XmlEngLeaveOOML
 * XmlEngXmlCharFromDesL
 * XmlEngEscapeForXmlValueL
 * XmlEnginePushL
 * XmlEnginePopAndClose
 * CopyL
 * Free
 * Length
 * Set
 * HBufCFromXmlCharLC
 * HBufCFromXmlCharL
 * AllocL
 * AllocLC
 * SetL
 * SetL
 * AllocAndFreeL
 * AllocAndFreeLC
 * AppendL(TXmlEngString str)
 * AppendL(TXmlEngString str1, TXmlEngString str2)
 * 
 */
class CXml2TestServer : public CTestServer
	{
public:
	static CXml2TestServer* NewL();
	void ConstructL(const TDesC& aName);
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
