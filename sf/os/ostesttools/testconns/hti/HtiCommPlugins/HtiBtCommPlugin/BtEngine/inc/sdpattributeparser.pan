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
* Description:  Bluetooth SDP attribute parser panic codes.
*
*/


#ifndef __SDP_ATTRIBUTE_PARSER_PAN__
#define __SDP_ATTRIBUTE_PARSER_PAN__

/** Panic Category */
_LIT( KPanicSAP, "SAP" );

/** BTPointToPoint application panic codes */
enum TSdpAttributeParserPanics
    {
    ESdpAttributeParserInvalidCommand = 1,
    ESdpAttributeParserNoValue,
    ESdpAttributeParserValueIsList,
    ESdpAttributeParserValueTypeUnsupported
    };

inline void Panic( TSdpAttributeParserPanics aReason )
    {
    User::Panic( KPanicSAP, aReason );
    }


#endif // __SDP_ATTRIBUTE_PARSER_PAN__

// End of File
