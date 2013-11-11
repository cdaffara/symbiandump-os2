/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "testexecuteinterface.h"
#include "server.h"
#include "step.h"

TestExecuteInterface::CServer::CServer(TScriptTests* aTestTypes)
	: iTestTypes(aTestTypes)
	{
	}

CTestStep* TestExecuteInterface::CServer::CreateTestStep(const TDesC& /*aStepName*/)
	{
	// Ignore the stepname; we'll always return our step
	return new CStep(iTestTypes);
	}

