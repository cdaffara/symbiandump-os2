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
 @test
*/
 
#ifndef __COMPARISONTEST_H
#define __COMPARISONTEST_H

#include <e32base.h>
#include <e32cmn.h>

#include "t_testaction.h"

class CX509Certificate;

/**
Validates the comparison of certificates and issuer-names.
*/
class CComparisonTest : public CTestAction  
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CComparisonTest();

public:
	// From CTestAction
	void DoPerformPrerequisite(TRequestStatus& aStatus);
	void DoPerformPostrequisite(TRequestStatus& aStatus);
	void PerformAction(TRequestStatus& aStatus);

	void DoReportAction();
	void DoCheckResult(TInt aError);
    
private:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	CX509Certificate* ReadCertificateL(const TDesC8& aFileName);
	CComparisonTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void DoActionL();	

private:
	RFs& iFs;						/// shared file server session
	CX509Certificate* iCert1;		/// LHS of comparison
	CX509Certificate* iCert2;		/// RHS of comparison
	TBool iMatchExpected;			/// ETrue if CX509Certificate::IsEqualL is expected to return ETrue
};

#endif 
