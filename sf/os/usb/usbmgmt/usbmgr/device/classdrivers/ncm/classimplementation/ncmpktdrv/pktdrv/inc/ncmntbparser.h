/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
/**
@file
@internalComponent
*/
#ifndef NCMNTBPARSER_H
#define NCMNTBPARSER_H

#include <e32base.h>

//DEBUG MACRO to dump the parsed NDP info.
//#define DEBUG_DUMP_NTBINNER
#ifdef DEBUG_DUMP_NTBINNER
#include <e32debug.h>
#endif

#ifndef __OVER_DUMMYUSBSCLDD__
#include <nifmbuf.h>
#else
#include <usb/testncm/pktdrv.h>
#endif

#include "ncmpktdrvcommon.h"

/**
 * interface for parsed NDP(ethernet frame).
 */
NONSHARABLE_CLASS(MNcmNdpFrameObserver)
    {
public:
    virtual void ProcessEtherFrame(RMBufPacket&) = 0;
    virtual void ExpireBuffer(TAny*) = 0;
    };

#ifdef DEBUG_DUMP_NTBINNER
class NTHNDPDumper : public MNcmNdpFrameObserver
    {
public:
    inline void ProcessEtherFrame(RMBufPacket& aPacket)
        {
        RDebug::Printf("NTHNDPDumper::parsed Ethernet Frame, length=%d", aPacket.Length());
        aPacket.Free();
        }
    inline void ExpireBuffer(TAny* aBuf)
        {
        }
    inline void FatalError(TInt aErrCode)
        {
        RDebug::Printf("NTHNDPDumper::Fatal Error=%d", aErrCode);
        }
    };
#endif //DEBUG_DUMP_NTBINNER

/**
 * NCM NTH for NTB16
 */
NONSHARABLE_CLASS(TNcmNth16)
    {
public:
    TUint8    dwSignature[4];
    TUint16   wHeaderLength;
    TUint16   wSequence;
    TUint16   wBlockLength;
    TUint16   wNdpIndex;

public:
    inline void Dump();
    };

/**
 * NCM NDP 16
 */
NONSHARABLE_CLASS(TNcmNdp16)
    {
public:
    TUint8    dwSignature[4];
    TUint16   wLength;
    TUint16   wNextNdpIndex;
    TUint16   wDatagram0Index;
    TUint16   wDatagram0Length;
    TUint16   wDatagram1Index;
    TUint16   wDatagram1Length;

public:
    inline void Dump();
    };

/**
 * NCM packet parser
 *    NOTE:the buffer to be parsed was not guaranteed as a whole USB transfer.
 *         So, a internal state is used to remember previous parsed result.
 */
NONSHARABLE_CLASS(CNcmNtbParser) : public CBase
    {
public:
    virtual TInt Parse(const TUint8* aBuf, TInt aBufLen, TBool aZlp = EFalse) = 0;
    virtual void Reset(TInt aResetType=0) = 0;
    virtual TUint NtbOutMaxSize() const = 0;
    virtual ~CNcmNtbParser();

protected:
    inline CNcmNtbParser(MNcmNdpFrameObserver&);

    MNcmNdpFrameObserver& iObserver;
    };

/**
 * NCM NTB 16 parser
 */
NONSHARABLE_CLASS(CNcmNtb16Parser): public CNcmNtbParser
    {
public:
    //From CNcmNtbParser
    TInt Parse(const TUint8* aBuf, TInt aBufLen, TBool aZlp = EFalse);
    TUint NtbOutMaxSize() const;
    void Reset(TInt aType = 0);

    virtual ~CNcmNtb16Parser();

    static CNcmNtb16Parser* NewL(MNcmNdpFrameObserver&);

private:
    inline CNcmNtb16Parser(MNcmNdpFrameObserver&);
    //parse NTH
    TInt ParseNcmNtb16Header(const TUint8* aBuf, TInt aBufLen, TBool aZlp);
    //parse NDP
    TInt ParseNcmNtb16Ndp(const TUint8* aBuf, TInt aBufLen);
    //parse NdpDatagram
    TInt ParseNcmNtb16NdpDatagram(const TUint8* aBuf, TInt aBufLen);
    //queue buffer.
    TInt QueueBuf(const TUint8* aBuf, TInt aBufLen);
    TUint8 DataTUint8(TUint aOffset, const TUint8* aBuf, TInt aBufLen);
    TUint16 DataTUint16(TUint aOffset, const TUint8* aBuf, TInt aBufLen);
    TInt DataPacket(const TUint8* aBuf, TInt aBufLen);
    //search NCMH in the stream if the NTH header signature is wrong.
    TInt SearchNcmHead(const TUint8* aBuf, TInt aBufLen);


    enum TNcmNtb16ResetType
        {
        ENcmNtb16TypeResetAll = 0,
        ENcmNtb16TypeResetQueue = 1,
        ENcmNtb16TypeResetData = 2
        };

    enum TNcmNtb16ParseState
        {
        //initial state, NTH is not ready.
        ENcmNtb16ParseStateUninitialized = 0,
        //NTH is ready, NDP is not ready.
        ENcmNtb16ParseStateNdpParsing,
        //NDP is ready, NdpDatagram is not finished.
        ENcmNtb16ParseStateNdpSeeking,
        //Error state or NULL NDP, ignore coming bytes until this NTB end.
        ENcmNtb16ParseStateSkipBytes
        };

    enum TNcmNtb16ParseNdpState
        {
        ENcmNtb16ParseNdpStateUninitialized = 0,
        ENcmNtb16ParseNdpStateDatagramed
        };

private:
    TNcmNth16              iNth;
    TNcmNdp16              iNdp;

    TNcmNtb16ParseState    iState;

    TUint                  iDataLen; //NTB data length.
    TUint                  iQueLen;  //Queue length.
    TUint                  iDataOffset; //start offset in first buffer in queue.
    RArray<TPtrC8>         iBufQueue;

    TNcmNtb16ParseNdpState iNdpState;
    TUint                  iNdpDataOffset;
    TUint                  iNdpBound;
    TUint16                iDatagramIndex;
    TUint16                iDatagramLength;

    RMBufPacket            iPacket;

#ifdef _DEBUG
    //error statistics
    TUint16                iPrevSeq;
    TUint                  iTotalSeq;
#endif

    TUint16                iMaxSize;
    };

// inline functions
#include "ncmntbparser.inl"

#endif /* NCMNTBPARSER_H_ */
