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
* UloggerFilePluginTest.h
* 
*
*/



#ifndef __ULOGGERSERIALPLUGINTEST_H__
#define __ULOGGERSERIALPLUGINTEST_H__

#include <test/tefunit.h>

#include "uloggerserialplugin.h"
#include "te_serialsuitestepbase.h"
	
class CSerial0Step : public CTe_serialSuiteStepBase
	{
public:
	CSerial0Step();
	~CSerial0Step();
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

_LIT(KSerial0Step,"serial0Step");

#endif // __ULOGGERSERIALPLUGINTEST_H__
