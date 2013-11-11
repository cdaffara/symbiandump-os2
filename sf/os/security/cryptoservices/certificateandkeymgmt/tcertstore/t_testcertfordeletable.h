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
* CTestCertForDeletable class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_TESTCERTFORDELETABLE_H__
#define __T_TESTCERTFORDELETABLE_H__

#include "t_certstoreactions.h"
#include "t_testactionspec.h"
#include <mctkeystore.h>
#include <unifiedkeystore.h>

/** 
 * This class tests the addition of a certificate to the store.
 */
class CTestCertForDeletable : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(	RFs& aFs, 
								CConsoleBase& aConsole, 
								Output& aOut, 
								const TTestActionSpec& aTestActionSpec);
	~CTestCertForDeletable();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void AfterOOMFailure();
	virtual void Reset();

private:
	CTestCertForDeletable(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

	void WriteFormat(TCertificateFormat aFormat);
	void WriteOwnerType();
	void SetKeyId(TKeyIdentifier& aKeyIdentifier, const TDesC8& aKeyInfo);
	void SetCertFormatL(const TDesC8& aFormat);
	void SetCertOwnerTypeL(const TDesC8& aOwnerType);
	void SetCertLabelL(const TDesC8& aLabel);
	void SetStoreToUse(const TDesC8& aStoreToUse);
	void SetDeletable(const TDesC8& aStringValue);
	void SetSubActionL(const TDesC8& aStringValue);
	void SetCertificateContentL(const TDesC8& aFileName);
	void ConstructCertL(const TDesC8& aCert);

	void TestSerializationL(MCTToken& aToken,
							TKeyIdentifier* aIssuerKeyId,
							TKeyIdentifier* aSubjectKeyId, 
							const TInt aCertificateId,
							const TDesC8* aIssuerHash );
private:
	enum TState
		{
		ETestSerialization,
		ETestNewCSClasswDeletable,
		ETestOldCSClasswoDeletable,
		EFinished
		};

private:
	TState iState;
	CCertificate* iCertificate;
	HBufC8* iCertificateURL;
	HBufC* iCertificateLabel;
	TCertificateFormat iCertificateFormat;
	TCertificateOwnerType iOwnerType;
	TKeyIdentifier iIssuerKeyId;
	TKeyIdentifier iSubjectKeyId;
	TInt iStoreIndex;
	TBool iSelfInit;
	TBool iResultGood;

	HBufC8* iCertificateContent;

	CCTCertInfo* iCertInfo;
	CCertificate* iCert;
	TBool iDeletable;
	};
	
	
#endif
