/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYENGINEUTILS_H
#define MEMSPYENGINEUTILS_H

// Configuration defines
#define __INCLUDE_CAPABILITY_NAMES__
#define __INCLUDE_ALL_SUPPORTED_CAPABILITIES__

// System includes
#include <e32base.h>

// User includes
#include <memspy/engine/memspyenginelogger.h>

// Type definitions
typedef TBuf<20> TMemSpySizeText;
typedef TBuf<30> TMemSpyPercentText;
typedef TBuf<60> TMemSpyTimeText;

// Classes referenced
class CMemSpyEngineSinkMetaData;

// Literal constants
_LIT( KMemSpyLogDefaultExtension, ".txt" );

// Enumerations
enum TMemSpyEnginePanic
    {
    EMemSpyEnginePanicInvalidContainer1 = 0,
    EMemSpyEnginePanicInvalidContainer2 = 1,
    EMemSpyEnginePanicSuspendRequest1 = 2,
    EMemSpyEnginePanicSuspendRequest2 = 3,
    EMemSpyEnginePanicProcessHandleNullWhenAttemptingToIdentifyThreads = 4,
    EMemSpyEnginePanicEncounteredKernelUnexpectedly = 5,
    EMemSpyEnginePanicCyclesHaveGoneCyclic = 6,
    EMemSpyEnginePanicTrackerNull1 = 7,
    EMemSpyEnginePanicTrackerNull2 = 8,
    EMemSpyEnginePanicTrackerNull3 = 9,
    EMemSpyEnginePanicTrackerNull4 = 10,
    EMemSpyEnginePanicSinkFileEntireFileBufferNull = 11,
    EMemSpyEnginePanicSinkFileWorkingFileBufferIsNull = 11,
    };


class MemSpyEngineUtils
    {
public:
    IMPORT_C static void FormatTimeL( TDes& aBuf, const TInt64& aTimeVal, TBool aLocalTime = ETrue );
    IMPORT_C static void FormatTimeL( TDes& aBuf, const TTime& aTime, TBool aLocalTime = ETrue );
    IMPORT_C static void FormatHex( TDes& aBuf, TInt aValue );
    IMPORT_C static TMemSpySizeText FormatSizeText( const TInt64& aValue, TInt aDecimalPlaces = 1, TBool aExtraRounding = EFalse );
    IMPORT_C static TMemSpySizeText FormatSizeText( TInt aValue );
    IMPORT_C static TMemSpySizeText FormatSizeTextPrecise( TInt aValue );
    IMPORT_C static TBool IsRomAddress( TAny* aAddress );
    IMPORT_C static void GetCapabilityName( TDes& aBuf, TCapability aCapability );
    IMPORT_C static TBool StripText( TDes& aText, const TDesC& aStrip );
    IMPORT_C static void TextBeforeDoubleColon( TDes& aText );
    IMPORT_C static void TextAfterDoubleColon( TDes& aText );
    IMPORT_C static TMemSpyPercentText FormatPercentage( TReal aOneHundredPercentValue, TReal aValue );
    IMPORT_C static HBufC* CleanupTextLC( const TDesC& aText );
    IMPORT_C static void GetRomInfoL( TDes& aPlatform, TDes& aChecksum );
    IMPORT_C static void GetFolderL( RFs& aFsSession, TDes& aFolder, const CMemSpyEngineSinkMetaData& aMetaData, const TDriveNumber* aForceDrive = NULL );

public: // Not exported
    static TDriveNumber LocateSuitableDrive( RFs& aFsSession );
    static void FormatTimeSimple( TDes& aBuf, const TTime& aTime );
    static void FormatTimeNowL( TDes& aBuf, TBool aLocalTime = ETrue );
    static HBufC* DataStreamFolderNameWithTimeStampLC( const TDesC& aFolderName );
    static void GetKernelHeapThreadName( TDes& aBuf, TBool aFullName = ETrue );
    static void GetKernelHeapThreadAndProcessNames( TDes& aThreadName, TDes& aProcessName );
    static TUint32 Hash( const TDesC& aText );
    static HBufC* CleanContextInfoLC( const TDesC& aContext );
    static TProcessId IdentifyFileServerProcessIdL();
    static TPtrC TextAfterLastDoubleColon( const TDesC& aText );

public: // Panic utility
    static void Panic( TMemSpyEnginePanic aPanic );
    };



// Utility functions & classes
template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef);

private:
    static void ResetAndDestroy(TAny *aPtr);
    };

template <class T>
inline void CleanupResetAndDestroyL(T& aRef);

// Template class CleanupClose
template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef) { CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) ); }
template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr) { static_cast< T* >( aPtr )->ResetAndDestroy(); }
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef) { CleanupResetAndDestroy<T>::PushL(aRef); }

// Overflow utility class
class TMemSpyTruncateOverflow : public TDesOverflow
	{
public:
	virtual void Overflow( TDes& ) { }
	};

#endif
