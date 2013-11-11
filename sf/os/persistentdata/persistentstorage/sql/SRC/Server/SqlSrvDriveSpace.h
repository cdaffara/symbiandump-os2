// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reserving/Accessing/Releasing drive space - CSqlDriveSpace and RSqlDriveSpaceCol classes declarations
// 
//

#ifndef __SQLSRVDRIVESPACE_H__
#define __SQLSRVDRIVESPACE_H__

#include <f32file.h>

//Forward declarations
class RSqlDriveSpaceCol;
class CSqlServer;

#ifdef _DEBUG
#define SQLDRIVESPACE_INVARIANT() Invariant()
#define SQLDRIVESPACECOL_INVARIANT() Invariant()
#else
#define SQLDRIVESPACE_INVARIANT() void(0)
#define SQLDRIVESPACECOL_INVARIANT() void(0)
#endif

/**
This class describes an object, which is responsible for handling
"reserve/get access/release" requests for a particular drive.

Since the drive may be shared between more than one server side session and there is only one RFs instance,
the current solution is that CSqlDriveSpace objects reserve some predefined amount of disk space
at the time of their creation and then the access to the reserved disk space is reference counted.
    There is one obvious disadvantage of this solution: if a bad application "forgets" to release
the access to the reserved disk space, then the reserved (but not released) disk space may be used by the server.
At the moment, when some client will really need the reserved disk space to complete its "delete" transaction, 
it may happen that there is no reserved disk space at all. 

@see RSqlDriveSpaceCol
@internalComponent
*/
NONSHARABLE_CLASS(CSqlDriveSpace) : public CBase
	{
public:
	static CSqlDriveSpace* NewLC(RFs& aFs, TDriveNumber aDrive);
    virtual ~CSqlDriveSpace();
	inline TDriveNumber Drive() const;
	void GetAccessL();
	void ReleaseAccess();
	void Invariant() const;

private:
	CSqlDriveSpace(RFs& aFs, TDriveNumber aDrive);
    void ConstructL();

private:
	RFs& 			iFs;               	//File session reference
	TDriveNumber 	iDrive;    			//Drive number
	TInt 			iGetAccessRefCnt;	//"Get access" requests count

	};

/**
@return The drive number, where the CSqlDriveSpace object handles the reservation requests
*/
inline TDriveNumber CSqlDriveSpace::Drive() const
    {
    return iDrive;
    }

/**
This class describes a collection of CSqlDriveSpace objects. Each CSqlDriveSpace object in the
collection is responsible for handling "reserve/get access/release" requests for a particular
drive.

Only one RSqlDriveSpaceCol instance should be created (resp. destroyed) by the CSqlServer object.

@see CSqlServer
@see CSqlDriveSpace

@internalComponent
*/
NONSHARABLE_CLASS(RSqlDriveSpaceCol)
	{
public:
	RSqlDriveSpaceCol();
	void Create(RFs& aFs);
	void ResetAndDestroy();
    CSqlDriveSpace* Find(TDriveNumber aDrive);
    CSqlDriveSpace* AddL(TDriveNumber aDrive);
	void Invariant() const;

private:
	RFs*				        	iFs;            //File session reference
	RPointerArray<CSqlDriveSpace>	iDriveSpaceCol; //Collection of CSqlDriveSpace objects: one per drive

	};

#endif//__SQLSRVDRIVESPACE_H__
