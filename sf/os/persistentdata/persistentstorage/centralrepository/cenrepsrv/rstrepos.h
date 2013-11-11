// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RSTREPOS_H
#define RSTREPOS_H

#include <e32base.h>

/**
@internalComponent
A class to represent a repository whose key(s) have been restored.
It stores the uid of the repository and the list of keys which are
changed during a restoration. 
*/
class CRestoredRepository : public CBase
	{
public:
	CRestoredRepository(TUid aUid);
	~CRestoredRepository();		

	TUid Uid() const;	
	
	const RArray<TUint32>& ChangedKeys() const;  
	
	void AddKeyL(TUint32 aKey);  
	
	static TInt CompareUids(const CRestoredRepository& aRstRepos1,const CRestoredRepository& aRstRepos2);

private:
	TUid iReposUid;
	RArray<TUint32> iChangedKeys;
	};
	
#endif // RSTREPOS_H
