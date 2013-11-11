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
#ifndef NCMDATARECEIVER_INL
#define NCMDATARECEIVER_INL

inline TUint CNcmDataReceiver::NtbOutMaxSize() const
    {
    return iNtbParser ? iNtbParser->NtbOutMaxSize() : 0xFFFF;
    }

#endif //NCMDATARECEIVER_INL
