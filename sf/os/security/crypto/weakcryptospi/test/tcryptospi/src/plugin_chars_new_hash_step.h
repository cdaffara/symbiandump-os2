/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#if (!defined __PLUGINCHARSNEWHASH_STEP_H__)
#define __PLUGINCHARSNEWHASH_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_cryptospistepbase.h"

class CPluginCharsNewHashStep : public CTe_CryptoSpiStepBase
	{
public:
	CPluginCharsNewHashStep();
	~CPluginCharsNewHashStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KPluginCharsNewHashStep,"PluginCharsNewHashStep");

#endif
