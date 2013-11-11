// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Support routines for managing Time values,
// provided by the LTIME.LIB component of STDLIB
// 
//

#include <e32std.h>	// for TTime
#include <time.h>	// for time_t
/**
@internalComponent
*/
GLREF_C time_t as_time_t(const TTime& t);
