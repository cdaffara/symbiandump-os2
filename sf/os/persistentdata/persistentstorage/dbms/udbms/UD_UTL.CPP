// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "UD_STD.H"
#include <e32svr.h>

GLDEF_C void Panic(TDbPanic aPanic)
//
// Panic the client.
//
	{
	_LIT(KCategory,"DBMS");
	Dbms::Panic(KCategory,aPanic);
	}


// Class RDbHandleBase

void RDbHandleBase::Close()
	{
	CDbObject* obj=iObject;
	iObject=0;
	CDbObject::Destroy(obj);
	}

CDbObject& RDbHandleBase::Object() const
	{
	CDbObject* obj=iObject;
	__ASSERT_ALWAYS(obj,Panic(EDbUninitialised));
	return *obj;
	}
