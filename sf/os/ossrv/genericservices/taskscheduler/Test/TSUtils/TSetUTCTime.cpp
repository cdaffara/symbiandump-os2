// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32debug.h>
	
// This function is used to update month and day in aBufLocalTime
// to actual month and day to be passed to TTime() constructor.
// Remember using TTime::FormatL() in thelpers.cpp 
// has added extra month and a day to aBufLocalTime.
// aBufLocalTime is in format YYMMDD:HHMMSS.MMMMMM
// see TTime::Set() for aBufLocalTime expected format details.
TInt UpdateToActualMonthAndDay(TDes& aBufUTCTime)
	{	
	TInt mVal 	= 0;
	TInt dVal 	= 0;
	
	TBuf <4> tempBuf;
	_LIT(KFormat, "%02d");
	
	//Get the position of colon separator	
	TInt colon 	= aBufUTCTime.Locate(':');
		
	// Get Month & Day if Present	
	switch(colon)
		{
		case 0: break;
		case 8:
			{
			TLex month 	= aBufUTCTime.Mid(4,2);
			TLex day 	= aBufUTCTime.Mid(6,2);
			month.Val(mVal);
			day.Val(dVal);			
			}
			break;
		default:
			{
			// If the colon is at the wrong position
			return (KErrArgument);
			}
				
		}
		
		// Deduct extra month and a day and update aBufLocalTime
		if(mVal > 0 && dVal > 0)
			{
			mVal-=1;
			dVal-=1;
				
			tempBuf.Format(KFormat, mVal);
			aBufUTCTime.Replace(4,2, tempBuf);
				
			tempBuf.Format(KFormat, dVal);
			aBufUTCTime.Replace(6,2, tempBuf);				
			}
			
		return(KErrNone);		
	}
	
GLDEF_C TInt E32Main()
	{
	TInt err = KErrNone;	
	TBuf<64> bufUTCTime;
	
	User::CommandLine(bufUTCTime);	
	err = UpdateToActualMonthAndDay(bufUTCTime);
	
	if( err == KErrNone)
	{
		TTime utcTime(bufUTCTime);	
		err = User::SetUTCTime(utcTime);	
	}
		
	return err;
	}

