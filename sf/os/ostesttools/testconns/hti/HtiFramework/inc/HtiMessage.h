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
* Description:  Hti Message incapsulates HTI protocol messages structure.
*
*/


#ifndef MESSAGEPARSER_H__
#define MESSAGEPARSER_H__

#include <e32std.h>
#include <e32base.h>


const TInt KDefaultPriority = 0;
const TInt KPriorityMask = 0x2;
const TInt KHtiMsgServiceUidLen = 4;


NONSHARABLE_CLASS(CHtiMessage)  : public CBase
    {
public:
    /**
    * Constructs and parses a new message from descriptor in question.
    * This method is used to construct hti message from incoming data.
    * The descriptor must start from HTI header.
    * Available amount of data is copied from the descriptor depending
    * on the message size value in the header. The remaining data of
    * message body must be added using method AddToBody().
    *
    * @return new Hti message
    *
    * @see CheckValidHeader() for function to define either descriptor
    *               starts from a valid Hti message header
    * @see AddToBody()
    */
    static CHtiMessage* NewL(const TDesC8& aMessage);

    /**
    * Creates a new HTI message without extension.
    * Can leave with KErrArgument if passed data are invalid
    * or other system-wide leave code (e.g. KErrNoMemory).
    * Ownership of aMessagBody is transfered.
    * This method is used to create a new outgoing HTI message.
    *
    * @param aMessageBody message body content
    * @param aServiceUid target service name
    * @param aWraped flag to indicate either
    *       Security manager must wrap message body
    * @param aPriority message priority
    *
    */
    static CHtiMessage* NewL(TDesC8* aMessageBody,
        const TUid aServiceUid,
        TBool aWraped = EFalse,
        TInt aPriority = KDefaultPriority);

    /**
    * Creates a new HTI message with extension.
    * Can leave with KErrArgument if passed data are invalid
    * or other system-wide leave code (e.g. KErrNoMemory).
    * Ownership of aMessagBody is transfered.
    * This method is used to create a new outgoing HTI message.
    *
    * @param aMessageBody message body content
    * @param aServiceUid target service name
    * @param aExtBody extension content
    * @param aWraped flag to indicate either
    *       Security manager must wrap message body
    * @param aPriority message priority
    *
    */
    static CHtiMessage* NewL(TDesC8* aMessageBody,
        const TUid aServiceUid,
        const TDesC8& aExtBody,
        TBool aWraped = EFalse,
        TInt aPriority = KDefaultPriority);

    /**
    * Deletes all message data allocated in heap.
    */
    virtual ~CHtiMessage();

    /**
    * Returns ETrue when the descriptor in question starts from
    * valid HTI header.
    *
    * @param aMessage descriptor with probable HTI message
    *
    * @return ETrue HTI header is valid
    */
    static TBool CheckValidHtiHeader(const TDesC8& aMessage);

    /**
    * Returns message size based on the header in question
    *
    * @param aMessage descriptor with an HTI message
    */
    static TInt Size(const TDesC8& aMessage);

    /**
    * Returns the minimal size of HTI message header.
    *
    */
    static TInt MinHeaderSize();

    /**
    * Check either HTI message body has body of required size as specified
    * in Hti header.
    */
    TBool IsBodyComplete() const;

    /**
    * Appends to the body a new part. If the descriptor length bigger
    * than remaining space for message body, only part
    * of the descritpor is copied to fill in all available space.
    *
    * @param aBodyPart descriptor with remaining message body
    *
    * @return number of bytes copied from the descriptor
    */
    TInt AddToBody(const TDesC8& aBodyPart);

    /**
    * Returns the size of a whole message (BodySize()+HeaderSize())
    */
    TInt Size() const;

    /**
    * Returns message service uid
    */
    TUid DestinationServiceUid() const;

    /**
    * Returns message body size
    */
    TInt BodySize() const;

    /**
    * Returns message priority
    */
    TInt Priority() const;

    /**
    * Returns wrap flag
    */
    TBool IsWrapped() const;

    /**
    * Returns header extension
    */
    TPtrC8 Extension() const;

    /**
    * Returns extention section size
    */
    TInt ExtSize() const;

    /**
    * Returns header size including extension section.
    */
    TInt HeaderSize() const;

    /**
    * Returns message body
    */
    TPtrC8 Body() const;

    /**
    * Return message header
    */
    TPtrC8 Header() const;

public:
    /**
    * Returns the offset of a link data member
    */
    static const TInt iLinkOffset;

protected:
    /**
    * Constructor
    */
    CHtiMessage();

    /**
    * Second-level constructors
    */
    void ConstructL(const TDesC8& aMessage);
    void ConstructL(const TUid aServiceUid,
            TDesC8* aMessageBody,
            const TDesC8& aExtBody,
            TBool aWraped,
            TInt aPriority);
    /**
    * helper function that make TInt from 3 bytes in big-endian format
    * @param aSrc bytes with integer
    *
    * @return integer
    */
    //static TInt ParseLittleEndian3(const TUint8* aSrc);

private:
    /**
    * Link used when creating message queues
    */
    TSglQueLink iLink;

    /**
    * Message body used when constructing from parts
    * when body is ready pointer is copied to iBodyDes
    */
    HBufC8* iBody;

    /**
    * Complete body, used for access methods
    */
    TDesC8* iBodyDes;

    /**
    * Message header in binary format
    */
    TUint8* iHeader;

    /**
    * Body size copied from iHeader for quick access
    */
    TInt iBodySize;

    /**
    * Service UID extracted from iHeader.
    */
    TUid iServiceUid;

    /**
    * Size of extension section to be added to the header
    * Used during message construction from parts
    */
    TInt iExtRemainderSize;
    };

#endif
