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

#ifndef __USBUTILS_H__
#define __USBUTILS_H__

#include <e32std.h>
#include <ecom/ecom.h>

//Used to push a pointer array on to the cleanup stack
//ensuring that it will be reset and destroyed when popped
void CleanupResetAndDestroyPushL(RImplInfoPtrArray& aArray);
 	
#endif //__USBUTILS_H__
