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

#include "US_STD.H"

GLDEF_C void Panic(TStorePanic aPanic)
//
// Panic the process with DBMS-Store as the category.
//
	{
	_LIT(KCategory,"DBMS-Store");
	Dbms::Panic(KCategory,aPanic);
	}


// Class RDbStoreReadStream

void RDbStoreReadStream::FilterL(TType aType,TUint32 aInit)
//
// Apply a filter if appropriate
//
	{
	MDbStreamFilter* f=iDatabase.Filter();
	if (f)
		Attach(f->FilterL(Source(),aInit,aType));
	}

// Class RDbStoreWriteStream

void RDbStoreWriteStream::FilterL(TType aType,TUint32 aInit)
//
// Apply a filter if appropriate
//
	{
	MDbStreamFilter* f=iDatabase.Filter();
	if (f)
		Attach(f->FilterL(Sink(),aInit,aType));
	}
