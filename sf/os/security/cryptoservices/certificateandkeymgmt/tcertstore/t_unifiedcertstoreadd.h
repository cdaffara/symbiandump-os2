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
* CAddCertificate and CImportKey class implementations
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_UNIFIEDCERTSTOREADD_H__
#define __T_UNIFIEDCERTSTOREADD_H__

#include "t_certstoreactions.h"
#include "t_testactionspec.h"
#include <mctkeystore.h>
#include <unifiedkeystore.h>

/** 
 * This class tests the addition of a certificate to the store.
 */
class CAddCertificate : public CSubscriberAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CAddCertificate();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void AfterOOMFailure();
	virtual void Reset();

private:
	CAddCertificate(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void WriteFormat();
	void WriteOwnerType();
	void SetKeyId(TKeyIdentifier& aKeyIdentifier, const TDesC8& aKeyInfo);
	void SetDeletable(const TDesC8& aDeletable);
	void SetCertFormatL(const TDesC8& aFormat);
	void SetCertOwnerTypeL(const TDesC8& aOwnerType);
	void SetCertLabelL(const TDesC8& aLabel);
	void SetStoreToUse(const TDesC8& aStoreToUse);
	void SetCertificateContentL(const TDesC8& aFileName);
	void ConstructCertL(const TDesC8& aCert);

private:
	enum TState
		{
		EAdding,
		ECheckNotification,
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
	TBool iDeletable;
	TBool iDeletableFlagPresent;
	TInt iStoreIndex;

	/**
	 * This is the data of the certificate, it is the data that will be written to
	 * the store for any kind of certificates (URL, X.509, WTLS...)
	 */
	HBufC8* iCertificateContent;
	};


class CImportKey : public CCertStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CImportKey();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	TBool SetKeyUsage(const TDesC8& aKeyUsage);
	void SetKeyLabel(const TDesC8& aKeyLabel);
	TBool SetKeyAccessType(const TDesC8& aKeyAccessType);
	void SetKeyDataFileL(const TDesC8& aDes);
	void SetKeyPassphrase(const TDesC8& aDes);
private:
	CImportKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EInitialise,
		EImportKey,
		EFinished
		};
private:
	TState iState;
private:
	TKeyUsagePKCS15 iUsage;
	HBufC* iLabel;
	HBufC8* iKeyData;
	TInt iAccessType;
	RFs iFs;
	CUnifiedKeyStore* iUnifiedKeyStore;
	CCTKeyInfo* iKeyInfo;
};

#endif
