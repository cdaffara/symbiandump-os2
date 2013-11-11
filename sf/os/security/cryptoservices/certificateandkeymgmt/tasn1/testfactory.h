/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Factory object creating different CTestBase-derived classes for each index passed in
*
*/


#ifndef __TESTFACTORY_H__
#define __TESTFACTORY_H__

#include <e32def.h>

class CTestBase;

class TTestFactory
	{
	public:
		static CTestBase* MakeTestLC(const TInt aIndex);
	};

#endif
		
