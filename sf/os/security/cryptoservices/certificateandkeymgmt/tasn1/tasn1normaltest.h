/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ASN1TestAction.h: interface for the CASN1NormalTest class.
*
*/




/**
 @file
 @internalTechnology
*/

#ifndef __ASN1TESTACTION_H__
#define __ASN1TESTACTION_H__

#include "t_testaction.h"
#include "testbase.h"

class CASN1NormalTest : public CTestAction  
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CASN1NormalTest();
	void ReportProgressL(const TInt aError, const TInt aCount, const TInt aMax);
	Output& Out() { return iOut;};
protected:
	CASN1NormalTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

protected:
	CTestBase *iTestType;

private:
	
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void PerformAction(TRequestStatus& aStatus);
	TBool TestResult(TInt /*aError*/);
	void PerformCancel();
	void Reset();

	//RFs& iFs;
	TInt iPercentReported;
	const TInt iReportNumber;
	static TInt iReportCount;
};

#endif // !defined(AFX_ASN1TESTACTION_H__95894345_8529_11D6_AB96_00080214A261__INCLUDED_)
