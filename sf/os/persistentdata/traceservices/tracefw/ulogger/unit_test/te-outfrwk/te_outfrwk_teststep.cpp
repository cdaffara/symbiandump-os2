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
 @file Te_Outfrwk_TestStep.cpp
*/
 
#include "te_outfrwk_teststep.h"
#include <uloggeroutputplugin.h>
 
using namespace Ulogger;
/**
 * Destructor
 */
CUloggerOutfrwkTest::~CUloggerOutfrwkTest()
{
}

/**
 * Override of base class virtual
 * @return - TVerdict code
 * 
 */
TVerdict CUloggerOutfrwkTest::doTestStepPreambleL()
	{
	iOutfrwk = NULL;

	INFO_PRINTF1(_L("create plugin"));
	INFO_PRINTF1(_L("MOutputPlugin"));
	iPlugin = NULL;
	TRAPD (err,iPlugin = (CPlugin*)(CPlugin::NewL(_L8("uloggerfileplugin"))));
	if(err == KErrNone)
		{
		RPointerArray<TPluginConfiguration> settings;
		TPluginConfiguration outputPathConfig;
		outputPathConfig.SetKey(_L("output_path"));
		outputPathConfig.SetValue(_L("c:\\te_testUlog.log"));
		settings.Append(&outputPathConfig);
		//iPlugin->ConfigureOutputPlugin(settings); //don't need to do this here as output framework will
										//do this job during construction
		INFO_PRINTF1(_L("create Output framework"));
		iOutfrwk = COutputFramework::NewL(*((MOutputPlugin*)iPlugin->GetInterfaceL(MOutputPlugin::iInterfaceId)), settings);
		settings.Reset();
		settings.Close();
		if(!iOutfrwk)
			{
			INFO_PRINTF1(_L("Cannot create output framework"));
			SetTestStepResult(EFail);
			}
		}
	else
		{
		TBuf<64> b;
		b.AppendFormat(_L("Problem with loading plugin: %d"), err);
		INFO_PRINTF1(b);
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return - TVerdict code
 * 
 */
TVerdict CUloggerOutfrwkTest::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("deleting output framework"));
	delete iOutfrwk;
	delete iPlugin;
	SetTestStepResult(EPass);
	return TestStepResult();	
	}
	
	
/**
 * Tests CTextMessage
 * @return - TVerdict code
 * 
 */
TVerdict CTextMessageTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Output Framework test"));
	
	if(iOutfrwk)
		{
		INFO_PRINTF1(_L("sending sample data ()"));
		TInt err = iOutfrwk->SendData(_L8("sample data "));
		if(err == KErrNone)
			SetTestStepResult(EPass);
		else
			SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("there is no output framework"));
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();	
	}
	

