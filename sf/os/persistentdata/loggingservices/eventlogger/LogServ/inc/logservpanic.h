// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGSERVPANIC_H__
#define __LOGSERVPANIC_H__

// System includes
#include <e32base.h>

/**
Enumerations
@internalComponent
*/
enum TLogServPanic
	{
	ELogAlreadyActive1,			// 0
	ELogAlreadyActive2,
	ELogAlreadyActive3,
	ELogAlreadyActive4,
	ELogAlreadyActive5,
	ELogAlreadyActive6,
	ELogAlreadyActive7,
	ELogAlreadyActive8,
	ELogAlreadyActive9,
	ELogAlreadyActive10,
	ELogAlreadyActive11,		// 10
	ELogAlreadyActive12,
	ELogAlreadyActive13,
	ELogAlreadyActive14,
	ELogAlreadyActive15,
	ELogAlreadyActive16,
	ELogAlreadyActive17,
	ELogAlreadyActive18,
	ELogAlreadyActive19,
	ELogAlreadyActive20,
	ELogNoSuchColumn,			// 20
	ELogQueryEvaluated,
	ELogNoSuchState1,
	ELogNoSuchState2,
	ELogNoSuchState3,
	ELogNoSuchState4,
	ELogNoSuchState5,
	ELogNoSuchState6,
	ELogNoSuchState7,
	ELogTooManyRows1,
	ELogTooManyRows2,			// 30
	ELogTooManyRows3,
	ELogTooManyRows4,
	ELogTooManyRows5,
	ELogTooManyRows6,
	ELogNoFirstRow1,
	ELogNoFirstRow2,
	ELogStringTooBig1,
	ELogStringTooBig2,
	ELogBadState1,
	ELogBadState2,				// 40
	ELogBadState3,
	ELogBadState4,
	ELogBadState5,
	ELogBadState6,
	ELogBadState7,
	ELogDesEmpty1,
	ELogDesEmpty2,
	ELogDesEmpty3,
	ELogDesEmpty4,
	ELogNotValid,				// 50
	ELogDatabaseDamaged1,
	ELogDatabaseDamaged2,
	ELogDatabaseDamaged3,
	ELogDatabaseDamaged4,
	ELogRecoverError,
	ELogUnknownField,
	ELogNullRecentList,
	ELogNullFilterInList1,
	ELogNullFilterInList2,
	ELogNullFilterInList3,		// 60
	ELogNullFilterInList4,
	ELogDatabaseAlreadyOpen,
	ELogBadDescriptor,
	ELogIllegalFunction,
	ELogStartBackupFailure,
	ELogInvalidRecentView,
	ELogInvalidQuery,
	ELogNotifyListsNotEmpty,
	ELogBeginInTransaction,
	ELogCommitNotInTransaction,	// 70
	ELogRollbackNotInTransaction,
	ELogNothingHasChanged,
	ELogEventAlreadyInView,
	ELogServFactoryUnrecognizedOperationType,
	ELogServFactoryUnrecognizedOperationType2,
	ELogServFactoryUnrecognizedViewType,
	ELogNoOutstandingAsyncRequest,
	ELogBackupObserversStillRegistered,
	ELogNoResourceForId,
	ELogNoChangeInterfacePointer, // 80
	ELogUnrecognizedChangeType,
	ELogUnrecognizedChangeType2,
	ELogInappropriateChangeType,
	ELogViewChangeRequestAlreadyIssued,
	ELogViewNoClientChangeMessageOutstanding,
	ELogViewBadClientSideChangeBufferSize,
	ELogInvalidRecentView2,
	ELogCacheAccessDuringBackupStrings,
	ELogCacheAccessDuringBackupTypes,
	ELogCacheAccessDuringBackupConfig, // 90
	ELogTypeEntryCacheDescriptionMemoryError,
	ELogCacheConfigAlreadyInTransaction,
	ELogCacheConfigNotInTransaction,
	ELogChangeConfigLogicError,
	ELogBackupManagerMultipleObservationRequestsFromObject,
	ELogViewNotSetupForChangesFetch,
	ELogViewRecentViewNotYetReadyForFlagSetting,
	ELogViewBadWindow,
	ELogViewNoLockStatusChangeMessage,
	ELogViewLockStatusChangeRequestAlreadyIssued, // 100
	ELogViewWindowFetcherBadState,
	ELogViewWindowFetcherBadState2,
	ELogBadFilterConstructionType,
	ELogViewNoPendingChangesToDeliver,
	ELogExtendedMessageAlreadyActive,
	ELogViewWindowFetcherBadState3,
	ELogSecurityCapabilitiesUndefined,
	ELogTooManyCapabilities,
	ELogUnknownCapability,
	ELogNoEventTypeAtId,							// 110
	ELogMismatchedId,
	ELogQueryNullColSet,
	ELogStringsCacheReserved,
	ELogStringsCacheNullArg1,
	ELogStringsCacheNullArg2,
	ELogTypesCacheReserved,
	ELogTypesCacheNotInTransaction,
	ELogInvalidStringColNo,
	ELogInvalidTypeColNo,
	ELogTypeCacheNullArg,							//120
    ELogInvalidConfigColNo,
    ELogInvalidConstructionType,
    ELogArrayReserved
	};

const TInt KLogPanicLeave = -1001;

/**
Utility functions
@internalComponent
*/
void Panic(TLogServPanic aPanic);
void PanicClientL(const RMessage2& aMessage, TLogServPanic aPanic);
void PanicClient(const RMessage2& aMessage, TLogServPanic aPanic);

#ifdef LOGGING_ENABLED

#include <f32file.h>
#include <flogger.h>

/**
@internalComponent
*/
_LIT(KLogFileName, "LogEng.txt");
_LIT(KLogFolder,   "LogEng");
_LIT(KTimeFormat, "%02d.%02d:%02d:%06d    ");
_LIT(KTextFormat, "%S");

class Log : public RFileLogger
/**
@internalComponent
*/
	{
public:
	static void New();
	static void Write(const TDesC& aText);
	static void WriteFormat(TRefByValue<const TDesC> aFmt, ...);

private:
	static void PruneLogFile();
    };

/**
@internalComponent
*/
#define LOGNEW							Log::New()
#define LOGTEXT(AAA)					{ _LIT(KString, AAA); Log::Write(KString); }
#define LOGTEXT2(AAA, BBB)				{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB); }
#define LOGTEXT3(AAA, BBB, CCC)			{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB, CCC); }
#define LOGTEXT4(AAA, BBB, CCC, DDD)	{ _LIT(KString, AAA); Log::WriteFormat(TRefByValue<const TDesC>(KString()), BBB, CCC, DDD); }

#else

#define LOGNEW
#define LOGTEXT(AAA)
#define LOGTEXT2(AAA, BBB)
#define LOGTEXT3(AAA, BBB, CCC)
#define LOGTEXT4(AAA, BBB, CCC, DDD)

#endif


#endif
