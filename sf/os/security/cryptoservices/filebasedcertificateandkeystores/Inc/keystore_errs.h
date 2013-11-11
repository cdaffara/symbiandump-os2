/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains the list of error values which is returned to the client. 
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef KEYSTORE_ERRS_H
#define KEYSTORE_ERRS_H

#include <e32def.h>

/**
 Error codes that can be returned by File token.
 */

/**
	If Authentication fails for storing the keys.
 */
const TInt KErrAuthenticationFailure = -11012;
	
#endif // KEYSTORE_ERRS_H
