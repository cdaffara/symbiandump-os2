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


#ifndef __TRSAENCRYPTFB_H__
#define __TRSAENCRYPTFB_H__

#include "t_testaction.h"
#include <e32std.h>
#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <random.h>

class CRSAEncryptFB : public CTestAction
{
public:
	~CRSAEncryptFB();
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	void DoPerformPrerequisite(TRequestStatus& aStatus);
	void DoPerformPostrequisite(TRequestStatus& aStatus);
	void DoReportAction(void);
	void DoCheckResult(TInt);
	void PerformAction(TRequestStatus& aStatus);

private:
	CRSAEncryptFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	inline TRSAPrivateKeyType TypeOfCrypto() {return iCryptoType;}
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void Hex(HBufC8& string);
	void DoPerformActionL();
	void DoPerformanceTestActionL();
	void LogKeyData(const CRSAPublicKey* aPublicKey,
							const CRSAPrivateKeyCRT* aPrivateKeyCRT, const TDesC8& aCipherText);

private:
	RFs& iFs;
	HBufC8* iBody;
	HBufC8* iInput;
	TInt iActionResult;
	TInt iKeyBits;
	TInt iPerfTestIterations;
	TRSAPrivateKeyType iCryptoType;
};

#endif
