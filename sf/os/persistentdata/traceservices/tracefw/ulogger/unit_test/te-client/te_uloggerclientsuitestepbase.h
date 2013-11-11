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
 @file Te_uloggerclientSuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_ULOGGERCLIENT_STEP_BASE__)
#define __TE_ULOGGERCLIENT_STEP_BASE__
#include <test/testexecutestepbase.h>

#include "uloggerclient.h"

#define KPrimaryFlt 	5
#define KSecondaryFlt   0x1024FFFF

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTestUloggerClientApiStepBase : public CTestStep
	{
public:
	virtual ~CTestUloggerClientApiStepBase();
	CTestUloggerClientApiStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict prepareForStartTestL();
	virtual TVerdict prepareForStopTestL();

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	Ulogger::RULogger* iSession;
	};

#endif
