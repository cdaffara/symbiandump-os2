/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* This executable is only defined so kmslddtest.exe can instantiate
* it with the required capabilities and SID, and get a handle to it
* to test the platsec enforcement of the key store.
*
*/


/**
 @file
*/

#include <e32std.h>

TInt E32Main()
/**
	Standard entrypoint.  This function must be defined, but does not
	do anything useful.

	@return					KErrNone.  This value does not have any significance,
							because this executable is only used by the KMS LDD test
							code.
 */
	{
	return KErrNone;
	}

