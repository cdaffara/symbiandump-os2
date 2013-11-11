/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EVENTHANDLER_INL
#define EVENTHANDLER_INL

inline TInt RSubCommandQueue::Count() const
    {
    return iQueue.Count();
    }

inline const TDeviceEvent& CDeviceEventHandler::Event() const
    {
    return iIncomingEvent;
    }

inline const THostMsErrData& CDeviceEventHandler::ErrNotiData() const
    {
    return *iErrNotiData;
    }

#endif /*EVENTHANDLER_INL*/
