// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include <pcstore/uid.h>
#include "pcstoreconst.h"

namespace PCStore
{
/**
Compares two UIDs for equality.

@param aUid The UID to be compared with this UID.
@return TRUE, if the two UIDs are equal; FALSE otherwise. 
*/
TBool TUid::operator==(const TUid& aUid) const
	{
	return(iUid==aUid.iUid);
	}

/**
Compares two UIDs for inequality.

@param aUid The UID to be compared with this UID.
@return TRUE, if the two UIDs are unequal; FALSE otherwise. 
*/
TBool TUid::operator!=(const TUid& aUid) const
	{
	return(iUid!=aUid.iUid);
	}


/**
Constructs a Null-valued TUid object.

@return The constructed Null-valued TUid object.
*/
TUid TUid::Null()
	{
	TUid uid={KNullUidValue};
	return uid;
	}
}
