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
* This contains CTestPlatSecSetCapabilities. This class sets the capabilities
* of the executables. 
*
*/



#if (!defined __TEST_PLATSEC_SET_CAPABILITIES_H__)
#define __TEST_PLATSEC_SET_CAPABILITIES_H__

//TEF include
#include <test/testexecutestepbase.h>

/**
 This class sets the capabilities of the executables. 
 The executable name and its capabilities are read from
 the ini file. The executable "SetCap.exe" is used to
 set the capabilities.
*/
class CTestPlatSecSetCapabilities : public CTestStep
	{
public:
	//Constructor
	CTestPlatSecSetCapabilities();

	//Destructor
	~CTestPlatSecSetCapabilities();

	//Implementation of CTestStep pure virtual funtion
	virtual TVerdict doTestStepL();
private:
	//Set the capabilites of the executables
	void SetCapabilitiesL(const TDesC & aExecutable, const TDesC & aCapabilities, const TDesC & aDestination);

	//Get the capability
	TInt GetCapability(TPtrC aCapability, TCapability& aCapabilityValue);

	//return the hex value for the capabilities
	TInt CreateHexCapabilities(TDesC& aCapabilities );

	};
#endif //__TEST_PLATSEC_SET_CAPABILITIES_H__
