/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tmulticastclienttestserver.h
* 
*
*/



#ifndef __TMULTICASTCLIENTTESTSERVER_H__
#define __TMULTICASTCLIENTTESTSERVER_H__

#include <f32file.h>
#include <test/testexecuteserverbase.h>

class CMulticastClientTestServer : public CTestServer
	{
public:
	static CMulticastClientTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs() {return iFs;}

private:
	RFs iFs;
	};

#endif // 
