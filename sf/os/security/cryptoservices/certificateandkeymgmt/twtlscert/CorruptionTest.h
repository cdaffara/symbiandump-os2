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
* CCorruptionTest class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CCorruptionTest_H
#define __CCorruptionTest_H

#include "t_testaction.h"
#include "CertWriter.h"

class CCorruptionTest : public CTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	void TestValidation(TRequestStatus& aStatus);
	~CCorruptionTest();

public:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);

	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);

private:
	CCorruptionTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void RunCorruptionTestL(const TDesC &aFilename);

private:
	RFs& iFs;
	CDir* iDirList;
	TInt nFileNumber;
	WTLSCertWriter* iWriter;
	TBuf<128> iPath;
	NullOutput* iNullOut;
	TInt iIterations;
	RFile iLogFile;
	FileOutput* iCorruptOut;
	static TInt nInstances;
};

#endif // !defined(AFX_CORRUPTIONTEST_H__6C3C42AD_9E21_11D6_ABA5_00080214A261__INCLUDED_)
