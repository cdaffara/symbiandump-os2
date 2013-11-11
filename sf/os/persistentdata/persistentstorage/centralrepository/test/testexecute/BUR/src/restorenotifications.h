/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#if (!defined __RESTORENOTIFICATIONSSTEP_H__)
#define __RESTORENOTIFICATIONSSTEP_H__
#include <test/testexecutestepbase.h>
#include "Te_centrep_BURSuiteStepBase.h"

/**
These TEF test steps are designed for parts the test of restore 
notification of Central Repository. 

They must be run in the same thread by loading the test server with the 
switch of "SharedData".

See more information in the comment of the test server: 
CTe_centrep_BURObserverSuite 

They must be called in this order:
RestoreNotificationsInitialiseStep: initialise some global variables.
RestoreNotificationsRequestStep: setup the notification requests
RestoreNotificationsWaitStep: wait for the notification requests and check 
results of the restore.
RestoreNotificationsReleaseStep: clear memory allocated for the global variables.
*/
class CRestoreNotificationsInitialiseStep : public CTe_centrep_BURSuiteStepBase
	{
public:
	CRestoreNotificationsInitialiseStep();
	~CRestoreNotificationsInitialiseStep();
	virtual TVerdict doTestStepL();
	};

class CRestoreNotificationsRequestStep : public CTe_centrep_BURSuiteStepBase
	{
public:
	CRestoreNotificationsRequestStep();
	~CRestoreNotificationsRequestStep();
	virtual TVerdict doTestStepL();
	};

class CRestoreNotificationsWaitStep : public CTe_centrep_BURSuiteStepBase
	{
public:
	CRestoreNotificationsWaitStep();
	~CRestoreNotificationsWaitStep();
	virtual TVerdict doTestStepL();
	};

class CRestoreNotificationsReleaseStep : public CTe_centrep_BURSuiteStepBase
	{
public:
	CRestoreNotificationsReleaseStep();
	~CRestoreNotificationsReleaseStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KRestoreNotificationsInitialiseStep,"RestoreNotificationsInitialiseStep");
_LIT(KRestoreNotificationsRequestStep,"RestoreNotificationsRequestStep");
_LIT(KRestoreNotificationsWaitStep,"RestoreNotificationsWaitStep");
_LIT(KRestoreNotificationsReleaseStep,"RestoreNotificationsReleaseStep");

#endif
