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
// Name          : SdpBandwidthField.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include "sdpbandwidthfield.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"
#include "sdpcodecstringconstants.h"
#include "sdputil.h"
#include "sdpcodecconstants.h"
#include "sdpcodecerr.h"
#include "sdpcodecstringpool.h"

// LOCAL CONSTANTS
const TInt KLines = 1;
const TInt KElements = 3;
const TUint KMaxTInt64Digits = 30;

// -----------------------------------------------------------------------------
// CSdpBandwidthField::DecodeL
// Decodes bandwidth field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpBandwidthField* CSdpBandwidthField::DecodeL(const TDesC8& aText)
	{
	CSdpBandwidthField* obj = DecodeLC(aText);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::DecodeLC
// Decodes bandwidth field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpBandwidthField* CSdpBandwidthField::DecodeLC(const TDesC8& aText)
	{
    CSdpBandwidthField* obj = new (ELeave) CSdpBandwidthField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aText);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpBandwidthField* CSdpBandwidthField::NewL(RStringF aModifier, 
                                                      TUint32 aValue)
	{
	CSdpBandwidthField* obj = NewLC(aModifier, aValue);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpBandwidthField* CSdpBandwidthField::NewLC(RStringF aModifier, 
                                                       TUint32 aValue)
	{
	CSdpBandwidthField* obj = new (ELeave) CSdpBandwidthField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aModifier, aValue);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::~CSdpBandwidthField
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpBandwidthField::~CSdpBandwidthField()
	{
    iModifier.Close();
    }

// -----------------------------------------------------------------------------
// CSdpBandwidthField::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpBandwidthField::EncodeL(RWriteStream& aStream) const
	{
    RStringF header = iModifier.Pool().StringF( 
                                        SdpCodecStringConstants::EBandwidth,
                                        SdpCodecStringConstants::Table );
    aStream.WriteL( header.DesC() );

    aStream.WriteL( iModifier.DesC() );
    aStream.WriteL( KColonStr );

    TBuf8<KMaxTInt64Digits> number;
    number.Format( _L8("%u"), iValue );
	aStream.WriteL( number );

    aStream.WriteL( KCRLFStr );
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::CloneL
// Creates an exact copy of the bandwidth field
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpBandwidthField * CSdpBandwidthField::CloneL() const
	{
	CSdpBandwidthField* obj = NewL(iModifier, iValue);
	__ASSERT_DEBUG(*this == *obj, User::Panic(KSdpCodecPanicCat, 
                                            KSdpCodecPanicInternal));
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::operator ==
// Checks if two bandwidth fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpBandwidthField::operator == (const CSdpBandwidthField& aObj)
const
	{
	return iModifier == aObj.iModifier && iValue == aObj.iValue;
    }

// -----------------------------------------------------------------------------
// CSdpBandwidthField::Modifier
// Returns bandwidth modifier
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpBandwidthField::Modifier() const
	{
	return iModifier;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::SetModifier
// Set new bandwidth modifier
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpBandwidthField::SetModifier(RStringF aModifier)
	{
    iModifier.Close();
	iModifier = aModifier.Copy();
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::Value
// Returns bandwidth value
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CSdpBandwidthField::Value() const
	{
	return iValue;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::SetValue
// Set new bandwidth value
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpBandwidthField::SetValue(TUint32 aValue)
	{
	iValue = aValue;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::ExternalizeL
// Externalizes the object to stream
// -----------------------------------------------------------------------------
//
void CSdpBandwidthField::ExternalizeL(RWriteStream& aStream) const
	{
    aStream.WriteUint32L(iModifier.DesC().Length());
    aStream.WriteL(iModifier.DesC(), iModifier.DesC().Length());
    aStream.WriteUint32L(iValue);
    }

// -----------------------------------------------------------------------------
// CSdpBandwidthField::InternalizeL
// Internalizes the object from stream
// -----------------------------------------------------------------------------
//
CSdpBandwidthField* CSdpBandwidthField::InternalizeL(RReadStream& aStream)
	{
    TUint32 length = aStream.ReadUint32L();
    HBufC8* modifier = HBufC8::NewLC(length);
    TPtr8 ptr(modifier->Des());
    aStream.ReadL(ptr, length); 

    TUint32 value = aStream.ReadUint32L();

   	CSdpBandwidthField* obj = new (ELeave) CSdpBandwidthField();
    CleanupStack::PushL(obj);
    obj->ConstructL(*modifier, value);

    CleanupStack::PopAndDestroy(); // modifier
    CleanupStack::Pop(obj);
    return obj;
    }

// -----------------------------------------------------------------------------
// CSdpBandwidthField::CSdpBandwidthField
// Constructor
// -----------------------------------------------------------------------------
//
CSdpBandwidthField::CSdpBandwidthField()
	{
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpBandwidthField::ConstructL(const TDesC8& aText)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
	RArray <TPtrC8> lines;
    lines = SdpUtil::DivideToLinesL(aText, KErrSdpCodecBandwidthField);
    CleanupClosePushL(lines);
    RArray<TPtrC8> bwArray;
    bwArray = SdpUtil::GetElementsFromLineL(lines[0], KColonChar, 
                                            KErrSdpCodecBandwidthField);
    CleanupClosePushL(bwArray);
    const TDesC8& bwName = iStringPool.StringF(
                                        SdpCodecStringConstants::EBandwidth,
                                        SdpCodecStringConstants::Table).DesC();       

    __ASSERT_ALWAYS(lines.Count() == KLines
                    && bwArray.Count() == KElements
                    && bwArray[0].Find(bwName) != KErrNotFound,
                    User::Leave(KErrSdpCodecBandwidthField));

    CopyModifierL(bwArray[1]);
    TLex8 lexer(bwArray[2]);
    lexer.Val(iValue, EDecimal);
    
    CleanupStack::PopAndDestroy(2); //lines, bwArray
    }

// -----------------------------------------------------------------------------
// CSdpBandwidthField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpBandwidthField::ConstructL(RStringF aModifier, TUint32 aValue)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
	iModifier = aModifier.Copy();
    iValue = aValue;
	}

// -----------------------------------------------------------------------------
// CSdpBandwidthField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpBandwidthField::ConstructL(const TDesC8& aModifier, TUint32 aValue)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
	CopyModifierL(aModifier);
    iValue = aValue;
	}


// -----------------------------------------------------------------------------
// CSdpBandwidthField::CopyModifierL
// Copies modifier to object after check
// -----------------------------------------------------------------------------
//
void CSdpBandwidthField::CopyModifierL(const TDesC8& aModifier)
    {
    RStringF modifierCT = iStringPool.StringF(
                                        SdpCodecStringConstants::EModifierCT,
                                        SdpCodecStringConstants::Table );
    RStringF modifierAS = iStringPool.StringF( 
                                        SdpCodecStringConstants::EModifierAS,
                                        SdpCodecStringConstants::Table );
    
    if (modifierCT.DesC().CompareF(aModifier) == 0)
        {
        iModifier.Close();
        iModifier = modifierCT.Copy();
        }
    else if ( modifierAS.DesC().CompareF(aModifier) == 0)
        {
        iModifier.Close();
        iModifier = modifierAS.Copy();
        }
    else
        {
        iModifier.Close();
        iModifier = iStringPool.OpenFStringL(aModifier);
        }
    }


