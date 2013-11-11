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
// Methods for all kind of text nodes
//

#include <xml/dom/xmlengdatacontainer.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_tree.h>

// -------------------------------------------------------------------------------
// @return Cid of the container
//
// -------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngDataContainer::Cid() const
    {
    return TPtrC8( (const unsigned char *)LIBXML_NODE->ns);
    }

// -------------------------------------------------------------------------------
// @return Data size in bytes
// -------------------------------------------------------------------------------
//
EXPORT_C TUint TXmlEngDataContainer::Size() const
    {
    return (TUint)LIBXML_NODE->properties;
    }
    
