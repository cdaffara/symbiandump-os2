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

#include "UT_STD.H"

GLDEF_C void Panic(TTablePanic aPanic)
//
// Panic the process with DBMS-Table as the category.
//
	{
	_LIT(KCategory,"DBMS-Table");
	Dbms::Panic(KCategory,aPanic);
	}
