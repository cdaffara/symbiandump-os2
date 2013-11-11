/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <exception>
#include <rw/_defs.h>
#include<rw/_exception.h>

#ifdef __ARMCC__
#pragma diag_suppress 940
#endif

 _RWSTD_EXPORT __rw::__rw_exception::__rw_exception () _THROWS (()){}
 
 _RWSTD_EXPORT __rw::__rw_exception::__rw_exception (const __rw_exception& x){}
 
 _RWSTD_EXPORT __rw::__rw_exception::__rw_exception (const _STD::string& x ){}
 
 _RWSTD_EXPORT __rw::__rw_exception::__rw_exception (const char* x ){}
 
 _RWSTD_EXPORT  __rw::__rw_exception::~__rw_exception () _THROWS (()){}
 
 _RWSTD_EXPORT    const char* __rw::__rw_exception::what () const _THROWS (()){}
 
 _RWSTD_EXPORT    __rw::__rw_exception& __rw::__rw_exception::operator= (const __rw_exception& x) _THROWS (()){}
 
 _RWSTD_EXPORT   __rw::__rw_exception&  __rw::__rw_exception::_C_assign (const char*  , _RWSTD_SIZE_T/*= _RWSTD_SIZE_MAX*/){}
