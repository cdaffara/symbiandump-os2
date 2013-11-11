/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file authentication_TEF0Step.h
*/
#if (!defined __AUTHENTICATION_TEF0_STEP_H__)
#define __AUTHENTICATION_TEF0_STEP_H__

/*
	This test tests the functionality of the CAuthentication object. The following 
	is tested:
	1. The object is created supplying user name and password as descriptors.
	2. User name and password are checked to make sure they contain expected values.
	3. The object is created supplying a URI containing user name and password.
	4. User name and password are checked to make sure they contain expected values.
	5. The object is tested to make sure that it behaves appropriately for all possible
   	   	types of incomplete URI passed to it.
*/

#include <test/testexecutestepbase.h>
#include "Te_authentication_TEFSuiteStepBase.h"

// Test descriptors
_LIT8(KUser1, "UserName1");
_LIT8(KPwd1, "Pwd1");    
_LIT8(KUser2, "UserName2");
_LIT8(KPwd2, "Pwd2");
_LIT8(KUriUserInfoComplete, "http://UserName1:Pwd1@www.symbian.com:80/testexample.html");
_LIT8(KUriUserInfoIncomplete1, "http://UserName1@www.symbian.com:80/testexample.html");
_LIT8(KUriUserInfoIncomplete2, "http://UserName1:@www.symbian.com:80/testexample.html");
_LIT8(KUriUserInfoIncomplete3, "http://:Pwd1@www.symbian.com:80/testexample.html");
_LIT8(KUriUserInfoIncomplete4, "http://:@www.symbian.com:80/testexample.html");
_LIT8(KUriUserInfoIncomplete5, "http://@www.symbian.com:80/testexample.html");
_LIT8(KUriNoUserInfo, "http://www.symbian.com:80/testexample.html");

// Logging messages
_LIT(KNameNotRetreivedProperly, "Failed to retreive user name properly");
_LIT(KPasswordNotRetreivedProperly, "Failed to retreive password properly");
_LIT(KMethodNotRetreivedProperly, "Failed to retreive method properly");
_LIT(KFailedToLeaveWithKErrNotFound, "Failed to Leave with KErrNotFound");

class CAuthentication_TEF0Step : public CTe_authentication_TEFSuiteStepBase
	{
public:
	CAuthentication_TEF0Step();
	~CAuthentication_TEF0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KAuthentication_TEF0Step,"authentication_TEF0Step");

#endif
