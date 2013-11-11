/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PKIXCERTSTEPBASE_H
#define PKIXCERTSTEPBASE_H

#include <test/testexecutestepbase.h>
#include <e32std.h>
#include <pkixcertchain.h>
#include <x509cert.h>


_LIT(KPerformOom, "performoom");
_LIT(KEndEntity, "endentity");
_LIT(KIntermediateCert, "intermediatecert");
_LIT(KRootCert, "rootcert");
_LIT(KUid, "uid");
_LIT(KOid, "oid");
_LIT(KKeyFormat, "-%02d");


class CPkixCertStepBase : public CTestStep
	{
public:
	~CPkixCertStepBase();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();		

	
protected:
	virtual void PerformTestL();
	void PerformOomTestL();
		
	//so keyname concat with -01, -02, -03, .... , -XX
	void GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray);
	void GetIntArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TInt>& aArray);
	HBufC8* ReadFileLC(const TDesC& aFileName);

protected:
	TBool iPerformOom;
	RFs iFileServer;
	CPKIXCertChain* iCertChain;
	HBufC8* iConcatenatedChain;	//string concatenation of the DER encoded certs in the chain, not including the root cert
	TUid iUid;
	RArray<TPtrC> iOids;
	RPointerArray<TDesC> iProcessedOids; //pointers into iOids, necessary format for API calls
	RPointerArray<CX509Certificate> iRootCerts;
	TPtrC8 iPtr;
	TBool iUseUidOverload;
	
	CActiveScheduler*	iScheduler;
	};	

#endif // PKIXCERTSTEPBASE_H
