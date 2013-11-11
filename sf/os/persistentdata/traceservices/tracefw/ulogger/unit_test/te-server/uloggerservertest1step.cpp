// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file UloggerServerTest1Step.cpp
 @internalTechnology
*/
#include "uloggerservertest1step.h"
#include "te_uloggerservertestsuitedefs.h"
#include "uloggerserver.h"
#include <e32const.h>
using namespace Ulogger;


CUloggerServerTest1Step::~CUloggerServerTest1Step()
/**
 * Destructor
 */
	{
	}

CUloggerServerTest1Step::CUloggerServerTest1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KUloggerServerTest1Step);
	}

TVerdict CUloggerServerTest1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest1Step::doTestStepL()
	{
	iErrors = 0;
	  if (TestStepResult()==EPass)
		{
		  iErrors += Test1L();//CULoggerServer::NewLC
		  iErrors += Test2L();//CULoggerServer::IncrementSessions
		  iErrors += Test3L();//CULoggerServer::Stop
		  iErrors += Test4L();//CULoggerServer::GetPrimaryFilters
		  iErrors += Test5L();//CULoggerServer::GetSecondaryFilters
		  iErrors += Test6L();//CULoggerServer::RemoveFilterfromConfig
		  iErrors += Test7L();//CULoggerServer::EnablePrimaryFilter
		  iErrors += Test8L();//CULoggerServer::SetSecondaryFilter
		  iErrors += Test9L();//CULoggerServer::Start
		  iErrors += Test12L();//CULoggerServer::SetActiveOutputMedia
		  iErrors += Test13L();//CULoggerServer::SetOutputPluginSettings
		  iErrors += Test14L();//CULoggerServer::GetActiveOutput
		  iErrors += Test15L();//CULoggerServer::GetInstalledPlugins
		  iMediaCount = 0;
		  iErrors += Test16L();//CULoggerServer::GetOutputPluginSettingsL
		  iErrors += Test18L();//CULoggerServer::SetBufferSizeL
		  iErrors += Test19L();//CULoggerServer::SetDataNotificationSizeL
		  iErrors += Test20L();//CULoggerServer::GetBufferSize
		  iErrors += Test21L();// CULoggerServer::GetDataNotificationSize
		  iErrors += Test22L();//CULoggerServer::SetGlobalSecondaryFilters
		  iErrors += Test23L();//CULoggerServer::ReadBufferL
		  iErrors += Test25L();//CULoggerServer::SetBufferMode
		  iErrors += Test26L();//CULoggerServer::UnconfigureCurrentChannel
		  iErrors += Test27L();//CULoggerServer::DoPostProcessing
		  iErrors += Test28L();//CULoggerServer::RemoveOutputPluginSettingsL
		  iErrors += Test29L();//CULoggerServer::GetInstalledControlPlugins
		  iErrors += Test30L();//CULoggerServer::SetActiveControlMedia
		  iErrors += Test31L();//CULoggerServer::ProcessCommandL
		  
		//display results
		TBuf<128> res;
		res.AppendFormat(_L("%d errors"), iErrors);
		INFO_PRINTF1(_L("****Results****"));
		INFO_PRINTF1(res);
		if(iErrors == 0)  
			SetTestStepResult(EPass);
			else
				SetTestStepResult(EFail);
		}
	  return TestStepResult();
	}



TVerdict CUloggerServerTest1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iScheduler;
	iScheduler = NULL;
	return TestStepResult();
	}

TInt CUloggerServerTest1Step::Test1L()
{//CULoggerServer::NewLC
	TInt errors = 0;
  	INFO_PRINTF1(_L("Testing - CULoggerServer::NewLC method"));
  	CULoggerServer *server = NULL;
  	server = CULoggerServer::NewLC(EPriorityBackground);
    if(server == NULL)
    	{
    	INFO_PRINTF1(_L("error"));
    	++errors;
    	}
    	else
    		CleanupStack::PopAndDestroy();
    return errors;
}

TInt CUloggerServerTest1Step::Test2L()
{//CULoggerServer::IncrementSessions
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::IncrementSessions method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	server->IncrementSessions();
	    	//note it's not possible to test method DecrementSessions without 
	    	//previous connection as server always stop CActiveScheduler when last session will dissconnect.
	    	//look for CULoggerServer::DecrementSessions() method
	    	CleanupStack::PopAndDestroy();
	    }
	 return errors;
}
  	
TInt CUloggerServerTest1Step::Test3L()
{//CULoggerServer::Stop
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::Stop method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TInt e = server->Stop();
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy();
	    }
	return errors;
}

TInt CUloggerServerTest1Step::Test4L()
{//CULoggerServer::GetPrimaryFilters
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::GetPrimaryFilters method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TUint32> array;
	    	TInt e = server->GetActiveFilters(array,EPrimaryFilter);
	    	if(e!=KErrNone) errors++;
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	buf.AppendFormat(_L("Num. of filters - %d"), array.Count());
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    	array.Close();
	    }
	return errors;
}	

TInt CUloggerServerTest1Step::Test5L()
{//CULoggerServer::GetSecondaryFilters
	TInt errors = 0;
    INFO_PRINTF1(_L("Testing - CULoggerServer::GetSecondaryFilters method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TUint32> array;
	    	TInt e = server->GetActiveFilters(array,ESecondaryFilter);
	    	if(e!=KErrNone) errors++;
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	buf.AppendFormat(_L("Num. of filters - %d"), array.Count());
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    	array.Close();
	    }
	return errors;
}	

TInt CUloggerServerTest1Step::Test6L()
{//CULoggerServer::RemoveFilterfromConfig
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::RemoveFilterfromConfig method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {	RArray<TUint32> filters;
			//CleanupClosePushL(filters);
			for(TInt i=20;i<40;i++)
			{
				filters.AppendL(i);
			}
			//	filters->AppendL(i);
	    	TInt e = server->RemoveActiveFilter(filters,EPrimaryFilter);
	    	filters.Close();
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    
	    	//CleanupStack::PopAndDestroy(); //filters
	    	CleanupStack::PopAndDestroy(); //server
	//}
	    }
	return errors;
}	

TInt CUloggerServerTest1Step::Test6AL()
{//CULoggerServer::RemoveFilterfromConfig
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::RemoveFilterfromConfig method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TUint32> filters;
			CleanupClosePushL(filters);
			for(TUint32 i=321;i<400;i++)
				filters.AppendL(i);
	    	TInt e = server->RemoveActiveFilter(filters,ESecondaryFilter);;
	    	filters.Close();
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    
	    	CleanupStack::PopAndDestroy(&filters); //server
	    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
}	

TInt CUloggerServerTest1Step::Test7L()
{//CULoggerServer::EnablePrimaryFilter
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::SetPrimaryFilter method"));
	CULoggerServer *server = NULL;
	server=CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TUint32> aCategory;
	   	    	
	    	aCategory.Append(3);
	    	aCategory.Append(14);
	    	aCategory.Append(194);
	    	
	    	TInt e = KErrNone;
	    	e = server->SetActiveFilterL(aCategory,EPrimaryFilter);
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
}


TInt CUloggerServerTest1Step::Test8L()
{//CULoggerServer::SetSecondaryFilter
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::SetSecondaryFilter method"));
	CULoggerServer *server = NULL;
	server=CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TUint32> array;
	    	array.Append(34);
	    	TInt e = server->SetActiveFilterL(array,ESecondaryFilter);
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    	array.Close();
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test9L()
{//CULoggerServer::Start
	TInt errors = 0;
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	/*
	    	 * here we are going to test sequence off methods
	    	 * */
	    	TBuf<128> buf;
	    	INFO_PRINTF1(_L("Testing - CULoggerServer::Start method"));
	    	TInt e = server->Start();
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	INFO_PRINTF1(_L("Testing - CULoggerServer::SetGlobalSecondaryFilters method"));
	    	e = server->SetSecondaryFiltering(_L8("current"));
	    	buf.Zero();
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	INFO_PRINTF1(_L("Testing - CULoggerServer::Stop method"));
	    	e = server->Stop();
	    	buf.Zero();
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }



TInt CUloggerServerTest1Step::Test12L()
{//CULoggerServer::SetActiveOutputPlugin
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::SetActiveOutputPlugin method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TInt e = server->SetActiveOutputPlugin(_L8("media1"));
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test13L()
{//CULoggerServer::SetOutputPluginSettings
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::SetOutputPluginSettings method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TPtrC8> value;
	    	value.AppendL(_L8("output_path"));
	    	value.AppendL(_L8("c:\\logs\\ulogger.log"));
	    	TInt e = server->SetPluginSettings(_L8("media1"),value);// _L8("output_path"), _L8("c:\\logs\\ulogger.log"));
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test14L()
{//CULoggerServer::GetActiveOutput
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::GetActiveOutputPlugin method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TPtrC8> listBuffer;
	    	TInt e = server->GetActiveOutputPlugin(listBuffer);
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	if(listBuffer.Count()>0)
	    	{
	    		INFO_PRINTF1(_L("values:"));
	    		buf.Copy(listBuffer[0]);					    		
	    		INFO_PRINTF1(buf);
	    	}
	    	listBuffer.Close();
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test15L()
{//CULoggerServer::GetInstalledPlugins
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::GetInstalledOutputPlugins method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	RArray<TPtrC8> listBuffer;
	    	TInt e = server->GetInstalledOutputPlugins(listBuffer);
	    	TBuf<128> buf;
	    	buf.AppendFormat(_L("Code returned from method - %d"), e);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
    		INFO_PRINTF1(_L("values:"));
	    	for(TInt i=0; i<listBuffer.Count(); ++i)
	    	{
	    		buf.Copy(listBuffer[i]);					    		
	    		INFO_PRINTF1(buf);
	    	}
	    	iMediaCount = listBuffer.Count()/2;
	    	listBuffer.Close();
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }
TInt CUloggerServerTest1Step::Test16L()
{//CULoggerServer::GetOutputPluginSettingsL
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::GetOutputPluginSettingsL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	for(TInt k=0; k<iMediaCount; k++)
	    		{
	    		TBuf8<32> mediaBuf;
	    		mediaBuf.AppendFormat(_L8("media%d"),k+1);
	    		RArray<TPtrC8> mediaList;
	    		TInt e = server->GetOptionsSettingsL(mediaBuf, mediaList);
	    		TBuf<128> buf;
	    		buf.AppendFormat(_L("Code returned from method - %d"), e);
	    		INFO_PRINTF1(buf);
	    		buf.Zero();
	    		INFO_PRINTF1(_L("values:"));
	    		for(TInt i=0; i<mediaList.Count(); ++i)
	    			{
	    			buf.Copy(mediaList[i]);					    		
	    			INFO_PRINTF1(buf);
	    			}
	    		mediaList.Close();
	    		}
	    	
	    	CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }



TInt CUloggerServerTest1Step::Test18L()
{//CULoggerServer::SetBufferSizeL
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::SetBufferSizeL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TInt e;
	    	INFO_PRINTF1(_L("testing value: -1"));
	    	e = server->SetBufferSizeL(-1);
	    	TBuf<128> buf;
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 0"));
	    	e = server->SetBufferSizeL(0);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 100"));
	    	e = server->SetBufferSizeL(100);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 65535"));
	    	e = server->SetBufferSizeL(65535);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 65536"));
	    	e = server->SetBufferSizeL(65536);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		
    		INFO_PRINTF1(_L("testing value: 165537"));
	    	e = server->SetBufferSizeL(165537);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }


TInt CUloggerServerTest1Step::Test19L()
{//CULoggerServer::SetDataNotificationSizeL
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::SetDataNotificationSizeL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TInt e;
	    	INFO_PRINTF1(_L("testing value: -1"));
	    	e = server->SetDataNotificationSizeL(-1);
	    	TBuf<128> buf;
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 0"));
	    	e = server->SetDataNotificationSizeL(0);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 100"));
	    	e = server->SetDataNotificationSizeL(100);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 65535"));
	    	e = server->SetDataNotificationSizeL(65535);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();

	    	INFO_PRINTF1(_L("testing value: 65536"));
	    	e = server->SetDataNotificationSizeL(65536);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		
    		INFO_PRINTF1(_L("testing value: 85536"));
	    	e = server->SetDataNotificationSizeL(85536);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		
    		INFO_PRINTF1(_L("testing value: 165538"));
	    	e = server->SetDataNotificationSizeL(165538);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }


TInt CUloggerServerTest1Step::Test20L()
{//CULoggerServer::GetBufferSize
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::GetBufferSize method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TInt e;
	    	INFO_PRINTF1(_L("testing value: -1"));
	    	server->GetBufandDataNotifyValuesL(KBuffer,e);
	    	TBuf<128> buf;
    		buf.AppendFormat(_L("Value - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }


TInt CUloggerServerTest1Step::Test21L()
{// CULoggerServer::GetDataNotificationSize
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::GetDataNotificationSize method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TInt e;
	    	INFO_PRINTF1(_L("testing value: -1"));
	    	server->GetBufandDataNotifyValuesL(KDataNotification,e);
	    	TBuf<128> buf;
    		buf.AppendFormat(_L("Value - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test22L()
{//CULoggerServer::SetGlobalSecondaryFilters
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::SetSecondaryFiltering method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	_LIT8(KEnable,"enable");
	  		_LIT8(KDisable,"disable");
	    	TInt e;
	    	TBuf<128> buf;
	    	buf.Copy(KEnable);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	e = server->SetSecondaryFiltering(KEnable);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		
    		buf.Copy(KDisable);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	e = server->SetSecondaryFiltering(KDisable);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test23L()
{//CULoggerServer::ReadBufferL
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::ReadBufferL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	TBuf<128> buf;
	    	TRAPD(err, server->ReadBufferL());
    		buf.AppendFormat(_L("Results of calling method - %d"), err);
    		INFO_PRINTF1(buf);
    		
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }

TInt CUloggerServerTest1Step::Test24L()
{//CULoggerServer::DataNotification
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::DataNotification method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	server->DataNotification();
    		INFO_PRINTF1(_L("OK"));	
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }
 
 
 TInt CUloggerServerTest1Step::Test25L()
{//CULoggerServer::SetBufferMode
	TInt errors = 0;
		INFO_PRINTF1(_L("Testing - CULoggerServer::SetGlobalSecondaryFilters method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
	    {
	    INFO_PRINTF1(_L("error"));
	    ++errors;
	    }
	    else
	    {
	    	_LIT8(KCircular,"circular");
	  		_LIT8(KStraight,"straight");
	    	TInt e;
	    	TBuf<128> buf;
	    	buf.Copy(KCircular);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	e = server->SetBufferMode(KCircular);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		
    		buf.Copy(KStraight);
	    	INFO_PRINTF1(buf);
	    	buf.Zero();
	    	e = server->SetBufferMode(KStraight);
    		buf.AppendFormat(_L("Code returned from method - %d"), e);
    		INFO_PRINTF1(buf);
    		buf.Zero();
    		CleanupStack::PopAndDestroy(); //server
	    }
	return errors;
 }
 
TInt CUloggerServerTest1Step::Test26L() //CULoggerServer::UnconfigureCurrentChannel
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::UnconfigureCurrentChannel method"));
	/* //removed - method isn't used anywhere
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
		{
		INFO_PRINTF1(_L("error"));
		++errors;
		}
	else
		{
    	server->UnconfigureCurrentChannel();
		CleanupStack::PopAndDestroy(); //server
		}
	*/
	return errors;
	}


TInt CUloggerServerTest1Step::Test27L()//CULoggerServer::DoPostProcessing
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::DoPostProcessing method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
		{
		INFO_PRINTF1(_L("error"));
		++errors;
		}
	else
		{
		INFO_PRINTF1(_L("DoPostProcessing(EUnknown)"));
    	server->DoPostProcessing(EUnknown);
		INFO_PRINTF1(_L("DoPostProcessing(ERestart)"));
    	server->DoPostProcessing(ERestart);
    	INFO_PRINTF1(_L("DoPostProcessing(EStop)"));
    	server->DoPostProcessing(EStop);
    	
    	CleanupStack::PopAndDestroy(); //server
		}
	return errors;
	}


TInt CUloggerServerTest1Step::Test28L()//CULoggerServer::RemoveOutputPluginSettingsL
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::RemoveOutputPluginSettingsL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
		{
		INFO_PRINTF1(_L("error"));
		++errors;
		}
	else
		{
		TInt errCode = KErrNone;
		RArray<TPtrC8> set;
		set.AppendL(_L8("testKey"));
		set.AppendL(_L8("testVal"));
		errCode = server->SetPluginSettings(_L8("uloggerserialplugin"), set);
		set.Close();
		if(errCode != KErrNone)
			{
			INFO_PRINTF1(_L("error"));
			CleanupStack::PopAndDestroy(); //server
			return ++errors;
			}
		
		INFO_PRINTF1(_L("RemoveOutputPluginSettingsL - uloggerserialplugin)"));
    	errCode = server->RemovePluginSettingsL(_L8("uloggerserialplugin"));
    	if(errCode != KErrNone)
			{
			INFO_PRINTF1(_L("error"));
			CleanupStack::PopAndDestroy(); //server
			return ++errors;
			}

    	INFO_PRINTF1(_L("RemoveOutputPluginSettingsL - dummyname)"));
    	server->RemovePluginSettingsL(_L8("dummyname"));
    	
    	CleanupStack::PopAndDestroy(); //server
		}
	return errors;
	}
	

TInt CUloggerServerTest1Step::Test29L()//CULoggerServer::GetInstalledInputPluginsL
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::GetInstalledInputPluginsL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	if(server == NULL)
		{
		INFO_PRINTF1(_L("error"));
		++errors;
		}
	else
		{
		RArray<TPtrC8> list;
    	server->GetInstalledInputPluginsL(list);
    	list.Close();
    	CleanupStack::PopAndDestroy(); //server
		}
	return errors;
	}


TInt CUloggerServerTest1Step::Test30L()//CULoggerServer::SetActiveInputPlugin  + DeActivateInputPlugin 
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::SetActiveInputPlugin  + DeActivateInputPlugin methods"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	
	_LIT8(KUsbPlugin,"uloggerusbplugin");
	server->DeActivateInputPlugin(KUsbPlugin);
	TInt errCode = server->SetActiveInputPlugin(KUsbPlugin);
	if(errCode != KErrNone && errCode != KErrNotFound)
		{
		INFO_PRINTF1(_L("error"));
		return ++errors;
		}
	errCode = server->SetActiveInputPlugin(KUsbPlugin);
	
	errCode = server->DeActivateInputPlugin(KUsbPlugin);
	TBuf<128> b;
	b.AppendFormat(_L("err1 %d"), errCode);
	INFO_PRINTF1(b);
	if(errCode != KErrNone && errCode != KErrNotFound)
		{
		INFO_PRINTF1(_L("error"));
		return ++errors;
		}
	errCode = server->DeActivateInputPlugin(KUsbPlugin);
	b.Zero();
	b.AppendFormat(_L("err2 %d"), errCode);
	INFO_PRINTF1(b);
	if(errCode != KErrNone && errCode != KErrNotFound && errCode != KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("error"));
		return ++errors;
		}
	
	server->Stop();
    CleanupStack::PopAndDestroy(); //server
    User::After(1*1000*1000);
	return errors;
	}


TInt CUloggerServerTest1Step::Test31L()//CULoggerServer::ProcessCommandL ver1
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerServer::ProcessCommandL method"));
	CULoggerServer *server = NULL;
	server = CULoggerServer::NewLC(EPriorityBackground);
	
	RArray<TPtrC8> arguments;
	//test all commands with empty array of arguments
	for(TInt i=0; i<=EVersion; i++)
		{
		TBuf<32> buf;
		buf.AppendFormat(_L("command number: %d"), i);
		INFO_PRINTF1(buf);
		ControlData *cd = server->ProcessCommandL((TCommand)i, arguments);
		if(!cd)
			{
			++errors;
			break;
			}
		delete cd;
		cd = NULL;
		}
	server->Stop();
	CleanupStack::PopAndDestroy(); //server
	return errors;
	}
