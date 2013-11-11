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
// This file contains generic XML constants
// 
//

/**
 @file
*/

#ifndef __GMXMLCONSTANTS_H__
#define __GMXMLCONSTANTS_H__

#include <e32base.h>
#include <gmxmldomconstants.h>

/** XML file type options. 
@publishedPartner
@released
*/
enum TXMLFileType
	{
	/** ASCII file. */
	EAscii = 1,
	/** Unicode file. */
	EUnicode = 2,
	/** UTF8 file. */
	EUtf8 = 3
	};

const TInt KReadBufferLen = 1024;
const TInt KWriteBufferLen = 1024;

// Error codes
/** XML element nesting error code. */
const TInt KErrXMLBadNesting = 					KErrXMLBase - 4;
/** XML element incomplete error code. */
const TInt KErrXMLIncomplete = 					KErrXMLBase - 5;
/** XML element name error code. */
const TInt KErrXMLBadElementName = 				KErrXMLBase - 6;
/** Error code that indicates that more than one DOCTYPE tag has been encountered: 
the first one encountered will be used. */
const TInt KErrXMLDuplicateDocTypeTags = 		KErrXMLBase - 7;
/** Error code that indicates that more than one XML version tag has been encountered: 
the first one encountered will be used. */
const TInt KErrXMLDuplicateVersionTags = 		KErrXMLBase - 8;
/** Error code that indicates that more than one element has been encountered at 
the root of the document (there should be a single <smil> element). The resulting 
tree may be confused, but is provided rather than being discarded. */
const TInt KErrXMLDuplicateRootElements =		KErrXMLBase - 9;
/** Error code that indicates that no DOCTYPE tag was encountered. */
const TInt KErrXMLMissingDocTypeTag = 			KErrXMLBase - 10;
/** Error code that indicates that no XML version tag was encountered. */
const TInt KErrXMLMissingVersionTag = 			KErrXMLBase - 11;
/** Error code that indicates that an element has two or more entries for the same 
attribute. */
const TInt KErrXMLDuplicateAttributeName =	KErrXMLBase - 12;
/** Error code that indicates there is no single root element.

This may be the result of two of more root elements being added incorrectly, 
or a missing root element leading to the child elements being assumed to be 
root nodes. */
const TInt KErrXMLMultipleRootElements = 		KErrXMLBase - 13;
/** Error code that indicates spurious characters have been found.

This may occur as the result of a missing or an extra attribute or element 
delimiter. */
const TInt KErrXMLCorruptFile = 					KErrXMLBase - 14;
/** Error code that indicates a built-in entity or element / attribute delimiter 
is at an inappropriate place. */
const TInt KErrXMLIllegalCharacter = 			KErrXMLBase - 15;
const TInt KErrXMLBadEntity =					KErrXMLBase - 16;
/** Error code that indicates an element was found to be invalid by the DTD */
const TInt KErrXMLInvalidElement =				KErrXMLBase - 17;
/** Error code that indicates an attribute was found to be invalid by the DTD */
const TInt KErrXMLInvalidAttribute =			KErrXMLBase - 18;

/** Severity levels for XML composer and parser errors. 
@publishedPartner
@released
*/
enum TXMLErrorCodeSeverity
	{
	/** Fatal error. */
	EXMLFatal,
	/** Severity level cannot be determined. */
	EXMLIndeterminate,
	/** Recoverable error. */
	EXMLWorkable,
	/** No error. */
	EXMLNone
	};


_LIT(KNewLine, "\r\n");

#ifdef _DEBUG
	_LIT(KTab, "   ");
#endif

_LIT(KXMLSemiColon, ";");
_LIT(KXMLDecimalId, "&#");
_LIT(KXMLHexidecimalId, "&#x");
_LIT(KXMLStartComment, "<!--");
_LIT(KXMLEndComment, "-->");
_LIT(KXMLStartEndTag, "</");
_LIT(KXMLStartTag, "<");
_LIT(KXMLEndStartTag, "/>");
_LIT(KXMLStartProcessingInstruction, "<?");
_LIT(KXMLEndProcessingInstruction, "?>");
_LIT(KXMLStartCDataSection, "<![CDATA[");
_LIT(KXMLEndCDataSection, "]]>");

_LIT(KBackSlash, "\\");
_LIT(KDash, "-");
_LIT(KExclamation, "!");
_LIT(KForwardSlash, "/");
_LIT(KEqualSign, "=");
_LIT(KLeftArrow, "<");
_LIT(KRightArrow, ">");
_LIT(KAmpersand, "&");
_LIT(KApostrophe, "\'");
_LIT(KQuotation, "\"");
_LIT(KSingleSpace, " ");


const TInt KNominalTagLength = 256;
const TInt KStartPosition = -1;

#endif
