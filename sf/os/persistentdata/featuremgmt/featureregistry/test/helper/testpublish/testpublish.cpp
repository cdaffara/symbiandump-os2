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
// For platsec tests: attempts to set the value of the Feature Registry property.
// Feature Registry property must be defined first.
// If run with argument "KErrNone", returns KErrNone if publish is successful, which
// should only happen if this EXE passes the write policy of the feature property.
// If run with argument "KErrPermissionDenied" it returns KErrNone (success) if publish
// failed for reason KErrPermissionDenied, otherwise it will return a different error code.
// Use SETCAP to vary the capabilities of this executable.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32base.h>
#include <e32property.h>
#include "featregcmn.h"

_LIT(KTestResultNone,				"KErrNone");
_LIT(KTestResultPermissionDenied,	"KErrPermissionDenied");

TInt E32Main()
	{
	TFileName cmdLine;
	User::CommandLine(cmdLine);

	TInt result = RProperty::Set(KFeaturePropCat, KFeaturePropKey, KNullDesC8);

	if (cmdLine.Find(KTestResultNone) >= 0)
		{
		// result should already be KErrNone if succeeded
		}
	else if (cmdLine.Find(KTestResultPermissionDenied) >= 0)
		{
		if (result == KErrPermissionDenied)
			{
			result = KErrNone;
			}
		else
			{
			result = KErrGeneral;
			}
		}
	else
		{
		result = KErrArgument;
		}

	return result;
	}
