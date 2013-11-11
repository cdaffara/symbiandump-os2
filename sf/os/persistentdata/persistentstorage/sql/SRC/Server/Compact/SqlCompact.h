/**
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* The main header file of the background compaction framework.
* Includes the declarations of the following classes:
* The CSqlCompactor is the main class controlling the background compaction.
* Only one instance of that class should be created by the server.
* The CSqlCompactor single instance manages a set of reference counted per-database entries and a CTimer compaction object.
* The MSqlCompactConn interface acts as an abstraction layer between the compaction entries and the 
* real object with member functions that are used to perform the compaction.
* 
*
*/



/**
 @file
 @see MSqlCompactConn
 @see CSqlCompactor
*/
#ifndef SQLCOMPACT_H
#define SQLCOMPACT_H

#include <e32base.h>

//Forward declarations
class CSqlCompactEntry;
class CSqlCompactTimer;
struct TSqlFreePageCallback;
//A test class that has a direct access to all data members and functions of the background compaction framework classes
class CSqlCompactTestActive;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////          TSqlCompactSettings structure declaration         //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define SQLCOMPACTSETTINGS_INVARIANT() Invariant()
#else
#define SQLCOMPACTSETTINGS_INVARIANT() void(0)
#endif

/**
Per-database background compaction settings/thresholds.
They are:
@code
 - The compaction step length in milliseconds;
 - The free pages threshold - the background compaction should be kicked-off if the free space in the free pages is 
 							  above this threshold (in Kb);
@endcode

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlCompactSettings)
	{
	TSqlCompactSettings();
	void Invariant() const;
	
	TInt 	iStepLength;
	TInt	iFreePageThresholdKb;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////          MSqlCompactConn interface declaration         //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The background compaction connection interface.
The interface separates the CSqlCompactor entries from the object which does the compaction.
The MSqlCompactConn interface exposes minimal set of methods needed to perform a compaction:
 - MSqlCompactConn::Compact(TInt aPageCount, TInt& aProcessedPageCount);

As the interface name suffix ("Conn") suggests, the interface creates and maintains a 
connection with the database to be compacted.

Interface creation:
 The TSqlCompactConnFactoryL() factory function should be used for that.

Interface destruction:
 The MSqlCompactConn offers a Release() function for that.

@see CSqlCompactor
@see TSqlCompactConnFactoryL

@internalComponent
*/
NONSHARABLE_CLASS(MSqlCompactConn)
	{
	friend class CSqlCompactTestActive;
	
public:
	virtual void Release() = 0;
	virtual TInt Compact(TInt aPageCount, TInt& aProcessedPageCount, TInt aLength) = 0;
	
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////          MSqlCompactConn factory function type declaration         /////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
MSqlCompactConn interface - factory function type definition.

@param aFullName The full name of the database to be compacted (including the path).
@param aFreePageCallback Input/Output parameter. Object containing the free page callback parameters.
						 aFreePageCallback.iThreshold must be set to be in Kb. 	
						 If the function call completes successfully and the free pages space is above the threshold,
						 the aFreePageCallback.iThreshold will be set to contain the free pages count.
						 Otherwise aFreePageCallback.iThreshold will be initialized with zero.

@return A pointer to the created MSqlCompactConn interface.

@leave KErrNoMemory, an out of memory condition has occurred,
	   KErrArgument, invalid data in the aFreePageCallback object;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@see MSqlCompactConn

@internalComponent
*/
typedef MSqlCompactConn* (*TSqlCompactConnFactoryL)(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////          CSqlCompactor class declaration         ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define SQLCOMPACTOR_INVARIANT() Invariant()
#else
#define SQLCOMPACTOR_INVARIANT() void(0)
#endif

/**
The main class of the background compaction framework, that acts as a container of reference counted 
per-database entries.
Only one instance of this class should be created by the SQL server.
Using the CSqlCompactor instance:
 - A new background compaction entry can be added to the container - the CSqlCompactor::AddEntryL() method.
   If an entry with the same name does exist, no new entry is created, the reference counter of the existing one 
   is incremented;
 - An existing background compaction entry can be removed from the container with CSqlCompactor::ReleaseEntry().
   The entry is reference-counted and when the reference counter reaches 0, the entry will be removed from the container;

@see MSqlCompactConn

@internalComponent
*/
NONSHARABLE_CLASS(CSqlCompactor) : public CBase
	{
	friend class CSqlCompactTestActive;
	
public:
	static CSqlCompactor* NewL(TSqlCompactConnFactoryL aConnFactoryL, TInt aCompactStepInterval);
	virtual ~CSqlCompactor();
	void RestartTimer();
	void AddEntryL(const TDesC& aFullName, const TSqlCompactSettings& aSettings);
	void ReleaseEntry(const TDesC& aFullName);
	void Invariant() const;

private:
	CSqlCompactor(TSqlCompactConnFactoryL aConnFactoryL);
	void ConstructL(TInt aCompactStepInterval);
	static TInt Search(const TDesC* aFullName, const CSqlCompactEntry& aEntry);
	static TInt Compare(const CSqlCompactEntry& aLeft, const CSqlCompactEntry& aRight);

private:
	TSqlCompactConnFactoryL iConnFactoryL;
	CSqlCompactTimer*		iTimer;
	typedef RPointerArray<CSqlCompactEntry> RCompactEntryArray;
	RCompactEntryArray		iEntries;

	};
	
#endif//SQLCOMPACT_H
