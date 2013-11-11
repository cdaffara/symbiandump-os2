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
 
#ifndef __T_CERTSTOREACTIONSLISTCERTIFICATES_H__
#define __T_CERTSTOREACTIONSLISTCERTIFICATES_H__

#include "t_certstoreactions.h"

/* Structure to hold the name of a certificate and whether
it is expected to be read only
*/
class TCert
	{
	public:
	TName iName;
	TBool iReadOnly;
	};
	
/**
 * This class is used to perform certificate loading tests.
 * It is an abstract base class.
 */
class CListCertificates : public CCertStoreTestAction
	{
 public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	
 public:
	~CListCertificates();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
	virtual void DoReportAction();
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);

 protected:
	CListCertificates(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(Output& aOut, const TTestActionSpec& aTestActionSpec);

 private:
 	
	void AppendCCTCertInfoL(const TDesC8& aCCTCertInfoBuf);
	virtual void TestAction();
	void DoCheckResult(TInt aError);
	virtual void DoWriteResultL();
	void AddDNL(const TDesC8& aDNBuf);
	

 private:
	enum TListCertState
		{
		EList,
		EFinished
		};
	
	TListCertState iState;
	
	/// The filter used to define which kind of certificates we want to retrieve.
	CCertAttributeFilter* iFilter;

	/// Array of distinguished names to filter by
	RPointerArray<const TDesC8> iDNs;

	/// Cert labels we expect list to return
	RArray<TCert> iExpectedLabels;

	/// Cert infos returned from list operation
	RMPointerArray<CCTCertInfo> iCertInfos;
	};
	
#endif
