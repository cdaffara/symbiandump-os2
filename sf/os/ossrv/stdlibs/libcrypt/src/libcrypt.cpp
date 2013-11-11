/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Dll entry point functions
*
*/


#include <e32uid.h>

#ifndef EKA2 
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) 
// DLL entry point
	{
	return(KErrNone);
	}
#endif 

// EXTERNAL FUNCTION PROTOTYPES
extern "C" char* _crypt (const char *, const char *);
extern "C" void _setkey (const char *);
extern "C" void _encrypt (char [], int);

extern "C"
{

// -----------------------------------------------------------------------------
// function_name: crypt
//
// Encodes a constant using the first argument to this function as the key to 
// encoding algorithms (DES or MD5)
//
// Returns: pointer to the buffer containing the encoded string
// -----------------------------------------------------------------------------
//
	EXPORT_C char *crypt(const char *key, const char *salt)
	{
		return _crypt (key, salt);
	}

// -----------------------------------------------------------------------------
// function_name: setkey
//
// Sets the key to used within the encryption/decryption algorithms
//
// Returns: void
// -----------------------------------------------------------------------------
//
	EXPORT_C void setkey(const char *key)
	{
		_setkey(key);
	}

// -----------------------------------------------------------------------------
// function_name: encrypt
//
// Encrypts or decrypts the contents of bit vector in place. Type of operation
// depends on the edflag argument
//
// Returns: void
// -----------------------------------------------------------------------------
//
	EXPORT_C void encrypt(char block[], int edflag)
	{
		// encrypt() does not change the setting of errno if successful.
		// However, if errno is non-zero on return, an error has 
		// occurred.
		_encrypt (block, edflag);
	}
}
