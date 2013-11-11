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

#ifndef NCMDATAINTERFACE_INL
#define NCMDATAINTERFACE_INL

/**
 * Indicate whether DataInterface is started or not, called by Engine.
 */
inline TBool CNcmDataInterface::IsStarted() const
    {
    return iStarted;
    }

/**
 * Return current alternate setting.
 */
inline TInt CNcmDataInterface::AltSetting() const
    {
    return iAltSetting;
    }

/**
 * Notify the Engine that unrecoverable data error, like no memory, etc.
 */
inline void CNcmDataInterface::DataError(TInt aError) const
    {
    iEngine.DataError(aError);
    }

/**
 * Resume sending, used when KStopSending returned by last send.
 */
inline void CNcmDataInterface::ResumeSending() const
    {
    iEngine.ResumeSending();
    }

/**
 * Process(upwards simply) received NCM Datagram (ethernet frame).
 */
void CNcmDataInterface::ProcessDatagram(RMBufPacket& aPacket) const
    {
    iEngine.ProcessReceivedDatagram(aPacket);
    }


#endif //NCMDATAINTERFACE_INL
