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
//

#if defined(__PAGE_CACHE_STATS)
inline void CPageCache::TStats::Hit()
	{++iRequests;}
inline void CPageCache::TStats::Miss()
	{++iRequests;++iMisses;}
#endif

inline TDblQueLink& TCachePage::Link()
	{return this[-1].iLink;}
inline const TDblQueLinkBase& TCachePage::Link() const
	{return this[-1].iLink;}
inline void TCachePage::Deque()
	{Link().Deque();Link().iNext=Link().iPrev=&Link();}
inline TBool TCachePage::IsQued() const
	{return Link().iNext!=&Link();}
inline void TCachePage::Reque(TDblQue<TCachePage>& aQue)
	{Link().Deque();aQue.AddLast(*this);}

inline TStorePagePoolToken::TStorePagePoolToken(TStreamId aHead,TPageRef anAvail)
	: iHead(aHead),iAvail(anAvail)
	{}

inline void RStorePagePool::CacheDeleteL(TPageRef aRef)
	{TCachePagePool::DoDeleteL(aRef);}
