/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tredirectionserver.h
*
*/
 

#ifndef __TREDIRECTIONSERVER_H__
#define __TREDIRECTIONSERVER_H__	

#include <f32file.h>
#include <test/testexecuteserverbase.h>

class CRedirTestServer : public CTestServer
	{
public:
	static CRedirTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	};

#endif // __TREDIRECTIONSERVER_H__
