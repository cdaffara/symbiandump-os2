/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines error codes for the security subsystem.
*
*
* @file 
* @publishedAll
* @released
*/

#ifndef __SECURITYERR_H__
#define __SECURITYERR_H__

#include <e32std.h>



/**
	If the passphrase entered is incorrect.
**/
const TInt KErrBadPassphrase				= -11000;



/**
	Operation in progress or its result is not secure.
**/
const TInt KErrNotSecure        			= -11001;



/** 
	If the key size is larger than that allowed 
	by the cipher strength restrictions of the crypto 
	library.
**/
const TInt KErrKeyNotWeakEnough				= -11002;



/**
	Length of the padding data is invalid.
**/
const TInt KErrInvalidPadding				= -11003;



/** 
	If the key is weaker than that allowed 
	by the cipher strength restrictions of the crypto 
	library.
 **/
const TInt KErrWeakKey						= -11004;



/**
	If the export of negative values is not
	supported.
**/
const TInt KErrNegativeExportNotSupported	= -11005;



/**
	If the requested key generation algorithm is not 
	supported.
**/
const TInt KErrKeyAlgorithm	   				= -11006;



/**
	If the key usage flags are not valid or not
	consistent with the key algorithm.
**/
const TInt KErrKeyUsage						= -11007;



/**
	If a validity period is specified during key generation,
	but the end date is in the past.
**/
const TInt KErrKeyValidity				   	= -11008;



/**
	If the requested key size is not supported.
**/
const TInt KErrKeySize						= -11009;



/**
	If an invalid combination of key access
	flags were specified.
**/
const TInt KErrKeyAccess					= -11010;



/**
	If the requested private key is not present.
**/
const TInt KErrPrivateKeyNotFound			= -11011;



/**
	If a setting in the repository is not found.
**/
const TInt KErrSettingNotFound				= -11012;	

#endif	//	__SECURITYERR_H__
