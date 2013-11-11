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
// classes declarations
// 
//

#ifndef __SD_DRIVESPACE_H__
#define __SD_DRIVESPACE_H__

#include <f32file.h>

//Forward declarations
class RDriveSpaceCol;
class CDbsSessDriveSpace;
class CDbsServer;

/**
This class describes an object, which is responsible for handling
"reserve/get access/release" requests for a particular drive.
Since the drive is shared between many DBMS sessions and there is only one RFs instance,
the current solution is that CDriveSpace objects reserve some predefined amount of disk space
at the time of their creation and then the access to the reserved disk space is reference counted.
There is one obvious disadvantage of this solution: if a bad application "forgets" to release
the access to the reserved disk space, it may be used by the DBMS file session and at the moment,
when some client will really need it to complete its "delete" transaction, it may happen that
there is no reserved disk space at all, because it is already used. But I don't think there
is an acceptable solution for this case, if there is only one shared file session.
The class shall not be used directly, that's the reason its constructor and NewLC() are private.
The class functionality shall be used by the controlling collection object - RDriveSpaceCol.
@internalComponent
*/
NONSHARABLE_CLASS(CDriveSpace) : public CBase
	{
	friend class RDriveSpaceCol;

public:
	inline TDriveNumber Drive() const;
	void GrantAccessL();
	void ReleaseAccess();

private:
	static CDriveSpace* NewLC(RFs& aFs, TDriveNumber aDrive);
	CDriveSpace(RFs& aFs, TDriveNumber aDrive);
    virtual ~CDriveSpace();
    void ConstructL();

private:
	RFs& iFs;               //File session reference
	TDriveNumber iDrive;    //Drive number
	TInt iGrantAccessRefCnt;//"Get access" requests count

	};

/**
@return The drive number, where the CDriveSpace object handles the reservation requests
*/
inline TDriveNumber CDriveSpace::Drive() const
    {
    return iDrive;
    }

/**
This class describes a collection of CDriveSpace objects. Each CDriveSpace object in the
collection is responsible for handling "reserve/get access/release" requests for a particular
drive.
Only one instace of RDriveSpaceCol class should be created and used in the DBMS server.
An object of this class shall be created by the DBMS server, but the DBMS server is alloved only
to close (destroy) it at the end. RDriveSpaceCol collection shall be used by CDbsSessDriveSpace
instances, which shall be created/used/destroyed by the DBMS sessions.
@see CDriveSpace
@internalComponent
*/
class RDriveSpaceCol
	{
    friend class CDbsServer;
    friend class CDbsSessDriveSpace;

private:
	RDriveSpaceCol(RFs& aFs);
	void Close();
    CDriveSpace* Find(TDriveNumber aDrive);
    CDriveSpace* CreateAddL(TDriveNumber aDrive);

private:
	RFs&				        iFs;            //File session reference
	RPointerArray<CDriveSpace>	iDriveSpaceCol; //Collection of CDriveSpace objects: one per drive

	};

/**
This structure describes an object, which handles all disk space related requests per
DBMS session instance.
Using it, the DBMS session can control the access to the reserved drive space for a 
particular drive(s).
Although it seems a good idea to save some binary code space and use directly CDriveSpace and
RDriveSpaceCol classes - no, it is not that good.
If you use directly CDriveSpace and RDriveSpaceCol, you have to control that every "Reserve"
call is matched by a "Free" call and every "Get/GrantAcces" call is matched by a "ReleaseAccess"
call. Also you have to take care about checking that the appropriate CDriveSpace object has
been created already or if not - you have to create and add it to the RDriveSpaceCol colelction.
All that is done in CDbsSessDriveSpace class.
@internalComponent
*/
NONSHARABLE_CLASS(CDbsSessDriveSpace) : public CBase
    {
private:
    /**
    This structure describes the state of the disk space reservation requests, related to a 
    particular drive. It holds an information like: drive number, was the disk space 
    already reserved?, was an access to it already granted?
    Using the TDriveSpaceRq objects, the caller can ensure that every "Reserve" call is
    matched by a "Free" call, every "GetAccess" call is matched by a "ReleaseAccess" call.
    @internalComponent
    */
    struct TDriveSpaceRq
	    {
        TDriveSpaceRq(CDriveSpace& aDriveSpace);
	    CDriveSpace&    iDriveSpace;        //This object will handle the reservation requests
        TUint8	        iReserved:		1;  //If non-zero then the space was already reserved
        TUint8	        iAccessGranted: 1;  //If non-zero then an access to the space was already granted
	    };

public:
    static CDbsSessDriveSpace* NewL(RDriveSpaceCol& aDriveSpaceCol);
    virtual ~CDbsSessDriveSpace();

    void ReserveL(TDriveNumber aDrive);
    void Free(TDriveNumber aDrive);
	void GrantAccessL(TDriveNumber aDrive);
	void ReleaseAccess(TDriveNumber aDrive);

private:
    CDbsSessDriveSpace(RDriveSpaceCol& aDriveSpaceCol);
    TDriveSpaceRq* Find(TDriveNumber aDrive);
    TDriveSpaceRq& CreateAddL(TDriveNumber aDrive);
    TDriveSpaceRq& GetL(TDriveNumber aDrive);

private:
    RDriveSpaceCol&         iDriveSpaceCol;//Reference to the RDriveSpaceCol colelction.
    RArray<TDriveSpaceRq>   iDriveSpaceRqCol;//Collection of TDriveSpaceRq objects: one per drive

    };

#endif//__SD_DRIVESPACE_H__
