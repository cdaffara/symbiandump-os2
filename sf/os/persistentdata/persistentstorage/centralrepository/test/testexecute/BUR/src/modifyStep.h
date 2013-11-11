/**
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



/**
 @file
*/
#if (!defined __MODIFY_STEP_H__)
#define __MODIFY_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_centrep_BURSuiteStepBase.h"

// Values that will be created

const TUint32 KNewBackupInt = 0x140;
const TInt KIntBackupValue = 1;
const TUint32 KNewBackupReal = 0x150;
const TReal KRealBackupValue = 1.2;
const TUint32 KNewBackupString8 = 0x160;
_LIT8(KString8BackupValue, "A");
const TUint32 KNewBackupString16 = 0x170;
_LIT(KString16BackupValue, "yz");
const TUint32 KNewInt = 0x241;
const TInt KIntValue = 5;
const TUint32 KNewReal = 0x250;
const TReal KRealValue = 6.7;
const TUint32 KNewString8 = 0x260;
_LIT8(KString8Value, "Z");
const TUint32 KNewString16 = 0x270;
_LIT(KString16Value, "UU");

// Values that will be changed

const TInt KInt1_UpdatedValue = 0;
const TReal KReal1_UpdatedValue = 0.12;

class CmodifyStep : public CTe_centrep_BURSuiteStepBase
	{
public:
	CmodifyStep();
	~CmodifyStep();
	virtual TVerdict doTestStepL();

private:
	};

_LIT(KmodifyStep,"modifyStep");

#endif
