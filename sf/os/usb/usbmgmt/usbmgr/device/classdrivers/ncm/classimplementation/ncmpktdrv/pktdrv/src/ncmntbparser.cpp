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

#include "ncmntbparser.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmntbparserTraces.h"
#endif


#ifdef _DEBUG
_LIT(KNTBParsePanic, "NtbParsePanic");
#endif


const TUint  KNcmNtb16HeadLength = sizeof(TNcmNth16);
const TUint  KNcmNtb16NdpMiniLength = sizeof(TNcmNdp16);
const TUint  KNcmNtb16NdpDatagramOffset = 4 + 2 + 2;//dwSignature + wLength + wNextNdpIndex;
const TUint  KNcmNtb16NdpDatagramLength = 2 + 2;//wDatagram0Index + wDatagram0Length;
const TUint  KNcmNtb16NdpDatagramLenOffset = 2;//wDatagram0Index
const TUint8 KLetterN = 0x4E;
const TUint8 KLetterC = 0x43;
const TUint8 KLetterM = 0x4D;
const TUint8 KLetterH = 0x48;
const TUint8 KLetter0 = 0x30;

CNcmNtb16Parser* CNcmNtb16Parser::NewL(MNcmNdpFrameObserver& aObserver)
    {
    return new(ELeave)CNcmNtb16Parser(aObserver);
    }

CNcmNtb16Parser::~CNcmNtb16Parser()
    {
    Reset();
    }

CNcmNtbParser::~CNcmNtbParser()
    {
    }

TUint CNcmNtb16Parser::NtbOutMaxSize() const
    {
    return iMaxSize;
    }

/**
 * Parsing a buffer as NCM Ntb16 data format.
 * NOTE: this function has "memory" because the Shared Chunk LDD can't guarantee the buffer as a whole USB Transfer.
 *       this function tries to parse the buffer as much as possible and save the buffer if it can't be parsed exactly.
 * @param   [in] aBuf, the buffer to be parsed, which can't be released(free) until the ExpireBuffer() is called.
 * @param   [in] aBufLen, the buffer length.
 * @param   [in] aZlp, flag of Zlp used to indicate a USB transfer completion.
 * @return  KErrNone, no error occurs during parsing this buffer.
 *          other, Symbian system error code.
 */
TInt CNcmNtb16Parser::Parse(const TUint8* aBuf, TInt aBufLen, TBool aZlp)
    {
    TInt ret = KErrNone;
    TInt dataLen = iDataLen + aBufLen - (iQueLen==0?iDataOffset:0);
    OstTraceExt3( TRACE_NORMAL, CNCMNTB16PARSER_PARSE, "==>Parse:aBuf=0x%08x, aBufLen=%d, aZlp=%d.", (TUint)aBuf, aBufLen, aZlp);
    OstTraceExt5( TRACE_NORMAL, CNCMNTB16PARSER_PARSE_DUP01, " iState=%d, dataLen=%d, iDataLen=%d, iQueLen=%d, iDataOffset=%d", iState, dataLen, iDataLen, iQueLen, iDataOffset);

    //Zlp comes so late, so updated here.
    if (aZlp)
        {
        iNth.wBlockLength = dataLen;
        __ASSERT_DEBUG(iNth.wBlockLength < iMaxSize, User::Panic(KNTBParsePanic, __LINE__));
        }

    switch(iState)
        {
        //iNth is not ready.
        case ENcmNtb16ParseStateUninitialized:
            if (dataLen >= KNcmNtb16HeadLength)
                {
                ret = ParseNcmNtb16Header(aBuf, aBufLen, aZlp);
                }
            else
                {
                ret = QueueBuf(aBuf, aBufLen);
                }
            break;

        //iNth is ready, but iNdp not.
        case ENcmNtb16ParseStateNdpParsing:
            if (dataLen >= iNth.wNdpIndex + KNcmNtb16NdpMiniLength)
                {
                ret = ParseNcmNtb16Ndp(aBuf, aBufLen);
                }
            else
                {
                ret = QueueBuf(aBuf, aBufLen);
                }
            break;

        //iNdp is ready, try to seek datagram.
        case ENcmNtb16ParseStateNdpSeeking:
            ret = ParseNcmNtb16NdpDatagram(aBuf, aBufLen);
            break;

        //iNth is ready, ignore the remaining bytes for errors or NULL NDP.
        case ENcmNtb16ParseStateSkipBytes:
            __ASSERT_DEBUG(0==iQueLen, User::Panic(KNTBParsePanic, __LINE__));
            __ASSERT_DEBUG(0==iDataLen||0==iDataOffset, User::Panic(KNTBParsePanic, __LINE__));
            //buffer contains the following NTB data.
            if (dataLen > iNth.wBlockLength)
                {
                iDataOffset += iNth.wBlockLength - iDataLen;
                Reset();
                ret = Parse(aBuf, aBufLen);
                }
            else if (dataLen == iNth.wBlockLength)
                {
                Reset();
                iDataOffset = 0;
                iObserver.ExpireBuffer((TAny*)aBuf);
                }
            else
                {
                iDataLen += aBufLen;
                iObserver.ExpireBuffer((TAny*)aBuf);
                }
            break;

        default:
            __ASSERT_DEBUG(0, User::Panic(KNTBParsePanic, __LINE__));
            break;
        }
    OstTraceExt3(TRACE_NORMAL, CNCMNTB16PARSER_PARSE_DUP02, "<==Parse:iDataLen=%d, iQueLen=%d, iDataOffset=%d.", (TInt)iDataLen, iQueLen, iDataOffset);

    return ret;
    }

//Reset the parser to clear the queue and indicators.
void CNcmNtb16Parser::Reset(TInt aType)
    {
    if (ENcmNtb16TypeResetAll == aType)
        {
        iState = ENcmNtb16ParseStateUninitialized;
        iDataLen = 0;
        }

    if (iQueLen > 0)
        {
        for (TInt i = 0; i < iQueLen; i++)
            {
            iObserver.ExpireBuffer((TAny*)(iBufQueue[i].Ptr()));
            }
        iBufQueue.Reset();
        iQueLen = 0;
        iDataOffset = 0;
        }
    }

//Search NCM header:"NCMH".
TInt CNcmNtb16Parser::SearchNcmHead(const TUint8* aBuf, TInt aBufLen)
    {
    TInt dataLen = aBufLen + (iQueLen==0?(0-iDataOffset):iDataLen);
    const TInt KNotFound = 0;
    const TInt KShrinkQueue = 1;
    const TInt KFound = 2;
    TInt ii = 1;//first one(0) has already tested(not matched) before entering this function.
    TInt ret = KNotFound;

    const TUint KMatchLastOne = 1;
    const TUint KMatchLastTwo = 2;
    const TUint KMatchLastThree = 3;

    while (KNotFound == ret)
        {
        while (ii < dataLen && KLetterN != DataTUint8(ii, aBuf, aBufLen))
            {
            ii++;
            }
        if (ii == dataLen)
            {
            Reset();
            iDataOffset = 0;
            iObserver.ExpireBuffer((TAny*)aBuf);
            return KErrCorrupt;
            }
        else if (ii + KMatchLastOne == dataLen)//last one matched 'N'
            {
            ret = KShrinkQueue;
            }
        else if (ii + KMatchLastTwo == dataLen)
            {
            if (KLetterC == DataTUint8(ii+KMatchLastOne, aBuf, aBufLen))//'NC' matched.
                {
                ret = KShrinkQueue;
                }
            else
                {
                ii++;
                }
            }
        else if (ii + KMatchLastThree == dataLen)
            {
            if (KLetterC != DataTUint8(ii+KMatchLastOne, aBuf, aBufLen))
                {
                ii++;
                }
            else if (KLetterM != DataTUint8(ii+KMatchLastTwo, aBuf, aBufLen))
                {
                ii += 2;
                }
            else //'NCM' matched.
                {
                ret = KShrinkQueue;
                }
            }
        else
            {
            if (KLetterC != DataTUint8(ii+KMatchLastOne, aBuf, aBufLen))
                {
                ii ++;
                }
            else if (KLetterM != DataTUint8(ii+KMatchLastTwo, aBuf, aBufLen))
                {
                ii += 2;
                }
            else if (KLetterH != DataTUint8(ii+KMatchLastThree, aBuf, aBufLen))
                {
                ii += 3;
                }
            else
                {
                ret = KFound;
                }
            }
        }

    OstTraceExt5(TRACE_NORMAL, CNCMNTB16PARSER_SEARCHNCMHEAD, "Search:ii=%d, dataLen=%d,iDataLen=%d,iDataOffset=%d,iQueLen=%d", ii, dataLen, iDataLen, iDataOffset, iQueLen);

    switch(ret)
        {
        case KShrinkQueue:
            {
            TInt err = QueueBuf(aBuf, aBufLen);
            if (KErrNone != err)
                {
                return err;
                }
            }
            //fall-through!! no break!!
        case KFound:
            {
            TInt newOffset = ii + iDataOffset;
            iDataLen += (iDataLen!=0?iDataOffset:0);
            while(iQueLen && iBufQueue[0].Length() <= newOffset)
                {
                iObserver.ExpireBuffer((TAny*)iBufQueue[0].Ptr());
                newOffset -= iBufQueue[0].Length();
                iDataLen -= iBufQueue[0].Length();
                iBufQueue.Remove(0);
                iQueLen--;
                }
            iDataOffset = newOffset;
            iDataLen -= (iQueLen!=0?iDataOffset:0);
            __ASSERT_DEBUG((iQueLen==0&&iDataLen==0)||(iQueLen>0&&iDataLen>0), User::Panic(KNTBParsePanic, __LINE__));
            if (KFound ==  ret)
                {
                return Parse(aBuf, aBufLen);
                }
            }
            break;

        default:
            __ASSERT_DEBUG(0, User::Panic(KNTBParsePanic, __LINE__));
            break;
        }

    return KErrNone;
    }

TInt CNcmNtb16Parser::ParseNcmNtb16Header(const TUint8* aBuf, TInt aBufLen, TBool aZlp)
    {
    const TUint KLetterNOffset = 0;                   //0
    const TUint KLetterCOffset = KLetterNOffset + 1;  //1
    const TUint KLetterMOffset = KLetterCOffset + 1;  //2
    const TUint KLetterHOffset = KLetterMOffset + 1;  //3
    const TUint KHeaderLengthOffset = KLetterHOffset + 1;//4
    const TUint KSequeceOffset = KHeaderLengthOffset + 2;//6
    const TUint KBlockLengthOffset = KSequeceOffset + 2;//8
    const TUint KNdpIndexOffset = KBlockLengthOffset + 2;//10
    //NCMH
    if (KLetterN != DataTUint8(KLetterNOffset, aBuf, aBufLen) ||
            KLetterC != DataTUint8(KLetterCOffset, aBuf, aBufLen) ||
            KLetterM != DataTUint8(KLetterMOffset, aBuf, aBufLen) ||
            KLetterH != DataTUint8(KLetterHOffset, aBuf, aBufLen))
        {
        //NOT a valid NTB header signature. Try to search NCMH in the buffer...
        OstTrace0(TRACE_ERROR, CNCMNTB16PARSER_PARSENCMNTB16HEADER, "BAD error, missed NCMH!!");
        return SearchNcmHead(aBuf, aBufLen);
        }

#ifdef _DEBUG
    iNth.wHeaderLength = DataTUint16(KHeaderLengthOffset, aBuf, aBufLen);
    iNth.wSequence = DataTUint16(KSequeceOffset, aBuf, aBufLen);
    //keep silence for release. not critical error.
    __ASSERT_DEBUG(iNth.wHeaderLength == 0xC, User::Panic(KNTBParsePanic, __LINE__));
#endif

#ifdef _DEBUG
    OstTrace1(TRACE_NORMAL, CNCMNTB16PARSER_PARSENCMNTB16HEADER_DUP01, "iNth.wBlockLength=%d", DataTUint16(KBlockLengthOffset, aBuf, aBufLen));
#endif

    //using Zlp first.
    if (aZlp)
        {
        iNth.wBlockLength = iDataLen + aBufLen - (iQueLen==0?iDataOffset:0);
        __ASSERT_DEBUG(iNth.wBlockLength < iMaxSize, User::Panic(KNTBParsePanic, __LINE__));
        }
    else
        {
        iNth.wBlockLength = DataTUint16(KBlockLengthOffset, aBuf, aBufLen);
        if (0 == iNth.wBlockLength)
            {
            iNth.wBlockLength = iMaxSize;
            }
        }
    iNth.wNdpIndex = DataTUint16(KNdpIndexOffset, aBuf, aBufLen);

#ifdef _DEBUG
    iNth.Dump();
#endif

    if ((iNth.wNdpIndex >= 0xC) &&
        (iNth.wNdpIndex + KNcmNtb16NdpMiniLength <= iNth.wBlockLength))
        {
        iState = ENcmNtb16ParseStateNdpParsing;
        }
    else //underflow or overflow
        {
        Reset(ENcmNtb16TypeResetQueue);
        iState = ENcmNtb16ParseStateSkipBytes;
        }

    return Parse(aBuf, aBufLen);
    }

TInt CNcmNtb16Parser::ParseNcmNtb16Ndp(const TUint8* aBuf, TInt aBufLen)
    {
    const TUint KLetterNOffset = 0;                  //0
    const TUint KLetterCOffset = KLetterNOffset + 1; //1
    const TUint KLetterMOffset = KLetterCOffset + 1; //2
    const TUint KLetterZeroOffset = KLetterMOffset + 1; //3
    const TUint KLengthOffset = KLetterZeroOffset + 1;//4

    //NCM0
    if (KLetterN != DataTUint8(iNth.wNdpIndex + KLetterNOffset, aBuf, aBufLen) ||
            KLetterC != DataTUint8(iNth.wNdpIndex+KLetterCOffset, aBuf, aBufLen) ||
            KLetterM != DataTUint8(iNth.wNdpIndex+KLetterMOffset, aBuf, aBufLen) ||
            KLetter0 != DataTUint8(iNth.wNdpIndex+KLetterZeroOffset, aBuf, aBufLen))
        {
        //Not valid NDP! Skip this NTB.
        Reset(ENcmNtb16TypeResetQueue);
        iState = ENcmNtb16ParseStateSkipBytes;
        return Parse(aBuf, aBufLen);
        }

    iNdp.wLength = DataTUint16(iNth.wNdpIndex+KLengthOffset, aBuf, aBufLen);
    if (iNdp.wLength < 0x10)
        {
        //Not valid ndp header, Skip this NTB.
        Reset(ENcmNtb16TypeResetQueue);
        iState = ENcmNtb16ParseStateSkipBytes;
        return Parse(aBuf, aBufLen);
        }
    iNdpBound  = iNth.wNdpIndex + iNdp.wLength;

#ifdef _DEBUG
    if (iNdpBound > iNth.wBlockLength)
        {
        //warning: overflow..
        }
    const TUint KNextNdpIndexOffset = KLengthOffset + 2;//6
    iNdp.wNextNdpIndex = DataTUint16(iNth.wNdpIndex + KNextNdpIndexOffset, aBuf, aBufLen);
    __ASSERT_DEBUG(iNdp.wNextNdpIndex==0, User::Panic(KNTBParsePanic, __LINE__));

    iNdp.Dump();
#endif

    iNdpDataOffset = iNth.wNdpIndex + KNcmNtb16NdpDatagramOffset;
    iState = ENcmNtb16ParseStateNdpSeeking;
    return Parse(aBuf, aBufLen);
    }

//parse NdpDatagram
TInt CNcmNtb16Parser::ParseNcmNtb16NdpDatagram(const TUint8* aBuf, TInt aBufLen)
    {
    TInt dataLen = aBufLen + (iQueLen==0?(0-iDataOffset):iDataLen);
    TInt  ret = KErrNone;
    OstTraceExt3(TRACE_NORMAL, CNCMNTB16PARSER_PARSENCMNTB16NDPDATAGRAM, "ParseNcmNtb16NdpDatagram:iNdpState=%d, iDataLen=%d, dataLen=%d", iNdpState, iDataLen, dataLen);

    switch(iNdpState)
        {
        case ENcmNtb16ParseNdpStateUninitialized:
            {
            TUint datagramBound = iNdpDataOffset+ KNcmNtb16NdpDatagramLength;
            if (iNdpDataOffset >= iNdpBound || datagramBound > iNth.wBlockLength)
                {
                //finish this NTB.
                Reset(ENcmNtb16TypeResetQueue);
                iState = ENcmNtb16ParseStateSkipBytes;
                ret = Parse(aBuf, aBufLen);
                }
            else if (datagramBound <= dataLen)
                {
                iDatagramIndex = DataTUint16(iNdpDataOffset, aBuf, aBufLen);
                iDatagramLength = DataTUint16(iNdpDataOffset+KNcmNtb16NdpDatagramLenOffset, aBuf, aBufLen);
                OstTraceExt2(TRACE_NORMAL, CNCMNTB16PARSER_PARSENCMNTB16NDPDATAGRAM_DUP01, "Index=%d, Length=%d", iDatagramIndex, iDatagramLength);
                // NULL NDP;
                if (iDatagramIndex == 0 || iDatagramLength == 0)
                    {
                    Reset(ENcmNtb16TypeResetQueue);
                    iState = ENcmNtb16ParseStateSkipBytes;
                    ret = Parse(aBuf, aBufLen);
                    }
                else if (iDatagramIndex + iDatagramLength <= iNth.wBlockLength)
                    {
                    TRAPD(err, iPacket.CreateL(iDatagramLength));
                    if (KErrNone == err)
                        {
                        iNdpState = ENcmNtb16ParseNdpStateDatagramed;
                        ret = ParseNcmNtb16NdpDatagram(aBuf, aBufLen);
                        }
                    else
                        {
                        iPacket.Free();

                        iNdpDataOffset += KNcmNtb16NdpDatagramLength;//skip this ethernet frame, do nothing else.
                        ret = ParseNcmNtb16NdpDatagram(aBuf, aBufLen);
                        }

                    }
                else
                    {
                    //skip this datagram because of overflow.
                    iNdpDataOffset += KNcmNtb16NdpDatagramLength;
                    ret = ParseNcmNtb16NdpDatagram(aBuf, aBufLen);
                    }
                }
            else
                {
                ret = QueueBuf(aBuf, aBufLen);
                }
            }
            break;

        case ENcmNtb16ParseNdpStateDatagramed:
            ret = DataPacket(aBuf, aBufLen);
            if (KErrCompletion == ret)
                {
                iPacket.Pack();
                iObserver.ProcessEtherFrame(iPacket);

                iNdpDataOffset += KNcmNtb16NdpDatagramLength;
                iNdpState = ENcmNtb16ParseNdpStateUninitialized;
                ret = ParseNcmNtb16NdpDatagram(aBuf, aBufLen);
                }
            break;
        }

    return ret;
    }

//queue buffer.
TInt CNcmNtb16Parser::QueueBuf(const TUint8* aBuf, TInt aBufLen)
    {
    __ASSERT_DEBUG(iQueLen==0?(iDataLen==0):ETrue, User::Panic(KNTBParsePanic, __LINE__));
    TPtrC8 ptr(aBuf, aBufLen);
    TInt ret = iBufQueue.Append(ptr);
    if(KErrNone != ret)
        {
        OstTrace1(TRACE_ERROR, CNCMNTB16PARSER_QUEUEBUF, "Append Queue Error: ret=%d!!", ret);
        Reset();
        return ret;
        }
    iDataLen += aBufLen - (iQueLen==0?iDataOffset:0);
    iQueLen ++;
    return KErrNone;
    }

TUint8 CNcmNtb16Parser::DataTUint8(TUint aOffset, const TUint8* aBuf, TInt aBufLen)
    {
    if (0 == iQueLen)
        {
        return aBuf[iDataOffset + aOffset];
        }
    else if (iDataLen <= aOffset)
        {
        return aBuf[aOffset - iDataLen];
        }
    else
        {
        TInt    i = 1;
        TPtrC8* pPtr = &iBufQueue[0];
        TUint   totalOffset = pPtr->Length() - iDataOffset;
        while(totalOffset <= aOffset)
            {
            pPtr = &iBufQueue[i];
            totalOffset += pPtr->Length();
            i++;
            __ASSERT_DEBUG(i<=iQueLen, User::Panic(KNTBParsePanic, __LINE__));
            }
        return pPtr->Ptr()[pPtr->Length() + aOffset - totalOffset];
        }
    }

TUint16 CNcmNtb16Parser::DataTUint16(TUint aOffset, const TUint8* aBuf, TInt aBufLen)
    {
    if (0 == iQueLen)
        {
        return LittleEndian::Get16(aBuf + aOffset + iDataOffset);
        }
    else if (iDataLen <= aOffset)
        {
        return LittleEndian::Get16(aBuf + aOffset - iDataLen);
        }
    else
        {
        TUint16 ret = 0;
        TInt    i = 1;
        TPtrC8* pPtr = &iBufQueue[0];
        TUint   totalOffset = pPtr->Length() - iDataOffset;

        while(totalOffset <= aOffset)
            {
            pPtr = &iBufQueue[i];
            totalOffset += pPtr->Length();
            i++;
            __ASSERT_DEBUG(i<=iQueLen, User::Panic(KNTBParsePanic, __LINE__));
            }
        ret = pPtr->Ptr()[pPtr->Length() + aOffset - totalOffset];
        if (totalOffset > aOffset + 1)
            {
            ret += ((pPtr->Ptr()[pPtr->Length() + aOffset - totalOffset + 1])<<8)&0xFF00;
            }
        else if (i < iQueLen)
            {
            pPtr = &iBufQueue[i];
            ret += ((pPtr->Ptr()[0])<<8)&0xFF00;
            }
        else
            {
            ret += (aBuf[0]<<8)&0xFF00;
            }

        return ret;
        }
    }

TInt CNcmNtb16Parser::DataPacket(const TUint8* aBuf, TInt aBufLen)
    {
    TInt dataLen = aBufLen + (iQueLen==0?(0-iDataOffset):iDataLen);
    OstTraceExt5(TRACE_NORMAL, CNCMNTB16PARSER_DATAPACKET, "DataPacket:dataLen=%d, index=%d, len=%d, iDataLen=%d, iDataOffset=%d", dataLen, iDatagramIndex, iDatagramLength, iDataLen, iDataOffset);
    if (dataLen < iDatagramIndex + iDatagramLength)
        {
        return QueueBuf(aBuf, aBufLen);
        }

    if (0 == iQueLen)
        {
        iPacket.CopyIn(TPtrC8(aBuf + iDatagramIndex + iDataOffset, iDatagramLength));
        }
    else if (iDataLen <= iDatagramIndex)
        {
        iPacket.CopyIn(TPtrC8(aBuf + iDatagramIndex - iDataLen, iDatagramLength));
        }
    else
        {
        TInt    i = 1;
        TPtrC8* pPtr = &iBufQueue[0];
        TUint   totalOffset = pPtr->Length() - iDataOffset;

        while (totalOffset <= iDatagramIndex)
            {
            pPtr = &iBufQueue[i];
            totalOffset += pPtr->Length();
            i++;
            __ASSERT_DEBUG(i<=iQueLen, User::Panic(KNTBParsePanic, __LINE__));
            }
        if (totalOffset >= iDatagramIndex + iDatagramLength)
            {
            iPacket.CopyIn(TPtrC8(pPtr->Ptr() + pPtr->Length() + iDatagramIndex - totalOffset, iDatagramLength));
            }
        else
            {
            TInt   len = totalOffset - iDatagramIndex;
            iPacket.CopyIn(TPtrC8(pPtr->Ptr() + pPtr->Length() + iDatagramIndex - totalOffset, len));

            while(i < iQueLen && iBufQueue[i].Length() + len <= iDatagramLength)
                {
                iPacket.CopyIn(iBufQueue[i], len);
                len += iBufQueue[i].Length();
                i++;
                }

            if (len < iDatagramLength)
                {
                if (i < iQueLen)
                    {
                    iPacket.CopyIn(TPtrC8(iBufQueue[i].Ptr(), iDatagramLength-len), len);
                    }
                else
                    {
                    iPacket.CopyIn(TPtrC8(aBuf, iDatagramLength - len), len);
                    }
                }
            }
        }

    return KErrCompletion;
    }
