// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Declaration for log engine backup and restore test server
// 
//

/**
 @file
 @internalComponent
*/

#ifndef BACKUPRESTORESERVER_H
#define BACKUPRESTORESERVER_H

#include <test/testexecuteserverbase.h>


class CLogEngTestExecuteServer : public CTestServer
	{
public:
	static CLogEngTestExecuteServer* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // #ifndef BACKUPRESTORESERVER_H
