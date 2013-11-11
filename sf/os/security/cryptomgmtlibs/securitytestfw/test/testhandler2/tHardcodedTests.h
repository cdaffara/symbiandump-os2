/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_HARDCODED_H__
#define __T_HARDCODED_H__

#include <e32base.h>

class CTestAction;
class RFs;
class CConsoleBase;
class Output;
class TTestActionSpec;

struct THardcodedTests
	{	
	THardcodedTests(CTestAction* (*aAction)(class RFs &, class CConsoleBase &,
		class Output &, const class TTestActionSpec &),TPtrC8 aName, TInt aGrouping)
						{
						name.Set(aName);
						action = aAction;
						//Set the default groupings when reading tests
						groupings= aGrouping;
						};
	CTestAction* (*action)(class RFs &,class CConsoleBase &,class Output &,const class TTestActionSpec &);
	TInt groupings;
	TPtrC8 name;
	};


#define START_HARDCODED_LIST static THardcodedTests theHardcodedTests[]={
// x is the class to create eg. TActionValidate
// y is the name we give to the class instance when displaying results
// z is the grouping parameter eg. INOOM | SMOKE
#define HARDCODED_ITEM(x,y,z) THardcodedTests(&x::NewL,y,z)
#define END_HARDCODED_LIST ,THardcodedTests(NULL,_L8(""),0)};


#endif

