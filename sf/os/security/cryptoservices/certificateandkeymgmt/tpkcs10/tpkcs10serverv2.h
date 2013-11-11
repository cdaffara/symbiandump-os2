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
 @internalTechnology
*/

#if (!defined __TPKCS10_SERVER_V2_H__)
#define __TPKCS10_SERVER_V2_H__
#include <test/testexecuteserverbase.h>


class CTPKCS10Server : public CTestServer
	{
public:
	static CTPKCS10Server* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

// Please Add/modify your class members
private:
	};

#endif
