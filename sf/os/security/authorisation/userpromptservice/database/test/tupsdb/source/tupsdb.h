/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#if (!defined __TUPSDB_H__)
#define __TUPSDB_H__

#include <test/testexecuteserverbase.h>

_LIT(KDbServerName, "tupsdb");

class CTUpsDbServer : public CTestServer
/**
	Defines UPS decision database test server.
 */
	{
public:
	static CTUpsDbServer* NewL();
	/**
		Create a new test step.
 	 */
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
