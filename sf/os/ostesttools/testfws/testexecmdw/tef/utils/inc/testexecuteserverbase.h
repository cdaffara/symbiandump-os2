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
* This contains CTestServer which is the base class for all test servers
*
*/



/**
 @file TestExecuteServerBase.h
*/
#if (!defined __TESTEXECUTE_SERVER_BASE_H__)
#define __TESTEXECUTE_SERVER_BASE_H__

#include <e32base.h>
#include <test/testexecutestepbase.h>

class CTestServer : public CServer2
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C virtual ~CTestServer();
	IMPORT_C CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	IMPORT_C virtual void ConstructL(const TDesC& aName);
	void StartLoggerL();

	/**
	 * Creates the test step.
	 *
	 * @param aStepName - test step name to run.
	 *
	 * @return - A CTestStep derived instance
	 */
	virtual CTestStep* CreateTestStep(const TDesC& aStepName) = 0;

	void SessionClosed();
	TInt64& RandSeed() {return iSeed;};
	inline CTestExecuteLogger& Logger() {return iLogger;};
	inline const TDesC& Name() const {return iServerName;};
	inline TBool LoggerStarted() { return iLoggerStarted; };
protected:
	IMPORT_C CTestServer();
	inline void IncSessionCount() {++iSessionCount;};
protected:
private:
	TInt64	iSeed;
	TInt	iSessionCount;
	CTestExecuteLogger iLogger;
	TBool iLoggerStarted;
	TBuf<KMaxTestExecuteNameLength> iServerName;
	};

#endif
