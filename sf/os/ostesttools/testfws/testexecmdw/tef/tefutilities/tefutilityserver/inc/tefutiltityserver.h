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
* Contributors:
*
* Description:  
* Test Server to run utility tests supported by TEF
*
*/



/**
 @file TEFUtilityServer.h
*/

#if (!defined __TEF_UTILTITY_SERVER_H__)
#define __TEF_UTILTITY_SERVER_H__

#include <test/testexecuteserverbase.h>

/**
This class will initialise test steps and return pointer to TEF
*/
class  CTEFUtilityServer : public CTestServer
{
public:
	static CTEFUtilityServer*	NewL();
	virtual CTestStep*			CreateTestStep(const TDesC& aStepName);
};

#endif
