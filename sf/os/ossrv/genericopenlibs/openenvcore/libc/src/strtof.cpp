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
// Name         : strtof.cpp
// Part of      : MRT
// Implementation for wcpcpy API
// Version      : 1.0
//


 

#include <e32math.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

extern "C" {

EXPORT_C float strtof(const char* s, char** tail)
{
	double d ;
	d = strtod(s, tail);
		
	if ( d > KMaxTReal32 )
	{
		errno = ERANGE;
		return -HUGE_VALF ;
	}
	if ( d < 0.0)
	{
		d *= -1;
		if (d < KMinTReal32)
		{
			errno = ERANGE;
			return HUGE_VALF ;
		}
		d *= -1;
	}		
		return d;
}

} //extern "C"
