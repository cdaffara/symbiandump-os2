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

#include "SqlSrvFileData.h"
#include "SqlSrvUtil.h"
#include "SqlAssert.h"
#include "SqlSrvStrings.h"
#include "SqlSrvResourceProfiler.h"

_LIT(KPrivateFmtStr, "\\private\\%08X\\");

/**
Creates SQL server private data path on the specified drive.

The idea for calling it is to make sure that the server's private data path exists before making any other 
operation - attempting to create a database file there for example. 

@param aFs File session instance
@param aDriveNumber Drive number on which the private path has to be created

@internalComponent
*/
static void CreatePrivateDataPathL(RFs& aFs, TDriveNumber aDriveNumber)
	{
	TDriveInfo driveInfo;
	__SQLLEAVE_IF_ERROR2(aFs.Drive(driveInfo, aDriveNumber));
	if(!(driveInfo.iDriveAtt & KDriveAttRom))
		{
		TInt err = aFs.CreatePrivatePath(aDriveNumber);
		if(err != KErrNone && err != KErrAlreadyExists)
			{
			__SQLLEAVE2(err);
			}
		}
	}

/**
@return Zero if aDbFileName is a non-secure file name (contains the path), non-zero otherwise.

@internalComponent
*/
static TBool IsSecureFileNameFmt(const TDesC& aDbFileName)
	{
	TParsePtrC parse(aDbFileName);//this call may panic if aDbFileName cannot be parsed, but SetL() already parsed it
	return !parse.PathPresent();
	}

/**
The function parses thr database file name argument and extracts the SID from it (if the name contains SID).
The SID is expected to be found at position 0 of the file name and must have 8 hex digits.

@param aDbFileName Database file name

@return Database security UID or KNullUid if the database name does not contain SID.

@internalComponent
*/
static TUid ExtractSID(const TDesC& aDbFileName)
	{
	TParsePtrC parse(aDbFileName);//this call may panic if aDbFileName cannot be parsed, but SetL() already parsed it
	TPtrC dbName = parse.Name();
	TInt pos1 = dbName.Locate(TChar('['));
	TInt pos2 = dbName.Locate(TChar(']'));
	if(pos1 == 0 && pos2 == 9)	//position 0 for '[', 8 digits SID, position 9 for ']'
		{
		TLex lex(dbName.Mid(pos1 + 1, pos2 - pos1 - 1));
		TUid securityUid;
		TInt err = lex.Val(*(TUint32*)&securityUid, EHex);
		if(err == KErrNone)
			{
			return securityUid;	
			}
		}
	return KNullUid;
	}

/**
@return ETrue if the aDbFileName argument contains aPrivatePath as a first directory in the file path, EFalse otherwise.

@internalComponent
*/
static TBool IsPrivatePathInFileName(const TDesC& aDbFileName, const TDesC& aPrivatePath)
	{
	TInt pos = aDbFileName.FindF(aPrivatePath);
	return (TUint)pos <= (TUint)KMaxDriveName;
	}

/**
The method parses aFileName argument and constructs the full database file name (including the path) there.
The full file name will be constructed in aFileName input/output argument.

@param aDbFileName Input/Output. Database file name will be constructed there.
@param aSysDrivePrivatePath SQL server private path on the system drive.   
@param aDrive Output parameter. The drive number.
				
@leave KErrBadName Missing file name.

@panic SqlDb 7 In _DEBUG mode - no drive in the final file path.

@internalComponent
*/
static void DoFullFileNameL(TDes& aDbFileName, const TDesC& aSysDrivePrivatePath, TDriveNumber& aDrive)
	{
	TParse parse;
	__SQLLEAVE_IF_ERROR2(parse.Set(aDbFileName, &aSysDrivePrivatePath, NULL));
	if(!parse.NamePresent())
		{
		__SQLLEAVE2(KErrBadName);	
		}
	aDbFileName.Copy(parse.FullName());
	TPtrC driveName = parse.Drive();
	__ASSERT_DEBUG(driveName.Length() > 0, __SQLPANIC2(ESqlPanicInternalError));
	TInt driveNumber = -1;
	__SQLLEAVE_IF_ERROR2(RFs::CharToDrive(driveName[0], driveNumber));
	aDrive = static_cast <TDriveNumber> (driveNumber);
	}

/**
Extracts file name properties, such as secure/non-secure file name, secure UID (SID).

@param aDbFileName Database file name
@param aServerPrivatePath SQL ser ver private path
@param aIsSecureFileNameFmt Output. Initialized with non-zero if aDbFileName format is "[drive:]name"
@param aSecureUid Output. Database secure UID. KNullUid for non-secure databases.

@internalComponent
*/
static void GetFileNamePropertiesL(const TDesC& aDbFileName, const TDesC& aServerPrivatePath, 
								   TBool& aIsSecureFileNameFmt, TUid& aSecureUid)
	{
	//If SQL server private path is in the file name - leave
	if(::IsPrivatePathInFileName(aDbFileName, aServerPrivatePath))
		{
		__SQLLEAVE2(KErrArgument);
		}
	//Extract database SID from the name
	aIsSecureFileNameFmt = ::IsSecureFileNameFmt(aDbFileName);
	aSecureUid = KNullUid;
	if(aIsSecureFileNameFmt)
		{
		aSecureUid = ::ExtractSID(aDbFileName);
		}
	}

/**
Extracts configuration parameters from client's config string.
For the rules how decision is made which parameter has to be used - from the config file or from the config string,
please check the TSqlSrvConfig class' comments.
If the client config string (aConfigStr argument) is NULL, then the config file parameters will be used (if there is a config file)
or the build-time partameters. Again, check the TSqlSrvConfig class' comments. 

@see TSqlSrvConfig
@see TSqlSrvConfigParams

@param aConfigStr Client configuration string, can be NULL
@param aConfigParams Output parameter, the place where config parameters will be stored
@param aConfig TSqlSrvConfig object used for the production of the config parameters

@see TSqlSrvConfig

@internalComponent
*/
static void ExtractConfigParamsL(const TDesC8* aConfigStr, TSqlSrvConfigParams& aConfigParams, const TSqlSrvConfig& aConfig)
	{
	TPtrC8 ptr(aConfigStr ? *aConfigStr : KNullDesC8());
	aConfig.GetConfigParamsL(ptr, aConfigParams);
	}

/**
1. Reads the database file name which is in "aFileNameArgNum" argument of aMessage and
	initializes with it iFileName.
2. Parses the file name and initializes iIsSecureFileNameFmt and iSecureUid.
3. Creates the full file name in iFileName.
4. Creates the server private directory on the related drive.

@leave KErrBadName, the database file name length is invalid (not in [1..KMaxFileName] range);
	   KErrArgument, the database file name contains the server private path;
       KErrArgument, the database file name format is secure but the name does not contain SID.

@panic SqlDb 4 In _DEBUG mode. Invalid aFileNameArgNum value.
@panic SqlDb 7 In _DEBUG mode. Invalid TSqlSrvFileData object. Not initialized system drive and path.
*/
void TSqlSrvFileData::SetL(const RMessage2& aMessage, TInt aFileNameLen, TInt aFileNameArgNum, 
#ifdef SQLSRV_STARTUP_TEST
                           const TDesC& aDbFileName,
#endif          
                           const TDesC8* aConfigStr)
	{
	__ASSERT_DEBUG((TUint)aFileNameArgNum < KMaxMessageArguments, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iSysDrivePrivatePath.DriveAndPath().Length() > 0, __SQLPANIC(ESqlPanicInternalError));
		
	if(aFileNameLen < 1 || aFileNameLen > KMaxFileName)
		{
		__SQLLEAVE(KErrBadName);
		}
#ifdef SQLSRV_STARTUP_TEST
	//To prevent compiler warning
	aMessage.Int0();
	aFileNameArgNum = aFileNameArgNum;
	//
	iFileName.Copy(aDbFileName);
#else
	aMessage.ReadL(aFileNameArgNum, iFileName);
#endif	
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, (aFileNameLen * sizeof(TText)));
	TParse parsedFileName;
	__SQLLEAVE_IF_ERROR(parsedFileName.Set(iFileName, 0, 0));//prophylactic check, leave if the file name cannot be parsed
	::GetFileNamePropertiesL(iFileName, iSysDrivePrivatePath.Path(), iIsSecureFileNameFmt, iSecureUid);
	::DoFullFileNameL(iFileName, iSysDrivePrivatePath.DriveAndPath(), iDrive);
	iFileName.Append(TChar(0));
	if(iIsSecureFileNameFmt)
		{
		if(iSecureUid == KNullUid)		
			{
			__SQLLEAVE(KErrArgument);	
			}
		::CreatePrivateDataPathL(iFs, iDrive);
		}
	iReadOnly = ::IsReadOnlyFileL(iFs, FileName());
	::ExtractConfigParamsL(aConfigStr, iConfigParams, iConfig);
	}

/**
1. Initializes iFileName with the database file name.
2. Initializes iDrive.
3. Checks that iFileName really refers to a file belonging to application's private data cage.

Since the file to be created/opened is a file which belongs to the client application's private data cage
and the file has been created/opened already on the client side, iFileName is formatted to contain useful
information for the OS layer, such as file handle, file session handle, etc. The information is passed
to the OS layer in this strange way (formatted string treted as a file name), because the infomation goes
through the SQLITE library first.
The format of iFileName is:
@code
Bytes:01         2                  10     11       20                   Last byte (before the terminating 0)
      |<R/O flag><RMessage2 pointer><drive><app SID><file_name><file_ext>|
@endcode

'|' is a symbol which cannot be placed in normal file names, so here it is used as an indication that the
string is not a file name (the string contains other information - handles message pointers, etc).

@leave KErrBadName, the database file name length is invalid (not in [1..KMaxFileName] range);
       KErrPermissionDenied, the database file name is not in the application's private data cage.
*/
void TSqlSrvFileData::SetFromHandleL(const RMessage2& aMessage, const TDesC& aDbFileName, TBool aCreated, TBool aReadOnly,
									 const TDesC8* aConfigStr)
	{
	TParse parsedFileName;
	__SQLLEAVE_IF_ERROR(parsedFileName.Set(aDbFileName, 0, 0));//prophylactic check, leave if the file name cannot be parsed
	iCreated = aCreated;
	iReadOnly = aReadOnly;
	iIsSecureFileNameFmt = EFalse;
	iSecureUid = KNullUid;
	iFileName.Copy(aDbFileName);
	TParsePtrC parse(iFileName);
	if(!parse.DrivePresent() || !parse.PathPresent())
		{
		__SQLLEAVE(KErrBadName);
		}
	//Get the drive number
	TPtrC driveName = parse.Drive();
	TInt driveNumber = -1;
	__SQLLEAVE_IF_ERROR(RFs::CharToDrive(driveName[0], driveNumber));
	iDrive = static_cast <TDriveNumber> (driveNumber);
	::CreatePrivateDataPathL(iFs, iDrive);
	//Create in "buf" variable calling application's private data path
	TBuf<KMaxFileName + 1> buf;
	buf.Format(KPrivateFmtStr(), aMessage.SecureId().iId);
	//Check that the file name refers to a file which is in the application's private data cage
	TInt pos = iFileName.FindF(buf);
	if((TUint)pos > (TUint)KMaxDriveName)
		{
		__SQLLEAVE(KErrPermissionDenied);
		}
	//Form a new unique name for the database. It will be used when creating transaction rollback files, etc.
	TPtrC nameAndExt = parse.NameAndExt();
	buf.Format(KFileHandleFmt(), iReadOnly ? 1 : 0, &aMessage, &driveName, aMessage.SecureId().iId, &nameAndExt);
	iFileName.Copy(buf);
	::ExtractConfigParamsL(aConfigStr, iConfigParams, iConfig);
	}

