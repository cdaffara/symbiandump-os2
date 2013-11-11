/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : twchar.h
* This material, including documentation and any related 
* computer programs, is protected by copyright controlled by 
* Nokia Corporation. All rights are reserved. Copying, 
* including reproducing, storing,  adapting or translating, any 
* or all of this material requires the prior written consent of 
* Nokia Corporation. This material also contains confidential 
* information which may not be disclosed to others without the 
* prior written consent of Nokia Corporation.
*
*/



#ifndef TWCHAR_H
#define TWCHAR_H

#include <test/testexecutestepbase.h>
#include <wchar.h>
#include <inttypes.h>
#include <wctype.h>
#include <e32svr.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <_ansi.h>

#include <sys/stat.h> //S_ISWUSR
#include <sys/syslimits.h> //PATH_MAX
#include <unistd.h>


_LIT(Kwcsupr1, "wcsupr1");
_LIT(Kwcsupr2, "wcsupr2");
_LIT(Kwcsupr3, "wcsupr3");
_LIT(Kwcsupr4, "wcsupr4");
_LIT(Kwcsupr5, "wcsupr5");
_LIT(Kwcsupr6, "wcsupr6");

_LIT(Kwcslwr1, "wcslwr1");
_LIT(Kwcslwr2, "wcslwr2");
_LIT(Kwcslwr3, "wcslwr3");
_LIT(Kwcslwr4, "wcslwr4");
_LIT(Kwcslwr5, "wcslwr5");
_LIT(Kwcslwr6, "wcslwr6");

_LIT(Kwcsrev1, "wcsrev1");
_LIT(Kwcsrev2, "wcsrev2");
_LIT(Kwcsrev3, "wcsrev3");
_LIT(Kwcsrev4, "wcsrev4");
_LIT(Kwcsrev5, "wcsrev5");
_LIT(Kwcsrev6, "wcsrev6");

_LIT(Kwcsicmp1, "wcsicmp1");
_LIT(Kwcsicmp2, "wcsicmp2");
_LIT(Kwcsicmp3, "wcsicmp3");
_LIT(Kwcsicmp4, "wcsicmp4");
_LIT(Kwcsicmp5, "wcsicmp5");
_LIT(Kwcsicmp6, "wcsicmp6");
_LIT(Kwcsicmp7, "wcsicmp7");
_LIT(Kwcsicmp8, "wcsicmp8");
_LIT(Kwcsicmp9, "wcsicmp9");

_LIT(Kwcsset1, "wcsset1");
_LIT(Kwcsset2, "wcsset2");
_LIT(Kwcsset3, "wcsset3");
_LIT(Kwcsset4, "wcsset4");

_LIT(Kwcsnset1, "wcsnset1");
_LIT(Kwcsnset2, "wcsnset2");
_LIT(Kwcsnset3, "wcsnset3");
_LIT(Kwcsnset4, "wcsnset4");
_LIT(Kwcsnset5, "wcsnset5");
_LIT(Kwcsnset6, "wcsnset6");

_LIT(Kwstrtime1, "wstrtime1");
_LIT(Kwstrtime2, "wstrtime2");

_LIT(Kwstrdate1, "wstrdate1");
_LIT(Kwstrdate2, "wstrdate2");

_LIT(Kwcsnicmp1, "wcsnicmp1");
_LIT(Kwcsnicmp2, "wcsnicmp2");
_LIT(Kwcsnicmp3, "wcsnicmp3");
_LIT(Kwcsnicmp4, "wcsnicmp4");
_LIT(Kwcsnicmp5, "wcsnicmp5");
_LIT(Kwcsnicmp6, "wcsnicmp6");
_LIT(Kwcsnicmp7, "wcsnicmp7");
_LIT(Kwcsnicmp8, "wcsnicmp8");
_LIT(Kwcsnicmp9, "wcsnicmp9");

_LIT(Kwcsicoll1, "wcsicoll1");
_LIT(Kwcsicoll2, "wcsicoll2");
_LIT(Kwcsicoll3, "wcsicoll3");

_LIT(Kwcsncoll1, "wcsncoll1");
_LIT(Kwcsncoll2, "wcsncoll2");
_LIT(Kwcsncoll3, "wcsncoll3");

_LIT(Kwcsnicoll1, "wcsnicoll1");
_LIT(Kwcsnicoll2, "wcsnicoll2");
_LIT(Kwcsnicoll3, "wcsnicoll3");

_LIT(Kwtmpnam1, "wtmpnam1");
_LIT(Kwtmpnam2, "wtmpnam2");


class CTestWChar : public CTestStep
	{
public:
	~CTestWChar(); 
	CTestWChar(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
protected:

	//wcsupr
    TInt wcsuprTest1();
    TInt wcsuprTest2();
    TInt wcsuprTest3();
    TInt wcsuprTest4();
    TInt wcsuprTest5();
    TInt wcsuprTest6();
        
    //wcslwr
    TInt wcslwrTest1();
    TInt wcslwrTest2();
    TInt wcslwrTest3();
    TInt wcslwrTest4();
    TInt wcslwrTest5();
    TInt wcslwrTest6();
        
    //wcsrev
    TInt wcsrevTest1();
    TInt wcsrevTest2();
    TInt wcsrevTest3();
    TInt wcsrevTest4();
    TInt wcsrevTest5();
    TInt wcsrevTest6();
        
	//wcsicmp
    TInt wcsicmpTest1();
    TInt wcsicmpTest2();
    TInt wcsicmpTest3();
    TInt wcsicmpTest4();
    TInt wcsicmpTest5();
    TInt wcsicmpTest6();
    TInt wcsicmpTest7();
    TInt wcsicmpTest8();
    TInt wcsicmpTest9();
        
    //wcsset
    TInt wcssetTest1();
    TInt wcssetTest2();
    TInt wcssetTest3();
    TInt wcssetTest4();
    
    //wcsnset
    TInt wcsnsetTest1();
    TInt wcsnsetTest2();
    TInt wcsnsetTest3();
    TInt wcsnsetTest4();
    TInt wcsnsetTest5();
    TInt wcsnsetTest6();
        
    //wstrtime
    TInt wstrtimeTest1();
    TInt wstrtimeTest2();
        
    //wstrdate
    TInt wstrdateTest1();
    TInt wstrdateTest2();
        
    //wcsnicmp
    TInt wcsnicmpTest1();
    TInt wcsnicmpTest2();
    TInt wcsnicmpTest3();
    TInt wcsnicmpTest4();
    TInt wcsnicmpTest5();
    TInt wcsnicmpTest6();
    TInt wcsnicmpTest7();
    TInt wcsnicmpTest8();
    TInt wcsnicmpTest9();
        
	//wcsicoll
    TInt wcsicollTest1();
    TInt wcsicollTest2();
    TInt wcsicollTest3();
        
    //wcsncoll
    TInt wcsncollTest1();
    TInt wcsncollTest2();
    TInt wcsncollTest3();
        
    //wcsnicoll
    TInt wcsnicollTest1();
    TInt wcsnicollTest2();
    TInt wcsnicollTest3();
		
	//wtmpnam
    TInt wtmpnamTest1();
    TInt wtmpnamTest2();	
	};
	
#endif 


