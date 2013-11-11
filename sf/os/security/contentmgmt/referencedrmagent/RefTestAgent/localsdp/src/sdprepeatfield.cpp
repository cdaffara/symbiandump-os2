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
// Name          : SdpRepeatField.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include "sdprepeatfield.h"
#include "sdputil.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"
#include "sdpcodecstringconstants.h"
#include "sdpdocument.h"
#include "sdpcodecconstants.h"
#include "sdpcodecerr.h"
#include "sdpcodecstringpool.h"


// -----------------------------------------------------------------------------
// CSdpRepeatField::DecodeL
// Decodes bandwidth field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpRepeatField* CSdpRepeatField::DecodeL(const TDesC8& aText)
	{
	CSdpRepeatField* obj = DecodeLC(aText);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::DecodeLC
// Decodes bandwidth field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpRepeatField* CSdpRepeatField::DecodeLC(const TDesC8& aText)
	{
    CSdpRepeatField* obj = new (ELeave) CSdpRepeatField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aText);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpRepeatField* CSdpRepeatField::NewL(
                                        const TSdpTypedTime aRepeatInterval,
										const TSdpTypedTime  aActiveDuration,
										const RArray<TSdpTypedTime>& aOffsets)
	{
	CSdpRepeatField* obj = NewLC(aRepeatInterval, aActiveDuration, aOffsets);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpRepeatField* CSdpRepeatField::NewLC(
                                        const TSdpTypedTime aRepeatInterval,
										const TSdpTypedTime aActiveDuration,
										const RArray<TSdpTypedTime>& aOffsets)
	{
	CSdpRepeatField* obj = new (ELeave) CSdpRepeatField(aRepeatInterval,
                                                    aActiveDuration);
	CleanupStack::PushL(obj);
	obj->ConstructL(aOffsets);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::~CSdpRepeatField
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpRepeatField::~CSdpRepeatField()
	{
    iTimeOffsets.Close();
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpRepeatField::EncodeL(RWriteStream& aStream) const
    {
    const TDesC8& repeatName = iPool.StringF(SdpCodecStringConstants::ERepeat, 
                                        SdpCodecStringConstants::Table).DesC();
    aStream.WriteL(repeatName);
    iRepeatInterval.EncodeL(aStream);
    aStream.WriteL(KSPStr);
    iActiveDuration.EncodeL(aStream);
    for (TInt i = 0; i < iTimeOffsets.Count(); i++)
        {
        aStream.WriteL(KSPStr);
        (iTimeOffsets)[i].EncodeL(aStream);
        }
    aStream.WriteL(KCRLFStr);
    }

// -----------------------------------------------------------------------------
// CSdpRepeatField::CloneL
// Creates an exact copy of the repeat field
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpRepeatField* CSdpRepeatField::CloneL() const
	{
	__TEST_INVARIANT;
	CSdpRepeatField* obj = NewL(iRepeatInterval, iActiveDuration, 
                                iTimeOffsets);
	__ASSERT_DEBUG(*this == *obj, User::Panic(KSdpCodecPanicCat, 
                                            KSdpCodecPanicInternal));
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::operator ==
// Checks if two repeat fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpRepeatField::operator == (const CSdpRepeatField & aObj) 
        const
	{
	__TEST_INVARIANT;
	TBool result = iRepeatInterval == aObj.iRepeatInterval
		&& iActiveDuration == aObj.iActiveDuration
		&& iTimeOffsets.Count() == (aObj.iTimeOffsets).Count();
	if (result)
		{
		int i = 0;
		while (result && i < iTimeOffsets.Count())
			{
			result = (iTimeOffsets)[i] == (aObj.iTimeOffsets)[i];
			i++;
			}
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::RepeatInterval
// Return repeat interval value
// -----------------------------------------------------------------------------
//
EXPORT_C const TSdpTypedTime CSdpRepeatField::RepeatInterval() const
	{
	__TEST_INVARIANT;
	return iRepeatInterval;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::SetRepeatIntervalL
// Sets repeat interval to one of predefined values
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpRepeatField::SetRepeatIntervalL(const TSdpTypedTime aValue)
	{
	__TEST_INVARIANT;
    __ASSERT_ALWAYS(aValue.iValue > 0, User::Leave(KErrSdpCodecRepeatField));
    iRepeatInterval = aValue;
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::ActiveDuration
// Returns active duration value
// -----------------------------------------------------------------------------
//
EXPORT_C const TSdpTypedTime CSdpRepeatField::ActiveDuration() const
	{
	__TEST_INVARIANT;
	return iActiveDuration;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::SetActiveDuration
// Sets new active duration value
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpRepeatField::SetActiveDuration(const TSdpTypedTime aValue)
	{
	__TEST_INVARIANT;
	iActiveDuration = aValue;
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::TimeOffsets
// CReturns timeoffsets
// -----------------------------------------------------------------------------
//
EXPORT_C const RArray<TSdpTypedTime>& CSdpRepeatField::TimeOffsets() const
	{
	__TEST_INVARIANT;
	return iTimeOffsets;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::SetTimeOffsetsL
// Sets timeoffsets
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpRepeatField::SetTimeOffsetsL(
                                        const RArray<TSdpTypedTime>& aValue)
    {
    RArray<TSdpTypedTime> tempOffsets;                 
    if (aValue.Count() > 0)
	    {
        CleanupClosePushL( tempOffsets );         
		for (TInt i = 0; i < aValue.Count(); i++)
		    {
            User::LeaveIfError(tempOffsets.Append(aValue[i]));
			}
        CleanupStack::Pop();    // tempOffsets
        iTimeOffsets.Close();
        iTimeOffsets = tempOffsets;
        }
    else
        {
   	    // remove old timeoffsets
        iTimeOffsets.Reset();
        }
    }

// -----------------------------------------------------------------------------
// CSdpRepeatField::ExternalizeL
// Externalizes the object to stream
// -----------------------------------------------------------------------------
//
void CSdpRepeatField::ExternalizeL(RWriteStream& aStream) const
    {
    iRepeatInterval.ExternalizeL(aStream);
    iActiveDuration.ExternalizeL(aStream);
    aStream.WriteUint32L(iTimeOffsets.Count());
    for (TInt i = 0; i < iTimeOffsets.Count(); i++)
        {
        (iTimeOffsets)[i].ExternalizeL(aStream);
        }
    }

// -----------------------------------------------------------------------------
// CSdpRepeatField::InternalizeL
// Internalizes the object from stream
// -----------------------------------------------------------------------------
//
CSdpRepeatField* CSdpRepeatField::InternalizeL(RReadStream& aStream)
    {
    const TSdpTypedTime repeatInterval = TSdpTypedTime::InternalizeL(aStream);
    TSdpTypedTime activeDuration = TSdpTypedTime::InternalizeL(aStream);
    TUint32 offsetsCount = aStream.ReadUint32L();
    RArray<TSdpTypedTime> offsets;
    CleanupClosePushL(offsets);

    if (offsetsCount > 0)
        {
		for (TUint i = 0; i < offsetsCount; i++)
			{
            User::LeaveIfError(offsets.Append(
                TSdpTypedTime::InternalizeL(aStream)));
			}
        
        }

    CSdpRepeatField* obj = CSdpRepeatField::NewL(repeatInterval,
                                                activeDuration, offsets);
    
    CleanupStack::PopAndDestroy(); // offsets
    return obj;
    }

// -----------------------------------------------------------------------------
// CSdpRepeatField::CSdpRepeatField
// Constructor
// -----------------------------------------------------------------------------
//
CSdpRepeatField::CSdpRepeatField(const TSdpTypedTime aRepeatInterval,
								const TSdpTypedTime aActiveDuration)
:	iRepeatInterval(aRepeatInterval), iActiveDuration(aActiveDuration)
	{
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::CSdpRepeatField
// Constructor
// -----------------------------------------------------------------------------
//
CSdpRepeatField::CSdpRepeatField()
	{
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpRepeatField::ConstructL(const TDesC8& aText)
	{
    iPool = SdpCodecStringPool::StringPoolL();
    RArray <TPtrC8> lines;
    lines = SdpUtil::DivideToLinesL(aText, KErrSdpCodecRepeatField);
    CleanupClosePushL(lines);
    RArray<TPtrC8> repeatArray;
    repeatArray = SdpUtil::GetElementsFromLineL(lines[0], 
                                                KErrSdpCodecRepeatField);
    CleanupClosePushL(repeatArray);
    const TDesC8& repeatName = iPool.StringF(SdpCodecStringConstants::ERepeat,
                                        SdpCodecStringConstants::Table).DesC();    
    __ASSERT_ALWAYS(lines.Count() == 1
        && repeatArray.Count() >= 3
        && repeatName.CompareF(repeatArray[0]) == 0,
        User::Leave(KErrSdpCodecRepeatField));

    // repeat interval
    __ASSERT_ALWAYS(TLex8(repeatArray[1]).Peek() != '0', 
        User::Leave(KErrSdpCodecRepeatField));
    iRepeatInterval = TSdpTypedTime::DecodeL(repeatArray[1]);
    
    // active duration
    iActiveDuration = TSdpTypedTime::DecodeL(repeatArray[2]);

	if (repeatArray.Count() > 3)
		{
		for (TInt i = 3; i < repeatArray.Count(); i++)
			{
            User::LeaveIfError(iTimeOffsets.Append(
                TSdpTypedTime::DecodeL(repeatArray[i])));
			}
		}
    CleanupStack::PopAndDestroy(2); // lines, repeatArray
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpRepeatField::ConstructL(const RArray<TSdpTypedTime>& aOffsets)
	{
    __ASSERT_ALWAYS(iRepeatInterval.iValue > 0, 
                    User::Leave(KErrSdpCodecRepeatField));
    iPool = SdpCodecStringPool::StringPoolL();
    if (aOffsets.Count() > 0)
		{
        for (TInt i = 0; i < aOffsets.Count(); i++)
			{
            User::LeaveIfError(iTimeOffsets.Append(aOffsets[i]));
			}
		}
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpRepeatField::__DbgTestInvariant
// Test invariant
// -----------------------------------------------------------------------------
//
void CSdpRepeatField::__DbgTestInvariant() const
	{
	TBool invariant = iRepeatInterval.iValue > 0;
	if (!invariant)
		User::Invariant();
	}


