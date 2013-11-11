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
*
*/

#include "leaveforever.h"

CLeaveForever::CLeaveForever()
	{
	SetTestStepName(KLeaveForever);
	}

CLeaveForever::~CLeaveForever()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * This step tests the behavior of TEF when leave occurs.
 */
TVerdict CLeaveForever::doTestStepL()
	{
	INFO_PRINTF1(_L("This step should leave!"));
	SetTestStepResult(EFail);
	TInt TheInt;
	_LIT(KLeaveNumber,"leavenum");
	if (!GetIntFromConfig(ConfigSection(),KLeaveNumber, TheInt))
		{
		ERR_PRINTF2(_L("The Leave Number section \"%S\" not found"),KLeaveNumber);
		}
	else
		{
		INFO_PRINTF2(_L("The Leave Number is %d"),TheInt);
		SetTestStepResult(EPass);
		User::Leave(TheInt);
		ERR_PRINTF1(_L("Test failed as there is no leave occurs!"));
		//This line should never approach if leave occurs.
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}
