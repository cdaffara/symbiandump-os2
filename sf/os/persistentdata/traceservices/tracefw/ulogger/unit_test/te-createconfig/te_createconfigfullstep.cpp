// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_createconfigfullstep.cpp
*/
 
#include "te_createconfigfullstep.h"
#include "te_clearconfig.h"
#include "uloggerdatatypes.h"

using namespace Ulogger;

CCreateConfigFUllTestStep::~CCreateConfigFUllTestStep()
/**
 * Destructor
 */
	{
	}

CCreateConfigFUllTestStep::CCreateConfigFUllTestStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCreateConfigFullStep);
	}
 
 
/**
 * Override of base class virtual
 * @return - TVerdict code
 * 
 */
TVerdict CCreateConfigFUllTestStep::doTestStepPreambleL()
	{
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return - TVerdict code
 */
TVerdict CCreateConfigFUllTestStep::doTestStepPostambleL()
	{
	return TestStepResult();	
	}

/**
 * Test 
 * @return - TVerdict code
 */
TVerdict CCreateConfigFUllTestStep::doTestStepL()
	{
	RULogger logger;

	logger.Connect();
	CClearConfig configIni;

	configIni.ClearL(logger);

	_LIT8(KTextplugin,"uloggerfileplugin");						
	logger.ActivateOutputPlugin(KTextplugin);

	_LIT(KTextsetting, "output_path");
	_LIT(KTextvalue, "c:\\logs\\Ulogger.log");

	_LIT(KTextsetting2, "max_size");
	_LIT(KTextvalue2, "10000000");

	const TPtrC  settingptr(KTextsetting);
	const TPtrC  valueptr(KTextvalue);

	const TPtrC  settingptr2(KTextsetting2);
	const TPtrC  valueptr2(KTextvalue2);

	TPluginConfiguration setpluginsettings1;
	setpluginsettings1.SetKey(settingptr);
	setpluginsettings1.SetValue(valueptr);

	TPluginConfiguration setpluginsettings2;
	setpluginsettings2.SetKey(settingptr2);
	setpluginsettings2.SetValue(valueptr2);			

	logger.SetPluginConfigurations(KTextplugin, setpluginsettings1);
	logger.SetPluginConfigurations(KTextplugin, setpluginsettings2);

	_LIT8(KTextplugin2,"uloggerserialplugin");			
	_LIT(KTextsetting3, "output_port");	
	_LIT(KTextvalue3, "1");

	const TPtrC settingptr3(KTextsetting3);
	const TPtrC valueptr3(KTextvalue3);

	TPluginConfiguration setpluginsettings3;
	setpluginsettings3.SetKey(settingptr3);
	setpluginsettings3.SetValue(valueptr3);		

	logger.SetPluginConfigurations(KTextplugin2, setpluginsettings3);

	CArrayFixFlat<TUint8> *setfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
	setfilter->AppendL(3);
	logger.SetPrimaryFiltersEnabled(*setfilter, true);

	RArray<TUint32> setfilter2;
	setfilter2.Append((TUint32)2223);
	logger.SetSecondaryFiltersEnabled(setfilter2, true);

	logger.SetSecondaryFilteringEnabled(false);

	SetTestStepResult(EPass);

	return TestStepResult();	
	}
