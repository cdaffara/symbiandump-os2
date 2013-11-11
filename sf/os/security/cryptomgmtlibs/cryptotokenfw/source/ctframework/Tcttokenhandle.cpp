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


#include "ct/tcttokenhandle.h"

EXPORT_C TCTTokenHandle::TCTTokenHandle()
	: iTokenTypeUid(), iTokenId(0)
	{
	}

EXPORT_C TCTTokenHandle::TCTTokenHandle(TUid aTokenTypeUid,
										TInt aTokenId)
	: iTokenTypeUid(aTokenTypeUid), iTokenId(aTokenId)
	{
	}

EXPORT_C TCTTokenHandle::TCTTokenHandle(const TCTTokenHandle& aTokenHandle)
	: iTokenTypeUid(aTokenHandle.iTokenTypeUid), iTokenId(aTokenHandle.iTokenId)
	{
	}

EXPORT_C TBool TCTTokenHandle::operator ==(const TCTTokenHandle& aTokenHandle) const
	{
	return ((iTokenTypeUid == aTokenHandle.iTokenTypeUid) && 
		(iTokenId == aTokenHandle.iTokenId));
	}
