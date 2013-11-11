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


#include "rtautilsstep.h"
#include <test/testexecutelog.h>


CRTAUtilsStep::CRTAUtilsStep()
	{
	}

CRTAUtilsStep::~CRTAUtilsStep()
	{
	}

TVerdict CRTAUtilsStep::doTestStepPreambleL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	return TestStepResult();
	}

TVerdict CRTAUtilsStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
	return TestStepResult();
	}


