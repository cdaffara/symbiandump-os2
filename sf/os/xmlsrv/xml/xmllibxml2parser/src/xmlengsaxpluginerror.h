// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Sax error maping functions.
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGSAXPLUGINERROR_H
#define XMLENGSAXPLUGINERROR_H

LOCAL_C const TInt KErrorArrayGeneralMapSize = 101;
LOCAL_C const TInt KErrorArrayGeneralMapOffset = 0;

LOCAL_C const TInt KErrorArrayGenMap[] = 
	{
    KErrNone,					/* 0 XML_ERR_OK */
    EXmlSyntax,					/* 1 XML_ERR_INTERNAL_ERROR */
    KErrNoMemory,	    		/* 2 XML_ERR_NO_MEMORY */
    EXmlSyntax,					/* 3 XML_ERR_DOCUMENT_START */
    EXmlNoElements,				/* 4 XML_ERR_DOCUMENT_EMPTY */
    EXmlJunkAfterDocElement,	/* 5 XML_ERR_DOCUMENT_END */
    EXmlBadCharRef,				/* 6 XML_ERR_INVALID_HEX_CHARREF */
    EXmlBadCharRef,				/* 7 XML_ERR_INVALID_DEC_CHARREF */
    EXmlBadCharRef,				/* 8 XML_ERR_INVALID_CHARREF */
    EXmlInvalidToken,			/* 9 XML_ERR_INVALID_CHAR */
    EXmlSyntax,					/* 10 XML_ERR_CHARREF_AT_EOF */
    EXmlSyntax,					/* 11 XML_ERR_CHARREF_IN_PROLOG */
    EXmlSyntax,					/* 12 XML_ERR_CHARREF_IN_EPILOG */
    EXmlSyntax,					/* 13 XML_ERR_CHARREF_IN_DTD */
    EXmlSyntax,					/* 14 XML_ERR_ENTITYREF_AT_EOF */
    EXmlSyntax,					/* 15 XML_ERR_ENTITYREF_IN_PROLOG */
    EXmlSyntax,					/* 16 XML_ERR_ENTITYREF_IN_EPILOG */
    EXmlSyntax,					/* 17 XML_ERR_ENTITYREF_IN_DTD */
    EXmlSyntax,					/* 18 XML_ERR_PEREF_AT_EOF */
    EXmlSyntax,					/* 19 XML_ERR_PEREF_IN_PROLOG */
    EXmlSyntax,					/* 20 XML_ERR_PEREF_IN_EPILOG */
    EXmlSyntax,					/* 21 XML_ERR_PEREF_IN_INT_SUBSET */
    EXmlInvalidToken,			/* 22 XML_ERR_ENTITYREF_NO_NAME */
    EXmlInvalidToken,			/* 23 XML_ERR_ENTITYREF_SEMICOL_MISSING */
    EXmlInvalidToken,			/* 24 XML_ERR_PEREF_NO_NAME */
    EXmlInvalidToken,			/* 25 XML_ERR_PEREF_SEMICOL_MISSING */
    EXmlUndefinedEntity,		/* 26 XML_ERR_UNDECLARED_ENTITY */
    KErrNone,					/* 27 XML_WAR_UNDECLARED_ENTITY */
    EXmlSyntax,					/* 28 XML_ERR_UNPARSED_ENTITY */
    EXmlSyntax,					/* 29 XML_ERR_ENTITY_IS_EXTERNAL */
    EXmlSyntax,					/* 30 XML_ERR_ENTITY_IS_PARAMETER */
    EXmlUnknownEncoding,		/* 31 XML_ERR_UNKNOWN_ENCODING */
    EXmlIncorrectEncoding,		/* 32 XML_ERR_UNSUPPORTED_ENCODING */
    EXmlSyntax,					/* 33 XML_ERR_STRING_NOT_STARTED */
    EXmlSyntax,					/* 34 XML_ERR_STRING_NOT_CLOSED */
    EXmlSyntax,					/* 35 XML_ERR_NS_DECL_ERROR */
    EXmlInvalidToken,			/* 36 XML_ERR_ENTITY_NOT_STARTED */
    EXmlUnclosedToken,			/* 37 XML_ERR_ENTITY_NOT_FINISHED */
    EXmlInvalidToken,			/* 38 XML_ERR_LT_IN_ATTRIBUTE */
    EXmlInvalidToken,			/* 39 XML_ERR_ATTRIBUTE_NOT_STARTED */
    EXmlUnclosedToken,			/* 40 XML_ERR_ATTRIBUTE_NOT_FINISHED */
    EXmlInvalidToken,			/* 41 XML_ERR_ATTRIBUTE_WITHOUT_VALUE */
    EXmlDuplicateAttribute,		/* 42 XML_ERR_ATTRIBUTE_REDEFINED */
    EXmlInvalidToken,			/* 43 XML_ERR_LITERAL_NOT_STARTED */
    EXmlUnclosedToken,			/* 44 XML_ERR_LITERAL_NOT_FINISHED */
    EXmlUnclosedToken,			/* 45 XML_ERR_COMMENT_NOT_FINISHED */
    EXmlInvalidToken,			/* 46 XML_ERR_PI_NOT_STARTED */
    EXmlUnclosedToken,			/* 47 XML_ERR_PI_NOT_FINISHED */
    EXmlInvalidToken,			/* 48 XML_ERR_NOTATION_NOT_STARTED */
    EXmlUnclosedToken,			/* 49 XML_ERR_NOTATION_NOT_FINISHED */
    EXmlSyntax,					/* 50 XML_ERR_ATTLIST_NOT_STARTED */
    EXmlUnclosedToken,			/* 51 XML_ERR_ATTLIST_NOT_FINISHED */
    EXmlInvalidToken,			/* 52 XML_ERR_MIXED_NOT_STARTED */
    EXmlUnclosedToken,			/* 53 XML_ERR_MIXED_NOT_FINISHED */
    EXmlInvalidToken,			/* 54 XML_ERR_ELEMCONTENT_NOT_STARTED */
    EXmlUnclosedToken,			/* 55 XML_ERR_ELEMCONTENT_NOT_FINISHED */
    EXmlInvalidToken,			/* 56 XML_ERR_XMLDECL_NOT_STARTED */
    EXmlUnclosedToken,			/* 57 XML_ERR_XMLDECL_NOT_FINISHED */
    EXmlInvalidToken,			/* 58 XML_ERR_CONDSEC_NOT_STARTED */
    EXmlUnclosedToken,			/* 59 XML_ERR_CONDSEC_NOT_FINISHED */
    EXmlUnclosedToken,			/* 60 XML_ERR_EXT_SUBSET_NOT_FINISHED */
    EXmlSyntax,					/* 61 XML_ERR_DOCTYPE_NOT_FINISHED */
    EXmlInvalidToken,			/* 62 XML_ERR_MISPLACED_CDATA_END */
    EXmlUnclosedToken,			/* 63 XML_ERR_CDATA_NOT_FINISHED */
    EXmlSyntax,					/* 64 XML_ERR_RESERVED_XML_NAME */
    EXmlInvalidToken,			/* 65 XML_ERR_SPACE_REQUIRED */
    EXmlSyntax,					/* 66 XML_ERR_SEPARATOR_REQUIRED */
    EXmlSyntax,					/* 67 XML_ERR_NMTOKEN_REQUIRED */
    EXmlInvalidToken,			/* 68 XML_ERR_NAME_REQUIRED */
    EXmlSyntax,					/* 69 XML_ERR_PCDATA_REQUIRED */
    EXmlSyntax,					/* 70 XML_ERR_URI_REQUIRED */
    EXmlSyntax,					/* 71 XML_ERR_PUBID_REQUIRED */
    EXmlInvalidToken,			/* 72 XML_ERR_LT_REQUIRED */
    EXmlInvalidToken,			/* 73 XML_ERR_GT_REQUIRED */
    EXmlSyntax,					/* 74 XML_ERR_LTSLASH_REQUIRED */
    EXmlSyntax,					/* 75 XML_ERR_EQUAL_REQUIRED */
    EXmlTagMismatch,			/* 76 XML_ERR_TAG_NAME_MISMATCH */
    EXmlUnclosedToken,			/* 77 XML_ERR_TAG_NOT_FINISHED */
    EXmlSyntax,					/* 78 XML_ERR_STANDALONE_VALUE */
    EXmlSyntax,					/* 79 XML_ERR_ENCODING_NAME */
    EXmlInvalidToken,			/* 80 XML_ERR_HYPHEN_IN_COMMENT */
    EXmlSyntax,					/* 81 XML_ERR_INVALID_ENCODING */
    EXmlSyntax,					/* 82 XML_ERR_EXT_ENTITY_STANDALONE */
    EXmlSyntax,					/* 83 XML_ERR_CONDSEC_INVALID */
    EXmlSyntax,					/* 84 XML_ERR_VALUE_REQUIRED */
    EXmlSyntax,					/* 85 XML_ERR_NOT_WELL_BALANCED */
    EXmlSyntax,					/* 86 XML_ERR_EXTRA_CONTENT */
    EXmlSyntax,					/* 87 XML_ERR_ENTITY_CHAR_ERROR */
    EXmlSyntax,					/* 88 XML_ERR_ENTITY_PE_INTERNAL */
    EXmlRecursiveEntity,		/* 89 XML_ERR_ENTITY_LOOP */
    EXmlSyntax,					/* 90 XML_ERR_ENTITY_BOUNDARY */
    EXmlSyntax,					/* 91 XML_ERR_INVALID_URI */
    EXmlSyntax,					/* 92 XML_ERR_URI_FRAGMENT */
    KErrNone,					/* 93 XML_WAR_CATALOG_PI */
    EXmlSyntax,					/* 94 XML_ERR_NO_DTD */
    EXmlSyntax,					/* 95 XML_ERR_CONDSEC_INVALID_KEYWORD */
    EXmlSyntax,					/* 96 XML_ERR_VERSION_MISSING */
    KErrNone,					/* 97 XML_WAR_UNKNOWN_VERSION */
    KErrNone,					/* 98 XML_WAR_LANG_VALUE */
    KErrNone,					/* 99 XML_WAR_NS_URI */
    KErrNone,					/* 100 XML_WAR_NS_URI_RELATIVE */ 
    EXmlSyntax,					/* 101 XML_ERR_MISSING_ENCODING */
	};

LOCAL_C const TInt KErrorArrayNSMapSize = 4;
LOCAL_C const TInt KErrorArrayNSMapOffset = 200;

LOCAL_C const TInt KErrorArrayNSMap[] = 
	{
    EXmlSyntax,					/* 200 XML_NS_ERR_XML_NAMESPACE */
    EXmlSyntax,					/* 201 XML_NS_ERR_UNDEFINED_NAMESPACE */
    EXmlSyntax,					/* 202 XML_NS_ERR_QNAME */
    EXmlDuplicateAttribute,		/* 203 XML_NS_ERR_ATTRIBUTE_REDEFINED */
	};

/**
 * GetErrorNum:
 * @param   aString     the string 
 *
 * Mapping from libxml2 error to xml symbian error
 */	
LOCAL_C TInt GetErrorNum(TInt errorNum)
{
	//symbian error
	if (errorNum < 0)
		{
		return errorNum;
		}
	//general error	mapping
	else if (errorNum >= KErrorArrayGeneralMapOffset 
				&& errorNum < KErrorArrayGeneralMapOffset + KErrorArrayGeneralMapSize)
		{
		return KErrorArrayGenMap[errorNum - KErrorArrayGeneralMapOffset];
		}
	//namespace error mapping
	else if (errorNum >= KErrorArrayNSMapOffset 
				&& errorNum < KErrorArrayNSMapOffset + KErrorArrayNSMapSize)
		{
		return KErrorArrayNSMap[errorNum - KErrorArrayNSMapOffset];
		}
	return EXmlSyntax;
}

#endif

