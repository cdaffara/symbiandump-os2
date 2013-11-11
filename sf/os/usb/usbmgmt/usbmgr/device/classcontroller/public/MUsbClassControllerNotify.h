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
* MClassControllerNotify.h
* Implements part of the USB Class API Framework
* This is the mixin used by the USB classes to notify UsbMan of failures,
* and to get an iterator over the set of classes UsbMan owns.
*
*/

/**
 @file
*/

#ifndef __MUSBCLASSCONTROLLERNOTIFY_H__
#define __MUSBCLASSCONTROLLERNOTIFY_H__

#include <e32def.h>

class CUsbClassControllerIterator;

/** The MUsbClassControllerNotify class
 *
 * Implements part of the USB Class API Framework.
 * This is the mixin used by the USB classes to notify UsbMan of any changes
 * in their state.

  @publishedPartner
  @released
 */
class MUsbClassControllerNotify
	{
public:
	/**
	 * Creates and returns a new iterator over USB classes. The caller takes
	 * ownership of this iterator.
	 *
	 * @return A new iterator
	 */
	 virtual CUsbClassControllerIterator*
		UccnGetClassControllerIteratorL() = 0;

	/**
	 * Called when a USB class has received an unexpected error and should be
	 * shut down.
	 *
	 * @param aError The error code
	 */
	 virtual void UccnError(TInt aError) = 0;
	};
#endif

