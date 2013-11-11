/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : dlla.h
* Part of     : standard c++ tests.
* 
*
*/



#include <e32def.h>

#ifndef _DLL_A_H_
#define _DLL_A_H_
class DynamicCastTest
{
public:
	/* no key function here...*/
	virtual void foo() {} 

IMPORT_C static DynamicCastTest*  Instance();
};

class DynamicCastTest_DerivedClass : public DynamicCastTest
{
};

#endif
