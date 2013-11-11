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
// This code implements the 'CEFMDSR' class. A 'CEFMDSR' object holds 
// minimum and maximum UID values which are within a 'default supported
// range' (DSR) which enables support for a range of UIDs without individually
// specifying them in a feature flag item in the feature manager data file.
// Each 'CEFMDSR' object holds only two items of data - the minimum and
// maximum UID values in the default supported range.
// Note that a feature data file can contain multiple default supported ranges,
// therefore if the data file generator (which uses this class) needs more
// than one default supported range, then more than one of these objects will
// be created.
// 
//

#include <e32base.h>
#include <e32cmn.h>
#include "efmdsr.h"

// The constructor for our default supported range - only two values are
// required, the minimum and maximum values for the range.
CEFMDSR::CEFMDSR( TUint32 alow, TUint32 ahigh )
    {
    ilowuid = alow;
    ihighuid = ahigh;
    return;
    }

// The descriptor passed in has the low and high uid values of this default
// supported range copied into it. The descriptor must therefore have the
// requisite space (8 bytes) to allow this.
TInt CEFMDSR::BinaryContent(TDes8 &abuf)
    {
	TAny *p = &ilowuid;
	abuf.Copy((TUint8 *)p, sizeof(ilowuid));
	p = &ihighuid;
	abuf.Append((TUint8 *)p, sizeof(ihighuid));
	return KErrNone;
	}

// Getter for the low bound of our default supported range.
TUint32 CEFMDSR::LowUID() const
	{
	return ilowuid;
    }

// Getter for the high bound of our default supported range.
TUint32 CEFMDSR::HighUID() const
	{
	return ihighuid;
    }

// Returns zero if this DSR object is not equal to the one passed in, or 1
// if it is equal.
TInt CEFMDSR::Compare(const CEFMDSR &a2)
	{
	if(LowUID() != a2.LowUID())
     	return 0;
	if(HighUID() != a2.HighUID())
     	return 0;
	return 1;
    }
