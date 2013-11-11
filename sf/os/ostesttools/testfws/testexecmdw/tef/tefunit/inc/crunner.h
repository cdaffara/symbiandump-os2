/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 @file CRunner.h
*/

#ifndef __RUNNER__
#define __RUNNER__

#include <e32base.h>

#include "tefunit.h"

class CPath
	{
public:
	CPath();
	void ConstructL( const TDes& aPath);
	static CPath* NewL( TDes& aPath );
	~CPath();
	TTestName Current() const;
	TTestName NextL();
	TInt Count() const;
	TInt Remaining() const;

private:
	RArray<TTestName>	iFields;
	TInt				iCurrent;
	};

class CRunner : public CBase, public MVisitor
	{
public: 
	CRunner( const CTestConfig& aConfig, const CTestExecuteLogger& aLogger);
	virtual ~CRunner();
	void ConstructL( const TTestPath& aPath );
	static CRunner* NewL( const TTestPath &aPath, const CTestConfig& aConfig, const CTestExecuteLogger& aLogger );
	virtual void VisitL(CTestCase* aTestCase);
	virtual void VisitL(CTestSuite* aTestSuite);
private:
	CPath				iPath;
	CTestConfig			iConfig;
	CTestExecuteLogger	iLogger;
	};

#endif // __RUNNER__

