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
//

/**
 
 @param aTableName UTF16 encoded table name.
 
 @return Non-zero if aTableName is one of the system tables (the table name prefix is KSystemPrefix16).
 
 @internalComponent
*/
inline TBool IsSystemTableName(const TDesC16& aTableName)
	{
	return ::CompareNoCase16(aTableName.Left(KSystemPrefix16().Length()), KSystemPrefix16) == 0;
   	}

/**
@param aTableName UTF8 encoded table name.

@return Non-zero if aTableName is one of the system tables (the table name prefix is KSystemPrefix8).

@internalComponent
*/
inline TBool IsSystemTableName(const TDesC8& aTableName)
	{
	return ::CompareNoCase8(aTableName.Left(KSystemPrefix8().Length()), KSystemPrefix8) == 0;
	}

/**
@param aTableName UTF8 encoded table name.

@return Non-zero if aTableName name prefix is is KSqlitePrefix8.

@internalComponent
*/
inline TBool IsSqliteTableName(const TDesC8& aTableName)
	{
	return ::CompareNoCase8(aTableName.Left(KSqlitePrefix8().Length()), KSqlitePrefix8) == 0;
	}

/**
Returns non-zero if a file with "aFileName" name exists.

@internalComponent
*/
inline TBool FileExists(RFs& aFs, const TDesC& aFileName)
	{
	TEntry entry;
	return aFs.Entry(aFileName, entry) == KErrNone;
	}
