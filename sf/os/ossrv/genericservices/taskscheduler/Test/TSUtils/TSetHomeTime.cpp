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

#include <e32debug.h>
#include <tz.h>
	
// This function is used to update month and day in aBufLocalTime
// to actual month and day to be passed to TTime() constructor.
// Remember using TTime::FormatL() in thelpers.cpp 
// has added extra month and a day to aBufLocalTime.
// aBufLocalTime is in format YYMMDD:HHMMSS.MMMMMM
// see TTime::Set() for aBufLocalTime expected format details.
TInt UpdateToActualMonthAndDay(TDes& aBufLocalTime)
	{	
	TInt mVal 	= 0;
	TInt dVal 	= 0;
	
	TBuf <4> tempBuf;
	_LIT(KFormat, "%02d");
	
	//Get the position of colon separator	
	TInt colon 	= aBufLocalTime.Locate(':');
	
	// Get Month & Day if Present	
	switch(colon)
		{
		case 0: break;
		case 8:
			{
			TLex month 	= aBufLocalTime.Mid(4,2);
			TLex day 	= aBufLocalTime.Mid(6,2);
			month.Val(mVal);
			day.Val(dVal);			
			}
			break;
		default:
			{
			// If the colon is at the wrong position.
			return (KErrArgument);
			}
				
		}
		
		// Deduct extra month and a day and update aBufLocalTime
		if(mVal > 0 && dVal > 0)
			{
			mVal-=1;
			dVal-=1;
				
			tempBuf.Format(KFormat, mVal);
			aBufLocalTime.Replace(4,2, tempBuf);
				
			tempBuf.Format(KFormat, dVal);
			aBufLocalTime.Replace(6,2, tempBuf);				
			}
			
		return(KErrNone);
	}
	
//This function is used in the test code to Set the system time to the given local time.
static TInt SetHomeTimeL(TTime& aLocalTime)
	{
	TInt retVal = KErrNone;
	RTz theTzSrv;
	
	User::LeaveIfError(theTzSrv.Connect());
	CleanupClosePushL(theTzSrv);	
	
	retVal = theTzSrv.SetHomeTime(aLocalTime);
		
	CleanupStack::PopAndDestroy(); //theTzSrv.Close()
	return retVal;
	}

GLDEF_C TInt E32Main()
	{	
	TInt err = KErrNone;
	TInt retVal = KErrNone;
	TBuf<64> bufLocalTime;
	
	User::CommandLine(bufLocalTime);	
	
	retVal = UpdateToActualMonthAndDay(bufLocalTime);
	
	if(retVal == KErrNone)
		{
		TTime localTime(bufLocalTime);		
		
		CTrapCleanup* cleanup = CTrapCleanup::New(); //can fail
		if (cleanup)
			{		
			TRAP(err, retVal = SetHomeTimeL(localTime))
			if(err != KErrNone)
				{
				retVal = err;
				}
			delete cleanup;
			}
		}	
	
	return retVal;
	}
