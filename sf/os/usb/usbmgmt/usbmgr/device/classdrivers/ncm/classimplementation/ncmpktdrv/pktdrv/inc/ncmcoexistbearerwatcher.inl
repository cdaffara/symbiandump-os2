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

#ifndef NCMCOEXISTBEARERWATCHER_INL
#define NCMCOEXISTBEARERWATCHER_INL

inline TBool CNcmCoexistBearerWatcher::IsIpOverUsbActived() const
    {
    return EIPBearerIPOverUSB==iBearerState;
    }

#endif //NCMCOEXISTBEARERWATCHER_INL
