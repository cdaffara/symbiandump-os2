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
// SQL DDL statements
// 
//

#include "UQ_STD.H"

// Class CSqlCreateTableStatement

CDbIncremental* CSqlCreateTableStatement::ExecuteL(CDbDatabase& aDatabase,TDbTextComparison,TInt& aStep)
	{
	aDatabase.CreateTableL(iName,iColumns,0);
	aStep=0;
	return 0;
	}

// Class CSqlDropTableStatement

CDbIncremental* CSqlDropTableStatement::ExecuteL(CDbDatabase& aDatabase,TDbTextComparison,TInt& aStep)
	{
	return aDatabase.DropTableL(iName,aStep);
	}

// Class CSqlAlterTableStatement

CSqlAlterTableStatement::~CSqlAlterTableStatement()
	{
	iDrop.Close();
	}

CDbIncremental* CSqlAlterTableStatement::ExecuteL(CDbDatabase& aDatabase,TDbTextComparison,TInt& aStep)
	{
	CDbColSet* set=CDbColSet::NewLC();
	aDatabase.ColumnsL(*set,iName);
	TInt todrop=iDrop.Count();
	for (TDbColSetIter iter(*set);iter;++iter)
		{
		TInt ii=-1;
		for (;;)
			{
			if (++ii==todrop)
				{
				iAdd.AddL(*iter);
				break;
				}
			if (iter->iName.CompareF(iDrop[ii])==0)
				{
				iDrop.Remove(ii);
				--todrop;
				break;
				}
			}
		}
	if (todrop!=0)
		__LEAVE(KErrNotFound);
	CleanupStack::PopAndDestroy();	// set
	return aDatabase.AlterTableL(iName,iAdd,aStep);
	}

// Class CSqlCreateIndexStatement

CDbIncremental* CSqlCreateIndexStatement::ExecuteL(CDbDatabase& aDatabase,TDbTextComparison aComparison,TInt& aStep)
	{
	iKey.SetComparison(aComparison);
	return aDatabase.CreateIndexL(iName,iTable,iKey,aStep);
	}

// Class CSqlDropIndexStatement

CDbIncremental* CSqlDropIndexStatement::ExecuteL(CDbDatabase& aDatabase,TDbTextComparison,TInt& aStep)
	{
	return aDatabase.DropIndexL(iName,iTable,aStep);
	}
