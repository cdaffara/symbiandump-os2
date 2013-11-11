/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_uloggerfilepluginstep.h
 @internalTechnology
*/

#ifndef __TE_ULOGGERFILEPLUGINTESTERR_H__
#define __TE_ULOGGERFILEPLUGINTESTERR_H__

#include <test/testexecutestepbase.h>
#include "te_uloggermclsuitestepbase.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>

//negative testing
class CULoggerFilePluginStepErr : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerFilePluginStepErr();
	~CULoggerFilePluginStepErr();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};
	
_LIT(KULoggerFilePluginStepErr,"ULoggerFilePluginStepErr");

#endif
