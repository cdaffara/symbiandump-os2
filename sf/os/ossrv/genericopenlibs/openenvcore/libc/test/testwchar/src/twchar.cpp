// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//




// INCLUDE FILES
#include "twchar.h"


CTestWChar::~CTestWChar() 
	{ 
	}  

CTestWChar::CTestWChar(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestWChar::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestWChar::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestWChar::doTestStepL()
	{
		int err;

   		if(TestStepName() == Kwcsupr1)
   			{
   	   		INFO_PRINTF1(_L("wcsupr1():"));
   	   		err = wcsuprTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsupr2)
   			{
   	   		INFO_PRINTF1(_L("wcsupr2():"));
   	   		err = wcsuprTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsupr3)
   			{
   	   		INFO_PRINTF1(_L("wcsupr3():"));
   	   		err = wcsuprTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsupr4)
   			{
   	   		INFO_PRINTF1(_L("wcsupr4():"));
   	   		err = wcsuprTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsupr5)
   			{
   	   		INFO_PRINTF1(_L("wcsupr5():"));
   	   		err = wcsuprTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsupr6)
   			{
   	   		INFO_PRINTF1(_L("wcsupr6():"));
   	   		err = wcsuprTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcslwr1)
   			{
   	   		INFO_PRINTF1(_L("wcslwr1():"));
   	   		err = wcslwrTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcslwr2)
   			{
   	   		INFO_PRINTF1(_L("wcslwr2():"));
   	   		err = wcslwrTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcslwr3)
   			{
   	   		INFO_PRINTF1(_L("wcslwr3():"));
   	   		err = wcslwrTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcslwr4)
   			{
   	   		INFO_PRINTF1(_L("wcslwr4():"));
   	   		err = wcslwrTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcslwr5)
   			{
   	   		INFO_PRINTF1(_L("wcslwr5():"));
   	   		err = wcslwrTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcslwr6)
   			{
   	   		INFO_PRINTF1(_L("wcslwr6():"));
   	   		err = wcslwrTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsrev1)
   			{
   	   		INFO_PRINTF1(_L("wcsrev1():"));
   	   		err = wcsrevTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsrev2)
   			{
   	   		INFO_PRINTF1(_L("wcsrev2():"));
   	   		err = wcsrevTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsrev3)  
   	   		{
   	   		INFO_PRINTF1(_L("wcsrev3():"));
   	   		err = wcsrevTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsrev4)
   			{
   	   		INFO_PRINTF1(_L("wcsrev4():"));
   	   		err = wcsrevTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsrev5)
   			{
   	   		INFO_PRINTF1(_L("wcsrev5():"));
   	   		err = wcsrevTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsrev6)
   			{
   	   		INFO_PRINTF1(_L("wcsrev6():"));
   	   		err = wcsrevTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp1)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp1():"));
   	   		err = wcsicmpTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp2)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp2():"));
   	   		err = wcsicmpTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp3)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp3():"));
   	   		err = wcsicmpTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp4)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp4():"));
   	   		err = wcsicmpTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp5)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp5():"));
   	   		err = wcsicmpTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp6)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp6():"));
   	   		err = wcsicmpTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp7)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp7():"));
   	   		err = wcsicmpTest7();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp8)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp8():"));
   	   		err = wcsicmpTest8();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsicmp9)
   			{
   	   		INFO_PRINTF1(_L("wcsicmp9():"));
   	   		err = wcsicmpTest9();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsset1)
   			{
   	   		INFO_PRINTF1(_L("wcsset1():"));
   	   		err = wcssetTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsset2)
   			{
   	   		INFO_PRINTF1(_L("wcsset2():"));
   	   		err = wcssetTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsset3)
   			{
   	   		INFO_PRINTF1(_L("wcsset3():"));
   	   		err = wcssetTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsset4)
   			{
   	   		INFO_PRINTF1(_L("wcsset4():"));
   	   		err = wcssetTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsnset1)
   			{
   	   		INFO_PRINTF1(_L("wcsnset1():"));
   	   		err = wcsnsetTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsnset2)
   			{
   	   		INFO_PRINTF1(_L("wcsnset2():"));
   	   		err = wcsnsetTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsnset3)
   			{
   	   		INFO_PRINTF1(_L("wcsnset3():"));
   	   		err = wcsnsetTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsnset4)
   			{
   	   		INFO_PRINTF1(_L("wcsnset4():"));
   	   		err = wcsnsetTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsnset5)
   			{
   	   		INFO_PRINTF1(_L("wcsnset5():"));
   	   		err = wcsnsetTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwcsnset6)
   			{
   	   		INFO_PRINTF1(_L("wcsnset6():"));
   	   		err = wcsnsetTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwstrtime1)
   			{
   	   		INFO_PRINTF1(_L("wstrtime1():"));
   	   		err = wstrtimeTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwstrtime2)
   			{
   	   		INFO_PRINTF1(_L("wstrtime2():"));
   	   		err = wstrtimeTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwstrdate1)
   			{
   	   		INFO_PRINTF1(_L("wstrdate1():"));
   	   		err = wstrdateTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwstrdate2)
   			{
   	   		INFO_PRINTF1(_L("wstrdate2():"));
   	   		err = wstrdateTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp1)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp1():"));
   	   		err = wcsnicmpTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp2)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp2():"));
   	   		err = wcsnicmpTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp3)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp3():"));
   	   		err = wcsnicmpTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp4)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp4():"));
   	   		err = wcsnicmpTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp5)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp5():"));
   	   		err = wcsnicmpTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp6)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp6():"));
   	   		err = wcsnicmpTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp7)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp7():"));
   	   		err = wcsnicmpTest7();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicmp8)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp8():"));
   	   		err = wcsnicmpTest8();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}  
   	   	else
   	   		if(TestStepName() == Kwcsnicmp9)
   			{
   	   		INFO_PRINTF1(_L("wcsnicmp9():"));
   	   		err = wcsnicmpTest9();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}      	   		 	   		
   	   	else
   	   		if(TestStepName() == Kwcsicoll1)
   			{
   	   		INFO_PRINTF1(_L("wcsicoll1():"));
   	   		err = wcsicollTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsicoll2)
   			{
   	   		INFO_PRINTF1(_L("wcsicoll2():"));
   	   		err = wcsicollTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsicoll3)
   			{
   	   		INFO_PRINTF1(_L("wcsicoll3():"));
   	   		err = wcsicollTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsncoll1)
   			{
   	   		INFO_PRINTF1(_L("wcsncoll1():"));
   	   		err = wcsncollTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsncoll2)
   			{
   	   		INFO_PRINTF1(_L("wcsncoll2():"));
   	   		err = wcsncollTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsncoll3)
   			{
   	   		INFO_PRINTF1(_L("wcsncoll3():"));
   	   		err = wcsncollTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicoll1)
   			{
   	   		INFO_PRINTF1(_L("wcsnicoll1():"));
   	   		err = wcsnicollTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicoll2)
   			{
   	   		INFO_PRINTF1(_L("wcsnicoll2():"));
   	   		err = wcsnicollTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwcsnicoll3)
   			{
   	   		INFO_PRINTF1(_L("wcsnicoll3():"));
   	   		err = wcsnicollTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   		
   	   	else
   	   		if(TestStepName() == Kwtmpnam1)
   			{
   	   		INFO_PRINTF1(_L("wtmpnam1():"));
   	   		err = wtmpnamTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwtmpnam2)
   			{
   	   		INFO_PRINTF1(_L("wtmpnam2():"));
   	   		err = wtmpnamTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   		  
    return TestStepResult(); 
 }


 
