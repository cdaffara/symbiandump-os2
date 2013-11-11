/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TLIBLOGGERSERVER_H__
#define __TLIBLOGGERSERVER_H__

#include <f32file.h>
#include <test/testexecuteserverbase.h>

class CLibLoggerTestServer : public CTestServer
	{
public:
	static CLibLoggerTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs() {return iFs;}

private:
	RFs iFs;
	};

#endif // TLIBLOGGERSERVER_H


