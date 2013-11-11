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

#ifndef __LOGSERVDATABASECHANGEDEFINITION_H__
#define __LOGSERVDATABASECHANGEDEFINITION_H__


/////////////////////////////////////////////////////////////////////////////////////////
// -----> TLogServDatabaseChangeDefinition (header)
/////////////////////////////////////////////////////////////////////////////////////////
class TLogServDatabaseChangeDefinition
/**
@internalComponent
*/
	{
public:
	inline TLogServDatabaseChangeDefinition(TUid aChangeType, TInt aChangeParam1 = 0, TInt aChangeParam2 = 0, TInt aChangeParam3 = 0) : iChangeType(aChangeType), iChangeParam1(aChangeParam1), iChangeParam2(aChangeParam2), iChangeParam3(aChangeParam3) { }

public:
	TUid iChangeType;
	TInt iChangeParam1;
	TInt iChangeParam2;
	TInt iChangeParam3;
	};


#endif
