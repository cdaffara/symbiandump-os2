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
// Reserving/Accessing/Releasing drive space - CDriveSpace, RDriveSpaceCol, CDbsSessDriveSpace
// classes implementations
// 
//

#include "U32STD.H"
#include "Sd_DriveSpace.h"

/**
That's the amount of the disk space, which will be reserved at the momemnt of creation of
each CDriveSpace object.
It should be enough for the most of the "delete" transactions, used by the applications.
@internalComponent
*/
const TInt KReservedDiskSpace = 64 * 1024;

/////////////////////////////////////////////////////////////////////////////////////////////
//CDriveSpace class 

/**
Grants access to the already reserved drive space. The method uses RFs::GetReserveAccess()
for that. RFs::GetReserveAccess() will be called only once, when iGrantAccessRefCnt is 0 - 
this is a shared file session instance.
@leave RFs::GetReserveAccess() return values
@see RFs::GetReserveAccess()
@panic In debug mode there will be a panic with the line number as an error code if (iGrantAccessRefCnt < 0)
*/
void CDriveSpace::GrantAccessL()
	{
	__ASSERT(iGrantAccessRefCnt >= 0);
	//Grant the access only once, there is only one RFs session instance.
	if(iGrantAccessRefCnt == 0)
		{
		__LEAVE_IF_ERROR(iFs.GetReserveAccess(static_cast <TInt> (iDrive)));
		}
	++iGrantAccessRefCnt;
	}

/**
Revokes access to the reserved drive space.
The method calls RFs::ReleaseReserveAccess() only when iGrantAccessRefCnt value reaches 0.
@see RFs::ReleaseReserveAccess()
@panic In debug mode there will be a panic with the line number as an error code if (iGrantAccessRefCnt <= 0)
*/
void CDriveSpace::ReleaseAccess()
	{
	__ASSERT(iGrantAccessRefCnt > 0);
    if(iGrantAccessRefCnt == 0)
        {
        return;
        }
	if(--iGrantAccessRefCnt == 0)
		{
        //I can't see any reason to bother the caller with errors, when the access to the 
        //reserved space is revoked.
		(void)iFs.ReleaseReserveAccess(static_cast <TInt> (iDrive));
		}
	}

/**
Standard phase-one factory method for creation of CDriveSpace objects.
@param aFs File session instance
@param aDrive Drive number
@return A pointer to the created CDriveSpace object.
@leave KErrNoMemory Out of memory
@leave RFs::ReserveDriveSpace() return values
@see RFs::ReserveDriveSpace()
*/
CDriveSpace* CDriveSpace::NewLC(RFs& aFs, TDriveNumber aDrive)
    {
    CDriveSpace* self = new (ELeave) CDriveSpace(aFs, aDrive);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/**
@param aFs File session instance
@param aDrive Drive number
@panic In debug mode there will be a panic with the line number as an error code if 
       (iDrive value is not between EDriveA and EDriveZ)
*/
CDriveSpace::CDriveSpace(RFs& aFs, TDriveNumber aDrive) :
	iFs(aFs),
	iDrive(aDrive)
	{
	__ASSERT(iDrive >= EDriveA && iDrive <= EDriveZ);
	}

/**
Frees the reserved disk space.
@panic In debug mode there will be a panic with the line number as an error code if 
       (iGrantAccessRefCnt != 0)
*/
CDriveSpace::~CDriveSpace()
    {
	__ASSERT(iGrantAccessRefCnt == 0);
    if(iGrantAccessRefCnt > 0)
        {
		(void)iFs.ReleaseReserveAccess(static_cast <TInt> (iDrive));
        }
	(void)iFs.ReserveDriveSpace(static_cast <TInt> (iDrive), 0);
    }

/**
Standard phase-two construction method for creation of CDriveSpace objects.
It will reserve a predefined amount of a disk space, enough for the most of the
"delete" transactions, used by the applications.
@leave RFs::ReserveDriveSpace() return values
@see RFs::ReserveDriveSpace()
*/
void CDriveSpace::ConstructL()
    {
	__LEAVE_IF_ERROR(iFs.ReserveDriveSpace(static_cast <TInt> (iDrive), KReservedDiskSpace));
    }

/////////////////////////////////////////////////////////////////////////////////////////////
//RDriveSpaceCol class 

/**
@param aFs File session instance
*/
RDriveSpaceCol::RDriveSpaceCol(RFs& aFs) :
	iFs(aFs)
	{
	}

/**
The method releases all the resources used by RDriveSpaceCol object, including and 
reserved drive spaces on all drives in the collection.
*/
void RDriveSpaceCol::Close()
	{
	//Forced release of the reserved space
	for(TInt i=iDriveSpaceCol.Count()-1;i>=0;--i)
		{
		delete iDriveSpaceCol[i];
		}
	iDriveSpaceCol.Close();
	}

/**
The method returns a pointer to the CDriveSpace object, which handles all drive space
requests, related to a particular drive.
@param aDrive Drive number.
@return A pointer to the CDriveSpace object, which handles drive space reservation requests
        for aDriveNo drive. If there is no such object, the method returns NULL.
*/
CDriveSpace* RDriveSpaceCol::Find(TDriveNumber aDrive)
    {
    for(TInt index=iDriveSpaceCol.Count()-1;index>=0;--index)
        {
        if(iDriveSpaceCol[index]->iDrive == aDrive)
            {
            return iDriveSpaceCol[index];
            }
        }
    return NULL;
    }

/**
The method creates a new CDriveSpace object, adds it to the collection and 
returns a pointer to it.
@param aDrive Drive number.
@return A pointer to the created CDriveSpace object, which handles drive space 
        reservation requests for aDriveNo drive.
@leave System-wide error codes, including KErrNoMemory.
@panic In debug mode there will be a panic with the line number as an error code if 
       a CDriveSpace object already exists for aDrive.
*/
CDriveSpace* RDriveSpaceCol::CreateAddL(TDriveNumber aDrive)
	{
    __ASSERT(!Find(aDrive));
    CDriveSpace* drvSpace = CDriveSpace::NewLC(iFs, aDrive);
    __LEAVE_IF_ERROR(iDriveSpaceCol.Append(drvSpace));
    CleanupStack::Pop(drvSpace);
    return drvSpace;
	}

/////////////////////////////////////////////////////////////////////////////////////////////
//TDriveSpaceRq class 

/**
@param aDriveSpace A reference to the CDriveSpace object, which handles the reservation requests
                   for a particular drive.
*/
CDbsSessDriveSpace::TDriveSpaceRq::TDriveSpaceRq(CDriveSpace& aDriveSpace) :
    iDriveSpace(aDriveSpace),
    iReserved(0),
    iAccessGranted(0)
    {
    }

/////////////////////////////////////////////////////////////////////////////////////////////
//CDbsSessDriveSpace class 

/**
Standard phase-one factory method for creation of CDbsSessDriveSpace objects.
@param aDriveSpaceCol A reference to the RDriveSpaceCol collection - there should be only one
                      per DBMS server.
@return A pointer to the created CDbsSessDriveSpace object.
@leave KErrNoMemory Out of memory
*/
CDbsSessDriveSpace* CDbsSessDriveSpace::NewL(RDriveSpaceCol& aDriveSpaceCol)
    {
    CDbsSessDriveSpace* self = new (ELeave) CDbsSessDriveSpace(aDriveSpaceCol);
    return self;
    }

/**
@param aDriveSpaceCol A reference to the RDriveSpaceCol collection - there should be only one
                      per DBMS server.
*/
CDbsSessDriveSpace::CDbsSessDriveSpace(RDriveSpaceCol& aDriveSpaceCol) :
    iDriveSpaceCol(aDriveSpaceCol)
    {
    }

/**
Releases the allocated resources, which means the CDriveSpace::ReleaseAccess()
will be called if TDriveSpaceRq::iAccessGranted for the related drive is set.
*/
CDbsSessDriveSpace::~CDbsSessDriveSpace()
    {
    for(TInt i=iDriveSpaceRqCol.Count()-1;i>=0;--i)
        {
        TDriveSpaceRq& rq = iDriveSpaceRqCol[i];
        if(rq.iAccessGranted)
            {
            rq.iDriveSpace.ReleaseAccess();
            }
        //That's all!
        //rq.iDriveSpace object shall not be destroyed or the reserved disk space - freed!
        //It will be freed at the DBMS server shutdown.
        }//end of: for(TInt i=iDriveSpaceRqCol.Count()-1;i>=0;--i)
    iDriveSpaceRqCol.Close();
    }

/**
The method reserves the requested amount of disk space.
Because it is a shared file session between all DBMS sessions, a predefined amount of
disk space per drive will be reserved at the moment of the first request made by some of
the DBMS sessions. All the requests after the first (for a particular disk drive) will
be completed successfully without actually reserving a disk space. 
Only the state of the related TDriveSpaceRq object will be updated to make it possible to
control reservation/freeing requests.
@param aDrive Drive number to reserve space on.
@leave KErrNoMemory Out of memory
@leave KErrInUse This DBMS session has already reserved a disk space.
@leave RFs::ReserveDriveSpace() return values
@see RFs::ReserveDriveSpace()
*/
void CDbsSessDriveSpace::ReserveL(TDriveNumber aDrive)
    {
    TDriveSpaceRq& rq = GetL(aDrive);
    if(rq.iReserved || rq.iAccessGranted)
        {
        //This DBMS session already reserved a disk space (or even has an access to it).
        //The DBMS session has to free it and then can re-reserve it again.
        __LEAVE(KErrInUse);
        }
    rq.iReserved = 1; //CDriveSpace object reserves a predefined amount of disk space at
                      //at the moment of its creation, so just set rq.iReserved to non-zero,
                      //indicating that this session has reserved a disk space.
    }

/**
The method frees the reserved by the DBMS session disk space.
The actual implementation won't free the reserved disk space. This will happen at the moment of
the destruction of the related CDriveSpace object for aDrive drive.
The method just updates the state of the related TDriveSpaceRq object to make it possible to
control reservation/freeing requests.
@param aDrive Drive number, which reserved space has to be freed.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no TDriveSpaceRq object for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       the reserved disk space is granted but not released.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no reserved disk space for aDrive.
*/
void CDbsSessDriveSpace::Free(TDriveNumber aDrive)
    {
    TDriveSpaceRq* rq = Find(aDrive);
    __ASSERT(rq);//Probably an attempt to free non-reserved disk space.
    __ASSERT(!rq->iAccessGranted);//An attempt to free reserved and granted disk space.
    __ASSERT(rq->iReserved);//Probably an attempt to free non-reserved disk space.
    if(rq)
        {
        rq->iReserved = 0;//Set it to 0, without actually freeing the reserved disk space.
                          //It is shared between all DBMS sessions.
        }
    }

/**
Gets an access to the reserved drive space.
@param aDriveNo Drive number with a reserved disk space, an access to which is requested
@leave KErrArgument There is no reserved disk space on aDrive.
@leave KErrInUse An access to the reserved space has already been granted.
@leave RFs::GetReserveAccess() return values
@see RFs::GetReserveAccess()
*/
void CDbsSessDriveSpace::GrantAccessL(TDriveNumber aDrive)
    {
    TDriveSpaceRq* rq = Find(aDrive);
    if(!rq)
        {//Probably an attempt to get an access to a non-reserved disk space.
        __LEAVE(KErrArgument);
        }
    if(!rq->iReserved)
        {//Probably an attempt to get an access to a non-reserved disk space.
        __LEAVE(KErrArgument);
        }
    if(rq->iAccessGranted)
        {//Probably an attempt to get an access twice to the reserved disk space.
        __LEAVE(KErrInUse);
        }
    rq->iDriveSpace.GrantAccessL();
    rq->iAccessGranted = 1;
    }

/**
Releases the access to the reserved drive space.
@param aDriveNo Drive number with a reserved disk space, the access to which has to be released.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no TDriveSpaceRq object for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no reserved disk space for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no granted access to the reserved disk space. 
*/
void CDbsSessDriveSpace::ReleaseAccess(TDriveNumber aDrive)
    {
    TDriveSpaceRq* rq = Find(aDrive);
    __ASSERT(rq);//Probably an attempt to release the access to a non-reserved disk space.
    if(rq)
        {
        __ASSERT(rq->iReserved);//Probably an attempt to release the access to a non-reserved disk space.
        __ASSERT(rq->iAccessGranted);//Probably an attempt to grant an access to the reserved disk spave twice.
        if(rq->iReserved && rq->iAccessGranted)
            {
            rq->iDriveSpace.ReleaseAccess();
            rq->iAccessGranted = 0;
            }
        }
    }

/**
The method searches iDriveSpaceRqCol collection for TDriveSpaceRq object, which drive number
is the same as aDrive parameter.
@param aDrive Drive number
@return A pointer to the TDriveSpaceRq object, which has the same drive number
        NULL, if there is no such opject.
*/
CDbsSessDriveSpace::TDriveSpaceRq* CDbsSessDriveSpace::Find(TDriveNumber aDrive)
    {
    for(TInt index=iDriveSpaceRqCol.Count()-1;index>=0;--index)
        {
        if(iDriveSpaceRqCol[index].iDriveSpace.Drive() == aDrive)
            {
            return &iDriveSpaceRqCol[index];
            }
        }
    return NULL;
    }

/**
The method creates a new TDriveSpaceRq object with aDrive as a drive number and adds it
to iDriveSpaceRqCol collection.
@param aDrive Drive number
@return A reference to the created TDriveSpaceRq object
@leave System-wide error codes, including KErrNoMemory.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is an existing TDriveSpaceRq object for aDrive. 
*/
CDbsSessDriveSpace::TDriveSpaceRq& CDbsSessDriveSpace::CreateAddL(TDriveNumber aDrive)
    {
    __ASSERT(!Find(aDrive));
    //Check iDriveSpaceCol collection if there is a CDriveSpace object, which was already
    //created for aDrive.
    CDriveSpace* drvSpace = iDriveSpaceCol.Find(aDrive);
    if(!drvSpace)
        {//There is no CDriveSpace object for aDrive. It has to be created.
        drvSpace = iDriveSpaceCol.CreateAddL(aDrive);
        }
    __ASSERT(drvSpace);
    //Now we have the related CDriveSpace object for aDrive.
    //Create new TDriveSpaceRq object and add it to iDriveSpaceRqCol collection.
    __LEAVE_IF_ERROR(iDriveSpaceRqCol.Append(TDriveSpaceRq(*drvSpace)));
    //If Append() call fails, then we have CDriveSpace object created but not deleted.
    //It is not a problem, because CDriveSpace object was added to iDriveSpaceCol and will
    //be destroyed when iDriveSpaceCol is destroyed.
    return iDriveSpaceRqCol[iDriveSpaceRqCol.Count() - 1];
    }

/**
This method combines the functionality of CDbsSessDriveSpace::Find() and 
CDbsSessDriveSpace::CreateAddL() methods.
It will search for a TDriveSpaceRq object with the same drive number as aDrive.
If such object doesn not exist, it will be created, added to iDriveSpaceRqCol and a reference
to it - returned.
@param aDrive Drive number
@return A reference to the TDriveSpaceRq object, responsible for aDrive drive.
@leave System-wide error codes, including KErrNoMemory.
@see CDbsSessDriveSpace::Find()
@see CDbsSessDriveSpace::CreateAddL()
*/
CDbsSessDriveSpace::TDriveSpaceRq& CDbsSessDriveSpace::GetL(TDriveNumber aDrive)
    {
    TDriveSpaceRq* rq = Find(aDrive);
    return rq ? *rq : CreateAddL(aDrive);
    }
