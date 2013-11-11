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
// This file contains definitions that are related 
// to memory mangement and OOM detection
//

#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengmem.h>
#include "libxml2_globals_private.h"

//---------------------------------------------------------
// Checks whether OOM flag was set up.
// If OOM flag is set, it is cleared and Leave occurs
// with code KErrNoMemory
//---------------------------------------------------------
//
EXPORT_C void XmlEngOOMTestL()
    {
    if (xmlOOMFlag())
        {
        XmlEngLeaveOOML();
        }
    }

//--------------------------------------------------------
// Performs leave action when OOM condition is met.
// This is a centralized point of initiation of OOM leave
//--------------------------------------------------------
//
EXPORT_C void XmlEngLeaveOOML()
    {
    xmlResetOOM(); // reset OOM flag in libxml2
    XmlEngLeaveL(KErrNoMemory);
    }

