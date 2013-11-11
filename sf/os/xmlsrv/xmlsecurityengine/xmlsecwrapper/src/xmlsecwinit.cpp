/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Methods for initializing and closing component.       
*
*/
// XML Engine
#include <stdapis/libxml2/libxml2_globals.h>
#include <xml/utils/xmlengxestd.h>

// XML Sec
#include "xmlsec_crypto.h"
#include "xmlsec_error_flag.h"
#include "xmlsecwerrors.h"
#include "xmlsecwinit.h"
#include "xmlsecwglobalstate.h"
#include "xmlsecwkeymanager.h"

#include "xmlsecwinternalutils.h"

// ---------------------------------------------------------------------------
// Get global state for dll
// ---------------------------------------------------------------------------
//
XmlSecGlobalState* XmlSecGetTls()
    {
    return STATIC_CAST(XmlSecGlobalState*, Dll::Tls());
    }

// ---------------------------------------------------------------------------
// Set global state for dll
// ---------------------------------------------------------------------------
//
XmlSecGlobalState* XmlSecSetTlsL()
    {
    XmlSecGlobalState* gs = new(ELeave) XmlSecGlobalState;
    gs->iKeyManager = NULL;
    gs->iUserCount = 0;
    TInt error = Dll::SetTls(gs);
    if(error)
        {
        delete gs;
        User::Leave(error);
        }
    return gs;
    } 

// ---------------------------------------------------------------------------
// UnSet global state for dll
// ---------------------------------------------------------------------------
//    
void XmlSecUnsetTlsDL( void* aGlobalState )
    {
    CleanupStack::PushL(aGlobalState);
    CleanupStack::PopAndDestroy(aGlobalState);
    Dll::SetTls( NULL );
    }
    
// ---------------------------------------------------------------------------
// Initialize wrapper
// ---------------------------------------------------------------------------
// 
EXPORT_C void XmlSecInitializeL()
    {
    /* Init libxml and libxslt libraries */
    XmlSecGlobalState* gs = XmlSecGetTls();
    if(!gs)
        {
        gs = XmlSecSetTlsL();
        }
    if(gs->iUserCount == 0)
        {
        CleanupStack::PushL( TCleanupItem( XmlSecUnsetTlsDL, gs ) );
        XmlEngineAttachL();
        
        RXmlEngDOMImplementation dom;
        dom.OpenL();
        gs->iDOMImpl = &dom;
        xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
        xmlSubstituteEntitiesDefault(1);

        // Init xmlsec library
        if(xmlSecInit() < 0)
            {
            User::Leave(KErrInit);
            }
        // Check loaded library version
        if(xmlSecCheckVersion() != 1)
            {
            User::Leave(KErrInit);
            }
        // Init crypto library
        if(xmlSecCryptoAppInit(NULL) < 0) 
            {
            User::Leave(KErrInit);
            }
        // Init xmlsec-crypto library
        if(xmlSecCryptoInit() < 0) 
            {
            User::Leave(KErrInit);
            }
        //initialization completed
        CleanupStack::Pop( gs );    //gs will be destroyed during cleanup of xmlsec
        }
    gs->iUserCount++;
    }
    
// ---------------------------------------------------------------------------
// Cleanup wrapper
// ---------------------------------------------------------------------------
// 
EXPORT_C void XmlSecCleanup(TAny* globalState)
    {
    XmlSecGlobalState* gs = NULL;
    if(globalState)
        {
        gs = reinterpret_cast<XmlSecGlobalState*>(globalState);
        }
    else
        {
        gs = XmlSecGetTls();
        }
    if(!gs)
        {
        return;
        }
    gs->iUserCount--;
    if(gs->iUserCount > 0)
        {
        return;
        }
    // Shutdown xmlsec-crypto library
    xmlSecCryptoShutdown();
  
    // Shutdown crypto library
    xmlSecCryptoAppShutdown();
  
    // Shutdown xmlsec library
    xmlSecShutdown();
  
#ifndef XMLSEC_NO_XSLT
    xsltCleanupGlobals();            
#endif /* XMLSEC_NO_XSLT */
    gs->iDOMImpl->Close();
	XmlEngineCleanup();
	if(gs->iKeyManager)
	    {
	    delete gs->iKeyManager;
	    }
	delete gs;
	Dll::SetTls(NULL);
	xmlSecResetErrorFlag();
    }

// ---------------------------------------------------------------------------
// Initialize wrapper
// ---------------------------------------------------------------------------
//    
EXPORT_C void XmlSecPushL()
    {
    XmlSecInitializeL();
    CleanupStack::PushL(TCleanupItem(XmlSecCleanup, NULL));
    }

// ---------------------------------------------------------------------------
// Cleanup wrapper
// ---------------------------------------------------------------------------
//     
EXPORT_C void XmlSecPopAndDestroy()
    {
    CleanupStack::PopAndDestroy();
    }

