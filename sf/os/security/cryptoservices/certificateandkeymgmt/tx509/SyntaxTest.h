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
* CSyntaxTest class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSyntaxTest_H
#define __CSyntaxTest_H

#include "t_testaction.h"
#include "tcertwriter.h"
#include "SyntaxResult.h"

_LIT(KResultsFile, "Results.txt");

class CSyntaxTest : public CTestAction  
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSyntaxTest();

public:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);

	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);
    virtual void Reset();

private:
	CSyntaxTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	HBufC8* readCertLC(const TDesC &aFilename);
	void ReadAndSyntaxCheckL(const TDesC &aFilename);
	void TestInternalizeExternalizeL(CX509Certificate* aCert);
	TBool LoadResultsL(void);
	TBool CheckResult(const TDesC &aFilename, const TInt &aError);
	void PrintError(const TInt &aError);

private:
	RFs& iFs;
	CDir* iDirList;
	TInt nFileNumber;
	TBuf<128> iPath;
	RFile iLogFile;
	FileOutput* iSyntaxOut;
	CertWriter* iWriter;
	static TInt nInstances;
	RPointerArray<CSyntaxResult> *iExpectedResults;
};

#endif 
