// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Closes the database handle.
// 
//

/**
 @internalComponent
*/
inline void CloseDbHandle(sqlite3* aDbHandle)
	{
	if(aDbHandle)
		{
		(void)sqlite3_close(aDbHandle);
		}
	}

/**
CloseDbCleanup() function is used to close the sqlite3 handle (aDbHandle parameter)
and may be called during cleanup stack unwinding process.

@internalComponent
*/
inline void CloseDbCleanup(void* aDbHandle)
	{
	CloseDbHandle(reinterpret_cast <sqlite3*> (aDbHandle));
	}
