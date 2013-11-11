// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _DOWNGRADEPATH_H_
#define _DOWNGRADEPATH_H_

#include <e32def.h>
#include <e32std.h>
#include <f32file.h>

typedef RArray<TLanguage> Languages;

/**
This class is used to cache previously stored language settings.
This is required for any subsequent call to RDowngradePath::HasChangedL
as changes in language downgrade path means ECOM needs to do a rediscover
of all the plugins to pick up the right language variant.
* @internalComponent
*/
class RDowngradePath
	{
private:
	static Languages iCurrentLanguage;

public:
	static TBool HasChangedL(RFs& aFs);
	static void Reset();
	};

#endif	// _DOWNGRADEPATH_H_
