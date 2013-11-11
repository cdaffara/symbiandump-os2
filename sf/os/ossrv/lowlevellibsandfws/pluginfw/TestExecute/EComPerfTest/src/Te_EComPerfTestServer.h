// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_EComPerfLoggerServer.h
// 
//

#ifndef	TE_ECOMPERFTESTSERVER_H
#define	TE_ECOMPERFTESTSERVER_H	
#include <test/testexecuteserverbase.h>
		
class CEComPerfTestServer : public CTestServer
	{
public:
	static CEComPerfTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};
#endif // TE_ECOMPERFTESTSERVER_H
