/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WLAN HW configure data structure.
*
*/

/*
* %version: 4 %
*/

#ifndef WLANHWCONFIGUREDATA_H
#define WLANHWCONFIGUREDATA_H

#include <wha.h>

NAMESPACE_BEGIN_WHA
struct SConfigureData
    {
    SConfigureDataBase cfgDB;
    unsigned int iSize; 
    unsigned short iPda[1];
    };
NAMESPACE_END_WHA

#endif      // WLANHWCONFIGUREDATA_H
