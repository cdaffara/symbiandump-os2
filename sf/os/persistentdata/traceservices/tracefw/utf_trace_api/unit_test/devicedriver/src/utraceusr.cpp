// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// e32\drivers\trace\btracec.cpp
// 
//

#include <e32std.h>
#include "d32utrace.h"
#include <e32svr.h>


void Panic(TInt aPanicNum)
	{
	_LIT(KRUTracePanic,"RUTrace");
	User::Panic(KRUTracePanic,aPanicNum);
	}

EXPORT_C TInt RUTrace::Open()
	{
	//_LIT(KBTraceLddName,"utracex");
	//TInt r = User::LoadLogicalDevice(KBTraceLddName);
	TInt r = User::LoadLogicalDevice(Name());
	if(r!=KErrNone && r!=KErrAlreadyExists)
		return r;
	r = DoCreate( Name(), TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
	return r;
	};

EXPORT_C TInt RUTrace::TestUtrace(TInt& aFailed)
	{
	return DoControl(ETestUTrace,(TAny*)aFailed,0);
	}
