/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
@publishedAll
@released
*/
#ifndef XMLENGDOMIMPLEMENTATION_H
#define XMLENGDOMIMPLEMENTATION_H

#include <e32base.h>

/**
The RXmlEngDOMImplementation interface provides a number of methods for performing 
operations that are independent of any particular instance of the document object model. 
   
http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-102161490
*/
class RXmlEngDOMImplementation
{
  public:
    
    
	/**
	Open method needed to initialize the DOM library and should be called
	before using any class/function from this library.  The initialization is
	required in every client thread in which it used. Calling this method
	multiple times in a thread is allowed. There should be a call to Close()
	for every call to Open, per thread.

	@param aHeap This argument not used at present and is ignored in the
	implementation.
	@leave - One of the system-wide error codes
    */
    IMPORT_C void OpenL( RHeap* aHeap = NULL );

	/**
	Close method. It cleans up resources used by the DOM library. Clients
	should call this method at the end of library usage. The library should not
	be used after this call. If needed, clients can initialize the library
	again by calling the Open() method. 
	
    @param aReserved This parameter is not used
    */
    IMPORT_C void Close(TBool aReserved);

	/**
	Close method. It cleans up resources used by the DOM library. Clients
	should call this method at the end of library usage. The library should not
	be used after this call. If needed, clients can initialize the library
	again by calling the Open() method. 
	*/	
    IMPORT_C void Close();
};

#endif /* XMLENGDOMIMPLEMENTATION_H */

