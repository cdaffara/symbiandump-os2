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
* trsasign.h
*
*/


#ifndef __TRSASIGN_H__
#define __TRSASIGN_H__

#include "t_testaction.h"
#include <e32std.h>
#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <random.h>
#include "tsignatureinput.h"

class CRSASignFB : public CTestAction
{
public:
	~CRSASignFB();
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
	CRSASignFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void Hex(HBufC8& string);
	inline TRSAPrivateKeyType TypeOfCrypto();
	void DoPerformActionL();
	void DoPerformanceTestActionL();

private:
	RFs& iFs;
	HBufC8* iBody;
	HBufC8* iInput;
	CSignatureInput* iSigInput;
	TInt iActionResult;
	TInt iKeyBits;
	TInt iPerfTestIterations;
	TRSAPrivateKeyType iCryptoType;
};

#endif
