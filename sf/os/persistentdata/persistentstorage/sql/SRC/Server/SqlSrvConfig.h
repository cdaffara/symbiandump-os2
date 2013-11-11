// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVCONFIG_H__
#define __SQLSRVCONFIG_H__

#include <e32std.h>
#include <f32file.h> 
#include "SqlAssert.h"
#include "SqlUtil.h"

//Forward declarations
class RFs;

/**
TSqlSrvConfigParams structure represents a set of configurable SQLITE parameters.
These are:
@code
 - iCacheSize  - page cache size in pages;
 - iPageSize   - page size in bytes;
 - iDbEncoding - database encoding - 16-bit or 8-bit;
 - iSoftHeapLimitKb - soft heap limit in Kb;
 - iCompactMode - manual, background, automatic;
 - iFreePageThresholdKb - free page threshold (in Kb) is used by the background compaction framework;
@endcode

There is only one way a valid TSqlSrvConfigParams object to be created - using TSqlSrvConfig::GetConfigParamsL().
GetConfigParamsL() uses an algorithm, described in TSqlSrvConfig class' comments, to decide which parameter value,
shall be used: build time, config file or the client defined one.

@see TSqlSrvConfig
@see TSqlSrvConfig::GetConfigParamsL()
@internalComponent
*/
NONSHARABLE_STRUCT(TSqlSrvConfigParams)
	{
	enum {KConfigPrmValueNotSet = -1};//iCacheSize, iPageSize, iSoftHeapLimitKb are initialized by default with KConfigPrmValueNotSet
	enum TDbEncoding {EEncNotSet, EEncUtf8, EEncUtf16};//Database encoding: the default value for iDbEncoding is EEncNotSet
	enum 
		{
#ifdef __WINSCW__
		KDefaultSoftHeapLimitKb = 1024,
#else
		KDefaultSoftHeapLimitKb = 8192,
#endif	
#ifdef SYSLIBS_TEST	
		KMinSoftHeapLimitKb = 8, 
#else
		KMinSoftHeapLimitKb = 512,
#endif
		KMaxSoftHeapLimitKb = KMaxTInt / 1024
		};
	
	TSqlSrvConfigParams();	
		
	TInt				iCacheSize;
	TInt				iPageSize;
	TDbEncoding			iDbEncoding;
	TInt				iSoftHeapLimitKb;
	TSqlCompactionMode	iCompactionMode;
	TInt				iFreePageThresholdKb;
	};
	
/**
TSqlSrvConfig class is used for:
@code 
 - keeping the SQL server configuration file parameter values;
 - producing a TSqlSrvConfigParams object with correct SQLITE parameter values when requested (TSqlSrvConfig::GetConfigParamsL());
@endcode

If SQL server config file exists at the moment of the SQL server startup, the SQL server will keep 
a copy of the config file parameter values in a TSqlSrvConfig object.

The SQL server will use the following rules, which config parameter should be used:
@code
----------------------------------------------------------------------------------------------
Server config file parameter | Client config string parameter | What parameter is used
----------------------------------------------------------------------------------------------
.No..........................|.No.............................|.Build-time config parameter
.No..........................|.Yes............................|.Client config string parameter
.Yes.........................|.No.............................|.Server config file parameter
.Yes.........................|.Yes............................|.Client config string parameter
----------------------------------------------------------------------------------------------
@endcode

TSqlSrvConfig::InitL() should be used once for loading the config parameter values from the SQL server comfiguration file.

TSqlSrvConfig::GetConfigParamsL() can be used for retrieving correctly initialized TSqlSrvConfigParams object.
The rules described in the table above will be used for producing correct set of config parameter values in
the created TSqlSrvConfigParams object.

There are exceptions from the rules and these are:
 - the "soft heap limit" parameter, which cannot be set using a configuration string, 
   it is a config file only parameter;
 - the "free page threshold" parameter, which cannot be set using a configuration string, 
   it is a config file only parameter;

The configuration string format is: "PARAM1=VALUE1;PARAM2=VALUE2;...."

@see TSqlSrvConfigParams
@internalComponent
*/
NONSHARABLE_CLASS(TSqlSrvConfig)
	{	
public:
	void InitL(RFs& aFs, const TDesC& aFileName);
	void GetConfigParamsL(const TDesC8& aConfigStr, TSqlSrvConfigParams& aConfigParams) const;
	
private:
	static void GetConfigStringFromFileL(RFs& aFs, const TDesC& aFileName, TDes8& aConfigStr);
	static void ExtractConfigParamsFromStringL(const TDesC8& aConfigStr, TSqlSrvConfigParams& aConfigParams);
	static TBool ExtractParamValuePairL(TPtrC8& aConfigStr, TPtrC8& aParamName, TPtrC8& aParamValue);
	static void ExtractParamValueL(const TDesC8& aParamName, const TDesC8& aParamValue, TSqlSrvConfigParams& aConfigParams);
	static TInt GetCacheSizeL(const TDesC8& aParamValue);
	static TInt GetPageSizeL(const TDesC8& aParamValue);
	static TSqlSrvConfigParams::TDbEncoding GetEncoding(const TDesC8& aParamValue);
	static TInt GetSoftHeapLimitL(const TDesC8& aParamValue);
	static TSqlCompactionMode GetCompactionModeL(const TDesC8& aParamValue);
	static TPtrC8 TrimAndConstructPtr(const TUint8* aStr, TInt aLength);
	static TInt GetFreePageThresholdL(const TDesC8& aParamValue);
	
private:
	TSqlSrvConfigParams	iConfigFileParams;
		
	};

/**
CDbConfigFiles class is used to store the name of each database configuration 
file that exists on the device. Database configuration files are currently 
supported only for shared, secure databases and the name of a configuration 
file is the name of the database itself prefixed with the string ‘cfg’ and 
suffixed with the extension ‘.N’, where N is the version of the configuration 
file. For example, for the database [12345678]a.db a valid configuration 
filename is cfg[12345678]a.db.01. All database configuration files must 
be located in the SQL Server’s private data cage on the Z: drive - 
namely, Z:\private\10281E17\. 

@internalComponent
*/
NONSHARABLE_CLASS(CDbConfigFiles) : public CBase
{
public:		
   	static CDbConfigFiles* NewL(const CDir& aDirEntries);
	~CDbConfigFiles();
	HBufC* FindConfigFile(const TDesC& aDbFilename) const;
	
private:
	CDbConfigFiles();
	void ConstructL(const CDir& aDirEntries);
	void StoreFileNamesL(const CDir& aDirEntries);
		
private:
	RPointerArray<HBufC> iConfigFileNames;
};

#endif//__SQLSRVCONFIG_H__
