// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BAFINDF.H"

//
// class CFindFileByType
//

EXPORT_C CFindFileByType::CFindFileByType(RFs& aFs)
//
// Constructor
//
	: iFind(aFs)
	{
	__DECLARE_NAME(_S("CFindFileByType"));
	}

EXPORT_C CFindFileByType::~CFindFileByType()
//
// Destructor
//
	{
	CloseDir();
	}

EXPORT_C TInt CFindFileByType::FindFirst(const TDesC& aName,const TDesC& aDir,const TUidType& aType)
//
// Look for first file
//
	{
	iType=aType;
	CloseDir();
	TInt r=iFind.FindWildByDir(aName,aDir,iDir);
	if (r!=KErrNone)
		return r;
	iCurrentFile=-1;
	return FindNext();
	}

EXPORT_C TInt CFindFileByType::FindNext()
//
// Look for next file
//
	{
	TInt ii=iCurrentFile;
	FOREVER
		{
		__ASSERT_DEBUG(iDir!=NULL,User::Invariant());
//
		TInt count=iDir->Count();
		while (++ii<count)
			{
			const TEntry& entry=(*iDir)[ii];
			TInt id=0;
			FOREVER
				{
				TUid uid=iType[id];
				if (uid!=KNullUid && uid!=entry[id])
					break;					// wrong type
				if (++id==KMaxCheckedUid)
					{						// we have a matching file
					iCurrentFile=ii;
					iFile.Set(entry.iName,&iFind.File(),NULL);
					return KErrNone;
					}
				}
			}
		CloseDir();
		TInt r=iFind.FindWild(iDir);
		if (r!=KErrNone)
			return r;
		ii=-1;
		}
	}

EXPORT_C const TEntry& CFindFileByType::Entry() const
	{
	return (*iDir)[iCurrentFile];
	}

void CFindFileByType::CloseDir()
//
// Close iDir 
//
	{
	delete(iDir);
	iDir=NULL;
	}
