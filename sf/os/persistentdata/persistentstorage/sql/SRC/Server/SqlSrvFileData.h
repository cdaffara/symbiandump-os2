// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVFILEDATA_H__
#define __SQLSRVFILEDATA_H__

#include <f32file.h>				//TParse, RFs
#include "SqlSrvStrings.h"			//KFileHandleSeparator
#include "SqlSrvConfig.h"			//TSqlSrvConfig & TSqlSrvConfigParams


/**
TSqlSrvFileData class.

TSqlSrvFileData class owns two sets of data:
- data, which never changes during the life time of TSqlSrvFileData object - file session instance, 
  system drive name, SQL server private data path, config file parameters, a store of names of existing 
  database configuration files. These permanent TSqlSrvFileData properties are initialized by calling 
  TSqlSrvFileData::InitL(). 
  The SQL server owns only one TSqlSrvFileData instance whose permanent properties are initialized during 
  the server startup and that TSqlSrvFileData instance is available to the other SQL server classes 
  via a call to CSqServer::FileData();
- data, which changes with every call to TSqlSrvFileData::SetL(const RMessage2& aMessage, TInt aArgNum). 
  The TSqlSrvFileData instance expects that argument aArgNum of aMessage object contains a file name.
  The file name will be parsed by the TSqlSrvFileData instance and then the full file name is made available
  through TSqlSrvFileData::FileName() and TSqlSrvFileData::FileNameZ() (zero-terminated file name);
  
The TSqlSrvFileData instance also can be used for (after calling TSqlSrvFileData::SetL() or TSqlSrvFileData::SetFromHandleL()):
- retrieving the full file name;
- retrieving the system drive name;
- retrieving the server private path;
- checking the file name format type - secure or non-secure;
- retrieving the SID of the file name, if that is a secure file name;

With TSqlSrvFileData class we can:
- Minimize the stack usage (there is only one, heap based TSqlSrvFileData instance, owned by the SQL server);
- Keep in one place all data needed for opening/creating/attaching/copying/deleting a database file;

@see CSqlServer
@see CSqlServer::FileData()
@see TSqlSrvFileData::SetL()
@see TSqlSrvFileData::SetFromHandleL()
@see TSqlSrvFileData::Fs()
@see TSqlSrvFileData::PrivatePath()
@see TSqlSrvFileData::Drive()
@see TSqlSrvFileData::FileName()
@see TSqlSrvFileData::FileNameZ()
@see TSqlSrvFileData::IsSecureFileNameFmt()
@see TSqlSrvFileData::SecureUid()
@see TSqlSrvFileData::IsCreated()
@see TSqlSrvFileData::IsReadOnly()
@see TSqlSrvFileData::ContainHandles()
@see TSqlSrvFileData::ConfigParams()
@see TSqlSrvFileData::DbConfigFiles()

@internalComponent
*/
NONSHARABLE_CLASS(TSqlSrvFileData)
	{
public:
	inline void InitL(RFs& aFs, const TDriveName& aSysDriveName, const TDesC& aServerPrivatePath, 
					  const TDesC& aConfigFileName, const CDbConfigFiles* aDbConfigFiles);
	void SetL(const RMessage2& aMessage, TInt aFileNameLen, TInt aFileNameArgNum,
#ifdef SQLSRV_STARTUP_TEST
	          const TDesC& aDbFileName,
#endif	        
	          const TDesC8* aConfigStr = NULL);
  	void SetFromHandleL(const RMessage2& aMessage, const TDesC& aDbFileName, TBool aCreated, TBool aReadOnly, const TDesC8* aConfigStr = NULL);

	inline RFs& Fs() const;
	inline TPtrC PrivatePath() const;
	inline TDriveNumber Drive() const;
	inline TPtrC FileName() const;
	inline TPtrC FileNameZ() const;
	inline TBool IsSecureFileNameFmt() const;
	inline TUid SecureUid() const;
	inline TBool IsCreated() const;
	inline TBool IsReadOnly() const;
	inline TBool ContainHandles() const;
	inline const TSqlSrvConfigParams& ConfigParams() const;
	inline const CDbConfigFiles* DbConfigFiles() const;

private:
	RFs						iFs;
	TParse					iSysDrivePrivatePath;// <System drive name> + <Private directory>
	TSqlSrvConfig			iConfig;			 // Contains the config file parameters (not used directly. TSqlSrvConfig::GetConfigParamsL()
												 // should be used to get the right set of configuration parameters)	
	TBuf<KMaxFileName + 1>	iFileName;
	TDriveNumber 			iDrive;
	TBool					iIsSecureFileNameFmt;
	TUid					iSecureUid;
	TBool					iCreated;			// If the file is a private database, iCreated is true if the file was created,
												// false if the file was opened by the client side
	TBool					iReadOnly;
	TSqlSrvConfigParams		iConfigParams;		// Contains the configuration parameters, which shall be used 
												// for the database connection (when creating/openning)
	const CDbConfigFiles*	iDbConfigFilesPtr;  // Pointer to server's store of existing database config file names, NULL if there are no config files
	};

/**
Initializes the permanent data of TSqlSrvFileData instance.
This type of initialization happens during the SQL server startup.
Once initialized, the permanent set of data stays unchanged for the whole life time of TSqlSrvFileData instance.

@code
The permanent set of data includes:
- a file session instance;
- system drive name;
- SQL server private data path;
- config file parameters;
- a store of names of existing database configuration files
@endcode

@param aFs A reference to a file session instance
@param aSysDriveName A reference to the system drive name
@param aServerPrivatePath A reference to SQL server private data path
@param aConfigFileName SQL server configuration file name
@param aDbConfigFiles A pointer to the server's store of existing database configuration file names
*/
inline void TSqlSrvFileData::InitL(RFs& aFs, const TDriveName& aSysDriveName, const TDesC& aServerPrivatePath, 
								   const TDesC& aConfigFileName, const CDbConfigFiles* aDbConfigFiles)
	{
	iFs = aFs;
	__SQLLEAVE_IF_ERROR(iSysDrivePrivatePath.Set(aSysDriveName, &aServerPrivatePath, 0));
	iConfig.InitL(aFs, aConfigFileName);
	iConfig.GetConfigParamsL(KNullDesC8, iConfigParams);//iConfigParams initialized with the config file params 
														//(because an empty configuration string is passed as an argument)
	iDbConfigFilesPtr = aDbConfigFiles;
	}

/**
@return A reference to the file session instance
*/
inline RFs& TSqlSrvFileData::Fs() const
	{
	return const_cast <RFs&> (iFs);	
	}

/**
@return SQL server private path.
*/
inline TPtrC TSqlSrvFileData::PrivatePath() const
	{
	return iSysDrivePrivatePath.Path();
	}
	
/**
@return The file drive.
*/
inline TDriveNumber TSqlSrvFileData::Drive() const
	{
	return iDrive;	
	}

/**
@return A read-only descriptor pointing to the file name.
*/
inline TPtrC TSqlSrvFileData::FileName() const
	{
	return iFileName.Left(iFileName.Length() - 1);
	}

/**
@return A read-only descriptor pointing to the file name, zero-terminated.
*/
inline TPtrC TSqlSrvFileData::FileNameZ() const
	{
	return iFileName;
	}

/**
@return True if the file name format refers to a secure file name.
*/
inline TBool TSqlSrvFileData::IsSecureFileNameFmt() const
	{
	return iIsSecureFileNameFmt;
	}
	
/**
@return The secure UID, if the file name is a secure file name, KNullUid otherwise.
*/
inline TUid TSqlSrvFileData::SecureUid() const
	{
	return iSecureUid;
	}

/**
Returns true if the file was created, false if the file was opened by the client side.
This function should be used only for private databases, e.g. for which ContainHandles() returns true.
@return True if the file was created, false if the file was opened by the client side.

@see TSqlSrvFileData::ContainHandles()
*/
inline TBool TSqlSrvFileData::IsCreated() const
	{
	return iCreated;
	}

/**
@return True if the file is a read-only file.
*/
inline TBool TSqlSrvFileData::IsReadOnly() const
	{
	return iReadOnly;
	}

/**
@return True if the file name contains file and session handles
*/
inline TBool TSqlSrvFileData::ContainHandles() const
	{
	return iFileName[0] == KFileHandleSeparator;
	}

/**
@return A reference to the configuration parameters.
*/
inline const TSqlSrvConfigParams& TSqlSrvFileData::ConfigParams() const
	{
	return iConfigParams;
	}

/**
@return A pointer to the server's store of existing database configuration file names 
        (will be NULL if there are no existing configuration files)
*/	
inline const CDbConfigFiles* TSqlSrvFileData::DbConfigFiles() const
	{
	return iDbConfigFilesPtr;
	}

#endif//__SQLSRVFILEDATA_H__
