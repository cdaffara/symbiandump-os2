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

#ifndef __SQLSRVMAIN_H__
#define __SQLSRVMAIN_H__

#include <f32file.h>
#include "SqlAssert.h" 			//TSqlPanic
#include "SqlSrvFileData.h"		//TSqlSrvFileData
#include "SqlSrvAuthorizer.h"	//MSqlPolicyInspector
#include "SqlSrvSecurityMap.h"	//
#include "SqlSrvDriveSpace.h"	//RSqlDriveSpaceCol
#include "SqlSrvBurInterface.h"	//MSqlSrvBurInterface
#include "SqlBur.h"
#include "SqlBufFlat.h"			//RSqlBufFlat

//Forward declarations
class CSqlCompactor;
class CSqlServer;

CSqlServer& SqlServer(void);

/**
CSqlServer class derives from CServer2 and offers methods for:
  - creating new server side sessions;
  - accessing the last RMessage2 object;
  - accessing the TSqlSrvFileData instance (one per server);
  - accessing the security settings map (one per server);
  
CSqlServer class also implements MSqlPolicyInspector and MSqlSrvBurInterface interfaces;

The class owns also the folowing set of data:
- iFileData    - see TSqlSrvFileData;
- iSecurityMap - see RSqlSecurityMap and RSqlMap;

@see CServer2
@see MSqlPolicyInspector
@see TSqlSrvFileData
@see RSqlMap
@see RSqlSecurityMap
@see CSqlServer::NewLC()
@see CSqlServer::NewSessionL()
@see CSqlServer::FileData()
@see CSqlServer::SecurityMap()
@see CSqlServer::Check()

@internalComponent
*/
NONSHARABLE_CLASS(CSqlServer) : public CServer2, public MSqlPolicyInspector, public MSqlSrvBurInterface
	{
public:		
	static CSqlServer* NewLC();	
	~CSqlServer();
	inline TSqlSrvFileData& FileData();
	inline RSqlSecurityMap& SecurityMap();
	inline RSqlDriveSpaceCol& DriveSpaceCol();
	inline const TDesC& CollationDllName() const;
	inline MSqlPolicyInspector& SecurityInspector();
	//Server buffers
	RSqlBufFlat& GetFlatBufL(TInt aMinLen);
	TDes8& GetBuf8L(TInt aMinLen);
	TDes16& GetBuf16L(TInt aMinLen);
	inline TDes& FileNameBuf();
	void MinimizeBuffers(); 
	//Compactor
	inline CSqlCompactor& Compactor();
	//Overriding CServer2 abstract methods
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	//Overriding MSqlPolicyInspector abstract methods
	virtual TBool Check(const TSecurityPolicy& aPolicy) const;
	//Overriding MSqlSrvBurInterface abstract methods
	virtual RFs& Fs();
	virtual void GetBackUpListL(TSecureId aUid, TDriveNumber aDrive, RArray<HBufC*>& aFileNameList);

private:
	CSqlServer();
	void ConstructL();
	void GetCollationDllNameL();
	void CacheDbConfigFileNamesL(RFs& aFs, const TDesC& aServerPrivatePath);
	TInt ReAllocBuf(TInt aNewBufSize);
	void DeleteTempFilesL(TInt aDriveNumber, const TDesC& aServerPrivatePath) const;

private:
	TSqlSrvFileData		iFileData;		//Used as a temporary storage for file data (file name, drive, path, secure uid)
	RSqlSecurityMap		iSecurityMap;	//Collection of database security policies
	RSqlDriveSpaceCol	iDriveSpaceCol;	//Collection of "drive space" objects (reserved drive space management)
	CSqlBurEventMonitor* iBurEventMonitor;//Monitors B&R events
	TFileName			iCollationDllName;//Default collation - dll name - uniquely identifies the collation method
	RSqlBufFlat  		iFlatBuf;		//general purpose flat buffer. do not keep a state in it between calls!
	TUint8*				iBuf;			//general purpose buffer. do not keep a state in it between calls!
	TPtr8				iBufPtr8;		//8-bit pointer to iBuf
	TPtr16				iBufPtr16;		//16-bit pointer to iBuf
	TFileName 			iFileNameBuf;	//General purpose buffer, can be used for storing file names
	CDbConfigFiles*	    iDbConfigFiles; //Holds the database config file names, if any exist (will be NULL if no files exist)
	CSqlCompactor*		iCompactor;		//The background compacting object (container for the db entries to be compacted)
	};

/**
@return A reference to the file data object (one per server).
*/
inline TSqlSrvFileData& CSqlServer::FileData()
	{
	return iFileData;
	}
	
/**
@return A reference to the security settings map object which maintains a map of 
        {Database name, security settings container} entries.
*/
inline RSqlSecurityMap& CSqlServer::SecurityMap()
	{
	return iSecurityMap;
	}

/**
@return A reference to the "drive space" collection
*/
inline RSqlDriveSpaceCol& CSqlServer::DriveSpaceCol()
	{
	return iDriveSpaceCol;
	}

/**
@return A reference to the default collation - the dll name.
*/
inline const TDesC& CSqlServer::CollationDllName() const
	{
	return iCollationDllName;	
	}

/**
Returns a reference to the security policy inspector interface.
@return A reference to the security policy inspector interface.
*/
inline MSqlPolicyInspector& CSqlServer::SecurityInspector()
	{
	return static_cast <MSqlPolicyInspector&> (*this);
	}

/**
@return A reference to the file name buffer.
*/
inline TDes& CSqlServer::FileNameBuf()
	{
	return iFileNameBuf;		
	}

/**
@return A reference to the compactor.
@panic SqlDb 7 In _DEBUG mode if the compactor is NULL.
*/
inline CSqlCompactor& CSqlServer::Compactor()
	{
	__ASSERT_DEBUG(iCompactor != NULL, __SQLPANIC(ESqlPanicInternalError));			
	return *iCompactor;
	}

#endif//__SQLSRVMAIN_H__
