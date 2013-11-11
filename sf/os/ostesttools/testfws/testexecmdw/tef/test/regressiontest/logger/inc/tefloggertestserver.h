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
* This contains CTEFLoggerTestServer
*
*/



/**
 @test
 @internalComponent
*/

#ifndef __C_TEF_LOGGER_TEST_SERVER__
#define __C_TEF_LOGGER_TEST_SERVER__

#include <test/testblockcontroller.h>
#include <test/testserver2.h>

#include "tefloggertestwrapper.h"
#include "tefloggertestactiveobjectwrapper.h"

_LIT(KTEFLoggerTestWrapper, "TEFLoggerTestWrapper");
_LIT(KTEFLoggerTestActiveObjectWrapper, "TEFLoggerTestActiveObjectWrapper");

class CTEFLoggerTestBlock : public CTestBlockController
	{
public:
	CTEFLoggerTestBlock() : CTestBlockController() {}
	~CTEFLoggerTestBlock() {}

	CDataWrapper* CreateDataL(const TDesC& aData)
		{
		CDataWrapper* wrapper = NULL;
		if (KTEFLoggerTestWrapper() == aData)
			{
			wrapper = CTEFLoggerTestWrapper::NewL();
			}
		else if (KTEFLoggerTestActiveObjectWrapper() == aData)
			{
			wrapper = CTEFLoggerTestActiveObjectWrapper::NewL();
			}
		return wrapper;
		}
	};

class CTEFLoggerTestServer : public CTestServer2
	{
public:
	CTEFLoggerTestServer() {}
	~CTEFLoggerTestServer() {}

	static CTEFLoggerTestServer* NewL();

	CTestBlockController*	CreateTestBlock()
		{
		CTestBlockController* controller = new (ELeave) CTEFLoggerTestBlock();
		return controller;
		}
	};

#endif // __C_TEF_LOGGER_TEST_SERVER__
