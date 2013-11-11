// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SQLDB internal API header
// 
//

#ifndef SQLRESOURCETESTER_H
#define SQLRESOURCETESTER_H

/**
TSqlResourceTester class is used internally by the SQL server out of memory and resource leaking
tests. 
It provides methods for heap allocation failure simulation and resource checking and counting.

@internalAll
@released
*/
class TSqlResourceTester
	{
public:	
	IMPORT_C static void Mark();
	IMPORT_C static void Check();
	IMPORT_C static TInt Count();
	IMPORT_C static void SetDbHeapFailure(TInt aAllocFailType,TInt aRate);
	IMPORT_C static void SetHeapFailure(TInt aAllocFailType,TInt aRate);

	};

#endif //__SQLDBINTERNAL_H__
