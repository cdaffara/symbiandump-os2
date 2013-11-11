/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_test_syscfgstep.h
 @internalTechnology
*/
#ifndef TE_SYSCFG_TESTSTEP_H
#define TE_SYSCFG_TESTSTEP_H

#include <test/testexecutestepbase.h>
#include <e32base.h> // cbase
#include <e32test.h>
#include <f32file.h> // rfs, rfile
#include "outputframework.h"	// coutputframework
#include "uloggersysconfig.h" 	// cconfig, cconfigsettingsiter

_LIT(KConfigFilePath,"c:\\private\\10273881\\uloggerconfig.ini");
_LIT8(KActiveChannelSection,"ACTIVE_OUTPUT_PLUGIN");
_LIT8(KPrimaryFilterSection,"PRIMARY_FILTERS");
_LIT8(KSecondaryFilterSection,"SECONDARY_FILTERS");
_LIT8(KActiveControlSection,"ACTIVE_CONTROL_CHANNEL");

//Test Sysconfig of Ulogger
class CTestSysCfgStep : public CTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL()=0;

public:
	Ulogger::CConfig *iConfig;
	
	};


class CSysCfgTest1: public CTestSysCfgStep
{
	public:
		virtual TVerdict doTestStepL();
};


class CSysCfgTest2: public CTestSysCfgStep
{
	public:
		virtual TVerdict doTestStepL();
};


class CSysCfgTest3: public CTestSysCfgStep
{
	public:
		virtual TVerdict doTestStepL();
};


class CSysCfgTest4: public CTestSysCfgStep
{
	public:
		virtual TVerdict doTestStepL();
};


	
_LIT(KSysCfgTeststep1,"SysCfgTeststep1");	
_LIT(KSysCfgTeststep2,"SysCfgTeststep2");	
_LIT(KSysCfgTeststep3,"SysCfgTeststep3");	
_LIT(KSysCfgTeststep4,"SysCfgTeststep4");	

#endif
