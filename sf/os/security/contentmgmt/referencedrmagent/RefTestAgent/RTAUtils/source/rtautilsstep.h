/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @file
 @internalAll
 @released
*/

#ifndef __RTAUTILSSTEP_H__
#define __RTAUTILSSTEP_H__

#include <test/testexecutestepbase.h>

class CRTAUtilsServer;

//Base class used to provide utility functions availble to test step classes
class CRTAUtilsStep : public CTestStep
	{
public:
	CRTAUtilsStep();
	virtual ~CRTAUtilsStep();

	/* 
	* Implementation of CTestStep virtual functions, create and install a CActiveScheduler
	*/
	virtual TVerdict doTestStepPreambleL();

	/* 
	* Implementation of CTestStep virtual functions, uninstalls and removes the CActiveScheduler
	*/
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler *iActiveScheduler;
	TUid iDtorIDKey;
	};

#endif
