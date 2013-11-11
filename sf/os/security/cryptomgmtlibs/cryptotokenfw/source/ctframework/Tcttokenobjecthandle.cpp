/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "ct/tcttokenobjecthandle.h"

EXPORT_C TCTTokenObjectHandle::TCTTokenObjectHandle()
	: iTokenHandle(), iObjectId(0)
	{
	}

EXPORT_C TCTTokenObjectHandle::TCTTokenObjectHandle(TCTTokenHandle aTokenHandle,
													TInt aObjectId)
	: iTokenHandle(aTokenHandle), iObjectId(aObjectId)
	{
	}

EXPORT_C TCTTokenObjectHandle::TCTTokenObjectHandle(const TCTTokenObjectHandle& aHandle)
	: iTokenHandle(aHandle.iTokenHandle), iObjectId(aHandle.iObjectId)
	{
	}

EXPORT_C TBool TCTTokenObjectHandle::operator ==(const TCTTokenObjectHandle& aHandle) const
	{
	return ((iTokenHandle == aHandle.iTokenHandle) &&
			(iObjectId == aHandle.iObjectId));
	}
