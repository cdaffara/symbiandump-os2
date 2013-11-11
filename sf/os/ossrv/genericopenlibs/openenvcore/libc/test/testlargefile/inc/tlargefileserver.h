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
//  Name        : tlargefileserver.h
 

#ifndef __TLARGEFILESERVER_H__
#define __TLARGEFILESERVER_H__

#include <f32file.h>
#include <test/TestExecuteServerBase.h>

class CLargefileTestServer : public CTestServer
	{
public:
	static CLargefileTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif 
