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
// RDbPropsFactory class - "DBMS Security" related - full security support
// 
//

#include <f32file.h>
#include "D32DRVR.H"
#include "U32STD.H"
#include "D32Strings.h"
#include "Sd_DbProps.h"
#include "Sd_DbList.h"

using namespace DBSC;

/**
Utility method, which can be used separately to remove the common part of a secure
shared database name. The input string format is:
"DBS_UID_<DbName>". 
The output string format is:
"<DbName>". 
@param aDbName An output/input parameter. Th input format is: "DBS_UID_<DbName>".
               The output format is: "<DbName>". 
@panic The method will panic in debug builds in case of a bad input string.
*/
void RDbPropsFactory::StripCommonPart(TDes& aDbName)
	{
	TInt pos = aDbName.FindF(KDbsPrefix);
	__ASSERT_DEBUG(pos != KErrNotFound, User::Invariant());
	aDbName.Delete(pos, KDbsPrefix().Length());
	//Remove the UID from the string
	TInt pos_b = aDbName.Locate('_');
	
	TPtrC right = aDbName.Mid(pos_b+1);
	TInt pos_e = right.Locate('_') + pos_b + 1;
	
	__ASSERT_DEBUG(pos_b != KErrNotFound && pos_e != KErrNotFound, User::Invariant());
	aDbName.Delete(pos_b, pos_e - pos_b + 1);
	}

/**
Utility method, which can be used separately to construct the common part of the secure
shared database name. The result string format is:
"DBS_UID_"
@param aPolicyUid Security policy UID.
@param aRes An output parameter, referencing the location, where the constructed string will be stored.
*/
void RDbPropsFactory::ConstructCommonPart(TUid aPolicyUid, TDes& aRes)
	{
	aRes.Zero();
	aRes.Append(KDbsPrefix);
	aRes.Append('_');
	aRes.AppendNumFixedWidthUC(aPolicyUid.iUid, EHex, 8);
	aRes.Append('_');
	}

/**
Standard factory method for TDbProps instances.
The created TDbProps instance will be pushed in the cleanup stack.
@return A pointer to the created TDbProps instance.
@leave KErrNoMemory
@internalComponent
*/
static TDbProps* NewDbPropsLC()
	{
	TDbProps* dbProps = new (ELeave) TDbProps;
	CleanupStack::PushL(dbProps);
	return dbProps;
	}

/**
Extracts the drive number from the supplied TParse instance.
@param aFileNameParser A reference to TParse instance, which will be used to extract the 
drive number.
@return Extracted TDriveNumber value.
@leave KErrArgument aFileNameParser parameter does not contain a drive number or it cannot
					be constructed from the string.
@internalComponent
*/
static TDriveNumber ExtractDriveNumberL(TParse& aFileNameParser)
	{
	TPtrC drvPtr = aFileNameParser.Drive();
	if(drvPtr.Length() == 0)
		{
		__LEAVE(KErrArgument);
		}
	TInt drvId = 0;
	__LEAVE_IF_ERROR(RFs::CharToDrive(drvPtr[0], drvId));
	return static_cast <TDriveNumber> (drvId);
	}

/**
Creates private directory of the DBMS server if it does not exist (on a specific drive).
If the supplied aDriveNumber parameter refers to a rom drive, the method does nothing.
@param aDriveNumber The drive number, where the private DBMS data directory has to be created.
@param aFs A file session instance.
@leave RFs::CreatePrivatePath() leave error codes.
@internalComponent
*/
static void CreatePrivateDataPathL(TDriveNumber aDriveNumber, RFs& aFs)
	{
	TDriveInfo driveInfo;
	__LEAVE_IF_ERROR(aFs.Drive(driveInfo, aDriveNumber));
	if(driveInfo.iDriveAtt & KDriveAttRom)
		{//ROM drive - do nothing.
		return;
		}
	TInt err = aFs.CreatePrivatePath(aDriveNumber);
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		__LEAVE(err);
		}
	}

/**
*/
RDbPropsFactory::RDbPropsFactory(RFs& aFs) :
	iFs(aFs),
	iFileNameParser(NULL),
	iPrivateDataPath(NULL)
	{
	}

/**
Initializes RDbPropsFactory instance
@leave One of the system wide error codes, including KErrNoMemory.
*/
void RDbPropsFactory::OpenL()
	{
	iFileNameParser = new (ELeave) TParse;
	iPrivateDataPath = HBufC::NewL(KMaxFileName);
	TPtr ptr(iPrivateDataPath->Des());
	__LEAVE_IF_ERROR(iFs.PrivatePath(ptr));
	}

/**
*/
void RDbPropsFactory::Close()
	{
	delete iPrivateDataPath;
	delete iFileNameParser;
	}

/**
Extracts database properties from the database path and format string.
The created and returned TDbProps instance will be pushed in the cleanup stack.
@param aPath Database path.
@param aFormatStr database format string.
@return A pointer to the created TDbProps instance.
@leave One of the system-wide error codes, including KErrNoMemory.
*/
TDbProps* RDbPropsFactory::ExtractLC(const TDesC& aPath, const TDesC& aFormatStr)
	{
	__ASSERT(iFileNameParser);
	__ASSERT(iPrivateDataPath);

	__LEAVE_IF_ERROR(iFileNameParser->Set(aPath, NULL, NULL));
	TDbProps* dbProps = ::NewDbPropsLC();

	//TDbProps::iDbsUid.iUid, TDbProps::iDbsUid.iRqAccess
	TPtrC fmtIdent;//fmtIdent may contain KSecure keyword.
	::ExtractUidAndName(aFormatStr, dbProps->iDbPolicyRequest.iUid, fmtIdent);
	dbProps->iDbPolicyRequest.iAccessType = (fmtIdent.CompareF(KSecure) == 0 ? EATSecure : EATNonSecure);

	//TDbProps::iDriveNumber
	dbProps->iDriveNumber = ::ExtractDriveNumberL(*iFileNameParser);
	::CheckDriveL(iFs, dbProps->iDriveNumber);

	if(dbProps->iDbPolicyRequest.iAccessType == EATSecure)
		{//requested access to a secure shared database
		ExtractSecureL(aFormatStr, *dbProps);
		}
	else
		{//requested access to a non-secure database
		ExtractNonSecureL(aPath, aFormatStr, *dbProps);
		}

	return dbProps;
	}

/**
Extracts database properties from the database path, assuming that this is a secure shared
database.
The created and returned TDbProps instance will be pushed in the cleanup stack.
@param aPath Database path.
@param aPolicyUid Security policy UID.
@return A pointer to the created TDbProps instance.
@leave One of the system-wide error codes, including KErrNoMemory.
*/
TDbProps* RDbPropsFactory::ExtractLC(const TDesC& aPath, TUid aPolicyUid)
	{
	TBuf<32> dbFormat;
	dbFormat.Copy(KSecure);
	dbFormat.Append(aPolicyUid.Name());
	return ExtractLC(aPath, dbFormat);
	}

/**
Utility method, which can be used separately to get the common part of the secure
shared database full path. The result string format is:
"<Drive>:\<PrivatePath>\"
@param aDriveNumber A drive number, for which the private data path string has to be constructed.
@param aRes An output parameter, referencing the location, where the created private path has to be copied.
@leave RFs::DriveToChar() leave error codes
*/
void RDbPropsFactory::GetPrivatePathL(TDriveNumber aDriveNumber, TDes& aRes) const
	{
	aRes.Zero();
	TChar driveChar;
	__LEAVE_IF_ERROR(RFs::DriveToChar(aDriveNumber, driveChar));
	aRes.Append(driveChar);
	aRes.Append(':');
	aRes.Append(*iPrivateDataPath);
	}

/**
Extracts secure shared database properties.
@param aFormatStr Secure shared database format string.
@param aDbProps An output parameter, referencing the location, where the datapase properties will be stored.
@leave KErrArgument Bad format string. Some of the other system-wide error codes.
*/
void RDbPropsFactory::ExtractSecureL(const TDesC& aFormatStr, TDbProps& aDbProps)
	{	
	if(aDbProps.iDbPolicyRequest.iUid == KNullUid)
		{//Secure shared database cannot have null uid.
		__LEAVE(KErrArgument);
		}
	if(iFileNameParser->PathPresent())
		{//The path can contain only the database name.
		__LEAVE(KErrArgument);
		}
	TPtrC dbName = iFileNameParser->NameAndExt();
	if(dbName.Length() > KDbMaxName)
		{//There is a limit for the secure shared database names
		__LEAVE(KErrArgument);
		}
	::CreatePrivateDataPathL(aDbProps.iDriveNumber, iFs);
	ConstructFullDbPathL(aDbProps);
	ConstructFormatString(aDbProps, aFormatStr);
	}

/**
Extracts non-secure database properties.
@param aPath Database path.
@param aFormatStr Database format string.
@param aDbProps An output parameter, referencing the location, where the datapase properties will be stored.
@leave KErrPermissionDenied The database path contains the DBMS server private data path.
*/
void RDbPropsFactory::ExtractNonSecureL(const TDesC& aPath, const TDesC& aFormatStr, 
										TDbProps& aDbProps)
	{
	//DBMS private data path cannot be the first in the database path. This is non-secure database.
	TInt pos = aPath.FindF(iPrivateDataPath->Des());
	if(pos != KErrNotFound)
		{//If pos is 2 (pos 0 - drive letter, pos 1 - ':'), then 
		 //the caller wants to create/open non-secure database in the DBMS private directory,
		 //which is not allowed. 
		if(pos == 2)
			{
			__LEAVE(KErrPermissionDenied);
			}
		}
	//The database path and format string stay the same
	aDbProps.iPath.Copy(aPath);
	aDbProps.iFormatStr.Copy(aFormatStr);
	}

/**
Constructs the full physical path of the secure shared database
@param aDbProps An output parameter, where the database path will be stored.
@leave RDbPropsFactory::GetPrivatePathL() leaving error codes
@see RDbPropsFactory::GetPrivatePathL()
*/
void RDbPropsFactory::ConstructFullDbPathL(TDbProps& aDbProps)
	{
	GetPrivatePathL(aDbProps.iDriveNumber, aDbProps.iPath);
	TBuf<32> dbNameCmnPart;
	RDbPropsFactory::ConstructCommonPart(aDbProps.iDbPolicyRequest.iUid, dbNameCmnPart);
	aDbProps.iPath.Append(dbNameCmnPart);
	aDbProps.iPath.Append(iFileNameParser->Name());
	aDbProps.iPath.Append(iFileNameParser->Ext());
	}

/**
Processes the format string of secure shared database. "SECURE" keyword and security policy UID
will be removed.
@param aDbProps An input/output parameter, referencing TDbProps instance, where the processed
                database format string will be stored.
@param aFormatStr The database format string.
*/
void RDbPropsFactory::ConstructFormatString(TDbProps& aDbProps, const TDesC& aFormatStr)
	{
	TDes& fmtStr = aDbProps.iFormatStr;
	fmtStr.Copy(aFormatStr);
	//Remove KSecure keyword from the format string 
	TInt pos = fmtStr.FindF(KSecure);
	__ASSERT_DEBUG(pos != KErrNotFound, User::Invariant());
	fmtStr.Delete(pos, KSecure().Length());
	//Remove the UID from the format string
	TInt pos_b = fmtStr.Locate('[');
	TInt pos_e = fmtStr.Locate(']');
	__ASSERT_DEBUG(pos_b != KErrNotFound && pos_e != KErrNotFound, User::Invariant());
	fmtStr.Delete(pos_b, pos_e - pos_b + 1);
	}
