/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This contains CTEFIntegrationTestServer
*
*/



/**
 @test
 @internalComponent
*/

#ifndef __C_TE_REG_BLOCKTEST_SERVER__
#define __C_TE_REG_BLOCKTEST_SERVER__

#include <test/testblockcontroller.h>
#include <test/testserver2.h>

#include "te_regblocktestwrapper.h"
#include "te_regblocktestactiveobjectwrapper.h"

_LIT(KTeRegBlockTestWrapper, "te_RegBlockTestWrapper");
_LIT(KTeRegBlockTestActiveObjectWrapper, "te_RegBlockTestActiveObjectWrapper");

class CTe_RegBlockTestController : public CTestBlockController
	{
public:
	CTe_RegBlockTestController() : CTestBlockController() {}
	~CTe_RegBlockTestController() {}

	CDataWrapper* CreateDataL(const TDesC& aData)
		{
		CDataWrapper* wrapper = NULL;
		if (KTeRegBlockTestWrapper() == aData)
			{
			wrapper = CTe_RegBlockTestWrapper::NewL();
			}
		else if (KTeRegBlockTestActiveObjectWrapper() == aData)
			{
			wrapper = CTe_RegBlockTestActiveObjectWrapper::NewL();
			}
		return wrapper;
		}
	};

class CTe_RegBlockTestServer : public CTestServer2
	{
public:
	CTe_RegBlockTestServer() {}
	~CTe_RegBlockTestServer() {}

	static CTe_RegBlockTestServer* NewL();

	CTestBlockController*	CreateTestBlock()
		{
		CTestBlockController* controller = new (ELeave) CTe_RegBlockTestController();
		return controller;
		}
	};

#endif // __C_TE_REG_BLOCKTEST_SERVER__
