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



/**
 @file
 @publishedPartner
 @released
*/

#ifndef XMLENGMEM_H
#define XMLENGMEM_H

#include <e32base.h>

/**
Checks whether the OOM flag was set.  If the OOM flag is set, it is cleared and 
the function leaves with KErrNoMemory.
@leave KErrNoMemory OOM flag was set
*/
IMPORT_C void XmlEngOOMTestL();

/**
Resets the OOM flag and leaves with KErrNoMemory
@leave KErrNoMemory This function always leaves
*/
IMPORT_C void XmlEngLeaveOOML();

/** Memory handling macros */
#define OOM_IF_NULL(condition)  if (condition) {} else OOM_HAPPENED
#define OOM_IF(condition)       if (!(condition)) {} else OOM_HAPPENED
#define TEST_OOM_FLAG           XmlEngOOMTestL(); 
#define OOM_HAPPENED            XmlEngLeaveOOML()

#endif /* XMLENGMEM_H */
