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
* Defines a helper class used to iterate over a set of USB classes.
*
*/

/**
 @file
*/

#ifndef __CUSBCLASSCONTROLLERITERATOR_H__
#define __CUSBCLASSCONTROLLERITERATOR_H__

#include <e32base.h>

class CUsbClassControllerBase;

/** This class is used by CUsbDevice and potentially 
 * CUsbClassControllerBase-derived classes to iterate over a collection of 
 * CUsbClassControllerBase objects.

  @publishedPartner
  @released
 */
NONSHARABLE_CLASS(CUsbClassControllerIterator) : public CBase
	{
public:
	/** Constructor
		@param aClassControllerArray Array of class controllers.
		*/
	IMPORT_C CUsbClassControllerIterator(RPointerArray<CUsbClassControllerBase>& aClassControllerArray);

	/** Destructor.
	*/
	IMPORT_C ~CUsbClassControllerIterator();

	/** First
		Sets the iterator to the first class controller.
		@return Error.
		*/
	IMPORT_C TInt First();

	/** Next
		Sets the iterator to the next class controller.
		@return Error.
		*/
	IMPORT_C TInt Next();

	/** Previous
		Sets the iterator to the previous class controller.
		@return Error.
		*/
	IMPORT_C TInt Previous();

	/** Seek
		Sets the current class controller to that given, if it's in the array.
		@param aClassController The class controller we want to find.
		@return Error.
		*/
	IMPORT_C TInt Seek(CUsbClassControllerBase* aClassController);

	/** Current
		@return The current class controller. Does not return ownership.
		*/
	IMPORT_C CUsbClassControllerBase* Current();

private:
	/**
		The array of class controllers.
		*/
	RPointerArray<CUsbClassControllerBase>& iClassControllerArray;

	/**
		Index of current class controller.
		*/
	TInt iClassControllerIndex;
	};


#endif //__CUSBCLASSCONTROLLERITERATOR_H__
