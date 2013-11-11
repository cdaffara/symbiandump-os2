/*
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


#ifndef TE_XMLOMXCLIENT_H
#define TE_XMLOMXCLIENT_H

#include <test/testserver2.h>

class CTe_XmlOmxClient : public CTestServer2
	{
public:
	static CTe_XmlOmxClient* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	CTestBlockController* CreateTestBlock();
	};

#endif //TE_XMLOMXCLIENT_H
