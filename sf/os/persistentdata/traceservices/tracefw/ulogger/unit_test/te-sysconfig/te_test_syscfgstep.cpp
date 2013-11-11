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
 @file Te_Test_SysCfgStep.cpp
*/

#include "te_test_syscfgstep.h"
#include "uloggersysconfig.h"
#include "uloggershared.h"
#include "pluginallocator.h"

/**
 * Override of base class virtual
 * @return - TVerdict code
 * 
 */
TVerdict CTestSysCfgStep::doTestStepPreambleL()
	{
	TFileName filename;
	filename.Copy(KConfigFilePath);
	iConfig=Ulogger::CConfig::NewLC(NULL,filename);
	CleanupStack::Pop();
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return - TVerdict code
 */
TVerdict CTestSysCfgStep::doTestStepPostambleL()
	{
	if (iConfig)
		{
		delete iConfig;
		iConfig=NULL;
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();	
	}
	
	
	
	
/**
 * Test CConfig server functions.Call SetActivePlugin, GetActiveChannels with 
 *			various values.
 * @return - TVerdict code
 */
	
TVerdict CSysCfgTest1::doTestStepL()
	{

	TPtrC8 key;
	TPtrC8 value;
	TPtrC8 media;
	TInt errors =0;
	TInt r;
	RImplInfoPtrArray infoArray;
	
	
	Ulogger::CConfigSettingsIter* iter=Ulogger::CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);

	//Checking SetchannelSetting API
	
	r = iConfig->SetPluginSetting(_L8("uloggerfileplugin"), _L8("output_path"),_L8("c:\\logs\\ulogger.log"));
	if(r == 0)
		INFO_PRINTF1(_L("Settings set successfuly\n"));	
	else 
		{
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to add settings\n"));
		}
			
	
	r = iConfig->SetPluginSetting(_L8("uloggerserialplugin"), _L8("output_port"),_L8("1"));
	if(r == 0)
		INFO_PRINTF1(_L("Settings set successfuly\n"));	
	else
		{
		errors++;
		INFO_PRINTF1(_L("Unable to add settings\n"));	
		}
	
	Ulogger::CPluginAllocator::ListAllImplementationsL(infoArray);
	
	//Checking SetActivePlugin API
	r=KErrNotFound;
	for(TInt i=0;i<infoArray.Count();i++)
		{
		if(_L8("uloggerfileplugin").Compare(infoArray[i]->DataType())==0)
			{
			r = iConfig->SetActiveOutputPlugin(_L8("uloggerfileplugin"));
			break;	
			}
		}
	if(r == KErrNone)
		INFO_PRINTF1(_L("UloggerFilePlugin.dll set as active plugin\n"));	
	else if(r==KErrAlreadyExists)
		INFO_PRINTF1(_L("UloggerFilePlugin.dll is already set as active plugin\n"));	
	else 
		{
		errors++;
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to Set plugin active\n"));
		}

	
	r=KErrNotFound;
	for(TInt i=0;i<infoArray.Count();i++)
		{
		if(_L8("uloggerserialplugin").Compare(infoArray[i]->DataType())==0)
			{
			r = iConfig->SetActiveOutputPlugin(_L8("uloggerserialplugin"));
			break;	
			}
		}
	if(r == 0)
		INFO_PRINTF1(_L("UloggerSerialPlugin.dll set as active plugin\n"));	
	else 
		{
		errors++;
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to Set plugin active\n"));
		}
			
	//Checking SetActivePlugin API - for Invalid ones
	r=KErrNotFound;
	for(TInt i=0;i<infoArray.Count();i++)
		{
		if(_L8("media9").Compare(infoArray[i]->DataType())==0)
			{
			r = iConfig->SetActiveOutputPlugin(_L8("media9"));
			break;	
			}
		}
	if(r == 0)
		{
		errors++;
		INFO_PRINTF1(_L("media9 set as active plugin\n"));	
		}
	else 
		{
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to Set plugin active\n"));
		}

//Checking GetActiveOutputPluginsChannels API
	r = iConfig->GetActivePlugins(*iter);

	if(r == KErrNone)		
		INFO_PRINTF1(_L("Succssful in getting the active output plugin\n"));	
	else
		errors++;
	
	if(iter->Next(key,value))
		{
		if(key.Compare(_L8("1")) == 0 && value.Compare(_L8("uloggerserialplugin"))==0)
			INFO_PRINTF1(_L("1 = uloggerserialplugin\n"));
		else
			errors++;
		}
	iter->Reset();
	
	INFO_PRINTF1(_L("Checking CConfigSettingsIter Reset() function is fine\n"));
	
	if(iter->Next(key,value))
		{
		if(key.Compare(_L8("1")) == 0 && value.Compare(_L8("uloggerserialplugin"))==0)
			INFO_PRINTF1(_L("Reset Function is Working Fine\n"));
		else
			errors++;
		}
	else
		{
		INFO_PRINTF1(_L("Reset Function is not Working Fine\n"));
		errors++;
		}

//Checking SetActiveInoutPlugin and GetActiveInputPlugins API		
	r=KErrNotFound;
	for(TInt i=0;i<infoArray.Count();i++)
		{
		if(_L8("uloggerserialplugin").Compare(infoArray[i]->DataType())==0)
			{
			r = iConfig->SetActiveInputPlugin(_L8("uloggerserialplugin"));
			break;	
			}
		}
	if(r == 0)
		INFO_PRINTF1(_L("uloggerserialplugin.dll set as active control plugin\n"));	
	else 
		{
		errors++;
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to Set uloggerserialplugin.dll as active control plugin\n"));
		}
		
	r=KErrNotFound;
	for(TInt i=0;i<infoArray.Count();i++)
		{
		if(_L8("uloggerusbplugin").Compare(infoArray[i]->DataType())==0)
			{
			r = iConfig->SetActiveInputPlugin(_L8("uloggerusbplugin"));
			break;	
			}
		}
	if(r == 0)
		INFO_PRINTF1(_L("UloggerUsbPlugin.dll set as active control plugin\n"));	
	else 
		{
		errors++;
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to Set UloggerUsbPlugin.dll as active control plugin\n"));
		}
		
		
	r = iConfig->GetActivePlugins(*iter);

	if(r == KErrNone)		
		INFO_PRINTF1(_L("Succssful in getting the active control plugin\n"));	
	else
		errors++;
	
	if(iter->Next(key,value))
		{
		if(key.Compare(_L8("1")) == 0 && value.Compare(_L8("uloggerusbplugin"))==0)
			INFO_PRINTF1(_L("1 = uloggerusbplugin\n"));
		else
			errors++;
		}
		
				
//Checking Deactivating Output and Control Plugins API
	r=KErrNotFound;
	
	r=iConfig->DeActivateOutputPlugin(_L8("uloggerserialplugin"));
	if(r == 0)
		INFO_PRINTF1(_L("Active output plugin is deactivated\n"));	
	else 
		{
		errors++;
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to deactivate active output plugin\n"));
		}


	r=KErrNotFound;
	
	r=iConfig->DeActivateInputPlugin(_L8("uloggerusbplugin"));
	if(r == 0)
		INFO_PRINTF1(_L("Active control plugin is deactivated\n"));	
	else 
		{
		errors++;
		INFO_PRINTF2(_L("Error codes %d \n"),r );
		INFO_PRINTF1(_L("Unable to deactivate active control plugin\n"));
		}

		
	if(infoArray.Count()>0)
		{
     	infoArray.ResetAndDestroy();
    	infoArray.Close();
		}
	CleanupStack::PopAndDestroy();	
	
	if(errors == 0)
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	
	return TestStepResult();	
	
	}


/**
 * Test CConfig get functions.Call GetMedia, GetChannels and GetChannelsettings with 
 *			various values.
 * @return - TVerdict code
 */

TVerdict CSysCfgTest2::doTestStepL()
	{
	TPtrC8 key;
	TPtrC8 value;
	TInt r;
	TInt errors = 0;

	Ulogger::CConfigSettingsIter* iter=Ulogger::CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);

		
										
	//Checking GetSectionValues API

	r = iConfig->GetSectionValues(_L8("uloggerfileplugin"), *iter);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successful in getting the iterator\n"));				
		}
	else 
		errors++;
	if(iter->Next(key,value))
		{
		if(key.Compare(_L8("output_path")) == 0 && value.Compare(_L8("c:\\logs\\ulogger.log"))==0)
			INFO_PRINTF1(_L("output_path = c:\\logs\\ulogger.log \n"));
		else
			errors++;
		}
	
	r = iConfig->GetSectionValues(_L8("uloggerserialplugin"), *iter);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successful in getting the iterator\n"));				
		}
	else 
		errors++;
	
	if(iter->Next(key,value))
		{
		if(key.Compare(_L8("output_port"))==0 && value.Compare(_L8("1"))==0)
			INFO_PRINTF1(_L("output_port = 1\n"));	
		else
			errors++;
		}

	if(!iter->Next(key,value))
		errors++;
	

//Checking GetSectionValues API with Invalid values

	r = iConfig->GetSectionValues(_L8("10"), *iter);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Failure: As there is no media 10 assigned\n"));				
		errors++;
		}
	
//Checking Removing Plugin Settings API

	r= iConfig->RemovePluginSettings(_L8("uloggerserialplugin"));
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successful in deleting UloggerSerialPlugin settings \n"));				
		}
	else 
		errors++;

	CleanupStack::PopAndDestroy();
	if(errors == 0)
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	return TestStepResult();	
	
	}


/**
 * Test CConfig Call EnablePrimaryFilter, SetSecondaryFilter,RemoveFilter,SetBufferSize with various error cases.
 * @return - TVerdict code
 *
 */	
 
TVerdict CSysCfgTest3::doTestStepL()
	{
	
	Ulogger::CConfigSettingsIter* iter=Ulogger::CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);
	
	TPtrC8 key;
	TPtrC8 value;
	TInt r,errors = 0;
	
	//set filters one by one
	RArray<TUint32> filters;

	RArray<TPtrC8> aValues;
	
	//set different values for primary filter ans test result
	filters.Append(4);
	if( iConfig->SetActiveFilter(filters,KPrimaryFilterSection) == KErrNone)
		{
		INFO_PRINTF1(_L("SetActiveFilter for primary filters Successful"));
		}
	
	filters.Reset();
	filters.Append(0);
	filters.Append(4);
	if( iConfig->SetActiveFilter(filters,KPrimaryFilterSection)  == KErrNone )
		{
		INFO_PRINTF1(_L("SetActiveFilter for primary filters Successful"));
		}

	
	filters.Reset();
	filters.Append(0);
	if( iConfig->SetActiveFilter(filters,KSecondaryFilterSection)  == KErrNone )
		{
		INFO_PRINTF1(_L("SetActiveFilter for secondary filters Successful"));
		}
	
	filters.Reset();
	filters.Append(4);
	filters.Append(0);
	filters.Append(2);
	if( iConfig->SetActiveFilter(filters,KSecondaryFilterSection)  == KErrNone )
		{
		INFO_PRINTF1(_L("SetActiveFilter for secondary filters Successful"));
		}
	
	filters.Reset();
	filters.Append(100);
	if( iConfig->SetActiveFilter(filters,KSecondaryFilterSection)  == KErrNone )
		{
		INFO_PRINTF1(_L("SetActiveFilter for secondary filters Successful"));
		}
		
	filters.Reset();
	filters.Append(10000);
	if( iConfig->SetActiveFilter(filters,KSecondaryFilterSection)  == KErrNone )
		{
		INFO_PRINTF1(_L("SetActiveFilter for secondary filters Successful"));
		}
	
	
	filters.Close();
	
	_LIT8(KBufferSizeValidHigh,"65000");
	
	_LIT8(KBuffer,"buffer_size");
	
	//Checking SetTraceSettings API
	r = iConfig->SetTraceSettings(KBufferSizeValidHigh, KBuffer);

	
	r =iConfig->GetTraceSettings(*iter);
	while(iter->Next(key,value))
		{
		if(key.Compare(KBuffer) == 0 && value.Compare(KBufferSizeValidHigh) == 0)
			{
			INFO_PRINTF1(_L("Buffer size equals\n"));	
			}		
		
		}
	
	r = iConfig->SetTraceSettings(_L8("35"),Ulogger::KDataNotification);

	
	r =iConfig->GetTraceSettings(*iter);
	while(iter->Next(key,value))
		{
		if(key.Compare(Ulogger::KDataNotification) == 0 && value.Compare(_L8("35")) == 0)
			{
			INFO_PRINTF1(_L("data notification size equals\n"));	
			}		
		
		}

	
	r = iConfig->SetTraceSettings(_L8("enable"), Ulogger::KSecondaryGlobalFilter);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Enable all secondary filter failure \n"));
		errors++;
		}
		
	r = iConfig->SetTraceSettings(_L8("disable"), Ulogger::KSecondaryGlobalFilter);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Disable all secondary filter failure \n"));
		errors++;
		}
		
	r = iConfig->SetTraceSettings(_L8("current"), Ulogger::KSecondaryGlobalFilter);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Current secondary filter failure \n"));
		errors++;
		}
		
	
	r = iConfig->SetTraceSettings(_L8("circular"), Ulogger::KBufferMode);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Circular buffer mode failure \n"));
		errors++;
		}	
	
	r = iConfig->SetTraceSettings(_L8("straight"), Ulogger::KBufferMode);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Straight buffer mode failure \n"));
		errors++;
		}	

	CleanupStack::PopAndDestroy();
		if(errors == 0)
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	return TestStepResult();	
	
	}
	
	

/**
 * Test CConfig error cases.Call AddPlugin, RemoveChannel with various error cases.
 * @return - TVerdict code
 *
 */	
TVerdict CSysCfgTest4::doTestStepL()
	{
	RArray<TUint32> filters;
	TUint int_val;
	
	Ulogger::CConfigSettingsIter* iter=Ulogger::CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);
	TPtrC8 key;
	TPtrC8 value;
	
	TInt r, errors = 0;
	//Check for Secondary filters
	r = iConfig->GetSectionValues(KSecondaryFilterSection,*iter);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successfully go the iterarator for secondary filters\n"));				
		
		}
	else 
		errors++;
	
	while(iter->Next(key,value))
		{
		TLex8 lex_val(value);	
		lex_val.Val(int_val,EDecimal);	
		filters.Append(int_val);	
		}
		
	for(TInt i=0; i<filters.Count();i++)
		{
		INFO_PRINTF3(_L("Secondary Filter %d = %d \n"),i,filters.operator[](i ));	
		}
		
	r = iConfig->RemoveActiveFilter(filters,2);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successfully removed secondary filters\n"));				
		}
	else 
		{
		INFO_PRINTF2(_L("Error in removing secondary filters %d \n"),r);				
		errors++;
		}
	
	filters.Reset();
	
	//Check for Primary filters

	r = iConfig->GetSectionValues(KPrimaryFilterSection,*iter);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successfully go the iterarator for primary filters\n"));				
		
		}
	else 
		errors++;
	while(iter->Next(key,value))
		{
		TLex8 lex_val(value);	
		lex_val.Val(int_val,EDecimal);	
		filters.Append(int_val);	
		}
	
	
	for(TInt i=0; i<filters.Count();i++)
		{
		INFO_PRINTF3(_L("Primary Filter%d = %d \n"),i,filters.operator[](i ));	
		}

	r = iConfig->RemoveActiveFilter(filters,1);
	if(r == 0)
		{
		INFO_PRINTF1(_L("Successfully removed primary filters\n"));				
		
		}
	else 
		{
		INFO_PRINTF2(_L("Error in removing primary filters %d \n"),r);				
		errors++;
		}


	CleanupStack::PopAndDestroy();
	if(errors == 0)
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

