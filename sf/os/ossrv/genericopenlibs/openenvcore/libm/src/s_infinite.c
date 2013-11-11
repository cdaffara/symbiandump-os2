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
* This material, including documentation and any related 
* computer programs, is protected by copyright controlled by 
* Nokia Corporation. All rights are reserved. Copying, 
* including reproducing, storing, adapting or translating, any 
* or all of this material requires the prior written consent of 
* Nokia Corporation. This material also contains confidential 
* information which may not be disclosed to others without the 
* prior written consent of Nokia Corporation.
* Function:
* This functions will return the infinite value for the 
* Double and single precision floating point value.
* The Sign of the result will be same as the of the argument 
* passed to the function. 
*
*/



#include <math.h>
#include "fpmath.h"

EXPORT_C double __infinite(double x)
{	
	union IEEEd2bits u, dummy;
	
	dummy.d = x;
	
	u.bits.sign=dummy.bits.sign;
	u.bits.exp=2047;
	u.bits.manh=0;
	u.bits.manl=0;

return u.d;
}

EXPORT_C float __infinitef(float x)
{	
	union IEEEf2bits u, dummy;
	
	dummy.f = x;
	u.bits.sign=dummy.bits.sign;
	u.bits.exp=255;
	u.bits.man=0;
	
return u.f;
}
