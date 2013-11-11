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
// Name          : SdpTimeField.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include "sdptimefield.h"
#include "sdputil.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"
#include "sdprepeatfield.h"
#include "sdpcodecstringconstants.h"
#include "sdpdocument.h"
#include "sdpcodecerr.h"
#include "sdpcodecconstants.h"
#include "sdpcodecerr.h"
#include "sdpcodecstringpool.h"

// LOCAL CONSTANTS
_LIT8(KZeroTime, "0");

// -----------------------------------------------------------------------------
// CSdpTimeField::DecodeL
// Decodes time field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpTimeField * CSdpTimeField::DecodeL(const TDesC8& aText, 
                                                TBool aRecurse)
	{
	CSdpTimeField* obj = DecodeLC(aText, aRecurse);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::DecodeLC
// Decodes time field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpTimeField * CSdpTimeField::DecodeLC(const TDesC8& aText, 
                                                TBool aRecurse)
	{
	CSdpTimeField* obj = new (ELeave) CSdpTimeField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aText, aRecurse);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
EXPORT_C CSdpTimeField * CSdpTimeField::NewL(const TDesC8& aStartTime, 
                                             const TDesC8& aStopTime)
	{
	CSdpTimeField* obj = NewLC(aStartTime, aStopTime);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
EXPORT_C CSdpTimeField * CSdpTimeField::NewLC(const TDesC8& aStartTime, 
                                              const TDesC8& aStopTime)
	{
	CSdpTimeField* obj = new (ELeave) CSdpTimeField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aStartTime, aStopTime);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::~CSdpTimeField
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpTimeField::~CSdpTimeField ()
	{
	iRFields.ResetAndDestroy();
	delete iStopTime;
	delete iStartTime;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpTimeField::EncodeL(RWriteStream& aStream, 
                                     TBool aRecurse) const
	{
	__ASSERT_ALWAYS(this->IsValid(), User::Leave(KErrNotReady));
    const TDesC8& timeName = iStringPool.StringF(SdpCodecStringConstants::ETime, 
                                        SdpCodecStringConstants::Table).DesC();
    aStream.WriteL(timeName);
    aStream.WriteL(*iStartTime);
    aStream.WriteL(KSPStr);
    aStream.WriteL(*iStopTime);
    aStream.WriteL(KCRLFStr);
    if (aRecurse)
        {
        for(TInt i=0; i<iRFields.Count(); i++)
            {
            iRFields[i]->EncodeL(aStream);
            }
        }
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::CloneL
// Creates an exact copy of the time field
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpTimeField* CSdpTimeField::CloneL(TBool aRecurse) const
	{
    CSdpTimeField* obj = CSdpTimeField::NewLC(*this->iStartTime, 
                                            *this->iStopTime);
	__TEST_INVARIANT;
    if (aRecurse)
        {
        for (TInt i = 0; i < iRFields.Count(); i++)
            {
            CSdpRepeatField* repObj = iRFields[i]->CloneL();
            CleanupStack::PushL(repObj);
            User::LeaveIfError(obj->RepeatFields().Append(repObj));
            CleanupStack::Pop(); // repObj
            }
        }
    CleanupStack::Pop(); //obj
    return obj;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::operator ==
// Checks if two time fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpTimeField::operator == (const CSdpTimeField & aObj) const
	{
	__TEST_INVARIANT;
    return (aObj.StartTime().Compare(this->StartTime()) == 0
            &&  aObj.StopTime().Compare(this->StopTime()) == 0
            && RepeatFieldsCompare(aObj));
    }

// -----------------------------------------------------------------------------
// CSdpTimeField::IsValid
// Checks if start time and stop time are valid
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpTimeField::IsValid() const
    {
    return 	( (iStartTime->Match(KZeroTime) 
                && iStopTime->Match(KZeroTime)) 
            || (iStartTime->Length() != 0 
                && iStopTime->Length() != 0
				&& *iStartTime <= *iStopTime));
    }

// -----------------------------------------------------------------------------
// CSdpTimeField::StartTime
// Returns start time 
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpTimeField::StartTime() const
	{
	__TEST_INVARIANT;
	return *iStartTime;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::StopTime
// Returns stop time 
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpTimeField::StopTime() const
	{
	__TEST_INVARIANT;
	return *iStopTime;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::SetTimesL
// Sets start time and stop time if they are valid ones 
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpTimeField::SetTimesL(const TDesC8& aStartTime, 
                                       const TDesC8& aStopTime)
	{
	__ASSERT_ALWAYS(
        (aStartTime.Length() == 1 && aStartTime[0] -'0' == 0
            && ((aStopTime.Length() == 1 && aStopTime[0] -'0' == 0)
                || (aStopTime.Length() > 1 
                    && SdpUtil::IsValidNtpTime(aStopTime))))
        || (aStartTime.Length() > 1 && SdpUtil::IsValidNtpTime(aStartTime)
            && aStopTime.Length() > 1 && SdpUtil::IsValidNtpTime(aStopTime)
            && aStartTime < aStopTime),
        User::Leave(KErrSdpCodecTimeField));
    
    HBufC8* tmpStartTime = aStartTime.AllocL();
    CleanupStack::PushL(tmpStartTime);
    HBufC8* tmpStopTime = aStopTime.AllocL();
	
    tmpStartTime->Des().Trim();
	delete iStartTime;
	iStartTime = tmpStartTime;
	tmpStopTime->Des().Trim();
	delete iStopTime;
	iStopTime = tmpStopTime;

    CleanupStack::Pop(); // tmpStartTime
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::RepeatFields
// Returns repeat fields
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpRepeatField>& CSdpTimeField::RepeatFields()
	{
	__TEST_INVARIANT;
	return iRFields;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::ExternalizeL
// Externalizes the object to stream
// -----------------------------------------------------------------------------
//
void CSdpTimeField::ExternalizeL(RWriteStream& aStream) const
    {
   	__TEST_INVARIANT;
    aStream.WriteUint32L(iStartTime->Length());
    aStream.WriteL(*iStartTime);
    aStream.WriteUint32L(iStopTime->Length());
    aStream.WriteL(*iStopTime);
    aStream.WriteUint32L(iRFields.Count());
    for(TInt i=0; i<iRFields.Count(); i++)
       {
       iRFields[i]->ExternalizeL(aStream);
       }
    }

// -----------------------------------------------------------------------------
// CSdpTimeField::InternalizeL
// Internalizes the object from stream
// -----------------------------------------------------------------------------
//
CSdpTimeField* CSdpTimeField::InternalizeL(RReadStream& aStream)
    {
    CSdpTimeField* self = new (ELeave) CSdpTimeField();
    CleanupStack::PushL(self);
    self->iStringPool = SdpCodecStringPool::StringPoolL();

    self->DoInternalizeL(aStream);

    CleanupStack::Pop(); //self

    return self; 
    }

// -----------------------------------------------------------------------------
// CSdpTimeField::DoInternalizeL
// Internalizes the object members from stream
// -----------------------------------------------------------------------------
//
void CSdpTimeField::DoInternalizeL(RReadStream& aStream)
    {
    
    TUint32 valueLength = aStream.ReadUint32L();
	iStartTime = HBufC8::NewL (valueLength);
	TPtr8 startTime(iStartTime->Des());
	aStream.ReadL (startTime,valueLength);
    valueLength = aStream.ReadUint32L();
	iStopTime = HBufC8::NewL (valueLength);
	TPtr8 stopTime(iStopTime->Des());
	aStream.ReadL (stopTime,valueLength);
    
    TUint rFieldsCount = aStream.ReadUint32L();
    for (TUint i = 0; i < rFieldsCount; i++)
        {
        CSdpRepeatField* rField = CSdpRepeatField::InternalizeL(aStream);
        CleanupStack::PushL(rField);
        User::LeaveIfError(iRFields.Append(rField));
        CleanupStack::Pop(); //rField
        }
    }

// -----------------------------------------------------------------------------
// CSdpTimeField::CSdpTimeField
// Constructor
// -----------------------------------------------------------------------------
//
CSdpTimeField::CSdpTimeField()
	{
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpTimeField::ConstructL(const TDesC8& aText, TBool aRecurse)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
    RArray <TPtrC8> lines;
    lines = SdpUtil::DivideToLinesL(aText, KErrSdpCodecTimeField);
    CleanupClosePushL(lines);
    RArray<TPtrC8> timeArray;
    timeArray = SdpUtil::GetElementsFromLineL(lines[0], KErrSdpCodecTimeField);
    CleanupClosePushL(timeArray);
    const TDesC8& timeName = iStringPool.StringF(SdpCodecStringConstants::ETime, 
        SdpCodecStringConstants::Table).DesC();        
    __ASSERT_ALWAYS(lines.Count() > 0
                    && timeArray.Count() == 3
                    && timeArray[0].Find(timeName) != KErrNotFound,
                    User::Leave(KErrSdpCodecTimeField));

    SetTimesL(timeArray[1], timeArray[2]);

    if (aRecurse && lines.Count() > 1)
        {
        const TDesC8& repeatName = 
                    iStringPool.StringF(SdpCodecStringConstants::ERepeat,
                               SdpCodecStringConstants::Table).DesC();   
        for( TInt i = 1; i<lines.Count(); i++)
            {
            RArray<TPtrC8> lineArray;
            lineArray = SdpUtil::GetElementsFromLineL(lines[i], KErrSdpCodecTimeField);
            CleanupClosePushL(lineArray);
            __ASSERT_ALWAYS(lineArray[0].Find(repeatName) != KErrNotFound,
                User::Leave(KErrSdpCodecTimeField));
            CSdpRepeatField* repeatField = CSdpRepeatField::DecodeL(lines[i]);
            CleanupStack::PushL(repeatField);
            User::LeaveIfError(iRFields.Append(repeatField));
            CleanupStack::Pop(); // repeatField
            CleanupStack::PopAndDestroy(); //lineArray
            }
        }
    CleanupStack::PopAndDestroy(2); // timeArray, lines
    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpTimeField::ConstructL(const TDesC8& aStartTime, 
                               const TDesC8& aStopTime)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
 	SetTimesL(aStartTime, aStopTime);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpTimeField::RepeatFieldsCompare
// Checks if two time field objects repeat fields are equal 
// -----------------------------------------------------------------------------
//
TBool CSdpTimeField::RepeatFieldsCompare(const CSdpTimeField& aObj) const
    {
    TBool returnValue = ETrue;
    if (iRFields.Count() == aObj.iRFields.Count())
        {
        for (TInt i = 0; i < iRFields.Count() && returnValue; i++)
            {
            if (!(*iRFields[i] == *aObj.iRFields[i]))
                returnValue = EFalse;
            }
        }
    
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CSdpTimeField::__DbgTestInvariant
// Test invariant
// -----------------------------------------------------------------------------
//
void CSdpTimeField::__DbgTestInvariant() const
	{
    TBool invariant = 
        ((iStartTime->Length() == 1 && (iStartTime->Des())[0] -'0' == 0)
            && ((iStopTime->Length() == 1 && (iStopTime->Des())[0] -'0' == 0)
                || (iStopTime->Length() > 1 
                    && SdpUtil::IsValidNtpTime(*iStopTime))))
        || (iStartTime->Length() > 1 && SdpUtil::IsValidNtpTime(*iStartTime)
            && iStopTime->Length() > 1 && SdpUtil::IsValidNtpTime(*iStopTime)
            && *iStartTime < *iStopTime);

	if (!invariant)
		User::Invariant();
	}


