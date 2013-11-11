/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file RFileLogger.h
*/

#if !(defined __ET_RFILELOGGER_H__)
#define __ET_RFILELOGGER_H__

#include <e32base.h>

const TInt KMaxSizOfTag=128;
const TInt KMaxSizOfString = 256;
const TInt KMaxFilename = 50;

// *** Maintaince warning: the constant designed
// used on both server side and client side. Any change shoud be
// checked on both side
_LIT(KMessageFormat," - %d - %S - %d - ");

_LIT(KSeperation," - ");
_LIT8(KSeperation8," - ");
_LIT(KTagSeperation,"LogFieldsRequiredBeingAddedToAboveLogMessage");
_LIT(KTagSeperationEnd,"\t\t\t\t\t\t"); // make them invisible
_LIT(KTagSeperation8,"LogFieldsRequiredBeingAddedToAboveLogMessage");
_LIT(KTagSeperationEnd8,"\t\t\t\t\t\t"); // make them invisible
// *** End of Maintaince warning


//#if !(defined __FILELOGGER_UREL)
_LIT(KFileLogrerServerName,"RFileLoggerServer");
const TInt KMaxLoggerFilePath = 256;
const TInt KMaxLoggerLineLength = 512;

const TInt KRFileLoggerMajorVersion = 1;
const TInt KRFileLoggerMinorVersion = 1;
const TInt KRFileLoggerBuildVersion = 1;

struct TExtraLogField
	{
	TBuf<KMaxSizOfTag> iLogFieldName;
	TBuf<KMaxSizOfString> iLogFieldValue;
	};
/**
 * RFileLoggerBody - class to provide internal data for the client side
 *
 * This class contains all data members which would otherwise be in the
 * RFileLogger class. They are instead in this file since that because
 * CreateSession is a protected member of RSessionBase, so we must derive
 *from it and provide a means to call this via pass-through inline functions.
 */
class RFileLoggerBody : public RSessionBase
{
public:
	inline TInt DoCreateSession(const TDesC& aServer,const TVersion& aVersion,TInt aAsyncMessageSlots);
	inline TInt DoSendReceive(TInt aFunction,const TIpcArgs& aArgs) const;
	inline TInt DoSendReceive(TInt aFunction) const;

	
};

inline TInt RFileLoggerBody::DoCreateSession(const TDesC& aServer,const TVersion& aVersion,TInt aAsyncMessageSlots)
	{
	return CreateSession(aServer,aVersion,aAsyncMessageSlots);
	}

inline TInt RFileLoggerBody::DoSendReceive(TInt aFunction,const TIpcArgs& aArgs) const
	{
	return SendReceive(aFunction,aArgs);
	}

inline TInt RFileLoggerBody::DoSendReceive(TInt aFunction) const
	{
	return SendReceive(aFunction);
	}

class RFileFlogger
/**
@internalComponent
@test
*/
	{
public:
	enum TLogMode{ELogModeAppend,ELogModeOverWrite};
	// Logging level
	enum TLogSeverity{ESevrErr  = 1,ESevrHigh, ESevrWarn, ESevrMedium, ESevrInfo, ESevrLow, ESevrTEFUnit, ESevrAll};
	enum TLogCommand{ECreateLog,EWriteLog};

	enum TLogType{EXml,ETxt};
	
	IMPORT_C RFileFlogger();
	IMPORT_C ~RFileFlogger();
	

	IMPORT_C TInt Connect(); 
	IMPORT_C TInt CreateLog(const TDesC& aLogFilePath,TLogMode aMode);
	IMPORT_C void Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity, TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity, TInt arraylength, TExtraLogField* aLogFields, TRefByValue<const TDesC> aFmt,...);

	IMPORT_C void SetLogLevel(TLogSeverity aloglevel); 
	IMPORT_C void Close();
	IMPORT_C void Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity,TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C void Log(const TText8* aFile, TInt aLine, TLogSeverity aSeverity, TInt arraylength, TExtraLogField* aLogFields, TRefByValue<const TDesC> aFmt, VA_LIST aList);	

private:
	void GetCPPModuleName(TDes& aModuleName, const TText8* aCPPFileName);
	void WriteL(const TDesC& aLogBuffer);
	void WriteL(TDes8& aLogBuffer);
	void AddTime(TDes8& aTime);

	RFileLoggerBody* ilogbody;

	TLogSeverity iloglevel;
	TBool iLogfileTag;


	};


#endif
