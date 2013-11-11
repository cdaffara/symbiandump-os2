/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CSERVEROOMTESTRUNNER_H__
#define __CSERVEROOMTESTRUNNER_H__

#include "clientsession.h"
#include "t_testrunner.h"
#include <f32file.h>

/**
 * A test runner that performs OOM testing on the file tokens server.
 */
NONSHARABLE_CLASS(CServerOOMTestRunner) : public COOMTestRunnerBase
	{
 public:
    CServerOOMTestRunner(Output& aOut);
    virtual ~CServerOOMTestRunner();

 private:
	// Implement COOMTestRunnerBase interface
	virtual void StartOOMTestL();	
	virtual void IncHeapFailPoint();
	virtual void ResetHeapFail();
	virtual TInt AllocCount();
	virtual void EndOOMTestL();

	TInt SendRequest(TFSTokenMessages aFunction);

 private:
	RFileStoreClientSession iClient;
	TInt iServerAlloc;
	};

#endif
