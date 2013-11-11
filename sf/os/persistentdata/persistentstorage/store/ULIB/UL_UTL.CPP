// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UL_STD.H"


#if defined (_DEBUG)

#pragma BullseyeCoverage off

//
// Leave to the current control region.
//
void Util::__DbgLeave(TInt aReason,const TDesC8& /*aFileName*/,TInt /*aLineNo*/)
	{
	//coverity[naming_error]
	User::Leave(aReason);
	}

//
// Leave with the KErrNoMemory error.
//
void Util::__DbgLeaveNoMemory(const TDesC8& aFileName,TInt aLineNo)
	{
	//coverity[naming_error]
	__DbgLeave(KErrNoMemory,aFileName,aLineNo);
	}

//
// Leave if aReason is negative.
//
TInt Util::__DbgLeaveIfError(TInt aReason,const TDesC8& aFileName,TInt aLineNo)
	{
	if (aReason<0)
		{
		//coverity[naming_error]
		__DbgLeave(aReason,aFileName,aLineNo);
		}
	return aReason;
	}

//
// Leave if aPtr is null.
//
TAny *Util::__DbgLeaveIfNull(TAny *aPtr,const TDesC8& aFileName,TInt aLineNo)
	{
	if (aPtr==NULL)
		{
		//coverity[naming_error]
		__DbgLeaveNoMemory(aFileName,aLineNo);
		}
	return aPtr;
	}

#pragma BullseyeCoverage on

#endif

