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
// This file contains methods definition
// that to add/remove XmlEngine user
//



/**
 @file
 @publishedPartner
 @released
*/

#ifndef XMLENGXESTD_H
#define XMLENGXESTD_H

#include <e32base.h>

/**
Leaves with the specified error code.
@param aCode Leave code
@leave - Leaves with the specified error code
*/
IMPORT_C void XmlEngLeaveL(TInt aCode);

/**
Initializes the global data required for libxml2 and pushes a cleanup 
item for XML Engine internal data to the CleanupStack.
 
This function should be used before any use of the DOM Parser or
RXmlEngDocument::SaveL() method.

This function should have matching call to XmlEnginePopAndClose().

@see XmlEnginePopAndClose
@see XmlEngineCleanup
@see XmlEngineAttachL

@leave KErrNoMemory Memory allocation error
*/
IMPORT_C void XmlEnginePushL();

/**
Removes a cleanup item for XML Engine internal data from the CLeanupStack
and frees any memory allocated by XML Engine's internals

@see XmlEnginePushL
*/
IMPORT_C void XmlEnginePopAndClose();

/**
Adds a user of Xml Engine and gets or initializes the global data required for libxml2.
It does NOT put a cleanup item on the CleanupStack. 

A "user" here is any processing entity that will use XML Engine's API (SAX,
DOM, XPath, etc)).  If several instances of XML parser are used within one
thread, then XmlEngineAttach() should be called for each of them.

Every call to this method should be matched with a call to XmlEngineCleanup().

@see XmlEngineCleanup
@see XmlEnginePushL
*/
IMPORT_C void XmlEngineAttachL();

/**
Cleans up resources internally allocated by XML parser and (optionally) frees a
parser context provided as an argument.  Does NOT pop a cleanup item off the 
CleanupStack

This function is to be used as a TCleanupItem on the CleanupStack
and may be invoked directly.

Libxml2 uses the Symbian implementation of POSIX standard libraries. During
closure of XmlEngine library, the standard library is also closed by
default.User can choose not to close the standard library when XmlEngine shuts
down, by using overloaded XmlEngineCleanup(TBool) method instead.

@see XmlEngineAttachL
@see XmlEnginePopAndClose

@param aParserContext A pointer to xmlParserCtxt
*/
IMPORT_C void XmlEngineCleanup(TAny* aParserContext = NULL);

#endif // XMLENGXESTD_H

