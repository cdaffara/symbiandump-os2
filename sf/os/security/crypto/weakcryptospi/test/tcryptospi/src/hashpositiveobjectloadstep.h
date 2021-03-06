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
#if (!defined __HASHPOSITIVEOBJECTLOAD_STEP_H__)
#define __HASHPOSITIVEOBJECTLOAD_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_cryptospistepbase.h"

class CHashPositiveObjectLoadStep : public CTe_CryptoSpiStepBase
	{
public:
	CHashPositiveObjectLoadStep();
	~CHashPositiveObjectLoadStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KHashPositiveObjectLoadStep,"HashPositiveObjectLoadStep");

#endif

