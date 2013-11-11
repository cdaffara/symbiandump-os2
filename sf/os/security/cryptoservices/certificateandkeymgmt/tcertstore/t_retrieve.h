/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CRetrieveCertificate class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_RETRIEVE_H__
#define __T_RETRIEVE_H__

#include "t_certstoreactions.h"

class CCertificate;

/**
 * An action to test retrieving certificate data.
 */
class CRetrieveCertificate : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs,
							 CConsoleBase& aConsole,
							 Output& aOut,
							 const TTestActionSpec& aTestActionSpec);
	~CRetrieveCertificate();
	
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
	virtual void DoReportAction();
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void DoCheckResult(TInt aError);

private:
	CRetrieveCertificate(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	void SetDeletable(const TDesC8& aDeletableString);
	void SetFormatL(const TDesC8& aFormatString);
	TBool ValidCertInfo();
	void DoWriteResultL();
	
private:
	enum TState
		{
		EList,
		ERetrieveData,
		EParseData,
		ERetrieveCert,
		ECheckRetrieve,
		EFinished
		};

	TState iState;
	TCertLabel iLabel;
	CCertAttributeFilter* iFilter;
	RMPointerArray<CCTCertInfo> iCertInfos;
	CCTCertInfo* iCertInfo;
	HBufC8* iData;
	TPtr8 iDataPtr;
	CCertificate* iCert;
	
	// for format and deletable attribute tests
	TCertificateFormat iFormat;
	TBool iDeletable;
	TBool iTestForDeletable;
	};

#endif
