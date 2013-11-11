// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLITESYMBIAN_H__
#define __SQLITESYMBIAN_H__

#include <e32std.h>

//Forward declarations
class RFs;
typedef struct sqlite3 sqlite3;

TInt sqlite3SymbianLastOsError(void);
TInt sqlite3SymbianLibInit(void);
void sqlite3SymbianLibFinalize(void);
RFs& sqlite3SymbianFs(void);

TInt sqlite3SymbianProfilerStart(TInt aCounterType);
TInt sqlite3SymbianProfilerStop(TInt aCounterType);
TInt sqlite3SymbianProfilerReset(TInt aCounterType);
TInt sqlite3SymbianProfilerQuery(TInt aCounterType, TDes8& aResult);

/**
The operation code for registering a free page callback.

@see TSqlFreePageCallback

@internalComponent
*/
const TInt KSqlFcntlRegisterFreePageCallback = 1000001;

/**
This structure is used for sending a request (via TVfs::FileControl()) to the OS porting layer to
register a free page callback - TSqlFreePageCallback::iCallback, which will be called when the number of 
the free pages reaches or is above the threshold - TSqlFreePageCallback::iThreshold.

@see TSqlFreePageCallback

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlFreePageCallback)
	{
	/**
	"Free page" callback type definition.
	*/
	typedef void (*TCallback)(void* aThis, TInt aFreePageCount);
	
	inline TSqlFreePageCallback();
	inline TSqlFreePageCallback(void* aThis, TInt aFreePageThreshold, TCallback aCallback);
	inline TBool IsValid() const;
	inline void CheckAndCallback(TInt aFreePageCount) const;

	void*		iThis;		//"this" pointer of the object that has to be called back
	TInt		iThreshold;	//"free page threshold" in pages
	TCallback	iCallback;	//a pointer to the callback function

	};

/**
Initalizes TSqlFreePageCallback data members with their default values.
*/
inline TSqlFreePageCallback::TSqlFreePageCallback() :
	iThis(NULL),
	iThreshold(0),
	iCallback(NULL)
	{
	}
	
/**
Initalizes TSqlFreePageCallback data members with the supplied callback values.

@param aThis		"this" pointer of the object that has to be called back,
@param aThreshold 	"free page threshold" in pages,
@param aCallback	a pointer to the callback function;
*/
inline TSqlFreePageCallback::TSqlFreePageCallback(void* aThis, TInt aFreePageThreshold, TSqlFreePageCallback::TCallback aCallback) :
	iThis(aThis),
	iThreshold(aFreePageThreshold),
	iCallback(aCallback)
	{
	}

/**
@return True if this is a valid (initialized) callback object.
*/
inline TBool TSqlFreePageCallback::IsValid() const
	{
	return 	iThis && iThreshold >= 0 && iCallback;
	}

/**
Calls the callback if aFreePageCount value is above the threshold.

@param aFreePageCount Database free pages count
*/	
inline void TSqlFreePageCallback::CheckAndCallback(TInt aFreePageCount) const
	{
	if(aFreePageCount > 0 && aFreePageCount >= iThreshold)
		{
		(*iCallback)(iThis, aFreePageCount);
		}
	}

#endif//__SQLITESYMBIAN_H__
