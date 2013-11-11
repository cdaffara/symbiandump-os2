/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_CERTSTOREACTIONSCLIENT_H__
#define __T_CERTSTOREACTIONSCLIENT_H__

#include "t_certstoreactions.h"

class CInitCertificateAppInfoManager : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs &aFs,
		CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs &aFs, 
		CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CInitCertificateAppInfoManager();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CInitCertificateAppInfoManager(RFs &aFs, 
		CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	void HandleEInit();

	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EInit,
		EFinished
		};

private:
	TState iState;
	RFs& iFs;
	};

class CDeleteCertificateAppInfoManager : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CDeleteCertificateAppInfoManager();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CDeleteCertificateAppInfoManager(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EDelete,
		EFinished
		};

private:
	TState iState;
	};

/**
 * Client management tests
 */
class CAddClient : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CAddClient();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CAddClient(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EAdd,
		EFinished
		};

private:
	TState iState;
	TCertificateAppInfo* iClientInfo;
	};

class CGetClients : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CGetClients();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CGetClients(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EGet,
		EFinished
		};

private:
	TState iState;
	const RArray<TCertificateAppInfo>* iClients;
	RArray<TUid> iExpectedClients;
	};

class CRemoveClient : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	~CRemoveClient();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CRemoveClient(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		ERemove,
		EFinished
		};

private:
	TState iState;
	TUid iUid;
	};

#endif
