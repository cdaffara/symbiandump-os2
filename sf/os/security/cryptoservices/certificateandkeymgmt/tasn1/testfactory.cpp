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
* Implementation for test object factory
*
*/


#include "testfactory.h"
#include "testnull.h"
#include "testboolean.h"
#include "testint.h"
#include "testbigint.h"
#include "testoctetstr.h"
#include "testoid.h"
#include "testgeneralizedtime.h"
#include "testsequence.h"
#include "testexplicittag.h"
#include "testimplicittag.h"
#include "testoutput.h"


// Make different object for each index, else return 0
CTestBase* TTestFactory::MakeTestLC(const TInt aIndex)
	{
	switch (aIndex)
		{
		case 0:
			return CTestNull::NewLC();
		case 1: 
			return CTestBoolean::NewLC();
		case 2:
			return CTestInt::NewLC();
		case 3:
			return CTestBigInt::NewLC();
		case 4:
			return CTestOctetString::NewLC();
		case 5:
			return CTestOID::NewLC();
		case 6:
			return CTestGeneralizedTime::NewLC();
		case 7:
			return CTestSequence::NewLC();
		case 8:
			return CTestExplicitTag::NewLC();
		case 9:
			return CTestImplicitTag::NewLC();
		case 10:
			return CTestOutput::NewLC();
		default:
			return 0;
		}
	}
