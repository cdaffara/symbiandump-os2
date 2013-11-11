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
* Description:
* Name        : tselectserver.h
* 
*/

#ifndef __TAESELECTSERVER_H__
#define __TAESELECTSERVER_H__

#include <f32file.h>
#include <test/testexecuteserverbase.h>

class CAESelectTestServer : public CTestServer
	{
public:
	static CAESelectTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs() {return iFs;}

private:
	RFs iFs;
	};

#endif // __TAESELECTSERVER_H__
