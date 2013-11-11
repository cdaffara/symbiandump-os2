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
// Utils for XML Engine
//

#define IN_LIBXML
#include <stdlib.h>
#include <string.h>
#include "libxml2_globals_private.h"
#include <e32std.h>

/**
Return address at the limit of the system stack.

When this address is reached, stack overflow occurs.
*/
unsigned int xeStackLimitAddress()
    {
    TThreadStackInfo aInfo;
    RThread().StackInfo(aInfo);
    return (unsigned int) aInfo.iLimit;
    }

/**
Retrieves a pointer to the 'global state' structure that contains all global settings

Initialization of global data is performed when first user of XML Engine calls
XmlEngine::XmlEngineAttachL() or XmlEngine::XmlEnginePushL()
*/
XMLPUBFUNEXPORT xmlGlobalStatePtr xmlGetGlobalState(void)
    {
    // DONE: Move code that initializes global data into XmlEngine::XmlEngineAttachL()
    xmlGlobalStatePtr gs = STATIC_CAST(xmlGlobalStatePtr, Dll::Tls());
    
    return gs;
    }

/**
Sets value of tread-local storage.
*/
void xeSetTLS(void* aPtr)
    {
    Dll::SetTls(aPtr);
    }

/**
Returns pointer to the global state structure as it is stored in TLS.

Use only xeGetTLS() for checking the content of libxml2's TLS, otherwise
Dll:Tls() returns value, which is specific to the caller's DLL and thread, not libxml2's!
*/
XMLPUBFUNEXPORT xmlGlobalStatePtr xeGetTLS()
    {
    return STATIC_CAST(xmlGlobalStatePtr, Dll::Tls());
    }
