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
* This contains all definitions required by clients accessing the 
* Content Access Framework.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CAF_H__
#define __CAF_H__

#include <e32std.h>

// Enumerations
#include <caf/caftypes.h>

// Errors and Panics
#include <caf/caferr.h>
#include <caf/cafpanic.h>

// Client side classes for use by the general public
#include <caf/data.h>
#include <caf/content.h>
#include <caf/agent.h>

// Utiltiy classes from CafUtils
#include <caf/metadata.h>
#include <caf/metadataarray.h>
#include <caf/virtualpath.h>
#include <caf/virtualpathptr.h>
#include <caf/embeddedobject.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <caf/streamableptrarray.h>
#include <caf/dirstreamable.h>

#ifndef SYMBIAN_ENABLE_PUBLIC_PLATFORM_HEADER_SPLIT
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#endif // __CAF_H__
