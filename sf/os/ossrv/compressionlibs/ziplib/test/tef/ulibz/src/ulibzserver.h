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
* Name        : ulibzserver.h
* 
*
*/




#ifndef __ULIBZSERVER_H__
#define __ULIBZSERVER_H__

#include <f32file.h>
#include <test/testexecuteserverbase.h>

class ClibzTestServer : public CTestServer
	{
public:
	static ClibzTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	const RFs& Fs() {return iFs;}

private:
	RFs iFs;
	};
#endif // __ULIBZSERVER_H__
