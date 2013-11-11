/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Function: __nan(), __nanf();
* This functions will return the NAN(Not a number) value for the 
* Double and single precision floating point value.
*
*/



#include <math.h>
#include "fpmath.h"

EXPORT_C double nanval()
{	
	union IEEEd2bits u;
	
	/*
	NAN = 7FF8000000000000;
	*/
	u.bits.sign 	= 	0;
	u.bits.exp	=	2047;
	u.bits.manh	=	524288;
	u.bits.manl	=	0;

return u.d;
}

EXPORT_C float nanvalf()
{	
	union IEEEf2bits u;
	
	/*
	NANF = 7FC00000;
	*/
	u.bits.sign=0;
	u.bits.exp=255;
	u.bits.man=4194304;
	
return u.f;
}
