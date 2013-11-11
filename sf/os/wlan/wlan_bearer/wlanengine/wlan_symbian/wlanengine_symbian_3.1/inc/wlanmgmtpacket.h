/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Callback interface to pass management packets.
*
*/


#ifndef WLANMGMTPACKET_H
#define WLANMGMTPACKET_H

// CLASS DECLARATION

/**
*  Interface to forward received management packets.
*/
class MWlanMgmtPacket
    {
    public:
        /**
        * Inform about new received management packet.
        * @param aLength Length of the data.
        * @param aData   The data.
        * @return General Symbian error code.
        */
        virtual TInt ReceivePacket( const TUint aLength, const TUint8* const aData )=0;
    };

#endif    // WLANMGMTPACKET_H

//  End of File
