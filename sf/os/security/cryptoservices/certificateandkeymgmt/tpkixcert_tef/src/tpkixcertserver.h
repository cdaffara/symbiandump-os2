/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/

#ifndef TPKIXCERTSERVER_H
#define TPKIXCERTSERVER_H

#include <test/testexecuteserverbase.h>
#include <f32file.h>

class CPkixCertTestServer : public CTestServer
	{
public:
	static CPkixCertTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	
	};
	
#endif	//TPKIXCERTSERVER_H
