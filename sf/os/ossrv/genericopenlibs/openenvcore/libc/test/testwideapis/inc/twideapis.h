/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : twideapis.h
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




#ifndef TWIDEAPIS_H
#define TWIDEAPIS_H

//  INCLUDES
#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <wchar.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/syslimits.h>

_LIT(Kwfreopen_val, "wfreopen_val");
_LIT(Kwfreopen_valm, "wfreopen_valm");
_LIT(Kwfreopen_valinv, "wfreopen_valinv");
_LIT(Kwfdopen_val, "wfdopen_val");
_LIT(Kwfdopen_ivalm, "wfdopen_ivalm");
_LIT(Kgetws_val, "getws_val");
_LIT(Kgetws_null, "getws_null");
_LIT(Kputws_val1, "putws_val1");
_LIT(Kputws_val2, "putws_val2");
_LIT(Kputws_null, "putws_null");
_LIT(Kwremove_val1, "wremove_val1");
_LIT(Kwremove_val2, "wremove_val2");
_LIT(Kwremove_val3, "wremove_val3");
_LIT(Kwremove_null, "wremove_null");
_LIT(Kwfdopen_ivalm1, "wfdopen_ivalm1");
_LIT(Kwpopen_1, "wpopen_1");
_LIT(Kwfreopen1, "wfreopen1");
_LIT(Kwfreopen2, "wfreopen2");
_LIT(Kwfreopen3, "wfreopen3");
_LIT(Kwfreopen4, "wfreopen4");

class CTestWideApi : public CTestStep
	{
public:
	~CTestWideApi(); 
	CTestWideApi(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
protected:
	TInt wfreopen_val();
	TInt wfreopen_valm();            
	TInt wfreopen_valinv();
	TInt wfdopen_val();
	TInt wfdopen_ivalm();
 	TInt getws_val();
	TInt getws_null();
	TInt putws_val1();
	TInt putws_val2();
	TInt putws_null();	
	TInt wremove_val1();
	TInt wremove_val2();
	TInt wremove_val3();
	TInt wremove_null();
	TInt wfdopen_ivalm1();
	TInt wpopen_1();
	TInt wfreopen1(); 
	TInt wfreopen2();
	TInt wfreopen3();
	TInt wfreopen4();
	};
#endif //TWIDEAPIS_H



