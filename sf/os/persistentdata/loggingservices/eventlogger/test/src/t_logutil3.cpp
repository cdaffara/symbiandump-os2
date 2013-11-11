// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_logutil3.h"
#include <centralrepository.h>
#include "LOGREPDEFS.H"

//This function reads logeng repository file and returns the integer value of the given key. 
static TInt LogGetRepositoryValueL(CRepository& aRepository, TInt aKey)
	{		
	TInt val = -1;		
	User::LeaveIfError(aRepository.Get(aKey, val));
	return val;
	}

void LogGetContactmatchCountAndNameFormatL(TInt& aContactMatchCount, TLogContactNameFormat& aContactNameFormat)
	{
	CRepository* repository = NULL;
	TRAPD(err, repository = CRepository::NewL(KUidLogengRepository));		
	if(err == KErrNone)
		{
		CleanupStack::PushL(repository);
		aContactMatchCount = LogGetRepositoryValueL(*repository, KContactMatchCountRepKey);
		aContactNameFormat = static_cast <TLogContactNameFormat> (LogGetRepositoryValueL(*repository, KContactNameFormatRepKey));
		CleanupStack::PopAndDestroy(repository);
		}
	else if(err == KErrCorrupt)
		{
		__ASSERT_DEBUG(!repository, User::Invariant());
		User::Leave(err);
		}
	else
		{
		__ASSERT_DEBUG(!repository, User::Invariant());
		aContactMatchCount = KLogContactMatchCount;
		aContactNameFormat = KLogContactNameFormat;
		}
	}

