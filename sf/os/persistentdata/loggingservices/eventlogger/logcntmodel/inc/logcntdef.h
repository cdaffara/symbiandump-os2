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
//

#ifndef __LOGCNTDEF_H__
#define __LOGCNTDEF_H__

#include <e32def.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** Contact item ID. These are used to uniquely identify contact items within a contacts database.
@internalAll */
typedef TInt32 TLogContactItemId;

#else

#include <logwrap.h>

#endif

/** NULL contact item ID. Indicates that no contact item is present.	
@internalAll */
const TLogContactItemId KLogNullContactId=-1;

/** Format for Given name, Family name string.
  -  Western format is Given followed by Family
  -  Chinese format is Family followed by Given
   	
@internalAll */
enum TLogContactNameFormat {ELogWesternFormat, ELogChineseFormat} ;

#endif
