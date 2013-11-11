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
// This file contains methods allow to change XmlChar to TDesC
// and TDesC to XmlChar
//



/**
 @file
 @publishedPartner
 @released
*/
#ifndef XMLENGUTILS_H
#define XMLENGUTILS_H

#include <string.h>
#include <e32base.h>

/**
Converts a xml string that may contains entities to a xml string
with entity references, i.e. "t>x" -> "t&gt;x"

@param aUnescapedString The string to escape
@return The escaped string.  Ownership is transferred to the caller.  The
returned string should be freed with free().
@leave KErrNoMemory Memory allocation error
@leave - Otherwise any other system wide error code
*/
IMPORT_C char* XmlEngEscapeForXmlValueL(const char* aUnescapedString);

/**
Convert a UTF-16 descriptor into a UTF-8 zero-terminated string

@param aDes The descriptor to convert
@return The returned string.  Ownership is transferred to the caller.  The
returned string should be freed with delete. 
@leave KErrNoMemory Memory allocation error
@leave - Otherwise any other system wide error code
*/
IMPORT_C char* XmlEngXmlCharFromDesL(const TDesC& aDes);

/**
Convert a TDesC8 descriptor with UTF-8 characters into a UTF-8 zero-terminated
string.

@param aDes The descriptor to convert
@return The returned string.  Ownership is transferred to the caller.  The
returned string should be freed with delete.
@leave KErrNoMemory Memory allocation error
@leave - Otherwise any other system wide error code
*/
IMPORT_C char* XmlEngXmlCharFromDes8L(const TDesC8& aDes);

#endif // XMLENGUTILS_H


