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
* Name        : stl_rvct.h
* Part of     : standard c++ library. (config file)
* 
*
*/



#ifndef _STL_RVCT_H_
#define _STL_RVCT_H_

#define _STLP_COMPILER "ARM RVCT"

#ifdef __MARM__
  #define _M_ARM
#endif

#if (defined(__ARMCC__) && (__ARMCC_VERSION > 220000) )
#define _STLP_LONG_LONG long long
#endif

#endif //_STL_RVCT_H_
