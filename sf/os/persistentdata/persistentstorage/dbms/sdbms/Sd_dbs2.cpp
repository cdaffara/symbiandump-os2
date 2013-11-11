// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS server proxy database - CDbsSecureDatabase class - "DBMS security" related
// 
//

#include "SD_STD.H"

/**
Standard phase-one factory method for CDbsSecureDatabase objects.
Creates new secure shared database.
@param aDbs DBMS server session object.
@param aDatabase Database name.
@param aFormat Database format string. The string format is: "SECURE[<UID>]",
       where UID is your database security policy uid.
@return A pointer to just created CDbsSecureDatabase instance.
@leave System-wide error codes, including KErrNoMemory.
*/
CDbsSecureDatabase* CDbsSecureDatabase::NewL(const RDbs& aDbs,const TDesC& aDatabase,
											 const TDesC& aFormat)
	{
	CDbsSecureDatabase* self = new (ELeave) CDbsSecureDatabase;
	CleanupStack::PushL(self);
	TIpcArgs args(&aDatabase, &aFormat, TIpcArgs::ENothing, NULL);
	self->ConstructL(aDbs, args);
	CleanupStack::Pop();
	return self;
	}

/**
Standard phase-two construction method for CDbsSecureDatabase objects.
Constructs new secure shared database
@leave System-wide error codes, including KErrNoMemory.
*/
void CDbsSecureDatabase::ConstructL(const RDbs& aDbs,TIpcArgs& aArgs)
	{
	iObject.OpenL(aDbs, EDbsCreateDatabase, aArgs);
	}

/**
*/
CDbsSecureDatabase::~CDbsSecureDatabase()
	{
	}
