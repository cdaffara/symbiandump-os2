/*
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



#ifndef __ULOGGERFILEPLUGINTEST_H__
#define __ULOGGERFILEPLUGINTEST_H__

#include <test/tefunit.h>

#include "uloggerfileplugin.h"
#include "te_filesuitestepbase.h"


class CFile0Step : public CTe_fileSuiteStepBase
	{
public:
CFile0Step();
	~CFile0Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepPreambleL(); 
	
private:
	// Tests
	void TestNewL();
	void TestUnlockResourcesL(); 
	void TestSettingsL();
	void TestWriteL(); 
	RFs iFileServer;
	};

_LIT(KFile0Step,"file0Step");

	
#endif // __ULOGGERFILEPLUGINTEST_H__
