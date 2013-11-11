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


#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined (__BADESCA_H__)
#include <badesca.h>
#endif

#include <wtlscertchain.h>

/* Chain Orders:
	1) unstructured text, no T=CA chains
	Canadian Universities
	Waterloo noTCA
	Laurier noTCA
	McMaster noTCA
	Guelph noTCA

	2) proper x509 text with T=ca
	Canadian Universities
	Waterloo
	McMaster
	Guelph
	Queens
	Ottawa
	Lakehead
	Laurier

	3) proper structured text with T=ca
	Canadian Hockey Teams
	Toronto Maple Leafs
	Ottawa Senators
	Montreal Canadiens
	Winnipeg Jets (missing common name)
	Vancouver Canucks
*/

class CTestChain : public CBase
	{
public:
	~CTestChain();
	CTestChain();
	virtual void ConstructL();
	CDesCArray* iServerCerts;
	CDesCArray* iServerLabels;
	CDesCArray* iRootCerts;
	CDesCArray* iRootLabels;
	CDesCArray* iExtraCerts;
	CDesCArray* iExtraLabels;
	};
			
class CTestChainBuild : public CTestChain
	{
public:
	static CTestChainBuild* NewL();
	static CTestChainBuild* NewLC();
	~CTestChainBuild();
	HBufC* iCorrectChain;
	};

class CTestChainValidate : public CTestChain
	{
public:
	static CTestChainValidate* NewL();
	static CTestChainValidate* NewLC();
	void ConstructL();

	// warning:
	//We have test certificates issued at different times in such a way that test validating
	// these certificates with the same date will result in at least some of them being out
	// of their validity period 
	//Old certificates == 0 and should be validated with a date of July 2000
	//New certificates == 1 and should be validated with a date of July 2002
	//This defaults to 0 in the constructor
	TInt iDateIssued;

	~CTestChainValidate();
	TWTLSValidationStatus* iError;
	CArrayFixFlat<TWTLSValidationStatus>* iWarnings;
	void AddChainL(const TDesC& aValues);
	void AddIOL(const TDesC& aValues);
	void AddDateIssued(const TDesC& aValues);
private:
	TBool AddRootL(const TDesC& aBuf);
	TBool AddServerL(const TDesC& aBuf);
	};


