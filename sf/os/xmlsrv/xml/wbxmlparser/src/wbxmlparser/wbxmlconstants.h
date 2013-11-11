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

#ifndef __WBXMLCONSTANTS_H__
#define __WBXMLCONSTANTS_H__

#include <e32base.h>

/**
This file describes useful version 1.3 WBXML constants.

@file
*/

enum TGlobalToken 
/**
Lists enumerations used to describe the global unambiguous wbxml tokens
@see CParser

@internalTechnology
@see http://www.w3.org/TR/wbxml/
*/
	{

/**
Enumeration with a token value that represents the action to
change the code page for the current token state.
Followed by a single u_int8 indicating the new code page number.
*/
	EGlobalTokenSWITCH_PAGE	= 0x00,

/**
Enumeration with a token value that represents the action that
indicates the end of an attribute list or the end of an element.
*/
	EGlobalTokenEND			= 0x01,

/**
Enumeration with a token value that represents
a character entity. Followed by a mb_u_int32 encoding the character entity number.
*/
	EGlobalTokenENTITY		= 0x02,

/**
Enumeration with a token value that represents
an inline string, followed by a termstr.

*/
	EGlobalTokenSTR_I		= 0x03,

/**
Enumeration with a token value that represents
an unknown attribute name, or unknown tag posessing no attributes or content.
Followed by a mb_u_int32 that encodes an offset into the string table.
*/
	EGlobalTokenLITERAL		= 0x04,

/**
Enumeration with a token value that represents
an inline string document-type-specific extension token.
Token is followed by a termstr.
*/
	EGlobalTokenEXT_I_0		= 0x40,

/**
Enumeration with a token value that represents 
an inline string document-type-specific extension token.
Token is followed by a termstr.
*/
	EGlobalTokenEXT_I_1		= 0x41,

/**
Enumeration with a token value that represents 
an inline string document-type-specific extension token.
Token is followed by a termstr.
*/
	EGlobalTokenEXT_I_2		= 0x42,

/**
Enumeration with a token value that
indicates a processing instruction has occurred.
*/
	EGlobalTokenPI			= 0x43,

/**
Enumeration with a token value that represents
an unknown tag posessing content but no attributes.
*/
	EGlobalTokenLITERAL_C	= 0x44,

/**
Enumeration with a token value that represents 
an inline integer document-type-specific extension token.
Token is followed by a mb_u_int32.
*/
	EGlobalTokenEXT_T_0		= 0x80,
/**
Enumeration with a token value that represents
an inline integer document-type-specific extension token.
Token is followed by a mb_u_int32.
*/
	EGlobalTokenEXT_T_1		= 0x81,

/**
Enumeration with a token value that represents 
an inline integer document-type-specific extension token.
Token is followed by a mb_u_int32.
*/
	EGlobalTokenEXT_T_2		= 0x82,

/**
Enumeration with a token value that represents 
a String table reference. Followed by a mb_u_int32
encoding a byte offset from the beginning of the string table.

*/
	EGlobalTokenSTR_T		= 0x83, 
/**
Enumeration with a token value that represents 
an unknown tag posessing attributes but no content.
*/
	EGlobalTokenLITERAL_A	= 0x84,

/**
Enumeration with a token value that represents 
a single -byte document-type-specific extension token.
*/
	EGlobalTokenEXT_0		= 0xC0,

/**
Enumeration with a token value that represents
a single -byte document-type-specific extension token.
*/
	EGlobalTokenEXT_1		= 0xC1,

/**
Enumeration with a token value that represents
a single -byte document-type-specific extension token.
*/
	EGlobalTokenEXT_2		= 0xC2,

/**
Enumeration with a token value that represents
a opaque document-type-specific data.
*/
	EGlobalTokenOPAQUE		= 0xC3,

/**
Enumeration with a token value that represents 
an unknown tag posessing both attributes and content.
*/
	EGlobalTokenLITERAL_AC	= 0xC4,

	};

#endif
