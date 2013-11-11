// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32cmn.h>
#include "efmfeatflag.h"

//
// This code implements the 'CEFMFeatFlag' class for creation of feature set
// data files. Here, we are concerned with one 'Feature Flag' element of
// the file, each contains a 'UID', a status flag (which contains
// various bits determinining whether, for example, this feature is supported,
// blacklisted, upgradable, etc) plus a 32-bit user data word.
//
// A feature set data file may contain any number of feature flags. A single
// 'CEFMFeatFlag' object is created for each feature flag required within
// the data file.
//

//
// The constructor - the UID of the feature flag must be specified. The
// status flag may be specified (but defaults to zero), as does the
// user data word.
//
CEFMFeatFlag::CEFMFeatFlag( TUint32 auid, TUint32 asflag=0,
                                                       TUint32 auserdata=0 )
    {
    iuid = auid;
    isflag = asflag;
    iuserdata = auserdata;
    return;
    }

//
// This method expects a descriptor as an argument. We copy 12 bytes into
// the descriptor which are the binary representation of the UID, status
// flags and user data defined within this object.
//
TInt CEFMFeatFlag::BinaryContent(TDes8 &abuf)
    {
	TAny *p = &iuid;
	abuf.Copy((TUint8 *)p, sizeof(iuid));
	p = &isflag;
	abuf.Append((TUint8 *)p, sizeof(isflag));
	p = &iuserdata;
	abuf.Append((TUint8 *)p, sizeof(iuserdata));
	return KErrNone;
	}

// A getter for the UID of this feature flag object.
TUint32 CEFMFeatFlag::UID () const 
	{
	return iuid;
    }

// A getter for the status flags of this feature flag object.
TUint32 CEFMFeatFlag::StatusFlags() const
	{
	return isflag;
    }

// A getter for the user data word of this feature flag object.
TUint32 CEFMFeatFlag::UserData() const
	{
	return iuserdata;
    }

// This method compares another 'CEFMFeatFlag' object to this one, returning
// zero if the other object does not match this one, or 1 if it does.
TInt CEFMFeatFlag::Compare(const CEFMFeatFlag &a2)
	{
	if(UID() != a2.UID())
     	return 0;
	if(StatusFlags() != a2.StatusFlags())
     	return 0;
	if(UserData() != a2.UserData())
     	return 0;
	return 1;
    }

