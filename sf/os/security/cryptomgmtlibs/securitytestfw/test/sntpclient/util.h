#ifndef __UTIL_H__
#define __UTIL_H__/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <e32base.h>

class Util
	{
public:
	static void SetAppropriateTimezoneL();
	static TBool DaylightSavingsAppliesL(const TTime& utc);
	};

#endif /* __UTIL_H__ */
