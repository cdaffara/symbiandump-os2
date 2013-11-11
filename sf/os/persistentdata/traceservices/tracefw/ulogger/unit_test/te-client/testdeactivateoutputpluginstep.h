/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file TestSetActiveOutputPluginStep.h
 @internalTechnology
*/
#if (!defined __TESTDEACTIVATEOUTPUTPLUGIN_STEP_H__)
#define __TESTDEACTIVATEOUTPUTPLUGIN_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerclientsuitestepbase.h"

class CTestDeactivateOutputPluginStep : public CTestUloggerClientApiStepBase
	{
public:
	CTestDeactivateOutputPluginStep();
	~CTestDeactivateOutputPluginStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KTestDeactivateOutputPluginStep,"TestDeactivateOutputPluginStep");

#endif
