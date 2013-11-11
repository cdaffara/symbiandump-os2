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
//

/**
 @file UloggerServerTest4Step.cpp
 @internalTechnology
*/


#include "uloggerservertest4step.h"
#include "te_uloggerservertestsuitedefs.h"
#include "uloggerwatcher.h"
#include "uloggerdatatypes.h"

namespace Ulogger
{

CUloggerServerTest4Step::~CUloggerServerTest4Step()
/**
 * Destructor
 */
	{
	
	}

CUloggerServerTest4Step::CUloggerServerTest4Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KUloggerServerTest4Step);
	}

TVerdict CUloggerServerTest4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("****This is unit test for CControlFramework class****"));	
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	RPointerArray<TPluginConfiguration> controlPluginSettings;
	INFO_PRINTF1(_L("INFO: building plugin allocator with input plugin..."));	
	TRAPD(err, iPluginAllocator = CPluginAllocator::NewL(KFileTestPluginTest(), KUsbTestPluginTest()));
	INFO_PRINTF1(_L("INFO: plugin allocator created"));	
	if(err == KErrNone)
		{
		MInputPlugin* inputPlugin = iPluginAllocator->GetInputPlugin();
		if(inputPlugin)
			{
			iCF = CInputFramework::NewL(inputPlugin, controlPluginSettings, this);
			iMan = new CInputData();
			}
		else
			{
			INFO_PRINTF1(_L("INFO: input pluigin not avaiable."));	
			iCF = NULL;
			iMan = NULL;
			}
		}
	else
		{
		INFO_PRINTF1(_L("plugin allocator not fully created - skiping step."));	
		iCF = NULL;
		iMan = NULL;
		}
	
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest4Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  iErrors = 0;
	  if (TestStepResult()==EPass)
	  {
	  if(iCF && iMan)
		  {
		  iErrors += Test1L(); //CControlFramework::StartReading + StopReading
		  iErrors += Test2L(); //CControlFramework:: testing functionality of passing and translating commands
		
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
	  else
		  SetTestStepResult(EPass);
	  }
	  
	  return TestStepResult();
	}



TVerdict CUloggerServerTest4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	if(iCF)
		{
		iCF->StopReading();
		delete iCF;
		iCF = NULL;
		}
	delete iMan;
	
	delete iPluginAllocator;
	iPluginAllocator = NULL;
	
	delete iScheduler;
	iScheduler = NULL;
	
	return TestStepResult();
	}


ControlData* CUloggerServerTest4Step::ProcessCommandL(TCommand /*aOpCode*/, RArray<TPtrC8> &aArguments)
	{
	INFO_PRINTF1(_L("****Incominng parameters:****"));
	for(TInt i=0; i<aArguments.Count(); i++)
		{
		TBuf<256> b;
		b.Copy(aArguments[i]);
		INFO_PRINTF1(b);
		}
	
	return iMan->CreatePackage((void*)"-5",2);
	}

void CUloggerServerTest4Step::DoPostProcessing(TCommand /*aCmd*/)
	{
	
	}

TInt CUloggerServerTest4Step::Test1L()
	{
	TInt errors = 0;
	
	INFO_PRINTF1(_L("****Testing CControlFramework::StartReading****"));
	TInt errCode = iCF->StartReading();
	TBuf<32> buf;
	buf.AppendFormat(_L("code returned from method: %d"), errCode);
	INFO_PRINTF1(buf);
	
	INFO_PRINTF1(_L("****Testing CControlFramework::StopReading****"));
	iCF->StopReading();
	
	return errors;
	}


void CUloggerServerTest4Step::TestFunctionalityL(const TDesC8& aCommand)
	{
	ControlData* cData = NULL;
	cData = iMan->CreatePackage( (void*)aCommand.Ptr(), aCommand.Length());
	iCF->iDataPtr.Zero();
	iCF->iDataPtr.Copy((const TUint8*)cData, iMan->GetSize(cData));
	iCF->iStatus = KErrNone;
	iCF->RunL();
	delete [] cData;
	}


TInt CUloggerServerTest4Step::Test2L()
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("****Testing CControlFramework functionality****"));
	
	INFO_PRINTF1(_L("start command:"));
	TestFunctionalityL(_L8("-r"));
	
	INFO_PRINTF1(_L("stop command:"));
	TestFunctionalityL(_L8("-q"));
	
	INFO_PRINTF1(_L("set f1 command:"));
	TestFunctionalityL(_L8("-ef 4 5 6 7"));

	INFO_PRINTF1(_L("set f2 command:"));
	TestFunctionalityL(_L8("-es 114 115 1116 111117"));

	INFO_PRINTF1(_L("set ps command:"));
	TestFunctionalityL(_L8("-ec uloggerfileplugin output_path e:\\test.ulog"));

	INFO_PRINTF1(_L("set ia command:"));
	TestFunctionalityL(_L8("-ei uloggerusbplugin"));

	
	return errors;
	}

}
