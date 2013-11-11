/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file def130695_SuiteStepBase.h
 @internalTechnology
*/

#if (!defined ___DEF130695__STEP_BASE__)
#define ___DEF130695__STEP_BASE__
#include <test/testexecutestepbase.h>


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class C_def130695_SuiteStepBase : public CTestStep
	{
public:
	virtual ~C_def130695_SuiteStepBase();
	C_def130695_SuiteStepBase();

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif
