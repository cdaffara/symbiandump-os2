// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the test server of the Streaming CAF test harness
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCAFSERVER_H_
#define TSCAFSERVER_H_

#include <test/testexecuteserverbase.h>

_LIT(KStaServerName, "tscaf");

class CScafServer : public CTestServer
/**
	Describes the test server for the Stremaing CAF test harness.
 */
	{
public:
	static CScafServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();
	~CScafServer();
private:
	RFs iFs;
	};

inline RFs& CScafServer::Fs()
	{
	return iFs;
	}

#endif /*TSCAFSERVER_H_*/
