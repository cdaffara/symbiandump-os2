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
*
*/

#include "testbase.h"
#include "testmanager.h"

CTestBase::CTestBase(MTestManager& aManager)
 :	iManager(aManager)
	{
	}

CTestBase::~CTestBase()
	{
	}

void CTestBase::ProcessKeyL(TKeyCode /*aKeyCode*/)
	{
	}

void CTestBase::ProcessKeyL(const TDesC8& /*aString*/)
	{
	}
