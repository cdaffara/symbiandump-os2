// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Caching interface for server-side implementations
// 
//

#ifndef __D32CACHE_H__
#define __D32CACHE_H__

#ifndef __E32STD_H__
#include <e32std.h>
#endif

// Classes referenced
class CBase;

class RDbCache
/**
@internalComponent
*/
	{
private:
	enum { EDefaultSlots = 2 };
	class CCache;
public:
	inline RDbCache();
	TInt Open(TInt aSize = EDefaultSlots, TBool aUseTimer = EFalse );
	void Close();
//
	void Hold( CBase* aObject, TUint aMicroSeconds );
	void Release( const CBase& aObject ) const;
	void Flush();
private:
	CCache* iCache;
	};

inline RDbCache::RDbCache()
	:iCache( 0 )
	{}

#endif
