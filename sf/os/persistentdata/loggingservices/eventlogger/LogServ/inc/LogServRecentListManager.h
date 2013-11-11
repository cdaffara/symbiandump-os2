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

#ifndef __LOGSERVRECENTLISTMANAGER_H__
#define __LOGSERVRECENTLISTMANAGER_H__

#include <e32base.h>

// Classes referenced
class CLogEvent;
class TResourceReader;
class CLogServRecentList;
class CLogServResourceInterpreter;

/**
Holds an array of CLogServRecentList elements. 

@see CLogServRecentCondition
@see CLogServRecentList
@internalComponent
*/
class CLogServRecentListManager : public CBase
	{
public:
	static CLogServRecentListManager* NewL(CLogServResourceInterpreter& aResourceInterpreter, TInt aResourceId);
	~CLogServRecentListManager();

private:
	void ConstructL(CLogServResourceInterpreter& aResourceInterpreter, TInt aResourceId);

public:
	const CLogServRecentList* GetRecentList(const CLogEvent& aEvent) const;
	inline const CLogServRecentList& List(TInt aIndex) const;
	inline TInt Count() const;

private:
    typedef RPointerArray<CLogServRecentList> RLogServRecentListArray;
    RLogServRecentListArray iLists;
	};

inline TInt CLogServRecentListManager::Count() const
	{
	return iLists.Count();
	}

inline const CLogServRecentList& CLogServRecentListManager::List(TInt aIndex) const
	{
	return *iLists[aIndex];
	}

#endif
