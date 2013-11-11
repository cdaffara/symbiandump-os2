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
#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <e32std.h>
#include <e32base.h>
#include <test/testblockcontroller.h>
#include <test/datawrapper.h>

class CTe_MCLSuiteController : public CTestBlockController
	{
public:
	CDataWrapper* CreateDataL(const TDesC& aData);
	};

#endif // MYCONTROLLER_H
