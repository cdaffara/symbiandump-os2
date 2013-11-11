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
* Vector tests for RSA ciphers
*
*/


#ifndef __TRSAVECTOR_H__
#define __TRSAVECTOR_H__

#include "tvectortest.h"

/**
 * Runs RSA encryption vectors.
 */

class CRSAEncryptVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSAEncryptVector();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CRSAEncryptVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CRSAPublicKey* iPubKey;
	HBufC8* iPlaintext;
	HBufC8* iCiphertext;
    };

/**
 * Runs RSA decryption vectors.
 */

class CRSADecryptVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSADecryptVector();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CRSADecryptVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CRSAPrivateKey* iPrivKey;    
	HBufC8* iCiphertext;
	HBufC8* iPlaintext;
    };



/**
 * Runs RSA decryption vectors.
 */

class CRSADecryptVectorCRT : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSADecryptVectorCRT();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CRSADecryptVectorCRT(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	CRSAPrivateKey* iPrivKey;    
	HBufC8* iCiphertext;
	HBufC8* iPlaintext;
    };

/**
 * Runs RSA signing vectors.
 */

class CRSASignVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSASignVector();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CRSASignVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CRSAPrivateKey* iPrivKey;
	HBufC8* iDigestInfo;
	CRSASignature* iSignature;
    };

class CRSASignVectorCRT : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSASignVectorCRT();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CRSASignVectorCRT(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CRSAPrivateKey* iPrivKey;
	HBufC8* iDigestInfo;
	CRSASignature* iSignature;
    };
/**
 * Runs RSA verification vectors.
 */

class CRSAVerifyVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSAVerifyVector();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformanceTestActionL();
private:
	CRSAVerifyVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	CRSAPublicKey* iPubKey;    
	HBufC8* iDigestInfo;
	CRSASignature* iSignature;
    };

#endif
