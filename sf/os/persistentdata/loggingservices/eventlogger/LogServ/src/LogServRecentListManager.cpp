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

#include "LogServRecentListManager.h"
#include "LogServResourceInterpreter.h"
#include "LogServRecentList.h"
#include "logservpanic.h"

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServRecentListManager (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServRecentListManager::~CLogServRecentListManager()
	{
	iLists.ResetAndDestroy();
	iLists.Close();
	}

void CLogServRecentListManager::ConstructL(CLogServResourceInterpreter& aResourceInterpreter, TInt aResourceId)
	{
	TResourceReader reader;
	aResourceInterpreter.CreateResourceReaderLC(reader, aResourceId);
	const TInt count = reader.ReadInt16();
	iLists.ReserveL(count);
	for(TInt i=0; i<count; i++)
		{
		CLogServRecentList* list = CLogServRecentList::NewL(reader);
		TInt err = iLists.Append(list);
        __ASSERT_ALWAYS(err == KErrNone, Panic(ELogArrayReserved));
		}
	CleanupStack::PopAndDestroy(); // reader
#ifdef SYSLIBS_TEST
	//Creates a test recent list used by the unit tests. Helps covering most of the production code branches,
	//related to the recent lists functionality.
	CLogServRecentList* list = CLogServRecentList::TestNewL();
	iLists.AppendL(list);
#endif
	}

CLogServRecentListManager* CLogServRecentListManager::NewL(CLogServResourceInterpreter& aResourceInterpreter, TInt aResourceId)
	{
	CLogServRecentListManager* self = new(ELeave) CLogServRecentListManager;
	CleanupStack::PushL(self);
	self->ConstructL(aResourceInterpreter, aResourceId);
	CleanupStack::Pop();
	return self;
	}

const CLogServRecentList* CLogServRecentListManager::GetRecentList(const CLogEvent& aEvent) const
	{
	TInt count = iLists.Count();
	while(count--)
		{
		const CLogServRecentList* list = iLists[count];
		if (list->IsRecent(aEvent))
			return list;
		}
	return NULL;
	}

