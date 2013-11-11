// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// User defined collations
// 
//

#ifndef __SQLSRVCOLLATION_H__
#define __SQLSRVCOLLATION_H__

#include <e32std.h>

//Forward declarations
struct sqlite3;

/**
This class manages the installation of the user-defined collations.
Also it gives information about the user-defined collations count and their names.
5 different collations are supported at the moment: 
"CompareC0", "CompareC1", "CompareC2", "CompareC3" respectively for collation comparison levels 0, 1, 2 and 3
and "CompareF" for folded string comparison.

@internalComponent
*/
class TSqlCollationUtil
	{
public:
	TSqlCollationUtil(sqlite3* aDbHandle);
	void  InstallCollationsL();
	TInt  CollationCount() const;
	TPtrC CollationName(TInt aIndex) const;

private:
	sqlite3*	iDbHandle;
			
	};

#endif//__SQLSRVCOLLATION_H__
