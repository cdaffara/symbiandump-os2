// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Declaration of test execute steps for log engine backup and restore
// 
//

/**
 @file
 @internalComponent
*/

#ifndef BACKUPRESTORESTEP_H
#define BACKUPRESTORESTEP_H

#include <f32file.h>
#include <logcli.h>
#include <test/testexecutestepbase.h>
#include "logtestsession.h"
#include "testactive.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreStepBase  //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBackupRestoreStepBase : public CTestStep
	{
public:
	virtual ~CBackupRestoreStepBase();
	virtual TVerdict doTestStepPreambleL();

protected:
	void TestAddEventTypeL();
	void TestGetEventTypeL(TInt aExpectedError);
	void GetDataFromConfigL(TUid& aEventUid, TPtrC& aEventDesctiption);
	
protected:
	RFs iFs;
	CActiveScheduler *iScheduler;
	/** Client class for retrieving/setting values in log engine */
	CLogClient *iClient;
	/** Test active object for retrieving/setting values in log engine */
	CTestActive *iActive;
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreInitializeStep  ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBackupRestoreInitializeStep : public CBackupRestoreStepBase
	{
public:
	CBackupRestoreInitializeStep();
	virtual ~CBackupRestoreInitializeStep();
	virtual TVerdict doTestStepL();
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreVerifyStep  ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBackupRestoreVerifyStep : public CBackupRestoreStepBase
	{
public:
	CBackupRestoreVerifyStep();
	virtual ~CBackupRestoreVerifyStep();
	virtual TVerdict doTestStepL();
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CBackupRestoreVerifyStep2  ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBackupRestoreVerifyStep2 : public CBackupRestoreStepBase
	{
public:
	CBackupRestoreVerifyStep2();
	virtual ~CBackupRestoreVerifyStep2();
	virtual TVerdict doTestStepL();
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////  CStopLogServerStep  //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CStopLogServerStep : public CTestStep
	{
public:
	CStopLogServerStep();
	virtual ~CStopLogServerStep();
	virtual TVerdict doTestStepL();

private:
	/** Session with log engine to stop the server*/
	RLogTestSession iLogServ;
	};

_LIT(KBackupRestoreInitializeStep,"BackupRestoreInitializeStep");
_LIT(KBackupRestoreVerifyStep,"BackupRestoreVerifyStep");
_LIT(KBackupRestoreVerifyStep2,"BackupRestoreVerifyStep2");
_LIT(KStopLogServerStep,"StopLogServerStep");

#endif // #ifndef BACKUPRESTORESTEP_H
