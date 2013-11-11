/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This contains CTestPlatSecLaunchApp. This class sets the capabilities
* of the executables. 
*
*/



#if (!defined __TEST_PLATSEC_LAUNCHAPP_H__)
#define __TEST_PLATSEC_LAUNCHAPP_H__

//TEF include
#include <test/testexecutestepbase.h>

/**
 This class sets the capabilities of the executables. 
 The executable name and its capabilities are read from
 the ini file. The executable "SetCap.exe" is used to
 set the capabilities.
*/
class CTestPlatSecLaunchApp : public CTestStep
	{
public:
	//Constructor
	CTestPlatSecLaunchApp();

	//Destructor
	~CTestPlatSecLaunchApp();

	//Implementation of CTestStep pure virtual funtion
	virtual TVerdict doTestStepL();
	};
#endif //__TEST_PLATSEC_LAUNCHAPP_H__
