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
#include "CertWriter.h"

class CSyntaxTest : public CTestAction  
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	void TestValidation(TRequestStatus& aStatus);
	~CSyntaxTest();

public:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);

	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);

private:
	CSyntaxTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void ReadAndSyntaxCheckL(const TDesC &aFilename);

private:
	RFs& iFs;
	CDir* iDirList;
	TInt nFileNumber;
	WTLSCertWriter* iWriter;
	TBuf<128> iPath;
	RFile iLogFile;
	FileOutput* iSyntaxOut;
	static TInt nInstances;
};

#endif 
