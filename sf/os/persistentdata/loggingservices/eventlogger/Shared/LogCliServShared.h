// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGCLISERVSHARED_H__
#define __LOGCLISERVSHARED_H__

// System includes
#include <e32std.h>

// User includes
#include <logwrap.h>

/**
Type definitions
@internalComponent
*/
typedef TUint32 TLogOperationId;

/**
Constants
@internalComponent
*/
const TLogOperationId KLogNullOperationId = 0;
const TLogViewId KLogNullViewId = 0;
const TInt KLogMaxDateLength = 32;

/**
Enumerations
@internalComponent
*/
enum TLogServFunction
	{	
	ELogNone								=  0, 
	//
	ELogMakeTransient						=  1, // Debug
	ELogSetHeapFail							=  2, // Debug
	//
	ELogOperationCancel						=  3,
	ELogOperationGetResult					=  4,
	ELogOperationInitiate					=  5,
	//
	ELogNotify								=  6,
	ELogNotifyCancel						=  7,
	//
	ELogViewCreate							=  8,
	ELogViewDelete							=  9,
	ELogNOTUSED								= 10,
	ELogViewCount							= 11,
	ELogViewOperationInitiate				= 13,
	ELogViewChangeNotificationsRequest		= 14,
	ELogViewChangeNotificationsCancel		= 15,
	ELogViewFetchChanges					= 16,
	ELogViewNotifyLockStatusChange			= 17,
	ELogViewNotifyLockStatusChangeCancel	= 18,
	//
	ELogNotifyExtended						= 30,
	ELogNotifyExtendedCancel				= 31,
	//
	ELogIsServerReady						= 99, // Debug
	//
	ELogFunctionLast
	};

/**
@internalComponent
*/
enum TLogOperationType
	{
	ELogOperationEventAdd = 0, 
	ELogOperationEventGet,
	ELogOperationEventChange,
	ELogOperationEventDelete,
	//
	ELogOperationTypeAdd, 
	ELogOperationTypeGet,
	ELogOperationTypeChange,
	ELogOperationTypeDelete,
	//
	ELogOperationClearLog, 
	ELogOperationClearRecent, 
	//
	ELogOperationConfigGet, 
	ELogOperationConfigChange,
	//
	ELogOperationMaintain,
	//
	ELogOperationViewSetup,
	ELogOperationViewRemoveEvent,
	ELogOperationViewClearDuplicates,
	ELogOperationViewSetFlags,
	ELogOperationViewWindowFetch
	};

/**
@internalComponent
*/
enum TLogViewType
	{
	ELogViewTypeEvent = 0,
	ELogViewTypeRecent,
	ELogViewTypeDuplicate
	};

/**
@internalComponent
*/
enum TLogNavigation
	{
	ELogNavigateForwards = 0,
	ELogNavigateBackwards,
	ELogNavigateFirst,
	ELogNavigateLast
	};

/**
@internalComponent
*/
enum TLogViewLockStatus
	{
	ELogViewWindowOpen = 0,
	ELogViewWindowLocked
	};

/**
@internalComponent
*/
enum TLogFilterConstructionType
	{
	ELogFilterConstructFilterByFilterFieldByField = 0,
	ELogFilterConstructFieldByFieldFilterByFilter
	};


/**
@internalComponent
*/
class TLogClientServerData
	{
public:

	/**
	 * The type of operation
	 */
	TLogOperationType iOperationType;

	/** 
	 * The operation id
	 */
	TLogOperationId iOperationId;

	/**
	 * Other spare transfer data slots
	 */
	TInt iDataSlot1;
	TInt iDataSlot2;
	};

/**
@internalComponent
*/
class TLogWindow
	{
public:
	IMPORT_C TLogWindow();

public:
	IMPORT_C TBool Contains(TInt aPosition) const;
	IMPORT_C TInt Range() const;
	IMPORT_C TInt WindowIndexFromCursorPosition(TInt aCursorPosition) const;
	IMPORT_C void Reset();
	void Normalize();

public:
	TInt iLower;
	TInt iUpper;
	};

/**
@internalComponent
*/
class TLogWindowAndCursor : public TLogWindow
	{
public:
	enum TAffected
		{
		EWindowUnaffected = 0,
		EWindowAffected
		};

public:
	TLogWindowAndCursor();
	TLogWindowAndCursor(const TLogWindow& aWindow, TInt aCursorPosition);

public:
	TAffected AdjustForItemDeletion(TInt aItemIndex);
	TAffected AdjustForItemAddition(TInt aItemIndex);
	TInt WindowIndexFromCursorPosition() const;
	void Reset();
	void NormalizeWindowAndCursor();

public:
	TInt iCursorPosition;
	TBool iValid;
	};

/**
@internalComponent
*/
class TLogTransferWindow : public TLogWindow
	{
public:
	IMPORT_C TLogTransferWindow();
	IMPORT_C void Reset();

public:
	TInt iBufferSize;
	TInt iServerDataSize;//if iBufferSize is less than the length of the requested data from
						 //the server, the server will set iServerDataSize with the minimal
						 //size which the client side buffer should have.
	};

//**********************************
// LogUtils
//**********************************

/** 
Provide log engine system utilities.

@internalComponent
*/
class LogUtils
	{
public:
	IMPORT_C static const TDesC& DateFormatForLocale();
	};



#endif
