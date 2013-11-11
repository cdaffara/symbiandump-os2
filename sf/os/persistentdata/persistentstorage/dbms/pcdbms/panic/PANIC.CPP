// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PANIC.h
// 
//

#include <u32std.h>
#include "D32DBAS.H"
#include "D32Assert.h"

EXPORT_C void Dbms::Panic(const TDesC& aCategory,TInt aCode)
	{
	__TRACE(aCategory);
	__TRACE(aCode);
	User::Panic(aCategory,aCode);
	}
