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
// Methods for element node
//

#include <xml/dom/xmlengdomimplementation.h>
#include <xml/utils/xmlengxestd.h>
#include "xmlengdomdefs.h"

// ---------------------------------------------------------------------------------------------
// Opens DOMImplementation
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDOMImplementation::OpenL( RHeap* /*aHeap not implemented yet*/ )
    {
    XmlEngineAttachL();
	SetUserDataCallbacks();
    };

// ---------------------------------------------------------------------------------------------
// Close DOMImplementation
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDOMImplementation::Close(TBool /*aReserved*/)
    {
	Close();
    };

// ---------------------------------------------------------------------------------------------
// Close DOMImplementation
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDOMImplementation::Close()
    {
    XmlEngineCleanup();
    };

