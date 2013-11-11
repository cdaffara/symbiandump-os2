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

#if !defined(__UID_H__)
#define __UID_H__

#include <pcstore/pcstoredef.h>

namespace PCStore
{
#define KNullUid TUid::Null()

/**
@internalAll

A globally unique 32-bit number.
*/
class TUid
	{
public:
	TBool operator==(const TUid& aUid) const;
	TBool operator!=(const TUid& aUid) const;
	static TUid Null();

public:
	/**
	The 32-bit integer UID value.
	*/
	TInt32 iUid;
	};
}
#endif // !defined(__UID_H__)
