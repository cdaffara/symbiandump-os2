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
* This contains CTestPlatSecCleanup. This class deletes the extra 
* executables which are created with required capabilities 
*
*/



#if (!defined __TEST_PLATSEC_CLEANUP_H__)
#define __TEST_PLATSEC_CLEANUP_H__

//TEF include
#include <test/testexecutestepbase.h>

/**
 This class deletes the executables created with extra capabilities. 
 The executable name is read from
 the ini file. The executable "Plattest_Platsec_Cleanup.exe" is used to
 delete the extra servers .
*/
class CTestPlatSecCleanup : public CTestStep
	{
public:
	//Constructor
	CTestPlatSecCleanup();

	//Destructor
	~CTestPlatSecCleanup();
	//Implementation of CTestStep pure virtual funtion
	virtual TVerdict doTestStepL();

private:
	void DeleteExtraServersL(const TDesC& aDestination);
	
		
	};
#endif //__TEST_PLATSEC_LAUNCHAPP_H__
