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
* Name        : stl_winscw.h
* Part of     : standard c++ library. (config file)
* 
*
*/



#ifndef _STL_WINSCW_H_
#define _STL_WINSCW_H_

#ifdef __MWERKS__
# include <stl/config/_mwerks.h>
#else
//This is required to satisfy the C preprocessor run by ABLD.
# define _STLP_COMPILER "Metrowerks Codewarrior"
# define _STLP_LITTLE_ENDIAN

#ifdef __CW32__
  #define _M_IX86
#endif
#endif //__MWERKS__

#define __SYMBIAN32__WSD__


#endif //_STL_WINSCW_H_
