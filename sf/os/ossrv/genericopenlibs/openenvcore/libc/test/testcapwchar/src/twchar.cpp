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
// Name        : twchar.cpp
// 
//

 

#include "twchar.h"

CTestwchar::~CTestwchar() 
	{ 
	}  

CTestwchar::CTestwchar(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestwchar::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestwchar::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestwchar::doTestStepL()
	{
	int err;
    //captest steps
    if(TestStepName() == Kwfopensys)
 		{
   		INFO_PRINTF1(_L("wfopensys():"));
   		err = wfopensys();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kwfopenprivate)
   		{
   	   	INFO_PRINTF1(_L("wfopenprivate():"));
   	   	err = wfopenprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwmkdirsys)
   		{
   	   	INFO_PRINTF1(_L("wmkdirsys():"));
   	   	err = wmkdirsys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	
	else if(TestStepName() == Kwmkdirprivate)
   		{
   	   	INFO_PRINTF1(_L("wmkdirprivate():"));
   	   	err = wmkdirprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwaccesssys)
   		{
   	   	INFO_PRINTF1(_L("waccesssys():"));
   	   	err = waccesssys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	
	else if(TestStepName() == Kwaccessprivate)
   		{
   	   	INFO_PRINTF1(_L("waccessprivate():"));
   	   	err = waccessprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwchdirsys)
   		{
   	   	INFO_PRINTF1(_L("wchdirsys():"));
   	   	err = wchdirsys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwchdirprivate)
   		{
   	   	INFO_PRINTF1(_L("wchdirprivate():"));
   	   	err = wchdirprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kwchmodsys)
   		{
   	   	INFO_PRINTF1(_L("wchmodsys():"));
   	   	err = wchmodsys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwchmodprivate)
   		{
   	   	INFO_PRINTF1(_L("wchmodprivate():"));
   	   	err = wchmodprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwrenamesys)
   		{
   	   	INFO_PRINTF1(_L("wrenamesys():"));
   	   	err = wrenamesys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	
	else if(TestStepName() == Kwrenameprivate)
   		{
   	   	INFO_PRINTF1(_L("wrenameprivate():"));
   	   	err = wrenameprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwopensys)
   		{
   	   	INFO_PRINTF1(_L("wopensys():"));
   	   	err = wopensys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwopenprivate)
   		{
   	   	INFO_PRINTF1(_L("wopenprivate():"));
   	   	err = wopenprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwunlinksys)
   		{
   	   	INFO_PRINTF1(_L("wunlinksys():"));
   	   	err = wunlinksys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwunlinkprivate)
   		{
   	   	INFO_PRINTF1(_L("wunlinkprivate():"));
   	   	err = wunlinkprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	
	else if(TestStepName() == Kwopensys)
   		{
   	   	INFO_PRINTF1(_L("wstatsys():"));
   	   	err = wstatsys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwstatprivate)
   		{
   	   	INFO_PRINTF1(_L("wstatprivate():"));
   	   	err = wstatprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kwrmdirsys)
   		{
   	   	INFO_PRINTF1(_L("wrmdirsys():"));
   	   	err = wrmdirsys();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kwrmdirprivate)
   		{
   	   	INFO_PRINTF1(_L("wrmdirprivate():"));
   	   	err = wrmdirprivate();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	return TestStepResult(); 

	}
 
