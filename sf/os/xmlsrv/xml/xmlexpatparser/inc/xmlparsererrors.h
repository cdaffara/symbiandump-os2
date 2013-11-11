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

#ifndef XMLPARSERERRORS_H
#define XMLPARSERERRORS_H

/**
@file

This file contains XML specific errors returned by the XML Parser.

@internalTechnology
*/

enum TXmlParserError {
	EXmlParserError = -1000,
	EXmlSyntax = 2+EXmlParserError,		// -998
	EXmlNoElements,						// -997
	EXmlInvalidToken,					// -996
	EXmlUnclosedToken,					// -995
	EXmlPartialChar,					// -994
	EXmlTagMismatch,					// -993
	EXmlDuplicateAttribute,				// -992
	EXmlJunkAfterDocElement,			// -991
	EXmlPeRef,							// -990
	EXmlUndefinedEntity,				// -989
	EXmlRecursiveEntity,				// -988
	EXmlAsyncEntity,					// -987
	EXmlBadCharRef,						// -986
	EXmlBinaryEntityRef,				// -985
	EXmlAttributeExternalEntityRef,		// -984
	EXmlMisplacedPi,					// -983
	EXmlUnknownEncoding,				// -982
	EXmlIncorrectEncoding,				// -981
	EXmlUnclosedCdata,					// -980
	EXmlExternalEntityHandling,			// -979
	EXmlNotStandalone,					// -978
	EXmlUnexpectedState,				// -977
	EXmlEntityDeclInPe,					// -976
	EXmlDtdRequired,					// -975
	EXmlFeatureLockedWhileParsing		// -974
};

#endif  // XMLERRORS_H
