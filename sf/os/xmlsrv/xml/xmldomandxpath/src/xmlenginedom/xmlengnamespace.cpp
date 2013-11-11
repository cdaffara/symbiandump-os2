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
//

#include <xml/dom/xmlengnamespace.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlengdomdefs.h"
#include <xml/utils/xmlengxestrings.h>	//pjj18 new

#define LIBXML_NS (static_cast<xmlNsPtr>(iInternal))

// -----------------------------------------------------------------------------
// @return Namespace URI string 
// 
// For TXmlEngNamespace(NULL) or namespace undeclaration (xmlns=""), which are
// treatet as <i>undefined namespace</i> returns NULL,
// otherwise result is not a NULL string and not a "" (empty string).
//
// @note "" is never returned - it is replaced with NULL
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNamespace::Uri() const
    {
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(
               LIBXML_NS &&
               LIBXML_NS->href &&  
               *(LIBXML_NS->href) 
                     ? LIBXML_NS->href 
                     : NULL
           )).PtrC8();    
    }

// -----------------------------------------------------------------------------
// Returns prefix that is bound in the namespace declaration
// or NULL string for default namespace or if no binding exist
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNamespace::Prefix() const
    {
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(
             LIBXML_NS 
                ? LIBXML_NS->prefix 
                : NULL
           )).PtrC8();
    }

// -----------------------------------------------------------------------------
// @return Whether it is a definition of default namespace
//         TRUE  -- is a default namespace (no prefix)
//         FALSE -- not a default namespace (bound to prefix) or empty 
//                  TXmlEngNamespace(NULL) instance
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngNamespace::IsDefault() const
    {
    return IsUndefined() || (!LIBXML_NS->prefix);
    }

// -----------------------------------------------------------------------------
// @return Whether the namespace is undefined
//
// A node's namespace is undefined if no namespace declaration associated with it.
//
// @note Same as TXmlEngNode::IsNull()
// -----------------------------------------------------------------------------
// 
EXPORT_C TBool TXmlEngNamespace::IsUndefined() const
    {
    return IsNull() || (!LIBXML_NS->href);
    }

