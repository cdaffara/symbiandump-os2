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
#include <f32file.h>
#include <f32file64.h>
#include "SqlSrvConfig.h"
#include "SqlSrvUtil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvConfigTraces.h"
#endif
#include "SqlTraceDef.h"

/**
Initializes TSqlSrvConfigParams data members with their default values.
*/
TSqlSrvConfigParams::TSqlSrvConfigParams() :
	iCacheSize(TSqlSrvConfigParams::KConfigPrmValueNotSet),
	iPageSize(TSqlSrvConfigParams::KConfigPrmValueNotSet),
	iDbEncoding(TSqlSrvConfigParams::EEncNotSet),
	iSoftHeapLimitKb(TSqlSrvConfigParams::KConfigPrmValueNotSet),
	iCompactionMode(ESqlCompactionNotSet),
	iFreePageThresholdKb(TSqlSrvConfigParams::KConfigPrmValueNotSet)
	{
	}

/**
"Object initialization" method.
Opens and reads the SQL server configuration file (if it exists).
Initializes TSqlSrvConfig data members using the data from the config file.
If the configuration file does not exist, TSqlSrvConfig data members stay unchanged,
except: 
 - the soft heap limit, which default value is TSqlSrvConfigParams::KDefaultSoftHeapLimit;
 - the free pages threshold, which default value is KSqlCompactFreePageThreshold;

@param aFs File session
@param aFileName Config file name
@leave KErrEof No valid config string found in the configuration file;
               Note that the function may also leave with some other system-wide error codes.
*/
void TSqlSrvConfig::InitL(RFs& aFs, const TDesC& aFileName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLSRVCONFIGFILE_INITL_ENTRY, "Entry;0;TSqlSrvConfig::InitL;aFs.Handle()=0x%X;aFileName=%S", (TUint)aFs.Handle(), __SQLPRNSTR(aFileName)));
	if(::FileExists(aFs, aFileName))
		{
        SQL_TRACE_INTERNALS(OstTrace0(TRACE_INTERNALS, TSQLSRVCONFIGFILE_INITL1, "0;TSqlSrvConfig::InitL;Config file found"));
		TBuf8<KSqlSrvMaxConfigStrLen> configFileStr;
		//Step 1: get the config string from the config file and store the string in configFileStr
		TSqlSrvConfig::GetConfigStringFromFileL(aFs, aFileName, configFileStr);
		__SQLTRACE_INTERNALSVAR(TBuf<100> des16prnbuf);
        SQL_TRACE_INTERNALS(OstTraceExt1(TRACE_INTERNALS, TSQLSRVCONFIGFILE_INITL2, "0;TSqlSrvConfig::InitL;Config file string=%s", __SQLPRNSTR8(configFileStr, des16prnbuf)));
		//Step 2: extract config file parameters from the string (configFileStr)  and store them in iConfigFileParams
		TSqlSrvConfig::ExtractConfigParamsFromStringL(configFileStr, iConfigFileParams);
		}
	//If the soft heap limit is not set from the file or the file does not exist - then set the soft heap limit with the default value
	if(iConfigFileParams.iSoftHeapLimitKb == TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{
		iConfigFileParams.iSoftHeapLimitKb = TSqlSrvConfigParams::KDefaultSoftHeapLimitKb;
		}
	//If the free page threshold is not set from the file or the file does not exist - then set the free page threshold with the default value
	if(iConfigFileParams.iFreePageThresholdKb == TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{
		iConfigFileParams.iFreePageThresholdKb = KSqlCompactFreePageThresholdKb;
		}

#ifdef _SQL_RDEBUG_PRINT
    SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, TSQLSRVCONFIGFILE_INITL_EXIT1, "Exit;0;TSqlSrvConfig::InitL;iCacheSize=%d;iPageSize=%d;iDbEncoding=%d;iSoftHeapLimit=%d", iConfigFileParams.iCacheSize, iConfigFileParams.iPageSize, iConfigFileParams.iDbEncoding, iConfigFileParams.iSoftHeapLimitKb));
#else	
    SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, TSQLSRVCONFIGFILE_INITL_EXIT2, "Exit;0;TSqlSrvConfig::InitL;iCacheSize=%d;iPageSize=%d;iDbEncoding=%{TSqlSrvConfig_TDbEncoding};iSoftHeapLimit=%d", iConfigFileParams.iCacheSize, iConfigFileParams.iPageSize, iConfigFileParams.iDbEncoding, iConfigFileParams.iSoftHeapLimitKb));
#endif    
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLSRVCONFIGFILE_INITL_EXIT3, "Exit;0;TSqlSrvConfig::InitL;iCompactionMode=%d;iFreePageThresholdKb=%d", iConfigFileParams.iCompactionMode, iConfigFileParams.iFreePageThresholdKb));
	}

/**
Parses the config string parameter (aConfigStr), extracts configuration parameters values and 
and initialises with them aConfigParams data members.
The config string format is: "PARAM1=VALUE1;PARAM2=VALUE2;..."
If there is unknown parameter "name=value" pair in the config string, it will be skipped - not reported as an error.
In a case of a leave, the old content of aConfigStr is preserved.
The rules for TSqlSrvConfigParams data members initialization are described in TSqlSrvConfig class' comments.

@see TSqlSrvConfig
@param aConfigStr the config descriptor
@param aConfigParams Output argument, config parameters will be stored there.
@leave KErrArgument if the config is not good or the config string contains "soft heap limit" parameter/value pair.
@leave KErrArgument if the config is not good or the config string contains "free page threshold" parameter/value pair.
*/
void TSqlSrvConfig::GetConfigParamsL(const TDesC8& aConfigStr, TSqlSrvConfigParams& aConfigParams) const
	{
	__SQLTRACE_INTERNALSVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_INTERNALS(OstTraceExt1(TRACE_INTERNALS, TSQLSRVCONFIGFILE_GETCONFIGPARAMSL_ENTRY, "Entry;0;TSqlSrvConfig::GetConfigParamsL;aConfigStr=%s", __SQLPRNSTR8(aConfigStr, des16prnbuf)));
	TSqlSrvConfigParams tmpConfigParams;
	//Step 1: extract configuration parameters from aConfigStr, store them in tmpConfigParams.
	TSqlSrvConfig::ExtractConfigParamsFromStringL(aConfigStr, tmpConfigParams);
	if(tmpConfigParams.iSoftHeapLimitKb != TSqlSrvConfigParams::KConfigPrmValueNotSet || 
	   tmpConfigParams.iFreePageThresholdKb != TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{//It is not allowed the soft heap limit to be set from a config string, only from the SQL server config file.
		 //It is not allowed the free page threshold to be set from a config string, only from the SQL server config file.
		__SQLLEAVE(KErrArgument);
		}
	//Step 2: store tmpConfigParams in aConfigParams.
	aConfigParams = tmpConfigParams;
	//Step 3: replace each "not set" parameter in aConfigParams with the related parameter value from iConfigFileParams (the config file).
	if(aConfigParams.iPageSize == TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{
		aConfigParams.iPageSize = iConfigFileParams.iPageSize;
		}
	if(aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncNotSet)
		{
		aConfigParams.iDbEncoding = iConfigFileParams.iDbEncoding;
		}
	if(aConfigParams.iCompactionMode == ESqlCompactionNotSet)
		{
		aConfigParams.iCompactionMode = iConfigFileParams.iCompactionMode;
		}
	//Step 4: set the soft heap limit.
	aConfigParams.iSoftHeapLimitKb = iConfigFileParams.iSoftHeapLimitKb;
	//Step 5: set the free page threshold.
	aConfigParams.iFreePageThresholdKb = iConfigFileParams.iFreePageThresholdKb;
	//Step 6: assert the parameter values.
	__ASSERT_DEBUG(aConfigParams.iPageSize == TSqlSrvConfigParams::KConfigPrmValueNotSet || aConfigParams.iPageSize >= 0, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iCacheSize == TSqlSrvConfigParams::KConfigPrmValueNotSet || aConfigParams.iCacheSize >= 0, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncNotSet || 
				aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncUtf8 || 
				aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncUtf16, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iSoftHeapLimitKb == TSqlSrvConfigParams::KConfigPrmValueNotSet || 
	            (aConfigParams.iSoftHeapLimitKb >= TSqlSrvConfigParams::KMinSoftHeapLimitKb &&
	             aConfigParams.iSoftHeapLimitKb <= TSqlSrvConfigParams::KMaxSoftHeapLimitKb), __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iCompactionMode == ESqlCompactionNotSet || aConfigParams.iCompactionMode == ESqlCompactionManual || 
				aConfigParams.iCompactionMode == ESqlCompactionBackground || aConfigParams.iCompactionMode == ESqlCompactionAuto, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iFreePageThresholdKb == TSqlSrvConfigParams::KConfigPrmValueNotSet || 
				aConfigParams.iFreePageThresholdKb >= 0, __SQLPANIC(ESqlPanicInternalError));
#ifdef _SQL_RDEBUG_PRINT
    SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, TSQLSRVCONFIGFILE_GETCONFIGPARAMSL_EXIT1, "Exit;0;TSqlSrvConfig::GetConfigParamsL;cacheSize=%d;pageSize=%d;dbEncoding=%d;softHeapLimit=%d", aConfigParams.iCacheSize, aConfigParams.iPageSize, aConfigParams.iDbEncoding, aConfigParams.iSoftHeapLimitKb));
#else   
    SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, TSQLSRVCONFIGFILE_GETCONFIGPARAMSL_EXIT2, "Exit;0;TSqlSrvConfig::GetConfigParamsL;cacheSize=%d;pageSize=%d;dbEncoding=%{TSqlSrvConfig_TDbEncoding};softHeapLimit=%d", aConfigParams.iCacheSize, aConfigParams.iPageSize, aConfigParams.iDbEncoding, aConfigParams.iSoftHeapLimitKb));
#endif    
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLSRVCONFIGFILE_GETCONFIGPARAMSL_EXIT3, "Exit;0;TSqlSrvConfig::GetConfigParamsL;compactionMode=%d;freePageThresholdKb=%d", aConfigParams.iCompactionMode, aConfigParams.iFreePageThresholdKb));
	}

//The function opeans the aFileName config file and reads the config string, storring it in aConfigStr argument.
//Preconditions:
// - The config file does exist;
// - It is a file, containing 16-bit strings;
// - aConfigStr max size is at least KSqlSrvMaxConfigStrLen bytes;
//The function may leave if some of the file I/O operations (open file, read file) fails.
void TSqlSrvConfig::GetConfigStringFromFileL(RFs& aFs, const TDesC& aFileName, TDes8& aConfigStr)
	{
	__ASSERT_DEBUG(aConfigStr.MaxLength() >= KSqlSrvMaxConfigStrLen, __SQLPANIC2(ESqlPanicBadArgument));
	RFile64 cfgFile;
	CleanupClosePushL(cfgFile);
	__SQLLEAVE_IF_ERROR2(cfgFile.Open(aFs, aFileName, EFileRead));
	TFileText cfgFileReader;
	cfgFileReader.Set(cfgFile);
	TBuf<KSqlSrvMaxConfigStrLen> buf;
	TBool cfgLineFound = EFalse;
	TInt err = KErrNone;
	//Read the configuration file line by line until get the first "non-comment" line.
	while((err = cfgFileReader.Read(buf)) == KErrNone)
		{
		buf.TrimAll();
		if(buf.Length() == 0 || buf.Locate('#') == 0)	//'#' means - this line is a comment
			{
			continue;
			}
		cfgLineFound = ETrue;
		break;
		}
	CleanupStack::PopAndDestroy(&cfgFile);
	if(err != KErrEof)
		{//The "read configuration file" operation has failed with "err" (if err != KErrNone)
		__SQLLEAVE_IF_ERROR2(err);	
		}
	if(!cfgLineFound)
		{//End of config file reached - no valid configuration line found.
		__SQLLEAVE2(KErrEof);	
		}
	__ASSERT_DEBUG(err == KErrNone || err == KErrEof, __SQLPANIC2(ESqlPanicInternalError));
	aConfigStr.Copy(buf);
	}

//Parses the config string parameter (aConfigStr) and stores the extracted configuration parameter values in the aConfigParams argument.
//The config string format is: "PARAM1=VALUE1;PARAM2=VALUE2;..."
//If there is unknown parameter name in the config string, it will be skipped - not reported as an error.
//The function will leave with KErrArgument in a case of a bad config string (bad parameter values).
void TSqlSrvConfig::ExtractConfigParamsFromStringL(const TDesC8& aConfigStr, TSqlSrvConfigParams& aConfigParams)
	{
	//Search iteratively the config string for "PARAM=VALUE;" pairs. If such pair is found, extract the parameter name and
	//parameter value. Adjust the string start ptr to point to the rest of the string.
	for(TPtrC8 ptr(aConfigStr);ptr.Length()>0;)
		{
		TPtrC8 prmName(KNullDesC8);
		TPtrC8 prmValue(KNullDesC8);
		if(TSqlSrvConfig::ExtractParamValuePairL(ptr, prmName, prmValue))
			{
			TSqlSrvConfig::ExtractParamValueL(prmName, prmValue, aConfigParams);
			}
		}
	//Assert the extracted parameter values.
	__ASSERT_DEBUG(aConfigParams.iPageSize == TSqlSrvConfigParams::KConfigPrmValueNotSet || aConfigParams.iPageSize >= 0, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iCacheSize == TSqlSrvConfigParams::KConfigPrmValueNotSet || aConfigParams.iCacheSize >= 0, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncNotSet || 
				aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncUtf8 || 
				aConfigParams.iDbEncoding == TSqlSrvConfigParams::EEncUtf16, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iSoftHeapLimitKb == TSqlSrvConfigParams::KConfigPrmValueNotSet || 
	            (aConfigParams.iSoftHeapLimitKb >= TSqlSrvConfigParams::KMinSoftHeapLimitKb &&
	             aConfigParams.iSoftHeapLimitKb <= TSqlSrvConfigParams::KMaxSoftHeapLimitKb), __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iCompactionMode == ESqlCompactionNotSet || aConfigParams.iCompactionMode == ESqlCompactionManual || 
				aConfigParams.iCompactionMode == ESqlCompactionBackground || aConfigParams.iCompactionMode == ESqlCompactionAuto, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aConfigParams.iFreePageThresholdKb == TSqlSrvConfigParams::KConfigPrmValueNotSet || 
			    aConfigParams.iFreePageThresholdKb >= 0, __SQLPANIC2(ESqlPanicInternalError));
	}

//The function searches aConfigStr arguments for "PARAM=VALUE;" pair. If such pair is found, then 
//aParamName is set to point to the parameter name, aParamValue is set to point to the parameter value,
//aConfigStr is set to point to the rest of the config string (skipping the just found "param=value;" pair).
//The function leaves with KErrArgument in case of a bad config string.
//The function returns false if a ";" string is found instead of a "param=value;" pair.
//When the function returns true, it means that aParamName and aParamValue arguments are set to point to the
//parameter name and parameter value.
TBool TSqlSrvConfig::ExtractParamValuePairL(TPtrC8& aConfigStr, TPtrC8& aParamName, TPtrC8& aParamValue)
	{
	const TChar KSemiColon(';');
	const TChar KAssignment('=');
	TInt pos = aConfigStr.Locate(KSemiColon);
	TPtrC8 prmText(KNullDesC8);
	if(pos < 0)
		{
		pos = aConfigStr.Length() - 1;	
		prmText.Set(TSqlSrvConfig::TrimAndConstructPtr(aConfigStr.Ptr(), aConfigStr.Length()));
		}
	else
		{
		prmText.Set(TSqlSrvConfig::TrimAndConstructPtr(aConfigStr.Ptr(), pos));
		}
	//Set aConfigStr to the "point" right after the last found ';'
	if(pos == aConfigStr.Length() - 1)
		{
		aConfigStr.Set(NULL, 0);	
		}
	else
		{
		aConfigStr.Set(aConfigStr.Ptr() + pos + 1, aConfigStr.Length() - (pos + 1));
		}
	if(prmText.Length() == 0)
		{//Empty ";"
		return EFalse;	
		}
	//Find the parameter name and parameter value
	pos = prmText.Locate(KAssignment);
	if(pos < 0 || pos >= (prmText.Length() - 1))
		{
		__SQLLEAVE2(KErrArgument);
		}
	//we've got now prmText pointing to a " PARAM = VALUE " string.
	aParamName.Set(TSqlSrvConfig::TrimAndConstructPtr(prmText.Ptr(), pos));
	aParamValue.Set(TSqlSrvConfig::TrimAndConstructPtr(prmText.Ptr() + pos + 1, prmText.Length() - (pos + 1)));
	return ETrue;
	}

//The function compares aParamName argument against a set of predefined parameter names and if one of them is matched,
//then the function converts aParamValue argument to a numerical value and assigns it to the corresponding aConfigParams data member.
//The function may leave with KErrArgument, if the parameter value is invalid.
void TSqlSrvConfig::ExtractParamValueL(const TDesC8& aParamName, const TDesC8& aParamValue, TSqlSrvConfigParams& aConfigParams)
	{
	if(aParamName.CompareF(KCacheSize) == 0)
		{
		aConfigParams.iCacheSize = TSqlSrvConfig::GetCacheSizeL(aParamValue);
		}
	else if(aParamName.CompareF(KPageSize) == 0)
		{
		aConfigParams.iPageSize = TSqlSrvConfig::GetPageSizeL(aParamValue);
		}
	else if(aParamName.CompareF(KEncoding) == 0)
		{
		aConfigParams.iDbEncoding = TSqlSrvConfig::GetEncoding(aParamValue);
		}
	else if(aParamName.CompareF(KSoftHeapLimitKb) == 0)
		{
		aConfigParams.iSoftHeapLimitKb = TSqlSrvConfig::GetSoftHeapLimitL(aParamValue);
		}
	else if(aParamName.CompareF(KCompactionMode) == 0)
		{
		aConfigParams.iCompactionMode = TSqlSrvConfig::GetCompactionModeL(aParamValue);
		}
	else if(aParamName.CompareF(KFreePageThresholdKb) == 0)
		{
		aConfigParams.iFreePageThresholdKb = TSqlSrvConfig::GetFreePageThresholdL(aParamValue);
		}
	//else
	//	{
	//	Unrecognized parameter/value pair - no problem, skip it.
	//	}
	}

//The function converts aParamValue to a numerical value (the cache size in pages) and returns it.
//If the converted numerical value is less than 0, the function leaves with KErrArgument.
TInt TSqlSrvConfig::GetCacheSizeL(const TDesC8& aParamValue)
	{
	TLex8 lex(aParamValue);
	TInt cacheSize = TSqlSrvConfigParams::KConfigPrmValueNotSet;
	TInt err = lex.Val(cacheSize);
	if(err != KErrNone || cacheSize < 0) 	//The correct check is for "<=0", but it has to be backward 
		{									//compatible with the previous implementation
		__SQLLEAVE2(KErrArgument);
		}
	return cacheSize;
	}

//The function converts aParamValue to a numerical value (the page size in bytes) and returns it.
//If the converted numerical value is less than 0, the function leaves with KErrArgument.
TInt TSqlSrvConfig::GetPageSizeL(const TDesC8& aParamValue)
	{
	TLex8 lex(aParamValue);
	TInt pageSize = TSqlSrvConfigParams::KConfigPrmValueNotSet;
	TInt err = lex.Val(pageSize);
	if(err != KErrNone || pageSize < 0) 	//The correct check is for "<0", "power of 2", "between 512 and 32768",
		{									//but it has to be backward compatible with the previous implementation
		__SQLLEAVE2(KErrArgument);
		}
	return pageSize;
	}

//The function converts aParamValue to a numerical value (the database encoding) and returns it.
TSqlSrvConfigParams::TDbEncoding TSqlSrvConfig::GetEncoding(const TDesC8& aParamValue)
	{
	TSqlSrvConfigParams::TDbEncoding encoding = TSqlSrvConfigParams::EEncNotSet;
	if(aParamValue.CompareF(KUTF8) == 0 || aParamValue.CompareF(KUTF8Q) == 0)
		{
		encoding = TSqlSrvConfigParams::EEncUtf8;
		}
	else if(aParamValue.CompareF(KUTF16) == 0 || aParamValue.CompareF(KUTF16Q) == 0)
		{
		encoding = TSqlSrvConfigParams::EEncUtf16;
		}
	//else
	//	{
	//	Invalid encoding - bypass it in order to be compatible with the previous implementation
	//	}
	return encoding;
	}

//The function converts aParamValue to a numerical value (the soft heap limit in Kb) and returns it.
TInt TSqlSrvConfig::GetSoftHeapLimitL(const TDesC8& aParamValue)
	{
	TLex8 lex(aParamValue);
	TInt softHeapLimitKb = TSqlSrvConfigParams::KConfigPrmValueNotSet;
	TInt err = lex.Val(softHeapLimitKb);
	if(err != KErrNone || softHeapLimitKb < 0 || 
	   (softHeapLimitKb < TSqlSrvConfigParams::KMinSoftHeapLimitKb || softHeapLimitKb > TSqlSrvConfigParams::KMaxSoftHeapLimitKb))
		{					
		__SQLLEAVE2(KErrArgument);
		}
	return softHeapLimitKb;
	}
	
//The function converts aParamValue to a numerical value (the database compaction mode) and returns it.
TSqlCompactionMode TSqlSrvConfig::GetCompactionModeL(const TDesC8& aParamValue)
	{
	TSqlCompactionMode compactionMode = ESqlCompactionNotSet;
	if(aParamValue.CompareF(KManual) == 0)
		{
		compactionMode = ESqlCompactionManual;
		}
	else if(aParamValue.CompareF(KBackground) == 0)
		{
		compactionMode = ESqlCompactionBackground;
		}
	else if(aParamValue.CompareF(KAuto) == 0 || aParamValue.CompareF(KSynchronous) == 0)
		{
		compactionMode = ESqlCompactionAuto;
		}
	//else
	//	{
	//	Invalid compaction mode
	//	}
	return compactionMode;
	}

//The function converts aParamValue to a numerical value (the free page threshold in pages) and returns it.
TInt TSqlSrvConfig::GetFreePageThresholdL(const TDesC8& aParamValue)
	{
	TLex8 lex(aParamValue);
	TInt freePageThreshold = 0;
	TInt err = lex.Val(freePageThreshold);
	if(err != KErrNone || freePageThreshold < 0)
		{					
		__SQLLEAVE2(KErrArgument);
		}
	return freePageThreshold;
	}
	

//The function searches aStr for leading and trailing whitespace 
//characters, then creates and returns TPtrC object which points to the 
//aStr content without leading and trailing whitespace characters.
TPtrC8 TSqlSrvConfig::TrimAndConstructPtr(const TUint8* aStr, TInt aLength)
	{
	__ASSERT_DEBUG(aStr != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aLength >= 0, __SQLPANIC2(ESqlPanicBadArgument));
	//Trim left
	for(;aLength>0;--aLength,++aStr)
		{
		if(!TChar(*aStr).IsSpace())
			{
			break;
			}
		}
	//Trim right
	for(const TUint8* p=aStr+aLength-1;aLength>0;--aLength,--p)
		{
		if(!TChar(*p).IsSpace())
			{
			break;
			}
		}
	return TPtrC8(aStr, aLength);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
CDbConfigFiles* CDbConfigFiles::NewL(const CDir& aDirEntries)
	{
	CDbConfigFiles* self = new(ELeave) CDbConfigFiles();
	CleanupStack::PushL(self);
	self->ConstructL(aDirEntries);	
	CleanupStack::Pop();
	return self;
	}

CDbConfigFiles::CDbConfigFiles()
	{
	}

void CDbConfigFiles::ConstructL(const CDir& aDirEntries)
	{	
	StoreFileNamesL(aDirEntries);
	}
	
CDbConfigFiles::~CDbConfigFiles()
	{
	iConfigFileNames.ResetAndDestroy(); 
	}
	
//Stores the names of the given database configuration files.
//These files were found in the server's private data cage on 
//the Z: drive and begin with the prefix 'cfg'
void CDbConfigFiles::StoreFileNamesL(const CDir& aDirEntries)
	{
	//Store the file names in reverse alphabetical order so that
	//in FindConfigFile() if there is more than one version of the same
	//config file (which there shouldn't be) then the highest version
	//will be returned
	for(TInt i = aDirEntries.Count() - 1; i >= 0; --i)
		{
		const TEntry& entry = aDirEntries[i];
		if(!entry.IsDir())
			{
			HBufC* filename = entry.iName.AllocLC();
			iConfigFileNames.AppendL(filename);
			CleanupStack::Pop(); // filename
			}
		}
	}
	
//Finds the configuration file corresponding to the given database, if one exists.
//The database filename including the extension is passed as a parameter - for example,
//[12345678]a.db. Note that if more than one version of a configuration file exists 
//for the given database (which shouldn't happen) - for example, cfg[12345678]a.db.01 
//and cfg[12345678]a.db.02 - then the highest version will be returned 
HBufC* CDbConfigFiles::FindConfigFile(const TDesC& aDbFilename) const
	{	
	TInt count = iConfigFileNames.Count();
	for(TInt i = 0; i < count; ++i)
		{
		TInt offset = iConfigFileNames[i]->Des().FindF(aDbFilename);
		if(KErrNotFound != offset)
			{
			return iConfigFileNames[i];
			}
		}
	return NULL;
	}
