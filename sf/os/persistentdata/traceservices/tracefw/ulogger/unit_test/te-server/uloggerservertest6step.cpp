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
 @file UloggerServerTest6Step.cpp
 @internalTechnology
*/
#include "uloggerservertest6step.h"
#include "te_uloggerservertestsuitedefs.h"


namespace Ulogger
{

CUloggerServerTest6Step::~CUloggerServerTest6Step()
/**
 * Destructor
 */
	{
	
	}

CUloggerServerTest6Step::CUloggerServerTest6Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KUloggerServerTest6Step);
	}

TVerdict CUloggerServerTest6Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("****This is unit test for ControlDataManager class****"));	
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	iMan = new CInputData();
	if(!iMan)
		SetTestStepResult(EFail);

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
	  {
	  TBuf8<32> cmdDes;
	  cmdDes.Copy(_L8("set f1"));
	  ControlData* cData = iMan->CreatePackage((void*)cmdDes.Ptr(), cmdDes.Length());
	  if(!cData)
		  {
		  INFO_PRINTF1(_L("error"));	
		  SetTestStepResult(EFail);
		  }
	  
	  TBuf8<32> cmdDes3;
	  cmdDes3.Num(4);
	  iMan->AppendNewData(cData, (const void*)cmdDes3.Ptr(), cmdDes3.Length());
	  TInt chunks = iMan->GetChunksCount(cData);
	  if(chunks != 3)
		  {
		  INFO_PRINTF1(_L("error"));	
		  SetTestStepResult(EFail);
		  }
	  
	  //test method
	  TInt size = iMan->GetSize(cData);
	  
	  unsigned long cSize;
	  const void* _cmd3 = iMan->GetChunk(cData, 2, cSize);
	  TPtrC8 cmd3((const unsigned char*)_cmd3, cSize);
	  if(cmd3.Compare(cmdDes3) != 0)
		  {
		  INFO_PRINTF1(_L("error"));	
		  SetTestStepResult(EFail);
		  }

	  delete [] cData;
	  if(TestStepResult()!=EFail)
		  SetTestStepResult(EPass);
	  }
	  
	  return TestStepResult();
	}



TVerdict CUloggerServerTest6Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iScheduler;
	iScheduler = NULL;
	
	delete iMan;
	iMan = NULL;
	
	return TestStepResult();
	}
}
