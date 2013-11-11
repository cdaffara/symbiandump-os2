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


#ifndef TSERVICECONFIG_H
#define TSERVICECONFIG_H

#include <test/testexecutestepbase.h>
#include <f32file.h>
#include "policycache.h"
#include "serviceconfig.h"

/**
Tests the CPolicyCache::ServiceConfigL
*/
class CServiceConfigTestStep : public CTestStep
	{
public:
	CServiceConfigTestStep();
	~CServiceConfigTestStep();

	// From CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
	
private:
	UserPromptService::TAuthorisationPolicy PolicyEnumFromString(const TDesC& aEnumString);
	
	RFs iFs;										///< FS session for policy cache
	UserPromptService::CPolicyCache* iPolicyCache;	///< Policy cache for access to service config settings
	};
_LIT(KServiceConfigStep,"serviceconfig");

#endif // TSERVICECONFIG_H
