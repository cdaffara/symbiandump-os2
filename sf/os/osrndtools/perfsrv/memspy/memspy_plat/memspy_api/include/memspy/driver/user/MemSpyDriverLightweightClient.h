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

#ifndef MEMSPYDRIVERLIGHTWEIGHTCLIENT_H
#define MEMSPYDRIVERLIGHTWEIGHTCLIENT_H

// System includes
#include <e32std.h>

// Structs etc
_LIT(KMemSpyDriverDeviceName, "MEMSPYDRIVER");
inline TVersion KMemSpyDriverVersion() { return TVersion(1, 0, 0); }

enum TMemSpyDriverOpCode
	{
	EMemSpyDriverGetCodeSegsForProcess = 3,
	EMemSpyDriverGetCodeSegInfo = 4
    };

struct TMemSpyDriverInternalCodeSnapshotParams
	{
	TUint iPid;
	TAny** iHandles;
	TInt* iCountPtr;
	};
class TMemSpyDriverCodeSegInfo
	{
public:
    TCodeSegCreateInfo iCreateInfo;
    TProcessMemoryInfo iMemoryInfo;
	};
struct TMemSpyDriverInternalCodeSegParams
	{
	TUint iPid;
	TAny* iHandle;
	TMemSpyDriverCodeSegInfo* iInfoPointer;
	};


class RMemSpyDriverLightweightClient : public RBusLogicalChannel
	{
public:

	inline TInt Open()
        {
        TInt err = User::LoadLogicalDevice( KMemSpyDriverDeviceName );
        //
	    if  ( err == KErrNone || err == KErrAlreadyExists )
		    {
		    err = DoCreate( KMemSpyDriverDeviceName, KMemSpyDriverVersion(), KNullUnit, NULL, NULL, EOwnerThread );
		    }
        //
	    return err;
        }

	inline TInt GetCodeSegs( TUint aPid, TAny** aHandleArray, TInt& aHandleCount )
        {
	    TMemSpyDriverInternalCodeSnapshotParams params;
	    params.iPid = aPid;
	    params.iHandles = aHandleArray;
	    params.iCountPtr = &aHandleCount;
	    return DoControl( EMemSpyDriverGetCodeSegsForProcess, &params, NULL );
        }

	inline TInt GetCodeSegInfo( TAny* aHandle, TUint aPid, TMemSpyDriverCodeSegInfo& aInfo )
        {
	    TMemSpyDriverInternalCodeSegParams params;
	    params.iPid = aPid;
	    params.iHandle = aHandle;
	    params.iInfoPointer = &aInfo;
        //
	    const TInt r = DoControl( EMemSpyDriverGetCodeSegInfo, &params, NULL );
	    return r;
        }
    };


#endif
