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
// Sd_DbList.cpp
// CDbNamesFactory class
// "DBMS security" related - full DBMS security support
// 
//

#include "SD_STD.H"
#include "Sd_DbList.h"

using namespace DBSC;

extern TBool CheckDrivesL(const TDesC& aPath1, const TDesC& aPath2);

/**
A global method, which can be used to check presence of the drive, supplied as
a parameter.
@param aFs File session instance.
@param aDriveNumber Number of the drive, which will be checked. If there is no such drive
                    in the system, the function leaves with KErrNotReady error code.
@internalComponent.
*/
void CheckDriveL(RFs& aFs, TDriveNumber aDriveNumber)
	{
	TDriveInfo driveInfo;
	__LEAVE_IF_ERROR(aFs.Drive(driveInfo, aDriveNumber));
	if(driveInfo.iType == EMediaNotPresent)
		{
		__LEAVE(KErrNotReady);
		}
	}

/**
Standard phase-one factory method for CDbNamesFactory instances.
@return A pointer to created CDbNamesFactory instance.
*/
CDbNamesFactory* CDbNamesFactory::NewLC()
	{
	CDbNamesFactory* self = new (ELeave) CDbNamesFactory;
	CleanupStack::PushL(self);
	return self;
	}

/**
*/
CDbNamesFactory::~CDbNamesFactory()
	{
	}

/**
Extracts the parameters from an EDbsDatabaseList aMessage.
@param aMessage EDbsDatabaseList message
@param aDriveNumber An output parameter, referencing the location, where the drive number will be stored.
@param aDbPolicyRequest An output parameter, referencing the location, where the constructed 
                        TDbPolicyRequest will be stored.
*/
void CDbNamesFactory::ExtractArgs(const RMessage2& aMessage, 
								  TDriveNumber& aDriveNumber, 
								  TDbPolicyRequest& aDbPolicyRequest)
	{
	aDriveNumber = static_cast <TDriveNumber> (aMessage.Int0());
	aDbPolicyRequest.iUid = TUid::Uid(aMessage.Int1());
	aDbPolicyRequest.iAccessType = EATSecure;
	}

/**
Creates CDbDatabaseNames instance and return it.
@param aDriveNumber The drive number to be searched.
@param aDbPolicyRequest A const reference to an object packing the security policy UID and the 
                  request type - secure/nonsecure.
@param aDbPropsFactory A const reference to RDbPropsFactory instance, which methods will be used
                  in the process of constructing the list of CDbDatabaseNames names.
@param aFs A file session instance.
@return A pointer to the created CDbDatabaseNames instance.
*/
CDbDatabaseNames* CDbNamesFactory::DbNamesLC(TDriveNumber& aDriveNumber, 
										     const TDbPolicyRequest& aDbPolicyRequest, 
											 const RDbPropsFactory& aDbPropsFactory, 
											 RFs& aFs)
	{
	::CheckDriveL(aFs, aDriveNumber);
	aDbPropsFactory.GetPrivatePathL(aDriveNumber, iDbPath);
	ConstructSearchPattern(aDbPolicyRequest);
	CDbDatabaseNames* dbNames = CDbDatabaseNames::NewLC();
	SearchL(aFs, dbNames);
	return dbNames;
	}

/**
*/
CDbNamesFactory::CDbNamesFactory()
	{
	}

/**
Constructs search pattern for the database file names.
iDbPath & iDbNameCmnPart data members get initialized after the call.
@param aDbPolicyRequest A const reference to an object packing the security policy UID and the 
                  request type - secure/nonsecure.
*/
void CDbNamesFactory::ConstructSearchPattern(const TDbPolicyRequest& aDbPolicyRequest)
	{
	__ASSERT(iDbPath.Length() > 0);
	RDbPropsFactory::ConstructCommonPart(aDbPolicyRequest.iUid, iDbNameCmnPart);
	iDbNameCmnPart.Append('*');
	__ASSERT(iDbNameCmnPart.Length() > 0);
	}

/**
Searches for databases in DBMS private directory and fill aDbNames container.
@param aFs A file session instance.
@param aDbNames An output parameter, where the database names will be stored.
@leave One of the system-wide error codes, excluding KErrNotFound.
*/
void CDbNamesFactory::SearchL(RFs& aFs, CDbDatabaseNames* aDbNames)
	{
	__ASSERT(aDbNames);
	TFindFile findFile(aFs);
	CDir* paths = NULL;
	TInt err = findFile.FindWildByDir(iDbNameCmnPart, iDbPath, paths);
	if(err == KErrNone)
		{
		__ASSERT(paths);
		CleanupStack::PushL(paths);
		if(::CheckDrivesL(iDbPath, findFile.File()))
			{
			TInt cnt = paths->Count();
			for(TInt i=0;i<cnt;++i)
				{
				const ::TEntry& entry = (*paths)[i];
				//Reuse iDbPath, it is not needed anymore
				TDes& name = iDbPath;
				name.Copy(entry.iName);
				RDbPropsFactory::StripCommonPart(name);
				if (name.Length()<=KDbMaxName)
				    {
				    aDbNames->AddL(name);
				    }
				}
			}
		CleanupStack::PopAndDestroy(paths);
		}
	else if(err != KErrNotFound)
		{
		__LEAVE(err);
		}
	}
