/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
*/

#ifndef __USB_STD_H__
#define __USB_STD_H__

#include <e32def.h>

NONSHARABLE_CLASS(TUsbDescriptor)
/** Used by Class Controllers to express information about their descriptors.

  @publishedPartner
  @released
  */
	{
public:
	/**
	Number of interfaces this class controller is responsible for.
	*/
	TInt iNumInterfaces;

	/**
	Total length of interfaces this class controller is responsible for.
	*/
	TInt iLength;
	};

#endif // __USB_STD_H__

