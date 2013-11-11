/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Random Server Codes
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __RANDSVR_H__
#define __RANDSVR_H__

#include <e32base.h>
#include <hash.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "randsvrimpl.h"
#endif

IMPORT_C TInt RunRandomServer(TAny* /*someParameters*/);

#endif // __RANDSVR_H__
