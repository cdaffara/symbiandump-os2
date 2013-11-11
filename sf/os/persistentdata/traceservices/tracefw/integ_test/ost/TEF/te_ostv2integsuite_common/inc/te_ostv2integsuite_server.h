/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef MYTESTSERVER_H
#define MYTESTSERVER_H

#include <e32std.h>
#include <e32base.h>
#include <test/testserver2.h>

#include "te_ostv2integsuite_controller.h"

class CTe_MCLSuiteServer : public CTestServer2
	{
public:
	static CTe_MCLSuiteServer* NewL();
	virtual CTestBlockController* CreateTestBlock();
	};

GLDEF_C TInt E32Main();

#endif // MYTESTSERVER_H
