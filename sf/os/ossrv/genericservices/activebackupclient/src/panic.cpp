// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Server Panic methods
// 
//

/**
 @file
*/

#include <e32std.h>
#include "panic.h"

namespace conn
	{	
	void Panic(TInt aPanicCode)
	/**
    @internalComponent
    
	Calls User::Panic with reason "SB: <panic code>"

	@param aPanicCode The panic code as defined in panic.h
	@panic aPanicCode The panic code passed in
	*/	
		{
		_LIT(KPanicString,"SB:");
		User::Panic(KPanicString,aPanicCode);
		}
	}
