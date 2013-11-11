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
// XPath utils
//

#include <xml/dom/xmlengxpathutils.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include <stdapis/libxml2/libxml2_xpathinternals.h>
#include "xmlengdomdefs.h"
#include <xml/dom/xmlengnode.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

#define XPATH_OBJ (static_cast<xmlXPathObjectPtr>(iInternal))

// ---------------------------------------------------------------------------
// Converts node to number
// ---------------------------------------------------------------------------
//
EXPORT_C TReal XmlEngXPathUtils::ToNumberL(
    const TXmlEngNode& aNode )
    {
    // construct a fake node set with one node
    TXmlEngNode node = aNode;
    xmlNodeSet nset;
    xmlNodePtr pNode = INTERNAL_NODEPTR(node);
    nset.nodeNr = 1;
    nset.nodeMax = 1;
    nset.nodeTab = &pNode;
    //
    TReal number = xmlXPathCastNodeSetToNumber(&nset);
    TEST_OOM_FLAG;
    return number;
    }
    
// ---------------------------------------------------------------------------
// Converts node-set to number
// ---------------------------------------------------------------------------
//
EXPORT_C TReal XmlEngXPathUtils::ToNumberL(
    const RXmlEngNodeSet& aNodeSet )
    {
    TReal number = xmlXPathCastNodeSetToNumber(INTERNAL_NODESETPTR(aNodeSet));
    TEST_OOM_FLAG;    
    return number;
    }
    
// ---------------------------------------------------------------------------
// Converts TBool value to number
// ---------------------------------------------------------------------------
//
EXPORT_C TReal XmlEngXPathUtils::ToNumberL(
    TBool aValue )
    {
    return aValue ? (1.0) :(0.0);
    }
    
// ---------------------------------------------------------------------------
// Converts string to number
// ---------------------------------------------------------------------------
//
EXPORT_C TReal XmlEngXPathUtils::ToNumberL(
    const TDesC8& aString )
    {
    xmlChar* str = xmlCharFromDesC8L(aString);
    TReal number = (TReal)xmlXPathStringEvalNumber(str);
    delete str;
    TEST_OOM_FLAG;    
    return number;
    }
    
// ---------------------------------------------------------------------------
// Converts node to string
// ---------------------------------------------------------------------------
//
EXPORT_C void XmlEngXPathUtils::ToStringL(
    const TXmlEngNode& aNode, RBuf8& aOutput)
    {
    aNode.WholeTextContentsCopyL(aOutput);
    }

// ---------------------------------------------------------------------------
// Converts node-set to string
// ---------------------------------------------------------------------------
//
EXPORT_C void XmlEngXPathUtils::ToStringL(
    const RXmlEngNodeSet& aNodeSet, RBuf8& aOutput)
    {
    xmlChar* text= xmlXPathCastNodeSetToString(INTERNAL_NODESETPTR(aNodeSet));
    OOM_IF_NULL(text);    
    xmlCharAssignToRbuf8L(aOutput,text);
    }

// ---------------------------------------------------------------------------
// Converts TBool value to string
// ---------------------------------------------------------------------------
//
EXPORT_C void XmlEngXPathUtils::ToStringL(
    TBool aValue, RBuf8& aOutput)
    {
    xmlChar* text = xmlStrdup((const xmlChar *)(aValue ? "true" : "false"));
    OOM_IF_NULL(text);    
    xmlCharAssignToRbuf8L(aOutput,text);
    }

// ---------------------------------------------------------------------------
// Converts TReal value to string
// ---------------------------------------------------------------------------
//
EXPORT_C void XmlEngXPathUtils::ToStringL(
    TReal aValue, RBuf8& aOutput )
    {
    xmlChar* text = xmlXPathCastNumberToString(aValue);
    OOM_IF_NULL(text);    
    xmlCharAssignToRbuf8L(aOutput,text);
    }

// ---------------------------------------------------------------------------
// Converts node to boolean
// ---------------------------------------------------------------------------
//
EXPORT_C TBool XmlEngXPathUtils::ToBoolean(
    const TXmlEngNode& aNode )
    {
    return aNode.NotNull();
    }

// ---------------------------------------------------------------------------
// Converts node-set to boolean
// ---------------------------------------------------------------------------
//
EXPORT_C TBool XmlEngXPathUtils::ToBoolean(
    const RXmlEngNodeSet& aNodeSet )
    {
    return xmlXPathCastNodeSetToBoolean(INTERNAL_NODESETPTR(aNodeSet));
    }
    
// ---------------------------------------------------------------------------
// Converts string to boolean
// ---------------------------------------------------------------------------
//
EXPORT_C TBool XmlEngXPathUtils::ToBoolean(
    const TDesC8& aValue )
    {
    return aValue.Length() ? 1 : 0;
    }

// ---------------------------------------------------------------------------
// Converts stringArg to boolean
// @deprecated
// ---------------------------------------------------------------------------
//
EXPORT_C TBool XmlEngXPathUtils::ToBooleanL(
    const TDesC8& aValue )
    {
    return ToBoolean(aValue);
    }

// ---------------------------------------------------------------------------
// Converts TReal value to boolean
// ---------------------------------------------------------------------------
//
EXPORT_C TBool XmlEngXPathUtils::ToBoolean(
    TReal aValue )
    {
    return xmlXPathIsNaN(aValue) || (aValue == 0.0) ?  0 : 1;
    }

