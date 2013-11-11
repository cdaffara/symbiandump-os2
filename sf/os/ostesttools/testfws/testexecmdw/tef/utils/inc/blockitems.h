/*
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
* @file
* This contains BlockItems.h
*
*/



#ifndef __BLOCK_ITEMS_H__
#define __BLOCK_ITEMS_H__

#include <e32base.h>

const TInt	KTEFMaxNameLength		= 255;
typedef	TBuf<KTEFMaxNameLength>		TTEFSectionName;
typedef	TBuf<KTEFMaxNameLength>		TTEFFunction;
typedef	TBuf<KTEFMaxNameLength>		TTEFObjectType;

enum TTEFBlockItemType
/**
 * @internalComponent
 * @test
 *
 * Types of block item
 *
 * ETEFNull
 * - undefined type
 *
 * ETEFCreateObject
 * - create an object
 *
 * ETEFRestoreObject
 * - restore an object from the shared area
 *
 * ETEFCommand
 * - execute a command on an object
 *
 * ETEFStore
 * - store the object in shared area
 *
 * ETEFOutstanding
 * - wait for all outstandign requests to complete
 *
 * ETEFDelay
 * - delay all processing
 *
 * ETEFAsyncDelay
 * - delay but keep asynchronus operations running during delay.
 *
 * ETEFSharedActiveScheduler
 * - use active schedular from shared data
 *
 * ETEFStoreActiveScheduler
 * - store active schedular in shared data
 */
	{
	ETEFNull		 = 0,
	ETEFCreateObject,
	ETEFRestoreObject,
	ETEFCommand,
	ETEFStore,
	ETEFOutstanding,
	ETEFDelay,
	ETEFAsyncDelay,
	ETEFSharedActiveScheduler,
	ETEFStoreActiveScheduler
	};

class TTEFCommand
/**
 * @internalComponent
 * @test
 */
	{
public:
	TTEFSectionName		iObject;
	TTEFFunction		iFunction;
	};

class TTEFBlockItem
/**
 * @internalComponent
 * @test
 */
	{
public:
	TTEFBlockItem()
	:	iItemType(ETEFNull)
	,	iExpectedError(0)
	,	iExpectedAsyncError(0)
	,	iError(0)
	,	iAsyncError(0)
	,	iTime(0)
	,	iExecuted(EFalse) {}

	TTEFBlockItemType	iItemType;
	TTEFObjectType		iObjectType;
	TTEFCommand			iCommand;
	TTEFSectionName		iSection;
	TInt				iExpectedError;
	TInt				iExpectedAsyncError;
	TInt				iError;
	TInt				iAsyncError;
	TInt				iTime;
	TBool				iExecuted;
	};

/**
 * @internalComponent
 * @test
 */
typedef	CArrayFixFlat<TTEFBlockItem>	TTEFItemArray;

/**
 * @internalComponent
 * @test
 */
typedef	TPckgBuf<TTEFBlockItem>			TTEFItemPkgBuf;

class MSharedData
/**
 * @internalComponent
 * @test
 *
 * Access to the shared data
 */
	{
public:
	/*
	 * Create and active scheduler in the shared data
	 *
	 * @leave	system wide error
	 */
	virtual void	CreateActiveSchedulerL() =0;

	/*
	 * Delete the active scheduler in the shared data
	 *
	 * @leave	system wide error
	 */
	virtual void	DeleteActiveSchedulerL() =0;

	/*
	 * Get an object in the shared data and own it.
	 * As owner you will be resonsible for it's deletion.
	 *
	 * @param	aName - name of object to own
	 *
	 * @return	the obeject
	 *
	 * @leave	system wide error
	 */
	virtual TAny*	GetObjectAndOwnL(const TDesC& aName) =0;

	/*
	 * Put an object in the shared data and disown it.
	 * The shared data becomes will be resonsible for it's deletion.
	 *
	 * @param	aName - name of object to own
	 * @param	aAny - the object
	 * @param	aCleanupOperation - Operation to delete the object if still in shared data on exit
	 *
	 * @leave	system wide error
	 */
	virtual void	PutAndDisownL(const TDesC& aName, TAny* aAny, TCleanupOperation aCleanupOperation) =0;
	};

#endif // __BLOCK_ITEMS_H__
