// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SdpTypedTime.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include "sdptypedtime.h"
#include "_sdpdefs.h"
#include "sdpcodecerr.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecstringpool.h"

// LOCAL CONSTANTS
const TUint KMaxTInt64Digits = 30;

// -----------------------------------------------------------------------------
// TSdpTypedTime::TSdpTypedTime
// Contructor
// -----------------------------------------------------------------------------
//
EXPORT_C TSdpTypedTime::TSdpTypedTime()
:	iValue(0), iUnit(ETimeUnitNone)
	{
	}

// -----------------------------------------------------------------------------
// TSdpTypedTime::TSdpTypedTime
// Contructor
// -----------------------------------------------------------------------------
//
EXPORT_C TSdpTypedTime::TSdpTypedTime(TInt64 aValue, TUnit aUnit)
:	iValue(aValue), iUnit(aUnit)
	{
	}

// -----------------------------------------------------------------------------
// TSdpTypedTime::operator ==
// Checks if two TSdpTypedTimes are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TSdpTypedTime::operator == (const TSdpTypedTime& aObj) const
	{
	return iValue == aObj.iValue && iUnit == aObj.iUnit;
	}

// -----------------------------------------------------------------------------
// TSdpTypedTime::SetValue
// Sets typed time value
// -----------------------------------------------------------------------------
//
EXPORT_C void TSdpTypedTime::SetValue(TInt64 aValue, TUnit aUnit)
	{
	iValue = aValue;
	iUnit = aUnit;
	}

// -----------------------------------------------------------------------------
// TSdpTypedTime::ExternalizeL
// Externalizes the typed time to stream
// -----------------------------------------------------------------------------
//
void TSdpTypedTime::ExternalizeL(RWriteStream& aStream) const
    {
	aStream.WriteUint32L( I64HIGH( iValue ) );
    aStream.WriteUint32L( I64LOW( iValue ) );
    aStream.WriteInt16L(iUnit);
    }

// -----------------------------------------------------------------------------
// TSdpTypedTime::InternalizeL
// Internalizes typed time from stream
// -----------------------------------------------------------------------------
//
TSdpTypedTime TSdpTypedTime::InternalizeL(RReadStream& aStream)
    {
    TUint32 high = aStream.ReadUint32L();
    TUint32 low = aStream.ReadUint32L();
    TInt64 value = MAKE_TINT64( high, low );
    TUnit unit = static_cast<TUnit>(aStream.ReadInt16L());
    TSdpTypedTime typedTime(value, unit);
    return typedTime;
    }

// -----------------------------------------------------------------------------
// TSdpTypedTime::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
void TSdpTypedTime::EncodeL(RWriteStream& aStream) const
    {
    TBuf8<KMaxTInt64Digits> number;
    number.AppendNum( iValue );
	aStream.WriteL( number );

    RStringPool pool = SdpCodecStringPool::StringPoolL();
    const TDesC8& timeDays = pool.StringF(
                                    SdpCodecStringConstants::ETimeUnitDays,
                                    SdpCodecStringConstants::Table).DesC();
    const TDesC8& timeHours = pool.StringF(
                                    SdpCodecStringConstants::ETimeUnitHours,
                                    SdpCodecStringConstants::Table).DesC();
    const TDesC8& timeMinutes = pool.StringF(
                                    SdpCodecStringConstants::ETimeUnitMinutes,
                                    SdpCodecStringConstants::Table).DesC();
    const TDesC8& timeSeconds = pool.StringF(
                                    SdpCodecStringConstants::ETimeUnitSeconds,
                                    SdpCodecStringConstants::Table).DesC();

    if (iUnit != ETimeUnitNone)
        {
        TBuf8<1> unit;
        if (iUnit == ETimeUnitDays)
            {
            unit.Append(timeDays);
            }
        else if (iUnit == ETimeUnitHours)
            {
            unit.Append(timeHours);
            }
        else if (iUnit == ETimeUnitMinutes)
            {
            unit.Append(timeMinutes);
            }
        else if (iUnit == ETimeUnitSeconds)
            {
            unit.Append(timeSeconds);
            }
        else
            {}

        aStream.WriteL(unit);
        }
    }

// -----------------------------------------------------------------------------
// TSdpTypedTime::DecodeL
// Decodes typed time from TDesC
// -----------------------------------------------------------------------------
//
TSdpTypedTime TSdpTypedTime::DecodeL(const TDesC8& aText)
    {
    TLex8 typedTimeLex(aText);
    TSdpTypedTime typedTime;
    User::LeaveIfError(typedTimeLex.Val(typedTime.iValue, EDecimal));

    if (typedTimeLex.Peek() != 0)
        {
        TPtrC8 unitString = typedTimeLex.NextToken();

        RStringPool pool = SdpCodecStringPool::StringPoolL();
        const TDesC8& timeDays = pool.StringF(
                            SdpCodecStringConstants::ETimeUnitDays,
                            SdpCodecStringConstants::Table).DesC();
        const TDesC8& timeHours = pool.StringF(
                            SdpCodecStringConstants::ETimeUnitHours,
                            SdpCodecStringConstants::Table).DesC();
        const TDesC8& timeMinutes = pool.StringF(
                            SdpCodecStringConstants::ETimeUnitMinutes,
                            SdpCodecStringConstants::Table).DesC();
        const TDesC8& timeSeconds = pool.StringF(
                            SdpCodecStringConstants::ETimeUnitSeconds,
                            SdpCodecStringConstants::Table).DesC();

        if (unitString.Compare(timeDays) == 0)
            {
            typedTime.iUnit = ETimeUnitDays;
            }
        else if (unitString.Compare(timeHours) == 0)
            {
            typedTime.iUnit = ETimeUnitHours;
            }
        else if (unitString.Compare(timeMinutes) == 0)
            {
            typedTime.iUnit = ETimeUnitMinutes;
            }
        else if (unitString.Compare(timeSeconds) == 0)
            {
            typedTime.iUnit = ETimeUnitSeconds;
            }
        else
            {
            User::Leave(KErrSdpCodecTypedTime);
            }
        }
    else
        {
        typedTime.iUnit = ETimeUnitNone;
        }

    return typedTime;
    }
