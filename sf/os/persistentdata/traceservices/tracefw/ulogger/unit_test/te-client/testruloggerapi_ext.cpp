// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file testruloggerapi_ext.cpp
 @internalTechnology
*/
#include "testruloggerapi_ext.h"
#include "te_uloggerclientsuitedefs.h"
#include <uloggerdatatypes.h>

CTestRULoggerAPIStep::~CTestRULoggerAPIStep()
/**
 * Destructor
 */
	{
	}

CTestRULoggerAPIStep::CTestRULoggerAPIStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
		
	SetTestStepName(KTestRULoggerAPIStep);
	}

TVerdict CTestRULoggerAPIStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CTestRULoggerAPIStep -Extended API tests- started"));
	
	TInt errCode = iUlogger.Connect();
	if(errCode != KErrNone)
		{
		TBuf<64> b;
		b.AppendFormat(_L("connect error: %d"), errCode);
		SetTestStepResult(EFail);
		}
	else
		SetTestStepResult(EPass);
	
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestRULoggerAPIStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
{
	if (TestStepResult()==EPass)
		{
		TInt errors = 0;
		errors += SetGetPrimaryFiletr_TestL();
		errors += SetGetSecondaryFiletr_TestL();
		errors += SetGetSecondaryFiltering_TestL();
		errors += SetGetOutputPlugin_TestL();
		errors += GetInstalledOutputPlugins_TestL();
		errors += SetGetInputPlugin_TestL();
		errors += GetInstalledInputPlugins_TestL();
		errors += SetGetRemovePluginConf_TestL();
		errors += SetGetTraceBuffer_TestL();
		
		//check errors
		if(errors)
			{
			SetTestStepResult(EFail);
			TBuf<64> b;
			b.AppendFormat(_L("%d errors were found !!!!"), errors);
			INFO_PRINTF1(b);
			}
		else
			{
			SetTestStepResult(EPass);
			INFO_PRINTF1(_L("CTestRULoggerAPIStep completed without problems."));
			}
		}
	
	return TestStepResult();	
}

TVerdict CTestRULoggerAPIStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("CTestRULoggerAPIStep completed"));
		CTestUloggerClientApiStepBase::doTestStepPostambleL();	
		iUlogger.Close();
		return TestStepResult();
	}


TInt CTestRULoggerAPIStep::SetGetPrimaryFiletr_TestL()
{
	TInt err = 0;
	TInt i;
	CArrayFixFlat<TUint8> *filters = new (ELeave) CArrayFixFlat<TUint8>(1);
	CArrayFixFlat<TUint8> *gFilters = new (ELeave) CArrayFixFlat<TUint8>(1);
	TInt test;
	
	//set 256 filters
	for(i=0; i<256; i++)
		filters->AppendL((TUint8)i);	
	test = iUlogger.SetPrimaryFiltersEnabled(*filters, ETrue);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set 256 filters - failed"));
		err++;
		}

	//get 256 filetrs
	test = iUlogger.GetPrimaryFiltersEnabled(*gFilters);
	if(test!=KErrNone && gFilters->Count()!=256)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get 256 filters - failed!"));
		err++;
		}
	
	//set 257 filetrs
	filters->AppendL((TUint8)257); //value doesn't matter
	test = iUlogger.SetPrimaryFiltersEnabled(*filters, ETrue);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set 257 filters - failed!"));
		err++;
		}

	//get 257 filetrs (should be 256 vin array)
	gFilters->Reset();
	test = iUlogger.GetPrimaryFiltersEnabled(*gFilters);
	if(test!=KErrNone && gFilters->Count()!=256)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get 257 filters - failed!"));
		err++;
		}
	
	//remove 256 filetrs
	test = iUlogger.SetPrimaryFiltersEnabled(*gFilters, EFalse);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove 256 filetrs - failed!"));
		err++;
		}
	
	//remove 257 filetrs
	test = iUlogger.SetPrimaryFiltersEnabled(*filters, EFalse);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove 257 filetrs - failed!"));
		err++;
		}

	
	filters->Reset();
	gFilters->Reset();
	//set empty array ETrue test
	test = iUlogger.SetPrimaryFiltersEnabled(*filters, ETrue);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set empty array ETrue test - failed!"));
		err++;
		}

	//set empty array EFalse test
	test = iUlogger.SetPrimaryFiltersEnabled(*filters, EFalse);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set empty array EFalse test - failed!"));
		err++;
		}

	//getting empty array
	gFilters->Reset();
	test = iUlogger.GetPrimaryFiltersEnabled(*gFilters);
	if(test!=KErrNone && gFilters->Count()!=0)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("getting empty array - failed!"));
		err++;
		}
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetPrimaryFiletr_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetPrimaryFiletr_TestL passed"));
			
	
	delete gFilters;
	gFilters = NULL;
	delete filters;
	filters = NULL;
	return err;
}


TInt CTestRULoggerAPIStep::SetGetSecondaryFiletr_TestL()
{
	TInt err = 0;
	TInt i;
	RArray<TUint32> filters;
	RArray<TUint32> gFilters;
	TInt test;

	//clean
	iUlogger.GetSecondaryFiltersEnabled(gFilters);
	iUlogger.SetSecondaryFiltersEnabled(gFilters, EFalse);
	gFilters.Reset();
	
	//set 4096 filters
	for(i=0; i<4096; i++)
		filters.AppendL((TUint32)i);	
	test = iUlogger.SetSecondaryFiltersEnabled(filters, ETrue);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set 4096 filters - failed"));
		err++;
		}

	//get 4096 filetrs
	test = iUlogger.GetSecondaryFiltersEnabled(gFilters);
	if(test!=KErrNone && gFilters.Count()!=4096)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get 4096 filters - failed!"));
		err++;
		}
	
	//set 4096+1 filetrs
	filters.AppendL(4097); //value doesn't matter
	test = iUlogger.SetSecondaryFiltersEnabled(filters, ETrue);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set 4096+1 filters - failed!"));
		err++;
		}

	//get 4096+1 filetrs (should be 4096 in array)
	gFilters.Reset();
	test = iUlogger.GetSecondaryFiltersEnabled(gFilters);
	if(test!=KErrNone && gFilters.Count()!=4096)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get 4096+1 filters - failed!"));
		err++;
		}
	
	//remove 4096 filetrs
	test = iUlogger.SetSecondaryFiltersEnabled(gFilters, EFalse);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove 4096 filetrs - failed!"));
		err++;
		}
	
	//remove 4096+1 filetrs
	test = iUlogger.SetSecondaryFiltersEnabled(filters, EFalse);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove 4096+1 filetrs - failed!"));
		err++;
		}

	
	filters.Reset();
	gFilters.Reset();
	//set empty array ETrue test
	test = iUlogger.SetSecondaryFiltersEnabled(filters, ETrue);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set empty array ETrue test - failed!"));
		err++;
		}

	//set empty array EFalse test
	test = iUlogger.SetSecondaryFiltersEnabled(filters, EFalse);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set empty array EFalse test - failed!"));
		err++;
		}

	//getting empty array
	gFilters.Reset();
	test = iUlogger.GetSecondaryFiltersEnabled(gFilters);
	if(test!=KErrNone && gFilters.Count()!=0)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("getting empty array - failed!"));
		err++;
		}
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetSecondaryFiletr_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetSecondaryFiletr_TestL passed"));
	INFO_PRINTF1(b);
	
	gFilters.Close();
	filters.Close();
	return err;
}


TInt CTestRULoggerAPIStep::SetGetSecondaryFiltering_TestL()
{
	TInt err = 0;
	TInt test = 0;
	TBool val = ETrue;
	
	//set secondary filtering as ETrue
	iUlogger.SetSecondaryFilteringEnabled(EFalse);
	test = iUlogger.SetSecondaryFilteringEnabled(ETrue);
	if(test!=KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set secondary filtering as ETrue - failed!"));
		err++;
		}

	//set secondary filtering as ETrue again
	test = iUlogger.SetSecondaryFilteringEnabled(ETrue);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set secondary filtering as ETrue again - failed!"));
		err++;
		}

	//get secondary filtering ETrue
	test = iUlogger.GetSecondaryFilteringEnabled(val);
	if(test != KErrNone || !val)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get secondary filtering ETrue - failed!"));
		err++;
		}

	//set secondary filtering as EFalse
	test = iUlogger.SetSecondaryFilteringEnabled(EFalse);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set secondary filtering as EFalse - failed!"));
		err++;
		}
	
	//set secondary filtering as EFalse again
	test = iUlogger.SetSecondaryFilteringEnabled(EFalse);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set secondary filtering as EFalse again - failed!"));
		err++;
		}
	
	//get secondary filtering EFalse
	test = iUlogger.GetSecondaryFilteringEnabled(val);
	if(test != KErrNone || val)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get secondary filtering EFalse - failed!"));
		err++;
		}
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetSecondaryFiltering_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetSecondaryFiltering_TestL passed"));
	INFO_PRINTF1(b);
	return err;
}

TInt CTestRULoggerAPIStep::SetGetOutputPlugin_TestL()
{
	TInt err = 0;
	TInt test = KErrNone;
	TBuf8<256> plugin;
	_LIT8(KFilePlugin, "uloggerfileplugin");
	_LIT8(KSerialPlugin, "uloggerserialplugin");
	iUlogger.ActivateOutputPlugin(KSerialPlugin); //initial settings
	
	//set uloggerfileplugin as output
	test = iUlogger.ActivateOutputPlugin(KFilePlugin);
	if(test!=KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerfileplugin as output - failed!"));
		err++;
		}
	
	//get uloggerfileplugin
	test = iUlogger.GetActiveOutputPlugin(plugin);
	if(plugin.Compare(KFilePlugin) != 0 || test!=KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get uloggerfileplugin - failed!"));
		err++;
		}
	
	//set uloggerfileplugin as output again
	test = iUlogger.ActivateOutputPlugin(KFilePlugin);
	if(test != KErrNone) //dont return an error although already exists isnt a "failure"
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerfileplugin as output again - failed!"));
		err++;
		}
	
	TBuf8<2> smallBuf;
	//get uloggerfileplugin to small buf
	test = iUlogger.GetActiveOutputPlugin(smallBuf);
	if(test!=KErrOverflow)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get uloggerfileplugin to small buf - failed!"));
		err++;
		}
	
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetOutputPlugin_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetOutputPlugin_TestL passed"));
	INFO_PRINTF1(b);
	return err;
}

TInt CTestRULoggerAPIStep::GetInstalledOutputPlugins_TestL()
{
	TInt err = 0;
	TInt test = KErrNone;
	
	CArrayPtrFlat<HBufC8>* plugins = new (ELeave) CArrayPtrFlat<HBufC8>(1);
	test = iUlogger.GetInstalledOutputPlugins(*plugins);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("GetInstalledOutputPlugins_TestL - failed!"));
		err++;
		}
	
	plugins->ResetAndDestroy();
	delete plugins;
	plugins = NULL;
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("GetInstalledOutputPlugins_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("GetInstalledOutputPlugins_TestL passed"));
	INFO_PRINTF1(b);
	return err;
}

TInt CTestRULoggerAPIStep::SetGetInputPlugin_TestL()
	{
	TInt err = 0;
	TInt test = KErrNone;
	
	TBuf8<256> plugin;
	_LIT8(KFilePlugin, "uloggerfileplugin");
	_LIT8(KUsbPlugin, "uloggerusbplugin");
	iUlogger.DeActivateInputPlugin(); //set initial state
	
	//set uloggerusbplugin as input
	test = iUlogger.ActivateInputPlugin(KUsbPlugin);
	if(test!=KErrNone && test!=KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerusbplugin as input - failed!"));
		err++;
		}
	
	//set uloggerusbplugin as input again
	test = iUlogger.ActivateInputPlugin(KUsbPlugin);
	if(test!=KErrAlreadyExists && test!=KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerusbplugin as input again - failed!"));
		err++;
		}
	
	//set uloggerfileplugin as input
	test = iUlogger.ActivateInputPlugin(KFilePlugin);
	if(test!=KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerfileplugin as input - failed!"));
		err++;
		}
	
	//set empty as input
	test = iUlogger.ActivateInputPlugin(_L8(""));
	if(test!=KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set empty as inputt - failed!"));
		err++;
		}
	
	//get active input plugin
	test = iUlogger.GetActiveInputPlugin(plugin);
	if(test!=KErrNone && test != KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get active input plugin - failed!"));
		err++;
		}
	
	//deactivate input
	test = iUlogger.DeActivateInputPlugin();
	if(test!=KErrNone && test != KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("deactivate input - failed!"));
		err++;
		}
	
	//deactivate input again
	test = iUlogger.DeActivateInputPlugin();
	if(test != KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("deactivate input again - failed!"));
		err++;
		}
	
	//get active input plugin again
	test = iUlogger.GetActiveInputPlugin(plugin);
	if(test != KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("get active input plugin again - failed!"));
		err++;
		}
	
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetInputPlugin_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetInputPlugin_TestL passed"));
	INFO_PRINTF1(b);
	return err;
	}

TInt CTestRULoggerAPIStep::GetInstalledInputPlugins_TestL()
{
	TInt err = 0;
	TInt test = KErrNone;

	CArrayPtrFlat<HBufC8> *plugins = new (ELeave) CArrayPtrFlat<HBufC8>(1);
	test = iUlogger.GetInstalledInputPlugins(*plugins);
	if(test!=KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("GetInstalledInputPlugins - failed!"));
		err++;
		}
	
	plugins->ResetAndDestroy();
	delete plugins;
	plugins = NULL;
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("GetInstalledInputPlugins_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("GetInstalledInputPlugins_TestL passed"));
	INFO_PRINTF1(b);
	return err;
}

TInt CTestRULoggerAPIStep::SetGetRemovePluginConf_TestL()
	{
	TInt err = 0;
	TInt test = KErrNone;
	_LIT8(KFilePlugin, "uloggerfileplugin");
	_LIT8(KDummyPlugin, "uloggerdummyplugin");
	_LIT(KTestKey, "testKey");
	_LIT(KTestVal, "testVal");
	
	//set uloggerfileplugin settings
	TPluginConfiguration conf;
	conf.SetKey(KTestKey);
	conf.SetValue(KTestVal);
	test = iUlogger.SetPluginConfigurations(KFilePlugin, conf);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerfileplugin settings - failed!"));
		err++;
		}
	
	//check uloggerfileplugin settings
	RPointerArray<TPluginConfiguration> confCheck;
	test = iUlogger.GetPluginConfigurations(KFilePlugin, confCheck);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("check uloggerfileplugin settings - failed!"));
		err++;
		}
	else
		{
		TBool e = ETrue;
		for(TInt i=0; i<confCheck.Count(); ++i)
			{
			TPluginConfiguration* check = confCheck[i];
			INFO_PRINTF1(check->Key());
			INFO_PRINTF1(check->Value());
			if(!check->Key().Compare(KTestKey) && !check->Value().Compare(KTestVal))
				{
				e = EFalse;
				break;
				}
			}
		if(e)
			{
			INFO_PRINTF2(_L("err: %d"), test);
			INFO_PRINTF1(_L("check uloggerfileplugin settings (wrong values) - failed!"));
			err++;
			}
		}
	confCheck.ResetAndDestroy();
	confCheck.Close();
	
	
	//set uloggerdummyplugin settings
	test = iUlogger.SetPluginConfigurations(KDummyPlugin, conf);
	if(test != KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("set uloggerdummyplugin settings - failed!"));
		err++;
		}
	
	//pass empty descriptor
	test = iUlogger.SetPluginConfigurations(_L8(""), conf);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("pass empty descriptor - failed!"));
		err++;
		}
	
	
	//Get uloggerdummyplugin settings
	RPointerArray<TPluginConfiguration> confCheck2;
	test = iUlogger.GetPluginConfigurations(KDummyPlugin, confCheck2);
	if(test != KErrNotFound)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("Get uloggerdummyplugin settings - failed!"));
		err++;
		}
	confCheck2.ResetAndDestroy();
	confCheck2.Close();
	
	
	//Get emptyName settings
	RPointerArray<TPluginConfiguration> confCheck3;
	test = iUlogger.GetPluginConfigurations(_L8(""), confCheck3);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("Get emptyName settings - failed!"));
		err++;
		}
	confCheck3.ResetAndDestroy();
	confCheck3.Close();
	
	
	//remove uloggerfileplugin settings
	test = iUlogger.RemovePluginConfigurations(KFilePlugin);
	if(test != KErrNone) //expected error
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove uloggerfileplugin settings - failed!"));
		err++;
		}

	//remove uloggerfileplugin settings again
	test = iUlogger.RemovePluginConfigurations(KFilePlugin);
	if(test != KErrNotFound) //expected error
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove uloggerfileplugin settings again - failed!"));
		err++;
		}

	//remove uloggerdummyplugin settings
	test = iUlogger.RemovePluginConfigurations(KFilePlugin);
	if(test != KErrNotFound) //expected error
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove uloggerdummyplugin settings - failed!"));
		err++;
		}

	//remove empty desc plugin settings
	test = iUlogger.RemovePluginConfigurations(_L8(""));
	if(test != KErrArgument) //expected error
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("remove empty desc plugin settings - failed!"));
		err++;
		}
	
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetRemovePluginConf_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetRemovePluginConf_TestL passed"));
	INFO_PRINTF1(b);
	return err;
	}

TInt CTestRULoggerAPIStep::SetGetTraceBuffer_TestL()
	{
	TInt err = 0;
	TInt test = KErrNone;
	TInt val = 0;
	
	//test SetBufferSize(-1)
	test = iUlogger.SetBufferSize(-1);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetBufferSize(-1) - failed!"));
		err++;
		}
	
	//test SetBufferSize(1025)
	test = iUlogger.SetBufferSize(1025);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetBufferSize(1025) - failed!"));
		err++;
		}
	
	//test SetBufferSize(1000)
	test = iUlogger.SetBufferSize(1000);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetBufferSize(1000) - failed!"));
		err++;
		}
	
	test = iUlogger.GetBufferSize(val);
	if(test!=KErrNone || val != 1000)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test GetBufferSize(val) - failed!"));
		err++;
		}
	
	iUlogger.GetNotificationSize(val);
	test = iUlogger.SetBufferSize(val-1);
	if(test != KErrGeneral)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetBufferSize(val-1) (less than dns)  - failed!"));
		err++;
		}
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetTraceBuffer_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetTraceBuffer_TestL passed"));
	INFO_PRINTF1(b);
	return err;
	}

TInt CTestRULoggerAPIStep::SetGetDnsBuffer_TestL()
	{
	TInt err = 0;
	TInt test = KErrNone;
	TInt val = 0;
	
	test = iUlogger.SetNotificationSize(-1);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetNotificationSize(-1)  - failed!"));
		err++;
		}
	
	test = iUlogger.SetNotificationSize(1025);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetNotificationSize(1025)  - failed!"));
		err++;
		}
	
	test = iUlogger.SetNotificationSize(32);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetNotificationSize(32)  - failed!"));
		err++;
		}
	
	iUlogger.GetBufferSize(val);
	test = iUlogger.SetNotificationSize(val+1);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test SetNotificationSize(val+1) (greater than buffer)  - failed!"));
		err++;
		}
	
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetDnsBuffer_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetDnsBuffer_TestL passed"));
	INFO_PRINTF1(b);
	return err;
	}

TInt CTestRULoggerAPIStep::SetGetBufferMode_TestL()
	{
	TInt err = 0;
	TInt test = KErrNone;
	TInt val = 0;
	
	iUlogger.SetBufferMode(0);
	test = iUlogger.SetBufferMode(1);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test iUlogger.SetBufferMode(1) - failed!"));
		err++;
		}

	test = iUlogger.SetBufferMode(1);
	if(test != KErrNone)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test iUlogger.SetBufferMode(1) again - failed!"));
		err++;
		}

	test = iUlogger.SetBufferMode(3);
	if(test != KErrArgument)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test iUlogger.iUlogger.SetBufferMode(3) (wrong) - failed!"));
		err++;
		}
		
	test = iUlogger.GetBufferMode(val);
	if(test != KErrNone || val != 1)
		{
		INFO_PRINTF2(_L("err: %d"), test);
		INFO_PRINTF1(_L("test iUlogger.iUlogger.GetBufferMode(val) - failed!"));
		err++;
		}
	
	//summary
	TBuf<64> b;
	if(err)
		b.AppendFormat(_L("SetGetBufferMode_TestL failed with %d errors!!!"), err);
	else
		b.Append(_L("SetGetBufferMode_TestL passed"));
	INFO_PRINTF1(b);
	return err;
	}

