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
* Name        : ewsd.h
* Part of     : ewsd library
* Contains the declarations of the APIs of the emulator WSD library 
*
*/


 
#ifndef __EWSD_H__
#define __EWSD_H__

#ifdef __WINSCW__

#include <e32std.h>

/** 
* The following functions should not be used directly.
* Libraries that require to use the emulator WSD library
* should use the Pls() API defined in pls.h
*/

/**  
Finds and returns the PLS (Process Local Storage) object of the specified 
library for the current process, if the object exists.
Takes as a parameter the TUid of the library DLL whose PLS object is to 
be returned for the current process.
Returns a pointer to the PLS object if it exists, otherwise NULL					
*/
IMPORT_C TAny* CheckPls(const TUid& aLibraryUid);

/**  
Sets the PLS object of the specified library, for the current process.
Takes as parameters the PLS object to be set, and the TUid of the library 
DLL whose PLS object it is. 
Returns KErrNone on success, and KErrNoMemory if the PLS object could not
be stored because a size limit of the WSD array has been reached
*/
IMPORT_C TInt SetPls(TAny* aPls, const TUid& aLibraryUid);

/**  
Allocates the specified number of bytes of memory from the emulator's
Virtual Address Space.
Takes as a parameter the number of bytes of memory to allocate.
Returns	a pointer to the allocated memory on success, otherwise NULL
*/
IMPORT_C TAny* AllocatePls(const TInt& aSize);

/**  
Frees the specified memory in the emulator's Virtual Address Space.
Takes as a parameter the PLS object location to be freed.
Returns KErrNone on success, otherwise KErrAccessDenied
*/
IMPORT_C TInt FreePls(TAny* aPls); 

/**  
Gets a handle to the mutex, waits until it obtains ownership of it
and then returns the handle.
Returns a handle to the mutex, of which the caller now has ownership.
Note that the handle is returned as a TAny* (the Windows HANDLE type 
is typedefed to void*)
*/
IMPORT_C TAny* ObtainPlsMutex();

/**  
Releases ownership of the mutex, which allows another thread 
to acquire ownership of it.
Takes as a parameter a handle to the mutex										
*/
IMPORT_C void ReleasePlsMutex(TAny* aMutexHandle);

#endif // __WINSCW__

#endif // __EWSD_H__

