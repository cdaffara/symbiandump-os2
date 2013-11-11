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
// This file contains methods that to add/remove XmlEngine user
//

#include "libxml2_globals_private.h"
#include <stdapis/libxml2/libxml2_parser.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengmem.h>

EXPORT_C void XmlEngLeaveL(TInt aCode)
{
    User::Leave(aCode);
}   

EXPORT_C void XmlEngineCleanup(TAny* aParserContext)
    {
    xmlGlobalStatePtr gs = xeGetTLS();
    if (!gs)
        return; // Note: This should not happen in normal conditions!
    if (aParserContext)
        {
        xmlParserCtxtPtr ctxt = reinterpret_cast<xmlParserCtxtPtr>(aParserContext);
        if(ctxt->myDoc)                     
            {
            xmlFreeDoc(ctxt->myDoc); 
            }
        xmlFreeParserCtxt(ctxt);
        }

#undef xeUserCount
    if(gs->xeUserCount > 1)
        {
        gs->xeUserCount--;
        return;
        }

    xmlCleanupGlobalData();
    }


EXPORT_C void XmlEngineAttachL()
    {
    xmlGlobalStatePtr gs = xeGetTLS();
    if(!gs)
        {
        gs = xmlCreateAndInitializeGlobalState();
        if(!gs)
            {
            OOM_HAPPENED;
            }
        }
#undef xeUserCount
    gs->xeUserCount++;
    }

// ------------------------------------------------------------------------------
//  This function should have matching call to XmlEnginePopAndClose()
//  (which is CleanupStack::PopAndDestroy() so far but may become more complex)
// ------------------------------------------------------------------------------
//
EXPORT_C void XmlEnginePushL()
    {
    XmlEngineAttachL();
    CleanupStack::PushL(TCleanupItem(XmlEngineCleanup, NULL));
    }


EXPORT_C void XmlEnginePopAndClose()
    {
    CleanupStack::PopAndDestroy();
    }
