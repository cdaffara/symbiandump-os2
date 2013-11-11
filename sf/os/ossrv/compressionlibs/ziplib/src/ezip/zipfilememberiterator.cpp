// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <zipfile.h>
#include <zipfilememberiterator.h>

CZipFileMemberIterator::CZipFileMemberIterator(CZipFile* aZipFile):
	iZipFile(aZipFile),
	iIndex(0)
	{
	}

/**
Moves the entry iterator onto the next zip file member contained in the 
ZIP file. It returns a newly allocated CZipFileMember object that the
caller is responsibile for de-allocating.

@return new CZipFileMember* if next file member exist NULL when no more members exist
@leave ... Any one of the system-wide error codes for other errors.
*/
EXPORT_C CZipFileMember* CZipFileMemberIterator::NextL(void)
	{	
	return iZipFile->MakeMemberL(iIndex++);
	}
