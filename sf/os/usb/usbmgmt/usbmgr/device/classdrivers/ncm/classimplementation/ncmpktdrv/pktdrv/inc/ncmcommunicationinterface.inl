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
* NCM communication interface class inl file 
*
*/

/**
@file
@internalComponent
*/

#ifndef NCMCOMMUNICATIONINTERFACE_INL
#define NCMCOMMUNICATIONINTERFACE_INL

inline RDevUsbcScClient& CNcmCommunicationInterface::Ldd()
    {
    return iPort;
    }

inline TBool CNcmCommunicationInterface::IsStarted()
    {
    return iStarted;
    }


#endif //NCMCOMMUNICATIONINTERFACE_INL