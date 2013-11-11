// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __T_BACKUPSRVDEFECTS_H__
#define __T_BACKUPSRVDEFECTS_H__

#include <e32debug.h>
#include <e32test.h>
#include <babackup.h>

enum  BUDefectTestMachineStates
	 {
	 EMachineStart,
	 EMachineCloseAllsEndSameClient,
	 EMachineCloseAllsEndOtherClient,
	 EMachineCloseAllsEndCloseFile,
	 EMachineCloseFileEnd,
	 };
class CBackupFileObserver;

// The class to schedule the CBaBackupSessionWrapper requests for the Backup server defect tests.
class CBUDefectTestMachine : public CActive
	{
public:
	static CBUDefectTestMachine* NewL();
	~CBUDefectTestMachine();
	void RunL();	
	void DoCancel();
	void Complete();
	void SetFileObserver(CBackupFileObserver* aBackupFileObserver);
private: 
	CBUDefectTestMachine();
	void ConstructL();

private:
	CBaBackupSessionWrapper* iWrapper;	
	CBackupFileObserver* iBackupFileObserver;

public:	
	BUDefectTestMachineStates iNextState;
	};

	
class CBackupFileObserver : public CActive, public MBackupObserver
	{
public:
	static CBackupFileObserver* NewL(CBUDefectTestMachine& aTestMachine, const TDesC& aLockedFileName);
	~CBackupFileObserver();
	void RunL();	
	void DoCancel();
	void ChangeFileLockL(const TDesC& aFileName,TFileLockFlags aFlags);
	void CloseAll(MBackupObserver::TFileLockFlags aFlags);
	void CloseFileL(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlags);
	
private: 
	CBackupFileObserver(CBUDefectTestMachine& aTestMachine, const TDesC& aLockedFileName);
	void ConstructL();
private:
	CBUDefectTestMachine& iTestMachine;
	CBaBackupSessionWrapper* iWrapper;
	const TDesC& iLockedFileName;
	};
#endif // __T_BACKUPSRVDEFECTS_H__
