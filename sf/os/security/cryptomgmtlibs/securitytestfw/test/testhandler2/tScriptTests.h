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


#ifndef __T_SCRIPT_H__
#define __T_SCRIPT_H__

#include <e32base.h>

class CTestAction;
class RFs;
class CConsoleBase;
class Output;
class TTestActionSpec;

struct TScriptTests
	{
	CTestAction* (*action)(class RFs &,class CConsoleBase &,class Output &,const class TTestActionSpec &);
	
	TScriptTests(CTestAction* (*aAction)(class RFs &,class CConsoleBase &,class Output &,const class 
							TTestActionSpec &), const TDesC8& aName)
						{
						name.Set(aName);
						action=aAction;
						//Set the default groupings when reading tests
						//groupings= EXOOM | EXCANCEL | SMOKE;
						};
	//TInt groupings;
	TPtrC8 name;
	};


#define START_SCRIPT_LIST static TScriptTests theTestTypes[]={
// x is the class to create eg. TActionValidate
// y is the name we give to the class instance when displaying results
#define SCRIPT_ITEM(x,y) TScriptTests(&x::NewL,y)
#define END_SCRIPT_LIST ,TScriptTests(NULL,_L8(""))};


#endif

