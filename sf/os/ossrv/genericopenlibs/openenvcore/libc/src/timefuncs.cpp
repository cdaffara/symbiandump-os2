/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is a project specific include file for building the 
*                clock related functions as part of libc library.
*
*/


#include <e32base.h>
#include <e32cons.h>
#include <tz.h>
#include <tzconverter.h>
#include "tzlocalizer.h" 
#include "tzlocalizationdatatypes.h"
#include "vtzrules.h"
#include "utf.h" 
#include "timefuncs.h"
#include "lposix.h"
#include "sysif.h"
#include "libc_wsd_defs.h"

// This represents the base EpochTime in microseconds, 
// which happens to be January 1, 1970 (midnight UTC/GMT), 
// not counting leap seconds. This effectively represents 
// the number of microseconds in 1970 years.    
static const TInt64 KEpochTime = 62168256000000000;



#ifndef EMULATOR
struct tm cachetm;
time_t prevchange;
time_t nextchange;
#else //EMULATOR



GET_GLOBAL_VAR_FROM_TLS(cachetm, struct tm)
#define cachetm (*GET_WSD_VAR_NAME(cachetm, g)())
GET_GLOBAL_VAR_FROM_TLS(prevchange, time_t)
#define prevchange (*GET_WSD_VAR_NAME(prevchange, g)())
GET_GLOBAL_VAR_FROM_TLS(nextchange, time_t)
#define nextchange (*GET_WSD_VAR_NAME(nextchange, g)())
#endif //EMULATOR

#if !defined(__SERIES60_30__) 
TTzTimeReference FindChangeBracket(TTime& aTime ,TTime& aUTime, CVTzActualisedRules& aRule,TBool& aDstOn)
    {
    TInt cnt = aRule.Count();
    TInt lowestOffset = -1, curOffset=-1;
    TBool isInit = EFalse;
    TTime CurTime(aTime);
    TTime Previous(-1), Next(-1);
    TTzTimeReference ref(ETzWallTimeReference);
    
//datetime to be created from the UTC time.    
    TDateTime dateTime = aUTime.DateTime();
    
// Initially set aDstOn to False
    aDstOn = EFalse;
    
// Loop to find the lowest time offset applicable in the year.    
    for(TInt idx=0; idx<cnt; idx++)
        {
        TVTzActualisedRule& Actual = aRule[idx];  
        if (Actual.iTimeOfChange.DateTime().Year() == dateTime.Year())
            {
            if (!isInit)
                {
                lowestOffset = Actual.iNewOffset;
                isInit = ETrue;    
                }

            if (isInit && Actual.iNewOffset < lowestOffset)
                {
                lowestOffset = Actual.iNewOffset;
                }   
            }
        }
//    Loop to find the current offset, previous time of change and the next time of change.
    for(TInt idx=0; idx<cnt; idx++)
        {
        TVTzActualisedRule& Actual = aRule[idx];
        if(Actual.iTimeReference == ETzUtcTimeReference && CurTime != aUTime)
            {
            CurTime = aUTime;
            ref = ETzUtcTimeReference;
            }
        //  Rules are arranged in increasing order of iTimeofChange
        if(CurTime >= Actual.iTimeOfChange)
            {
            Previous = Actual.iTimeOfChange;
            curOffset = Actual.iNewOffset;
            }
        else
            {
            Next = Actual.iTimeOfChange;
            break;
            }
       }
/*
 * If the Next and Previous were not set due to some reason,
 * Set them to one year after and one year before the time passed.
 */     
    if(Next == -1)
        {
        /* Next year */
        Next = CurTime + (TTimeIntervalSeconds)(365*86400);
        }
    if (Previous == -1)
        {
        /* Prev Year */
        Previous = CurTime - (TTimeIntervalSeconds)(365*86400);
        }
    aTime = Previous;
    aUTime = Next;
    if(lowestOffset != -1 && curOffset > lowestOffset)
        {
        aDstOn = ETrue;
        }
    return ref;
    }
#endif

void UpdateDstAndTznameL(RTz &aServer,CTzId& aId,struct tm* aTmStruct, TTime aTime, TTime aUTime, TDateTime aDate, TTzTimeReference aReference)
    {
#if !defined(__SERIES60_30__) 
    CTzRules* zonerules = aServer.GetTimeZoneRulesL(aId, aTime, aTime, aReference);
    CleanupStack::PushL(zonerules);
    CVTzActualisedRules* actual = CVTzActualisedRules::NewL(aDate.Year()-1, aDate.Year()+1);
    CleanupStack::PushL(actual);
    zonerules->GetActualisedRulesL(*actual);
    TTime PrevTime(aTime), NextTime(aUTime);
    TBool IsDstOn;
    /* Send local TTimes */
    if(FindChangeBracket(PrevTime, NextTime, *actual, IsDstOn)== ETzWallTimeReference)
        {
        aServer.ConvertToUniversalTime(PrevTime,aId);
        aServer.ConvertToUniversalTime(NextTime,aId);
        }
    prevchange = (PrevTime.Int64() - KEpochTime)/1000000;
    nextchange = (NextTime.Int64() - KEpochTime)/1000000;

    aTmStruct->tm_isdst = IsDstOn;
    CleanupStack::PopAndDestroy(actual);
    CleanupStack::PopAndDestroy(zonerules);
#else // __SERIES60_30__ defined
    aServer;
    aTime;
    aUTime;
    aDate;
    aReference;
#endif
/*  //This part has been commented out because of performance issue. Also this requires capability.
    if(RProcess().HasCapability(ECapabilityReadUserData,ECapabilityWriteUserData,NULL))
        {
        //          tm_zone updated only in this case. Otherwise always has UTC.
        CTzLocalizer *localizer = CTzLocalizer::NewL();
        CleanupStack::PushL(localizer);
        CTzLocalizedTimeZone* timezone = localizer->GetLocalizedTimeZoneL(aId.TimeZoneNumericID());
        CleanupStack::PushL(timezone);
        TPtrC zoneNameDesc;
        if (aTmStruct->tm_isdst)
            {
            zoneNameDesc.Set(timezone->ShortDaylightName());
            }
        else            
            {
            zoneNameDesc.Set(timezone->ShortStandardName());
            }
        //TODO: Value should not be hardcoded here. Get it from the struct state again.
        TPtr8 ptr((unsigned char*)tzname[0],zoneNameDesc.Size());
        CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr,zoneNameDesc);
        CleanupStack::PopAndDestroy(timezone);  
        CleanupStack::PopAndDestroy(localizer);
        } 
    
    aTmStruct->tm_zone = tzname[0]; */
    
    aTmStruct->tm_zone = "WILDABBR";
    }  

void ConvertUtcToLocalTimeL(const time_t* aUTCTime, struct tm* const atmStruct)
    {
    if(!aUTCTime)
        {
        User::Leave(KErrArgument);
        }
    TTime   time(KEpochTime + (*aUTCTime) * (TInt64)1000000);
    TTime   Utime(time);

    // Cache Logic.
    TTimeIntervalSeconds UtcOffset = User::UTCOffset();
    if( cachetm.tm_gmtoff == UtcOffset.Int() && prevchange <= *aUTCTime && *aUTCTime<= nextchange)
        {
        time += UtcOffset;         // Convert to local time.
        TDateTime tdt = time.DateTime();
        cachetm.tm_sec = tdt.Second();
        cachetm.tm_min = tdt.Minute();
        cachetm.tm_hour = tdt.Hour();    
        cachetm.tm_mday = tdt.Day() + 1;
        cachetm.tm_mon = tdt.Month();
        cachetm.tm_year = tdt.Year() - 1900;    // This is as per the tm structure format.  
        cachetm.tm_wday = (time.DayNoInWeek()+1)%7; // Note: first day of the week is considered as Monday, so adding +1. 
        cachetm.tm_yday = time.DayNoInYear()-1; // Note: first day of the year is considered as 1, so
        *atmStruct = cachetm;
        return;
        }

    TDateTime tdt = time.DateTime();
    
    //enable the cache
	TInt status = KErrNone;
	RTz& rtzServer = Backend()->TZServer(status);
	if(status != KErrNone)
		User::Leave(status);

    CTzConverter* ctzConverter = CTzConverter::NewL(rtzServer);
    
    CleanupStack::PushL(ctzConverter);
    if(ctzConverter->ConvertToLocalTime(time) == KErrNone)      
        {
        tdt = time.DateTime();
        atmStruct->tm_sec = tdt.Second();
        atmStruct->tm_min = tdt.Minute();
        atmStruct->tm_hour = tdt.Hour();
        atmStruct->tm_mday = tdt.Day() + 1;
        atmStruct->tm_mon = tdt.Month();
        atmStruct->tm_year = tdt.Year() - 1900;    // This is as per the tm structure format.  
        atmStruct->tm_wday = (time.DayNoInWeek()+1)%7; // Note: first day of the week is considered as Monday, so adding +1. 
        atmStruct->tm_yday = time.DayNoInYear()-1; // Note: first day of the year is considered as 1, so 

        atmStruct->tm_gmtoff = (time.Int64() - Utime.Int64())/1000000;
        
        TInt timeZoneId = ctzConverter->CurrentTzId();
        CTzId* zoneid = CTzId::NewL((TUint)timeZoneId);
        CleanupStack::PushL(zoneid);
        
        atmStruct->tm_isdst  = -1;
        UpdateDstAndTznameL(rtzServer, *zoneid, atmStruct, time, Utime, tdt, ETzWallTimeReference);
        CleanupStack::PopAndDestroy(zoneid);
        }   
    

    CleanupStack::PopAndDestroy(1);
    cachetm = *atmStruct;
    }

void ConvertLocalToUtcTimeL(time_t* aUTCTime, struct tm* const aTmStruct)
    {
    if(!aTmStruct || aTmStruct->tm_year < 0 || aTmStruct->tm_mon < 0 || aTmStruct->tm_mday < 1 || aTmStruct->tm_hour < 0 || aTmStruct->tm_min < 0 || aTmStruct->tm_sec < 0 )
        {
        User::Leave(KErrArgument);
        }

    TDateTime tdt(aTmStruct->tm_year + 1900, (TMonth)aTmStruct->tm_mon, aTmStruct->tm_mday-1, 
                    aTmStruct->tm_hour, aTmStruct->tm_min, aTmStruct->tm_sec, 0);
    TTime   time(tdt);
    TTime   oldTime(time);

//  Cache Logic 
    TTimeIntervalSeconds UtcOffset = User::UTCOffset();
    if(cachetm.tm_gmtoff == UtcOffset.Int() && cachetm.tm_year == aTmStruct->tm_year && cachetm.tm_mon == aTmStruct->tm_mon && cachetm.tm_mday == aTmStruct->tm_mday)
        {
        aTmStruct->tm_gmtoff = cachetm.tm_gmtoff;
        aTmStruct->tm_isdst = cachetm.tm_isdst;
        aTmStruct->tm_zone = cachetm.tm_zone;    
        aTmStruct->tm_wday = (time.DayNoInWeek()+1)%7; // Note: first day of the week is considered as Monday, so adding +1. 
        aTmStruct->tm_yday = time.DayNoInYear()-1; // Note: first day of the year is considered as 1, so
        time -= UtcOffset;                 // Converting to UTC time.
        TInt64 utcTimeMicroSec = time.Int64();
        TInt64 epochRefTime = utcTimeMicroSec - KEpochTime;
        (*aUTCTime) = (TInt) (epochRefTime / 1000000);
        return;
        }

	TInt status = KErrNone;
	RTz& rtzServer = Backend()->TZServer(status);
	if(status != KErrNone)
		User::Leave(status);

    CTzConverter* ctzConverter = CTzConverter::NewL(rtzServer);      
    CleanupStack::PushL(ctzConverter);
 /* Following fields are updated if successful:
  * tm_wday
  * tm_yday
  * tm_isdst
  * tm_gmtoffset
  * tm_zone ( Conditional on capabilities available)
  */
    if(ctzConverter->ConvertToUniversalTime(time) == KErrNone)
        {
        //conversion successful
        TInt64 utcTimeMicroSec = time.Int64();
        TInt64 epochRefTime = utcTimeMicroSec - KEpochTime;
        (*aUTCTime) = (TInt) (epochRefTime / 1000000);
        aTmStruct->tm_wday = (oldTime.DayNoInWeek()+1)%7; // Note: first day of the week is considered as Monday, so adding +1. 
        aTmStruct->tm_yday = oldTime.DayNoInYear()-1; // Note: first day of the year is considered as 1, so
        aTmStruct->tm_gmtoff = (oldTime.Int64() - utcTimeMicroSec)/1000000;
        
        TInt timeZoneId = ctzConverter->CurrentTzId();
        CTzId* zoneid = CTzId::NewL((TUint)timeZoneId);
        CleanupStack::PushL(zoneid);

        aTmStruct->tm_isdst  = -1;
        UpdateDstAndTznameL(rtzServer, *zoneid, aTmStruct, oldTime, time, tdt, ETzUtcTimeReference);
        CleanupStack::PopAndDestroy(zoneid);
        }
    cachetm = *aTmStruct;
    CleanupStack::PopAndDestroy(1);
    }

// converts a UTC time to local time, or, local time to UTC time.  
TInt ConvertTime(const TInt atimeConverFlg, time_t *aUTCTime, struct tm *const atmStruct)
    {
    TInt err = 0;

    CActiveScheduler* oldAs = CActiveScheduler::Current();
    RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
    CActiveScheduler* newAs = new CActiveScheduler();
    User::SwitchHeap(oldHeap);
    if(!newAs)
        {
        errno = ENOMEM;
        return ENOMEM;
        }
    if(oldAs)
        {
        (void)CActiveScheduler::Replace(newAs);
        }
    else
        {
        CActiveScheduler::Install(newAs);
        }
    
    oldHeap = User::SwitchHeap(Backend()->Heap());
    // Cleanup stack needed
    CTrapCleanup* cleanup=CTrapCleanup::New();

    if (cleanup)
        {
        if(EUtcToLocal == atimeConverFlg)
            {
            TRAP(err, ConvertUtcToLocalTimeL(aUTCTime, atmStruct));
            }
        else if(ELocalToUtc == atimeConverFlg)
            {
            TRAP(err, ConvertLocalToUtcTimeL(aUTCTime, atmStruct));
            }
        delete cleanup;
        }
    else
        {
        err = ENOMEM;
        }
    if(oldAs)
        {
        (void)CActiveScheduler::Replace(oldAs);
        }


    delete newAs;
    User::SwitchHeap(oldHeap);
    return MapError(err,errno);
    }

