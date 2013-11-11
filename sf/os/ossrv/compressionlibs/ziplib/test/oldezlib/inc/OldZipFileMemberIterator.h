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
// ZipFileMemberIterator.h
// $Revision$
// 
//

#ifndef _EZIP_FILE_MEMBER_ITERATOR_H_
#define _EZIP_FILE_MEMBER_ITERATOR_H_

#include <e32std.h>
#include "oldzipfile.h"
#include "OldZipFileMember.h"
// Forward Class Declaration(s)

class TOLDEZIP::CZipFileMember;

namespace TOLDEZIP
	{
	class CZipFile;
	}

// End of  Forward Class Declaration(s)

/**
An iterator class for iterating though all the entries in the archive.

@publishedAll
@released
*/
namespace TOLDEZIP
{
class CZipFileMemberIterator
	{
friend class CZipFile;

public:
	IMPORT_C CZipFileMember* NextL(void);

private:
	CZipFileMemberIterator(CZipFile*);
	
private:
	CZipFile* iZipFile;
	TInt      iIndex;
	};
}//TOLDEZIP
#endif /* !_ZIP_FILE_MEMBER_ITERATOR_H_ */
