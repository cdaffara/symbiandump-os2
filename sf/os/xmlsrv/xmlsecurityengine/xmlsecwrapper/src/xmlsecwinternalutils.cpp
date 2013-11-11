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
* Description: Helper functions for internal use in Wrapper.       
*
*/


#include <e32base.h>

#include <stdapis/libxml2/libxml2_tree.h>

#include "xmlsec_error_flag.h"

#include "xmlsecwinternalutils.h"

// ---------------------------------------------------------------------------
// Checks the error flag, and if the flag is set: leaves and resets it
// ---------------------------------------------------------------------------
// 
void XmlSecErrorFlagTestL() 
    {
    TInt error = xmlSecCheckErrorFlag(); 
    if ( error )
        {
        xmlSecResetErrorFlag();
        User::Leave( error );
        }    
    }   

// ---------------------------------------------------------------------------
// Free node pointer
// ---------------------------------------------------------------------------
//     
void LibxmlNodeCleanup(
    TAny* aNodePtr )
    {
    xmlFreeNode((xmlNodePtr)aNodePtr);
    }

