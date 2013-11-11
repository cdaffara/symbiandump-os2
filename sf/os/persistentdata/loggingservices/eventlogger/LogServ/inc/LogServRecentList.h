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

#ifndef __LOGSERVRECENTLIST_H__
#define __LOGSERVRECENTLIST_H__

#include <logwrap.h>

// Classes referenced
class TResourceReader;
class CLogEvent;
class CLogFilter;
class CLogServRecentCondition;

/**
Holds an array of CLogServRecentCondition elements.

@see CLogServRecentCondition
@see CLogServRecentListManager
@internalComponent
*/
class CLogServRecentList : public CBase
	{
public:
	static CLogServRecentList* NewL(TResourceReader& aReader);
	static CLogServRecentList* TestNewL();
	~CLogServRecentList();
	
	TBool IsRecent(const CLogEvent& aEvent) const;
	void GetFilter(const CLogEvent& aEvent, CLogFilter& aFilter) const;
	inline TLogRecentList Id() const;

private:
    void ConstructL(TResourceReader& aReader);
    void TestConstructL();
	TBool IsFilterEmpty(const CLogFilter& aFilter) const;

private:
	TLogRecentList iId;
	TUint16 iDuplicates;//It must be 16-bit. The iDuplicates value is read from a resource file. 
						//See ConstructL() implementation. The Duplicates type width then restricts
						//the max number of field macro values - 
						//ELogContactField...ELogSimIdField. 
    typedef RPointerArray<CLogServRecentCondition> RLogRecentConditionArray;
	RLogRecentConditionArray iConditions;
	};

inline TLogRecentList CLogServRecentList::Id() const
	{
	return iId;
	}



#endif
