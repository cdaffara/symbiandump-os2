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

#include <e32std.h>
#include <e32base.h>
#include "UsbUtils.h"

void ResetAndDestroy(TAny* aPtr)
/**
 *Function passed in as a parameter to TCleanupItem constructor to 
 *reset and destroy the received aPtr.
 *
 *aPtr object
 *
 *@param aPtr a pointer to the object that is to be cleaned up
 */
	{
	reinterpret_cast<RImplInfoPtrArray*>(aPtr)->ResetAndDestroy();
	}

void CleanupResetAndDestroyPushL(RImplInfoPtrArray& aArray)
/**
 *Function that creates a clean up item for the received, aArray and
 *specifies the cleanup function, ResetAndDestroy. When the item is 
 *removed from the cleanup stack the function, ResetAndDestroy will be
 *called on aArray
 *
 *@param aArray the array to be pushed onto the cleanup stack
 */
   	{  	    
	TCleanupItem item(ResetAndDestroy, &aArray);
   	CleanupStack::PushL(item);
   	}
//
// End of file
