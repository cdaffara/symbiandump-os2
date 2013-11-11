// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVBURINTERFACE_H__
#define __SQLSRVBURINTERFACE_H__

#include <f32file.h>

/**
MSqlSrvBurInterface is needed for performing "Backup&Restore" actions on secure SQL databases.
It is used for retrieving from the SQL server a list with database names, located on a specific drive and
owned by a client with specific TSecureId id.
The returned list contains the full database names, including the path. 

@see CSqlServer::GetBackUpListL()

@internalComponent
*/
class MSqlSrvBurInterface
	{
public:		
	virtual RFs& Fs() = 0;
	virtual void GetBackUpListL(TSecureId aUid, TDriveNumber aDrive, RArray<HBufC*>& aFileNameList) = 0;
	};

#endif//__SQLSRVBURINTERFACE_H__
