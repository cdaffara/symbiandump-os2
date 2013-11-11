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

#ifndef NCMPKTDRVFACTORY_H
#define NCMPKTDRVFACTORY_H

#include <e32base.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <es_mbuf.h>
#include <networking/pktdrv.h>
#else
#include <usb/testncm/pktdrv.h>
#endif

const TUint KNcmDrvMajorVersionNumber=1;
const TUint KNcmDrvMinorVersionNumber=0;
const TUint KNcmDrvBuildVersionNumber=0;

NONSHARABLE_CLASS(CNcmPktDrvFactory) : public CPktDrvFactory
    {
public:
    virtual CPktDrvBase* NewDriverL(CLANLinkCommon* aParent);
    virtual TVersion Version() const;
    };

#endif // NCMPKTDRVFACTORY_H

