// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This code contains the UID code definitions
// for ECom plug-ins
// 
//

/**
 @internalComponent
 @file
*/

#ifndef __ECOMUIDCODES_H__
#define __ECOMUIDCODES_H__

#include <e32def.h>

/** 
The UID identifiying an ECom plug-in resource file as having version 2 format 
@internalComponent
*/
const TUid KUidEComResourceFormatV2 = {0x101FB0B9};

/** 
The UID identifiying an ECom plug-in resource file as having version 3 format 
@internalComponent
*/
const TUid KUidEComResourceFormatV3 = {0x10009E47};

/** 
The UID identifiying Interface Implementation Collections. 
@internalComponent
*/
const TUid KUidInterfaceImplementationCollection = {0x10009D8D};
/** 
The UID identifiying Interface Implementation Collections v3. 
@internalComponent
*/
const TUid KUidInterfaceImplementationCollection3 = {0x10009D93};

/** The UID identifiying Interface Implementation Resource Files. 
@internalComponent
*/
const TUid KUidInterfaceImplementationCollectionInfo = {0x101F747D};

/** 
The UID identifiying client provided CResolver Interface Implementations. 
@internalComponent
*/
const TUid KEComResolverInterfaceUid = {0x10009D90};

/** 
The UID identifiying the default CResolver Interface Implementation. 
@internalComponent
*/
const TUid KDefaultResolverUid = {0x10009D91};

/** 
The UID identifiying the Ecom type for an SPI file. 
@internalComponent
*/
const TUid KEcomSpiFileTypeUid = {0x10205C2C};

/**
@internalComponent
*/
const TInt KFileTypeUidIndex	= 0;
const TInt KAltFileTypeUidIndex	= 1;
const TInt KFileUniqueUidIndex	= 2;

#endif // __ECOMUIDCODES_H__
