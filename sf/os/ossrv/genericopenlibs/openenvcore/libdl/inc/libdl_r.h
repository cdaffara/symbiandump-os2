/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : libdl_r.h
* Part of     : libdl library
* Header file for libdl_r.cpp
* 
*
*/



#ifndef _LIBDL_R_H_
#define _LIBDL_R_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

void* __dlopen_r(const char* filename, const int flag);
const char* __dlerror_r(void);
void* __dlsym_r(const void *handle, const char *symbol);
int __dlclose_r(const void *handle);

#ifdef __cplusplus
}//extern "C"
#endif//__cplusplus

#endif//_LIBDL_R_H_
