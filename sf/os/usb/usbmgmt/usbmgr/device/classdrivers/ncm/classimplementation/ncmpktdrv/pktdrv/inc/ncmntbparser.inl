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
#ifndef NCMNTBPARSER_INL
#define NCMNTBPARSER_INL

inline void TNcmNth16::Dump()
    {
#ifdef DEBUG_DUMP_NTBINNER
    RDebug::Printf("NcmNth16:%08x\n"
            "\tdwSignature=%S,\n"
            "\twHeaderLength=%d,\n"
            "\twSequence=%d,\n"
            "\twBlockLength=%d,\n"
            "\twNdpIndex=%d.",
            this, &TPtrC8((TUint8*)&dwSignature, 4), wHeaderLength, wSequence, wBlockLength,
            wNdpIndex);
#endif
    }

inline void TNcmNdp16::Dump()
    {
#ifdef DEBUG_DUMP_NTBINNER
    RDebug::Printf("NcmNdp16:%08x,\n"
            "\tdwSignature=%S,\n"
            "\twLength=%d,\n"
            "\twNextNdpIndex=%d,\n"
            "\twDatagram0Index=%d,\n"
            "\twDatagram0Length=%d,\n"
            "\twDatagram1Index=%d,\n"
            "\twDatagram1Length=%d,\n",
            this, &TPtrC8((TUint8*)&dwSignature, 4),
            wLength, wNextNdpIndex, wDatagram0Index, wDatagram0Length,
            wDatagram1Index, wDatagram1Length);
#endif
    }

inline CNcmNtb16Parser::CNcmNtb16Parser(MNcmNdpFrameObserver& aObserver)
            : CNcmNtbParser(aObserver), iMaxSize(0xFFFF)
    {
    }

inline CNcmNtbParser::CNcmNtbParser(MNcmNdpFrameObserver& aObserver)
            : iObserver(aObserver)
    {
    }


#endif //NCMNTBPARSER_INL
