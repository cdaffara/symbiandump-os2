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
// Dom Utilities not part of SDK API
//

#include <xml/dom/xmlengdomutils.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include "xmlengdomdefs.h"

EXPORT_C void TDomUtils::XmlEngRenameElementL(TXmlEngElement aElement, const TDesC8& aLocalName, 
                               const TDesC8& aNamespaceUri, 
                               const TDesC8& aPrefix)
    {
            aElement.RenameElementL(aLocalName, aNamespaceUri, aPrefix);
    }
