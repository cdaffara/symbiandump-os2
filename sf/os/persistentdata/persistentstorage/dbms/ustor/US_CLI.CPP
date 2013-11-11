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
//

#include "US_STD.H"

/**
Creates a new non-secure, non-shareable database, which will be located in a stream store.

Note: RDbNamedDatabase should always be the preferred choice, except the following two cases:
@code
 - your intention is to create, fill and then compress a database, which will be used only in a read-only mode;
 - your intention is to create a database in a stream store, which stream store will be used for storing
   some additional, non-database data;
@endcode

@param aStore The stream store object, where the database will be created.

@leave System-wide or database specific error codes.

@return The id of the root stream of the created database.

@see RDbNamedDatabase

@publishedAll
@released
*/
EXPORT_C TStreamId RDbStoreDatabase::CreateL(CStreamStore* aStore)
	{
	TStreamId id;
	iDatabase=CDbStoreDatabase::CreateL(aStore,id);
	return id;
	}

/**
Opens an existing non-secure, non-shareable database, which is located in a stream store.

Note: RDbNamedDatabase should always be the preferred choice, except the following two cases:
@code
 - your intention is to create, fill and then compress a database, which will be used only in a read-only mode;
 - your intention is to create a database in a stream store, which stream store will be used for storing
   some additional, non-database data;
@endcode

@param aStore The stream store object, where the database is located.
@param aId The id of the root stream of the database.

@leave System-wide or database specific error codes.

@see RDbStoreDatabase::CreateL(CStreamStore* aStore)
@see RDbNamedDatabase

@publishedAll
@released
*/
EXPORT_C void RDbStoreDatabase::OpenL(CStreamStore* aStore,TStreamId aId)
	{
	CDbSource* source=CDbStoreDatabase::OpenL(aStore,aId);
	CleanupStack::PushL(source);
	iDatabase=source->OpenL();
	CleanupStack::PopAndDestroy(source);
	}
