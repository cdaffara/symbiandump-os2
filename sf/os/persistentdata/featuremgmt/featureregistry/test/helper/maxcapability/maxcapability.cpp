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
// Checks capabilities of featreg.dll. Succeeds if this links and runs a query.
// (Changing capability from "All -Tcb" to "All" should make this test fail.
// Note: Must have a valid feature registry configuration.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32base.h>
#include <featreg.h>

TInt E32Main()
	{
	TInt result = RFeatureRegistry::QuerySupportS(TUid::Null());
	if (result >= 0)
		{
		return KErrNone; // success
		}
	return result;
	}
