// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include <e32base.h>


//
// This file describes version 1.3 WBXML Extension enumerations.
//

enum TExtensionToken 
/**
Lists enumerations used to describe the global unambiguous wbxml tokens
@see CParser
@see http://www.w3.org/TR/wbxml/

@publishedPartner
@released
*/
	{

/**
Enumeration with a token value that represents
an inline string document-type-specific extension token.
Token is followed by a termstr.
*/
	EExtensionTokenEXT_I_0		= 0x40,

/**
Enumeration with a token value that represents 
an inline string document-type-specific extension token.
Token is followed by a termstr.
*/
	EExtensionTokenEXT_I_1		= 0x41,

/**
Enumeration with a token value that represents 
an inline string document-type-specific extension token.
Token is followed by a termstr.
*/
	EExtensionTokenEXT_I_2		= 0x42,

/**
Enumeration with a token value that represents 
an inline integer document-type-specific extension token.
Token is followed by a mb_u_int32.
*/
	EExtensionTokenEXT_T_0		= 0x80,
/**
Enumeration with a token value that represents
an inline integer document-type-specific extension token.
Token is followed by a mb_u_int32.
*/
	EExtensionTokenEXT_T_1		= 0x81,

/**
Enumeration with a token value that represents 
an inline integer document-type-specific extension token.
Token is followed by a mb_u_int32.
*/
	EExtensionTokenEXT_T_2		= 0x82,

/**
Enumeration with a token value that represents 
a single -byte document-type-specific extension token.
*/
	EExtensionTokenEXT_0		= 0xC0,

/**
Enumeration with a token value that represents
a single -byte document-type-specific extension token.
*/
	EExtensionTokenEXT_1		= 0xC1,

/**
Enumeration with a token value that represents
a single -byte document-type-specific extension token.
*/
	EExtensionTokenEXT_2		= 0xC2,

	};

#endif // __EXTENSION_H__
