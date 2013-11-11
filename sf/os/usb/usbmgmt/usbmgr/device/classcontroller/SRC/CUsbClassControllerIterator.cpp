/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a helper class for iterating over CUsbClassControllerBase 
* objects.
*
*/

/**
 @file
*/


#include <cusbclasscontrolleriterator.h>
#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbClassControllerIteratorTraces.h"
#endif



// Panic category only used in debug builds
#ifdef _DEBUG
_LIT( KUsbCcIteratorPanicCategory, "UsbCcIterator" );
#endif

/**
 * Panic codes for the USB Class Controller Iterator.
 */
enum TUsbCcIteratorPanic
	{
	/** Class controller index is out of range */
	EIndexOutOfRange  = 0,
	};


/**
 * Constructor. Initialises the internal reference to a class array.
 */
EXPORT_C CUsbClassControllerIterator::CUsbClassControllerIterator(
	RPointerArray<CUsbClassControllerBase>& aClassControllerArray)
	: iClassControllerArray(aClassControllerArray)
{
}

/**
 * Destructor.
 */
 EXPORT_C CUsbClassControllerIterator::~CUsbClassControllerIterator()
	{
	}

/**
 * Sets the iterator to the first USB class.
 *
 * @return KErrNotFound if there are no USB classes.
 */
 EXPORT_C TInt CUsbClassControllerIterator::First()
	{
	if (iClassControllerArray.Count() > 0)
		{
		iClassControllerIndex = 0;
		return KErrNone;
		}
	return KErrNotFound;
	}

/**
 * Sets the iterator to the next USB class.
 *
 * @return KErrNotFound if there are no more classes
 */
EXPORT_C TInt CUsbClassControllerIterator::Next()
	{
	if (iClassControllerIndex < (iClassControllerArray.Count()-1))
		{
		iClassControllerIndex++;
		return KErrNone;
		}
	return KErrNotFound;
	}

/**
 * Sets the iterator to the previous USB class.
 *
 * @return KErrNotFound if there are no more classes
 */
EXPORT_C TInt CUsbClassControllerIterator::Previous()
	{
	if (iClassControllerIndex > 0)
		{
		iClassControllerIndex--;
		return KErrNone;
		}
	return KErrNotFound;
	}

/**
 * Finds the specified class in the list of USB classes.
 *
 * @return KErrNotFound if the class isn't in the array
 */
EXPORT_C TInt CUsbClassControllerIterator::Seek(
	CUsbClassControllerBase* aClassController)
	{
	TInt ret = iClassControllerArray.Find(aClassController);

	if (ret != KErrNotFound)
		iClassControllerIndex = ret;
	else
		return KErrNotFound;

	return KErrNone;
	}

/**
 * Gets the current class the iterator's pointing to.
 *
 * @return The current class the iterator refers to
 */
EXPORT_C CUsbClassControllerBase* CUsbClassControllerIterator::Current()
	{
#ifdef _DEBUG
    if(!((iClassControllerIndex >= 0) && (iClassControllerIndex < iClassControllerArray.Count())))
        {
        OstTrace1( TRACE_FATAL, CUSBCLASSCONTROLLERITERATOR_CURRENT, "CUsbClassControllerIterator::Current;panic reason=%d", EIndexOutOfRange );
        User::Panic(KUsbCcIteratorPanicCategory, EIndexOutOfRange );
        }
#endif
	return iClassControllerArray[iClassControllerIndex];
	}

