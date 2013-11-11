/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CHtiMessage implementation
*
*/


#include "HtiMessage.h"
#include "HtiLogging.h"

//format constants

const TInt KMsgServiceNameOffset = 0;
const TInt KMsgBodySizeLen = 4;
const TInt KMsgBodySizeOffset = KMsgServiceNameOffset + KHtiMsgServiceUidLen;
const TInt KMsgVersionOffset = KMsgBodySizeOffset+KMsgBodySizeLen;
const TInt KMsgPriorityOffset = KMsgVersionOffset+1;
const TInt KMsgWrapFlagOffset = KMsgPriorityOffset+1;
const TInt KMsgExtSizeOffset = KMsgWrapFlagOffset+1;
const TInt KMsgCrcOffset = KMsgExtSizeOffset+1; //two bytes field
const TInt KMsgExtOffset = KMsgCrcOffset+2;

const TInt KMsgHeaderMinSize = KMsgExtOffset;

const TInt KMsgMaxBodySize = 0xFFFFFF; //max in KMsgBodySizeLen bytes
const TInt KMsgMaxExtSize = 0xFF;

const TUint16 KCrcInitValue = 0xFFFF;

const TUint8 KDefaultVersion = 1;

const TInt CHtiMessage::iLinkOffset = _FOFF( CHtiMessage, iLink );

CHtiMessage::CHtiMessage():
    iBody( NULL ),
    iBodyDes( NULL ),
    iBodySize( 0 ),
    iServiceUid(),
    iExtRemainderSize( 0 )
    {

    }

CHtiMessage::~CHtiMessage()
    {
    HTI_LOG_FUNC_IN("~CHtiMessage");
    delete iHeader;
    delete iBodyDes;
    delete iBody; //delete if incomplete message is destructed
    HTI_LOG_FUNC_OUT("~CHtiMessage");
    }

CHtiMessage* CHtiMessage::NewL(const TDesC8& aMessage)
    {
    CHtiMessage* obj = new(ELeave) CHtiMessage();
    CleanupStack::PushL(obj);
    obj->ConstructL( aMessage );
    CleanupStack::Pop();
    return obj;
    }

CHtiMessage* CHtiMessage::NewL( TDesC8* aMessageBody,
                               const TUid aServiceUid,
                               TBool aWraped,
                               TInt aPriority)
    {
    CHtiMessage* obj = new(ELeave) CHtiMessage();
    CleanupStack::PushL(obj);
    obj->ConstructL( aServiceUid, aMessageBody,
                    KNullDesC8, aWraped, aPriority );
    CleanupStack::Pop();
    return obj;
    }

CHtiMessage* CHtiMessage::NewL( TDesC8* aMessageBody,
                               const TUid aServiceUid,
                               const TDesC8& aExtBody,
                               TBool aWraped,
                               TInt aPriority)
    {
    CHtiMessage* obj = new(ELeave) CHtiMessage();
    CleanupStack::PushL(obj);
    obj->ConstructL( aServiceUid, aMessageBody,
            aExtBody, aWraped, aPriority );
    CleanupStack::Pop();
    return obj;
    }

void CHtiMessage::ConstructL(const TUid aServiceUid,
        TDesC8* aMessageBody,
        const TDesC8& aExtBody,
        TBool aWraped,
        TInt aPriority)
    {
    if ( aMessageBody->Length()>KMsgMaxBodySize ||
         aExtBody.Length()>KMsgMaxExtSize)
        {
        User::Leave(KErrArgument);
        }

    iBodyDes = aMessageBody;
    iBodySize = iBodyDes->Length();

    //allocate header space
    iHeader = new(ELeave) TUint8[MinHeaderSize() + aExtBody.Length()];

    //set UID
    *((TInt32*)(iHeader+KMsgServiceNameOffset)) = aServiceUid.iUid;
    iServiceUid = aServiceUid;

    //set msg body size in little-endian
    *((TUint32*)(iHeader+KMsgBodySizeOffset)) = iBodySize;

    //set version
    iHeader[KMsgVersionOffset] = KDefaultVersion;

    //priority
    iHeader[KMsgPriorityOffset] = aPriority;

    //wrapped flag
    iHeader[KMsgWrapFlagOffset] = aWraped?1:0;

    //ext size
    iHeader[KMsgExtSizeOffset] = aExtBody.Length();

    //set CRC16
    TUint16 crc16 = KCrcInitValue;
    Mem::Crc( crc16, iHeader, KMsgCrcOffset );
    //put crc16 in little-endian format
    *((TUint16*)(iHeader + KMsgCrcOffset)) = crc16;

    if ( aExtBody.Length()>0 )
        {
        Mem::Copy( iHeader + KMsgExtOffset,
                   aExtBody.Ptr(),
                   aExtBody.Length() );
        }
    }

void CHtiMessage::ConstructL(const TDesC8& aMessage)
    {
    if ( !CheckValidHtiHeader( aMessage ) )
        {
        User::Leave( KErrArgument );
        }

    const TUint8* src = aMessage.Ptr();
    iBodySize = * ( ( TUint32* ) ( src + KMsgBodySizeOffset ) );
    TInt extSize = src[KMsgExtSizeOffset];
    TInt headerSize = MinHeaderSize() + extSize;

    //allocate header space
    iHeader = new ( ELeave ) TUint8[headerSize];
    iBody = HBufC8::NewL( iBodySize );

    //copy header wo ext
    Mem::Copy( iHeader, src, MinHeaderSize() );

    //set iServiceUid
    iServiceUid.iUid = *( ( TInt32* ) ( src + KMsgServiceNameOffset ) );

    //copy ext
    if ( extSize>0 )
        {
        if ( aMessage.Length() >= headerSize )
            {
            //copy whole extension section
            Mem::Copy( iHeader + KMsgExtOffset, src + MinHeaderSize(),
                       extSize );
            }
        else
            {
            //copy part
            TInt copyLen = aMessage.Length() - MinHeaderSize();
            Mem::Copy( iHeader + KMsgExtOffset, src + MinHeaderSize(),
                       copyLen );
            iExtRemainderSize = extSize - copyLen;
            }
        }

    //copy body
    if ( iExtRemainderSize==0 ) //if it's not 0, then there is nothing in
        {                       //aMessage left
        TInt availableData = aMessage.Length() - headerSize;
        if ( availableData >= iBodySize )
            {
            iBody->Des().Copy( aMessage.Mid( headerSize, iBodySize ) );
            //body ready
            iBodyDes = iBody;
            iBody = NULL;
            }
        else
            {
            iBody->Des().Copy( aMessage.Mid( headerSize, availableData ) );
            }
        }
    }

TBool CHtiMessage::CheckValidHtiHeader( const TDesC8& aMessage )
    {
    TBool parsingResult = aMessage.Length() >= MinHeaderSize();
    if ( parsingResult )
        {
        //check CRC16
        const TUint8* data = aMessage.Ptr();
        TUint16 headerCrc16 = * ( ( TUint16* ) ( data + KMsgCrcOffset ) );
        TUint16 calcCrc16 = KCrcInitValue;
        Mem::Crc( calcCrc16, data,  KMsgCrcOffset );
        parsingResult = ( headerCrc16 == calcCrc16 );
        }
    return parsingResult;
    }

TInt CHtiMessage::Size( const TDesC8& aMessage )
    {
    const TUint8* data = aMessage.Ptr();
    return ( *( ( TUint32* ) ( data + KMsgBodySizeOffset ) ) ) + KMsgExtOffset
             + data[KMsgExtSizeOffset];
    }

TBool CHtiMessage::IsBodyComplete() const
    {
    return iBody==NULL;
    }

TInt CHtiMessage::AddToBody( const TDesC8& aBodyPart )
    {
    if ( ( aBodyPart.Length() + iBody->Length() ) >= iBodySize )
        {
        //body ready
        TInt copyLen = iBodySize - iBody->Length();
        iBody->Des().Append( aBodyPart.Left( copyLen ) );

        iBodyDes = iBody;
        iBody = NULL;

        return copyLen;
        }
    else
        {
        //continue
        iBody->Des().Append( aBodyPart );
        }
    return aBodyPart.Length();
    }

TInt CHtiMessage::MinHeaderSize()
    {
    return KMsgHeaderMinSize;
    }

TUid CHtiMessage::DestinationServiceUid() const
    {
    return iServiceUid;
    }

TInt CHtiMessage::Size() const
    {
    return BodySize() + HeaderSize();
    }

TInt CHtiMessage::BodySize() const
    {
    return iBodySize;
    }

TInt CHtiMessage::Priority() const
    {
    return iHeader[KMsgPriorityOffset];
    }

TBool CHtiMessage::IsWrapped() const
    {
    return iHeader[KMsgWrapFlagOffset]!=0;
    }

TInt CHtiMessage::ExtSize() const
    {
    return iHeader[KMsgExtSizeOffset];
    }

TPtrC8 CHtiMessage::Extension() const
    {
    return TPtrC8( iHeader + KMsgExtOffset, ExtSize() );
    }

TInt CHtiMessage::HeaderSize() const
    {
    return KMsgExtOffset + ExtSize();
    }

TPtrC8 CHtiMessage::Header() const
    {
    return TPtrC8( iHeader, HeaderSize() );
    }

TPtrC8 CHtiMessage::Body() const
    {
    if ( iBodyDes != NULL )
        {
        return TPtrC8( *iBodyDes );
        }
    return TPtrC8( KNullDesC8 );
    }
