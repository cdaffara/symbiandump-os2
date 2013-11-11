// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
*/

#ifndef __TEF_SSNDSERVER_H__
#define __TEF_SSNDSERVER_H__

#include <test/testexecuteserverbase.h>

/** Test server to implement the pure virtual CreateTestStep
method.
*/
class CSSndTestServer : public CTestServer
	{
public:
	static CSSndTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
