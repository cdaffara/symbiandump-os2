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
//

#ifndef __SACLSPARTNER_H__
#define __SACLSPARTNER_H__

/**
@publishedPartner
*/

/**
This is a duplicate of Swi::KSwisOperationMask from swi/swispubsubdefs.h
@see KSAUidSoftwareInstallKeyValue
@publishedPartner
*/
const TUint KSASwisOperationMask = 0xFF;

/**
This is a duplicate of KUidJavaInstallKey from swi/swispubsubdefs.h
This is the Publish and Subscribe property used by Java install to publish its current 
state. The publish and subscribe key is defined by SysAgent2 server, but we
do not want to have a dependency to the security subsystem, so we redefine a constant 
with the same value here.
@publishedPartner
*/
const TInt KSAUidJavaInstallKeyValue = 0x1020806E;

#endif //__SACLSPARTNER_H__
