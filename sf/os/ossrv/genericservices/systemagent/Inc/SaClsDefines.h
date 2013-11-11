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

#ifndef __SACLSDEFINES_H__
#define __SACLSDEFINES_H__

#include <e32std.h>


/** This is the global definition of KUidJmiLatestInstallation.
This is the publish and subscribe property used to publish the most
recently installed application. If the application has 
subsequently been uninstalled the value may refer to an application
that no longer exists. Has value KUidNull if no applications have been 
installed 

@publishedPartner
@released
@capability ECapabilityTrustedUI capability is required to modify the value of this property
*/
const TUint KUidJmiLatestInstallation = 0x10272D3D;


#endif //__SACLSDEFINES_H__
