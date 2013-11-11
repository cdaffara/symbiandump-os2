/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* trdavector.h
* Vector tests for RSA ciphers
*
*/


#ifndef __TDSAVECTOR_H__
#define __TDSAVECTOR_H__

#include "tvectortest.h"
#include "asymmetrickeys.h"
#include "tsignatureinput.h"

/**
 * Runs DSA signing vectors.
 */

class CDSASignVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CDSASignVector();
	virtual void DoCheckResult(TInt aError);
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CDSASignVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CDSAPrivateKey* iPrivKey;
	TPtrC8 iMessage;
	HBufC8* iK;
	CDSASignature* iSignature;
	CSignatureInput* iSigInput;
    };

/**
 * Runs DSA verification vectors.
 */

class CDSAVerifyVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CDSAVerifyVector();
	//virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoCheckResult(TInt aError);
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();

private:
	CDSAVerifyVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CDSAPublicKey* iPubKey;    
	HBufC8* iMessage;
	CDSASignature* iSignature;
	CSignatureInput* iSigInput;
    };

#endif
