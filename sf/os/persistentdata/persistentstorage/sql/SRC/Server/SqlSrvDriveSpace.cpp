// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Reserving/Accessing/Releasing drive space - CSqlDriveSpace and RSqlDriveSpaceCol classes implementations
// 
//

#include "SqlSrvDriveSpace.h"
#include "SqlAssert.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvDriveSpaceTraces.h"
#endif
#include "SqlTraceDef.h"

/**
The amount of the disk space, which will be reserved at the moment of creation of
each CDriveSpace object.
It should be enough for the most of "delete" transactions, if they do not require a rollback transaction
file, bigger than KReservedDiskSpace bytes.

Note: this is the max possible amount, which can be reserved per file session.
      Look for KMaxSessionDriveReserved constant value.

@internalComponent
*/
const TInt KReservedDiskSpace = 64 * 1024;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////    CSqlDriveSpace class    //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Standard phase-one factory method for creation of CSqlDriveSpace objects.

@param aFs File session instance
@param aDrive Drive number

@return A pointer to the created CSqlDriveSpace object.

@leave KErrNoMemory,  Out of memory condition has occured;
	   RFs::ReserveDriveSpace() return values.

@panic SqlDb 4 In _DEBUG mode if aDrive is invalid
	   
@see RFs::ReserveDriveSpace()
*/
CSqlDriveSpace* CSqlDriveSpace::NewLC(RFs& aFs, TDriveNumber aDrive)
    {
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLDRIVESPACE_NEWLC_ENTRY, "Entry;0;CSqlDriveSpace::NewLC;aFs.Handle()=0x%X;aDrive=%d", (TUint)aFs.Handle(), (TInt)aDrive));
	__ASSERT_DEBUG(aDrive >= EDriveA && aDrive <= EDriveZ, __SQLPANIC2(ESqlPanicBadArgument));
    CSqlDriveSpace* self = new (ELeave) CSqlDriveSpace(aFs, aDrive);
    CleanupStack::PushL(self);
    self->ConstructL();
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLDRIVESPACE_NEWLC_EXIT, "Exit;0x%X;CSqlDriveSpace::NewLC", (TUint)self));
    return self;
    }

/**
Frees the reserved disk space.
*/
CSqlDriveSpace::~CSqlDriveSpace()
    {
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLDRIVESPACE_CSQLDRIVESPACE2, "0x%X;CSqlDriveSpace::~CSqlDriveSpace;iDrive=%d;iGetAccessRefCnt=%d", (TUint)this, (TInt)iDrive, iGetAccessRefCnt));
	SQLDRIVESPACE_INVARIANT();
	(void)iFs.ReleaseReserveAccess(static_cast <TInt> (iDrive));
	(void)iFs.ReserveDriveSpace(static_cast <TInt> (iDrive), 0);
    }

/**
Gives an access to the already reserved drive space. The method uses RFs::GetReserveAccess()
for that. RFs::GetReserveAccess() will be called only once, when iGetAccessRefCnt is 0 - 
this is a shared file session instance.

@leave RFs::GetReserveAccess() return values

@see RFs::GetReserveAccess()

@panic SqlDb 12 In _DEBUG mode if iGetAccessRefCnt < 0
*/
void CSqlDriveSpace::GetAccessL()
	{
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLDRIVESPACE_GETACCESSL_ENTRY, "Entry;0x%X;CSqlDriveSpace::GetAccessL;iDrive=%d;iGetAccessRefCnt=%d", (TUint)this, (TInt)iDrive, iGetAccessRefCnt));
	SQLDRIVESPACE_INVARIANT();
	//Gets an access only once, there is only one RFs session instance.
	if(iGetAccessRefCnt == 0)
		{
		__SQLLEAVE_IF_ERROR(iFs.GetReserveAccess(static_cast <TInt> (iDrive)));
		}
	++iGetAccessRefCnt;
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLDRIVESPACE_GETACCESSL_EXIT, "Exit;0x%X;CSqlDriveSpace::GetAccessL;iDrive=%d;iGetAccessRefCnt=%d", (TUint)this, (TInt)iDrive, iGetAccessRefCnt));
	SQLDRIVESPACE_INVARIANT();
	}

/**
Revokes access to the reserved drive space.

The method calls RFs::ReleaseReserveAccess() only when iGetAccessRefCnt value reaches 0.

@see RFs::ReleaseReserveAccess()

@panic SqlDb 12 In _DEBUG mode if iGetAccessRefCnt < 0
*/
void CSqlDriveSpace::ReleaseAccess()
	{
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLDRIVESPACE_RELEASEACCESS, "0x%X;CSqlDriveSpace::ReleaseAccess;iDrive=%d;iGetAccessRefCnt=%d", (TUint)this, (TInt)iDrive, iGetAccessRefCnt));
	SQLDRIVESPACE_INVARIANT();
    if(iGetAccessRefCnt == 0)
        {
        return;
        }
	if(--iGetAccessRefCnt == 0)
		{
        //I can't see any reason to bother the caller with errors, when the access to the 
        //reserved space is revoked.
		(void)iFs.ReleaseReserveAccess(static_cast <TInt> (iDrive));
		}
	SQLDRIVESPACE_INVARIANT();
	}

/**
@param aFs File session instance
@param aDrive Drive number

@panic SqlDb 4 In _DEBUG mode if aDrive is invalid
*/
CSqlDriveSpace::CSqlDriveSpace(RFs& aFs, TDriveNumber aDrive) :
	iFs(aFs),
	iDrive(aDrive)
	{
	__ASSERT_DEBUG(aDrive >= EDriveA && aDrive <= EDriveZ, __SQLPANIC(ESqlPanicBadArgument));
	}

/**
Standard phase-two construction method for creation of CSqlDriveSpace objects.

It will reserve a predefined amount of a disk space, enough for the most of the
"delete" transactions, used by the applications.

@leave RFs::ReserveDriveSpace() return values

@see RFs::ReserveDriveSpace()
*/
void CSqlDriveSpace::ConstructL()
    {
	__SQLLEAVE_IF_ERROR(iFs.ReserveDriveSpace(static_cast <TInt> (iDrive), KReservedDiskSpace));
	SQLDRIVESPACE_INVARIANT();
    }

#ifdef _DEBUG
/**
CSqlDriveSpace invariant.
*/
void CSqlDriveSpace::Invariant() const
	{
	__ASSERT_DEBUG(iDrive >= EDriveA && iDrive <= EDriveZ, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(iGetAccessRefCnt >= 0, __SQLPANIC(ESqlPanicMisuse));
	}
#endif//_DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////    RSqlDriveSpaceCol class    ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RSqlDriveSpaceCol::RSqlDriveSpaceCol() :
	iFs(NULL)
	{
	}

/**
@param aFs A reference to file session instance
*/
void RSqlDriveSpaceCol::Create(RFs& aFs)
	{
	iFs = &aFs;
	SQLDRIVESPACECOL_INVARIANT();
	}

/**
The method releases all the resources used by RSqlDriveSpaceCol object, including and 
reserved drive spaces on all drives in the collection.
*/
void RSqlDriveSpaceCol::ResetAndDestroy()
	{
	//Forced release of the reserved space
	for(TInt i=iDriveSpaceCol.Count()-1;i>=0;--i)
		{
		delete iDriveSpaceCol[i];
		}
	iDriveSpaceCol.Close();
	iFs = NULL;
	}

/**
Searches the collection for a CSqlDriveSpace object holding information about the reserved space on aDrive drive.

@param aDrive Drive number.

@return A pointer to the CSqlDriveSpace object, which handles drive space reservation requests
        for aDriveNo drive. If there is no such object, the method returns NULL.

@panic SqlDb 4 In _DEBUG mode if aDrive is invalid
*/
CSqlDriveSpace* RSqlDriveSpaceCol::Find(TDriveNumber aDrive)
    {
	__ASSERT_DEBUG(aDrive >= EDriveA && aDrive <= EDriveZ, __SQLPANIC(ESqlPanicBadArgument));
	SQLDRIVESPACECOL_INVARIANT();
    for(TInt index=iDriveSpaceCol.Count()-1;index>=0;--index)
        {
        if(iDriveSpaceCol[index]->Drive() == aDrive)
            {
            return iDriveSpaceCol[index];
            }
        }
	SQLDRIVESPACECOL_INVARIANT();
    return NULL;
    }

/**
The method creates a new CSqlDriveSpace object, adds it to the collection and returns a pointer to it.

@param aDrive Drive number.

@return A pointer to the created CDriveSpace object, which handles drive space 
        reservation requests for aDriveNo drive.
        
@leave System-wide error codes, including KErrNoMemory.

@panic SqlDb 4, In _DEBUG mode if aDrive is invalid.
@panic SqlDb 12, In _DEBUG mode if a CDriveSpace object already exists for aDrive.
*/
CSqlDriveSpace* RSqlDriveSpaceCol::AddL(TDriveNumber aDrive)
	{
	__ASSERT_DEBUG(aDrive >= EDriveA && aDrive <= EDriveZ, __SQLPANIC(ESqlPanicBadArgument));
    __ASSERT_DEBUG(!Find(aDrive), __SQLPANIC(ESqlPanicMisuse));
	SQLDRIVESPACECOL_INVARIANT();
    CSqlDriveSpace* drvSpace = CSqlDriveSpace::NewLC(*iFs, aDrive);
    __SQLLEAVE_IF_ERROR(iDriveSpaceCol.Append(drvSpace));
    CleanupStack::Pop(drvSpace);
	SQLDRIVESPACECOL_INVARIANT();
    return drvSpace;
	}

#ifdef _DEBUG
/**
RSqlDriveSpaceCol invariant.
*/
void RSqlDriveSpaceCol::Invariant() const
	{
	__ASSERT_DEBUG(iFs != NULL, __SQLPANIC(ESqlPanicInternalError));
    for(TInt index=iDriveSpaceCol.Count()-1;index>=0;--index)
    	{
		iDriveSpaceCol[index]->Invariant();
    	}
	}
#endif//_DEBUG

