/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#if !defined(__UCS2_H__)
#define __UCS2_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__CHARCONV_H__)
#include <charconv.h>
#endif

/**
Check the contents of aBuffer for a byte Order Mark
@internalComponent
*/
class CnvUCS2 
	{
public:

	LOCAL_D TInt CheckForByteOrderMark(const TDesC8& aBuffer, TInt& aBOMFound);
	};

#endif

