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
//

#ifndef EMISC_H
#define EMISC_H

#include <e32base.h>

/**
	@file

	Miscellaneous utilities that can help make code more readable.
*/

/**
   A convenience postfix macro that is equivalent to wrapping the
   preceding expression with User::LeaveIfError().

   The code:
   @code
   fs.Connect() OR_LEAVE;
   @endcode

   is equivalent to:

   @code
   User::LeaveIfError(fs.Connect());
   @endcode

   The implementation happens to rely on overloading operator|| for
   TLeave. However, this macro is not intended for use within general
   expressions, just to guard individual calls to methods that may
   return an error code.
 */
#define OR_LEAVE || ELeave

/**
   Enabling overload for OR_LEAVE. Should only be invoked via
   OR_LEAVE, never directly.
 */
inline void operator||(TInt aStatus, TLeave /*aTag*/)
	{
	User::LeaveIfError(aStatus);
	}

#endif // !EMISC_H

