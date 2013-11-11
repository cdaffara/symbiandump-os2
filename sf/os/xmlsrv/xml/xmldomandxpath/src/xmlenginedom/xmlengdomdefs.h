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
// Declaration of macros used in DOM and XPath
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGDOMDEFS_H
#define XMLENGDOMDEFS_H

#include <stdapis/libxml2/libxml2_parser.h>
#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengmem.h>
#include <xml/dom/xmlengerrors.h>

// warning C4238: nonstandard extension used : class rvalue used as lvalue
// it happens often during casts or passing non-variable arguments to methods expecting a non-const reference
//#pragma warning(disable: 4238)

#define INTERNAL_NODEPTR(aNodeObject)   (*reinterpret_cast<xmlNodePtr*>(&(aNodeObject)))
#define INTERNAL_ATTRPTR(aAttrObject)   (*reinterpret_cast<xmlAttrPtr*>(&(aAttrObject)))
#define INTERNAL_NSPTR(aNsDefObject)    (*reinterpret_cast<xmlNsPtr*>(&(aNsDefObject)))
#define INTERNAL_DOCPTR(aDocObject)     (*reinterpret_cast<xmlDocPtr*>(&(aDocObject)))
#define INTERNAL_NODESETPTR(aSetObj) \
           (*reinterpret_cast<xmlNodeSetPtr*>(const_cast<RXmlEngNodeSet*>(&(aSetObj))))
#define INTERNAL_XPATHOBJPTR(aObj) \
           (*reinterpret_cast<xmlXPathObjectPtr*>(const_cast<RXmlEngXPathResult*>(&(aObj))))
#define LIBXML_NODE (static_cast<xmlNodePtr>(iInternal))
#define CAST_DOMSTRING_TO_XMLCHAR(domstring) \
           (reinterpret_cast<const xmlChar*>(domstring.Cstring()))
#define CAST_XMLCHAR_TO_DOMSTRING(libxmlString) \
		(TXmlEngConstString(reinterpret_cast<const char*>(libxmlString)))
#define CAST_XMLCHAR_TO_STRING(libxmlString) \
		(TXmlEngString(reinterpret_cast<char*>(libxmlString)))
#define CAST_NODELIST_REF(List) ((TXmlEngNodeListImpl&)(List))

#define TDOMNODETYPENONE ((TXmlEngDOMNodeType)0)
//data container type is stored in psvi field of text node
#define TEXT_NODE_DATA_TYPE LIBXML_NODE->psvi

#define WRONG_USE_OF_API    XmlEngLeaveL(KXmlEngErrWrongUseOfAPI)
#define XE_ASSERT_DEBUG(assertion)  __ASSERT_DEBUG((assertion),  XmlEngLeaveL(KXmlEngErrWrongUseOfAPI))
#define XE_ASSERT_ALWAYS(assertion) __ASSERT_ALWAYS((assertion), XmlEngLeaveL(KXmlEngErrNullNode))

void SetUserDataCallbacks();

xmlChar* xmlCharFromDesC8L(const TDesC8& aDesc);

void xmlCharAssignToRbuf8L(RBuf8& aOutput,xmlChar *text);

#endif /* XMLENGDOMDEFS_H */
