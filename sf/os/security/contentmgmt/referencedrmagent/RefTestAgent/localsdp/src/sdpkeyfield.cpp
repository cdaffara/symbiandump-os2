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
// Name          : SdpKeyField.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <uri8.h>
#include <s32strm.h>
#include "sdpkeyfield.h"
#include "sdputil.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecconstants.h"
#include "sdpcodecerr.h"
#include "sdpcodecstringpool.h"

// LOCAL CONSTANTS
const TText8 KPlusChar = '+';

// -----------------------------------------------------------------------------
// CSdpKeyField::DecodeL
// Decodes key field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField* CSdpKeyField::DecodeL(const TDesC8& aFieldValue)
	{
	CSdpKeyField* obj = DecodeLC(aFieldValue);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::DecodeL
// Decodes key field from TDesC
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField* CSdpKeyField::DecodeLC(const TDesC8& aFieldValue)
	{
	CSdpKeyField* obj = new (ELeave) CSdpKeyField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aFieldValue);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField* CSdpKeyField::NewL(RStringF aMethod,
                                          const TDesC8& aEncryptionKey)
	{
	CSdpKeyField* obj = NewLC(aMethod, aEncryptionKey);
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField* CSdpKeyField::NewLC(RStringF aMethod,
                                           const TDesC8& aEncryptionKey)
	{
	CSdpKeyField* obj = new (ELeave) CSdpKeyField();
	CleanupStack::PushL(obj);
	obj->ConstructL(aMethod, aEncryptionKey);
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::~CSdpKeyField
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField::~CSdpKeyField()
	{
    iMethod.Close();
	delete iEncryptionKey;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpKeyField::EncodeL(RWriteStream& aStream) const
	{
	__TEST_INVARIANT;
    RStringF header = iStringPool.StringF(SdpCodecStringConstants::EKey,
                                        SdpCodecStringConstants::Table);
    aStream.WriteL( header.DesC() );

    aStream.WriteL( iMethod.DesC() );
    if (iMethod != iStringPool.StringF(SdpCodecStringConstants::EMethodPrompt,
                                    SdpCodecStringConstants::Table))
        {
        aStream.WriteL( KColonStr );
        if (iEncryptionKey->Length() > 0)
            {
            aStream.WriteL( *iEncryptionKey );
            }
        }

    aStream.WriteL( KCRLFStr );
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::CloneL
// Creates an exact copy of the key field
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField * CSdpKeyField::CloneL() const
	{
	__TEST_INVARIANT;
	CSdpKeyField* obj = CSdpKeyField::NewL(iMethod, *iEncryptionKey);
	__ASSERT_DEBUG(*this == *obj, User::Panic(  KSdpCodecPanicCat,
                                                KSdpCodecPanicInternal));
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::operator ==
// Checks if two key fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpKeyField::operator == (const CSdpKeyField& aObj) const
	{
	__TEST_INVARIANT;
	return iMethod == aObj.iMethod
			&& ((iEncryptionKey == 0 && aObj.iEncryptionKey == 0)
				|| (iEncryptionKey != 0 && aObj.iEncryptionKey != 0
                && aObj.iEncryptionKey->Compare(*iEncryptionKey) == 0));
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::Method
// Returns method
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpKeyField::Method() const
	{
	__TEST_INVARIANT;
	return iMethod;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::EncryptionKey
// Returns encryption key
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpKeyField::EncryptionKey() const
	{
	__TEST_INVARIANT;
	return *iEncryptionKey;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::SetL
// Sets new values to key method and encryption key
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpKeyField::SetL(RStringF aMethod,
                                 const TDesC8& aEncryptionKey)
	{
    RStringF methodClear = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodClear,
                                        SdpCodecStringConstants::Table );
    RStringF methodBase64 = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodBase64,
                                        SdpCodecStringConstants::Table );
    RStringF methodUri = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodUri,
                                        SdpCodecStringConstants::Table );
    RStringF methodPrompt = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodPrompt,
                                        SdpCodecStringConstants::Table );

    if (aMethod == methodClear)
        {
        SetMethodClearAndKeyL(methodClear, aEncryptionKey);
        }
    else if (aMethod == methodBase64)
        {
        SetMethodBase64AndKeyL(methodBase64, aEncryptionKey);
        }
    else if (aMethod == methodUri)
        {
        SetMethodUriAndKeyL(methodUri, aEncryptionKey);
        }
    else if (aMethod == methodPrompt)
        {
        SetMethodPromptL(methodPrompt, aEncryptionKey);
        }
    else
        {
        User::Leave(KErrSdpCodecKeyField);
        }

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::ExternalizeL
// Externalizes the object to stream
// -----------------------------------------------------------------------------
//
void CSdpKeyField::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteUint32L( iMethod.DesC().Length() );
    aStream.WriteL( iMethod.DesC() );

    aStream.WriteUint32L( iEncryptionKey->Des().Length() );
	if (iEncryptionKey->Des().Length() > 0)
        {
        aStream.WriteL(iEncryptionKey->Des(), iEncryptionKey->Des().Length());
        }
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::InternalizeL
// Internalizes the object from stream
// -----------------------------------------------------------------------------
//
CSdpKeyField* CSdpKeyField::InternalizeL(RReadStream& aStream)
    {
    CSdpKeyField* self = new (ELeave) CSdpKeyField();
    CleanupStack::PushL(self);
    self->iStringPool = SdpCodecStringPool::StringPoolL();

    self->DoInternalizeL(aStream);

    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::CSdpKeyField
// Constructor
// -----------------------------------------------------------------------------
//
CSdpKeyField::CSdpKeyField()
	{
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpKeyField::ConstructL(const TDesC8& aText)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
    RArray <TPtrC8> lines;
    lines = SdpUtil::DivideToLinesL(aText, KErrSdpCodecKeyField);
    CleanupClosePushL(lines);

    TLex8 lexer(lines[0]);
    RArray<TPtrC8> keyArray = GetElementsFromLineL(lexer);
    CleanupClosePushL(keyArray);
    const TDesC8& keyName = iStringPool.StringF(SdpCodecStringConstants::EKey,
        SdpCodecStringConstants::Table).DesC();

    __ASSERT_ALWAYS(lines.Count() == 1
                    && keyArray.Count() >= 2 && keyArray.Count() <= 3
                    && keyArray[0].Find(keyName) != KErrNotFound,
                    User::Leave(KErrSdpCodecKeyField));

    RStringF method = iStringPool.OpenFStringL(keyArray[1]);
    CleanupClosePushL(method);

    if (keyArray.Count() == 3)
        {
        SetL(method, keyArray[2]);
        }
    else
        {
        SetL(method, KNullDesC8);
        }
    CleanupStack::PopAndDestroy(3);  // method, lines, keyArray
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpKeyField::ConstructL(RStringF aMethod, const TDesC8& aEncryptionKey)
	{
    iStringPool = SdpCodecStringPool::StringPoolL();
    SetL(aMethod, aEncryptionKey);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::DoInternalizeL
// Internalizes the object members from stream
// -----------------------------------------------------------------------------
//
void CSdpKeyField::DoInternalizeL(RReadStream& aStream)
    {
     // <method>
    TUint32 length = aStream.ReadUint32L();
    HBufC8* method = HBufC8::NewLC(length);
    TPtr8 ptr(method->Des());
    aStream.ReadL(ptr, length);

    RStringF methodStr = iStringPool.OpenFStringL(*method);
    CleanupStack::PopAndDestroy(); // method
    CleanupClosePushL(methodStr);

    length = aStream.ReadUint32L();
    if (length > 0)
        {
        HBufC8* key = HBufC8::NewLC(length);
        ptr.Set(key->Des());
        aStream.ReadL(ptr, length);

        SetL(methodStr, *key);
        CleanupStack::PopAndDestroy(); //key
        }
    else
        {
        SetL(methodStr, KNullDesC8);
        }
    CleanupStack::PopAndDestroy(); // methodStr
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::SetMethodAndKeyL
// Sets new values to key method and encryption key
// -----------------------------------------------------------------------------
//
void CSdpKeyField::SetMethodAndKeyL(RStringF aMethod,
                                    const TDesC8& aEncryptionKey,
                                    TBool aAllowEmptyKeyValue)
    {
    HBufC8* tempKey = aEncryptionKey.AllocL();
    tempKey->Des().Trim();
    if (tempKey->Length() == 0 && !aAllowEmptyKeyValue)
        {
        delete tempKey;
        User::Leave(KErrSdpCodecKeyField);
        }
    iMethod.Close();
    iMethod = aMethod.Copy();
    delete iEncryptionKey;
    iEncryptionKey = tempKey;
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::SetMethodClearAndKeyL
// Sets key method to clear and new encryption key
// -----------------------------------------------------------------------------
//
void CSdpKeyField::SetMethodClearAndKeyL(RStringF aMethod,
                                         const TDesC8& aEncryptionKey)
    {
    __ASSERT_ALWAYS(SdpUtil::IsByteString(aEncryptionKey),
                    User::Leave(KErrSdpCodecKeyField));
    SetMethodAndKeyL(aMethod, aEncryptionKey, EFalse);
	}

// -----------------------------------------------------------------------------
// CSdpKeyField::SetMethodBase64AndKeyL
// Sets key method to Base64 and new encryption key
// -----------------------------------------------------------------------------
//
void CSdpKeyField::SetMethodBase64AndKeyL(RStringF aMethod,
                                          const TDesC8& aText)
    {
    __ASSERT_ALWAYS(aText.Length() > 0, User::Leave(KErrSdpCodecKeyField));

    for (TInt i = 0; i < aText.Length(); i++)
        {
        if ( !( ( aText[i] >= 'a' && aText[i] <= 'z' ) ||
                ( aText[i] >= 'A' && aText[i] <= 'Z' ) ||
                ( aText[i] >= '0' && aText[i] <= '9' ) ||
                ( aText[i] == KPlusChar ) ||
                ( aText[i] == KSlashChar ) ||
                ( aText[i] == KEqualChar ) ) )
            {
            User::Leave(KErrSdpCodecKeyField);
            }
        }
    SetMethodAndKeyL(aMethod, aText, EFalse);
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::SetMethodUriAndKeyL
// Sets key method to uri and new encryption key
// -----------------------------------------------------------------------------
//
void CSdpKeyField::SetMethodUriAndKeyL(RStringF aMethod, const TDesC8& aText)
    {
    TUriParser8 parser;
    User::LeaveIfError(parser.Parse(aText));
    SetMethodAndKeyL(aMethod, aText, EFalse);
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::SetMethodPromptL
// Sets key method to Prompt and new encryption key
// -----------------------------------------------------------------------------
//
void CSdpKeyField::SetMethodPromptL(RStringF aMethod, const TDesC8& aText)
    {
    __ASSERT_ALWAYS(aText.Length() == 0, User::Leave(KErrSdpCodecKeyField));
    SetMethodAndKeyL(aMethod, aText, ETrue);
    }

// -----------------------------------------------------------------------------
// CSdpKeyField::GetElementsFromLineL
// Gets all the elements from a single line
// -----------------------------------------------------------------------------
//
RArray<TPtrC8> CSdpKeyField::GetElementsFromLineL( TLex8& aLexer)
    {
    RArray<TPtrC8> lineArray;
    CleanupClosePushL(lineArray);

    aLexer.Mark();
    for (TChar curChar(KColonChar); curChar != KEqualChar;)
        {
        curChar = aLexer.Peek();
        if (curChar == KEofChar || curChar == KLFChar ||
             curChar == KCRChar)
            {
            User::Leave(KErrSdpCodecKeyField);
            }
        aLexer.Inc();
        }
    User::LeaveIfError(lineArray.Append(aLexer.MarkedToken()));

    TBool colonFound = EFalse;
    TBool eofcFound = EFalse;
    while (!eofcFound)
        {
        aLexer.Mark();
        while ((aLexer.Peek() != KColonChar || colonFound) &&
                aLexer.Peek() != KCRChar &&
                aLexer.Peek() != KLFChar &&
                aLexer.Peek() != KEofChar)
            {
            aLexer.Inc();
            }

        if (aLexer.MarkedToken().Length() > 0)
            {
            User::LeaveIfError(lineArray.Append(aLexer.MarkedToken()));
            }
        else
            {
            User::Leave(KErrSdpCodecKeyField);
            }

        if (aLexer.Peek() == KColonChar)
            {
            colonFound = ETrue;
            }

        if (aLexer.Peek() == KCRChar)
            {
            aLexer.Inc();
            }
        if (aLexer.Peek() == KLFChar)
            {
            aLexer.Inc();
            if (aLexer.Peek() == KEofChar)
                {
                eofcFound = ETrue;
                }
            }
        else
            {
            aLexer.Inc();
            }
        }

    CleanupStack::Pop();
    return lineArray;
    }

void CSdpKeyField::__DbgTestInvariant() const
	{
    RStringF methodClear = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodClear,
                                        SdpCodecStringConstants::Table );
    RStringF methodBase64 = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodBase64,
                                        SdpCodecStringConstants::Table );
    RStringF methodUri = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodUri,
                                        SdpCodecStringConstants::Table );
    RStringF methodPrompt = iStringPool.StringF(
                                        SdpCodecStringConstants::EMethodPrompt,
                                        SdpCodecStringConstants::Table );

    TBool invariant =   (iMethod == methodPrompt
	                        && (iEncryptionKey != 0 && (iEncryptionKey->Length() == 0)) )
						||  ((iMethod == methodClear || iMethod == methodBase64
	                            || iMethod == methodUri)
                        && (iEncryptionKey != 0 && (iEncryptionKey->Length() != 0)) );

	if (!invariant)
		User::Invariant();
	}
