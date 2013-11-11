/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include "tpkcs10stepbasev2.h"


TVerdict CTPKCS10StepBase::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTPKCS10StepBase::doTestStepPostambleL()
	{
	SetTestStepResult(EPass);  
	return TestStepResult();
	}

CTPKCS10StepBase::~CTPKCS10StepBase()
	{
	}

CTPKCS10StepBase::CTPKCS10StepBase()
	{
	}
