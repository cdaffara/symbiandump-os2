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
// Text node functions
//

#include <xml/dom/xmlengtext.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_parserinternals.h>

EXPORT_C TBool TXmlEngTextNode::IsElementContentWhitespace() const
    {
    // The code is taken and adapted from  xmlIsBlankNode:
    const xmlChar *cur;
    xmlNodePtr node = LIBXML_NODE;
    if (!node) 
        return EFalse;
    
    if (!(node->content)) 
        return ETrue;
    cur = node->content;
    while (*cur != 0) 
        {
	    if (!IS_BLANK_CH(*cur)) 
            return EFalse;
	    cur++;
        }
    return ETrue;
    }
