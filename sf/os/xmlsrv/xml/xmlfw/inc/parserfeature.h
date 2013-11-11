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

#ifndef PARSERFEATURE_H
#define PARSERFEATURE_H

/** 
@file
@SYMPurpose Declares XML Framework parser features 
@publishedAll
@released
*/


namespace Xml
{

/** Features defined for XML Framework parser plug-ins.

Every parser has a set of optional features that it may implement. This
enumeration is the set of all features known to the framework and its 
plug-ins. Each feature is assigned a flag bit from the 32 possible.

@see Xml::CParser::EnableFeature
*/
enum TParserFeature 
	{
	/** Convert elements and attributes to lowercase. This can be used for 
	case-insensitive HTML so that a tag can be matched to a static string in 
	the string pool.
	@see RStringPool */
	EConvertTagsToLowerCase		= 0x0001,

	/** Report an error when unrecognised tags are found. */
	EErrorOnUnrecognisedTags	= 0x0002,

	/** Reports unrecognised tags. */
	EReportUnrecognisedTags		= 0x0004,

	/** Report the namespace. */
	EReportNamespaces			= 0x0008,

	/** Report the namespace prefix. */
	EReportNamespacePrefixes	= 0x0010,

	/** Send all content data for an element in one chunk. */
	ESendFullContentInOneChunk	= 0x0020,

	/** Report namespace mappings via the OnStartPrefixMapping & 
	OnEndPrefixMapping methods.
	@see MContentHandler */
	EReportNamespaceMapping		= 0x0040,

	/** Describe the data in the specified encoding, otherwise use utf-8. */
	ERawContent					= 0x0080,

	/** When this feature is enabled the parser will allow (not generate an 
	error) entities from external subsets to appear in element attribute 
	values. When the replacement text of such entities are not known the
	parser will	return the entity reference as it appears in the document 
	e.g. "&ext_ref;" */
	EReplaceIntEntityFromExtSubsetByRef = 0x0100,

	/** When this feature is not supported the parser is an XML 1.0 parser.
	When set parser can handle both XML 1.1 & 1.0 documents, otherwise just 
	XML 1.0. Equivalent to the standard SAX2 feature "xml-1.1" */
	EXmlVersion_1_1				= 0x0200,

	/** Used to mark the highest flag in use.
	Update to reflect any new enumerations added. */
	ELastFeature               = EXmlVersion_1_1,
	};

}

#endif // PARSERFEATURE_H
