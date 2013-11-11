/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __T_UNIFIEDCERTSTOREREMOVE_H__
#define __T_UNIFIEDCERTSTOREREMOVE_H__

#include "t_certstoreactions.h"
class CDeleteCertificate : public CSubscriberAction
	{
	
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CDeleteCertificate();
	
public:
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

protected:
	CDeleteCertificate(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void GetCerts(TRequestStatus& aStatus);
	void DeleteCert(TRequestStatus& aStatus);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	TBool CheckCertType( CCTCertInfo& aCertInfo);
	
protected:
	enum TDeleteCertState
		{
		EIdle,
		EGettingCerts,
		EDeletingCert,
		ECheckNotification,
		EFinished
		};

protected:
	TDeleteCertState iState;
	TCertLabel iCertLabel;
	RMPointerArray<CCTCertInfo> iCertInfos;
	CCTCertInfo* iCertInfoToDelete;
	CCertAttributeFilter* iFilter;
	TBool iDeleteAllCerts;
	HBufC8* iCertDeleteType;
	};

class CDeleteRetrieveCertificate : public CSubscriberAction
	{
public:
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

protected:
	CDeleteRetrieveCertificate(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void GetCerts(TRequestStatus& aStatus) = 0;
	virtual void RetrieveCerts(TRequestStatus& aStatus)	 = 0;
	virtual void DeleteCert(TRequestStatus& aStatus) = 0;

protected:
	enum TDeleteCertState
		{
		EIdle,
		EGettingCerts,
		EDeletingCert,
		ECheckNotification,
		EFinished
		};

protected:
	TDeleteCertState iState;
	TCertLabel iCertLabel;
	RPointerArray<HBufC8>* iCertlisted;
	RMPointerArray<CCTCertInfo> iCertInfos;
	CCTCertInfo* iCertInfoToDelete;
	CCTCertInfo* iCertInfoToRetrieve;
	TPtr8* iBuf;
	};

class CDeleteRetrieveCACertificate : public CDeleteRetrieveCertificate
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	~CDeleteRetrieveCACertificate();

private:
	CDeleteRetrieveCACertificate(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void GetCerts(TRequestStatus& aStatus);
	void RetrieveCerts(TRequestStatus& aStatus);
	void DeleteCert(TRequestStatus& aStatus);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	CCertAttributeFilter* iFilter;
	};

class CDeleteRetrieveUserCertificate : public CDeleteRetrieveCertificate
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CDeleteRetrieveUserCertificate();
private:
	CDeleteRetrieveUserCertificate(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void GetCerts(TRequestStatus& aStatus);
	void RetrieveCerts(TRequestStatus& aStatus);
	void DeleteCert(TRequestStatus& aStatus);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	CCertAttributeFilter* iFilter;
	};

#endif

