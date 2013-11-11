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
* Description:  Constant definitions for IPProxyEngine
*
*/



#ifndef COMMROUTERDEFINITIONS_H
#define COMMROUTERDEFINITIONS_H


// Definitions for protocol frames
_LIT8( KTCPPrefix, "TCP:" );
_LIT8( KTCPOpenPrefix, "TCP_OPEN:" );
_LIT8( KTCPLstnPrefix, "TCP_LSTN:" );
_LIT8( KTCPClosePrefix, "TCP_CLOSE:" );
_LIT8( KTCPCloseAllPrefix, "TCP_CLOSEALL:" );
_LIT8( KUDPPrefix, "UDP:" );

const TInt KMaxPrefixLength = 10;

_LIT8( KPortSuffix, "," );
_LIT8( KLengthSuffix, ",[" );
_LIT8( KDataSuffix, "]" );
_LIT8( KMessageSuffix, "" );

_LIT8( KTCPPortPrefix, "[" );
_LIT8( KTCPPortSuffix, "]" );

const TInt KHexDecimalLength = 4;
_LIT8( KHexFormat,"%04x" );

#endif      // COMMROUTERDEFINITIONS_H

// End of File
