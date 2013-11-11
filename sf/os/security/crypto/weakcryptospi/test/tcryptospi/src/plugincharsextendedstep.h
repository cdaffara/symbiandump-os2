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
#if (!defined __PLUGINCHARSEXTENDED_STEP_H__)
#define __PLUGINCHARSEXTENDED_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_cryptospistepbase.h"

class CPluginCharsExtendedStep : public CTe_CryptoSpiStepBase
	{
public:
	CPluginCharsExtendedStep();
	~CPluginCharsExtendedStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:									
	};

_LIT(KPluginCharsExtendedStep,"PluginCharsExtendedStep");

#endif
