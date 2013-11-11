/*
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
* Key handle type used with KMS.  This class is defined in its own
* file because it is used by lots of different components which do
* not need to know about each other's existence.  For example, a
* KMS client does not need to know about the kernel extension.
* Therefore, it is defined independently of all these components.
*
*/




/**
 @file
 @internalTechnology
*/

#ifndef KEYHANDLE_H
#define KEYHANDLE_H

#include <e32def.h>

class TKeyHandle
/**
	Opaque 32-bit value identifies a key stored in hardware.
	@internalTechnology
 */
	{
public:
	/** @internalTechnology */
	TUint32 iValue;
	};

#endif	// #ifndef KEYHANDLE_H
