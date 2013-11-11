/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Functions handling error situations.
*
*/

#include <e32def.h>

#include "xmlsec_globals.h"
#include "xmlsec_error_flag.h"

static int XmlSecErrorFlag = 0;

// ---------------------------------------------------------------------------
// Set out of memory flag.
// ---------------------------------------------------------------------------
//
EXPORT_C 
void xmlSecSetErrorFlag(int error)
    {
    XmlSecErrorFlag = error;
    }
    
// ---------------------------------------------------------------------------
// Reset out of memory flag.
// ---------------------------------------------------------------------------
//
EXPORT_C 
void xmlSecResetErrorFlag()
    {
    XmlSecErrorFlag = 0;
    }

// ---------------------------------------------------------------------------
// Check out of memory flag.
// ---------------------------------------------------------------------------
//
EXPORT_C 
int xmlSecCheckErrorFlag()
    {
    return XmlSecErrorFlag;
    }
