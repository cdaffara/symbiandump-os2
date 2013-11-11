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

#include "ncmpktdrvfactory.h"
#include "ncmpktdrv.h"


/**
@file
@internalComponent
*/
extern "C"
    {
    IMPORT_C CPktDrvFactory * NewNcmPktDrvFactoryL(); ///< Force export
    }

/**
Library file is opened and this is the the first and only export.
@internalComponent
@return A pointer to CPktDrvFactory object.
*/
EXPORT_C CPktDrvFactory* NewNcmPktDrvFactoryL()
    {
    return new (ELeave) CNcmPktDrvFactory;
    }

/**
Create the packet driver object
@param aParent Pointer to the parent Ethint NIF class.
@return A pointer to CPktDrvBase object.
*/
CPktDrvBase* CNcmPktDrvFactory::NewDriverL(CLANLinkCommon* aParent)
    {
    CNcmPktDrv *drv = new (ELeave) CNcmPktDrv(*this);
    CleanupStack::PushL(drv);
    drv->ConstructL(aParent);
    CleanupStack::Pop(drv);
    return drv;
    }


/**
Packet Driver version number.
@return Version number of the Packet Driver
*/
TVersion CNcmPktDrvFactory::Version() const
    {
    return(TVersion(KNcmDrvMajorVersionNumber,KNcmDrvMinorVersionNumber,KNcmDrvBuildVersionNumber));
    }
