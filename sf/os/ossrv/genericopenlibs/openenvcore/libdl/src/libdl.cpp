// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : libdl.cpp
// Part of     : libdl library
// Dll entry point functions
//




// INCLUDE FILES
#include <dlfcn.h>  //RTLD_DEFAULT
#include "libdl_r.h"  //_r functions 



// EXTERNAL FUNCTION PROTOTYPES


//-----------------------------------------------------------------------------
//Function Name : void* dlopen(const char* filename, int flag)
//Description   : It takes the filename of the dynamic library to be loaded. 
//				  The dynamic library is loaded and a "handle" to this executable
//				  is returned to the caller.
//Return Value  : valid handle if succesful otherwise NULL and an error message
//				  is logged. The user can retrieve the error message through a 
//				  call to dlerror()
//-----------------------------------------------------------------------------
EXPORT_C void* dlopen(const char* filename, int flag)
	{
	if ( !filename )
		{
		return RTLD_DEFAULT;
		}
	return __dlopen_r(filename, flag);;
	}

//-----------------------------------------------------------------------------
//Function Name : void* dlsym(void* handle, const char* symbol)
//Description   : It takes the handle (as returned by dlopen or RTLD_DEFAULT/
//				  RTLD_NEXT/RTLD_SELF) and the NULL terminated string for symbol
//				  name or the ordinal number. It locates the symbol corresponding
//				  to this name or ordinal number.
//Return Value  : Symbols address address if the symbol name or ordinal number 
//				  is found, otherwise NULL is returned and an error message is
//				  logged. The user can retrieve the error message through a call
//				  to dlerror().
//-----------------------------------------------------------------------------
EXPORT_C void* dlsym(void* handle, const char* symbol)
	{
	return __dlsym_r(handle, symbol);
	}	

//-----------------------------------------------------------------------------
//Function Name : char* dlerror(void)
//Description   : Gives the last error that occurred due to any of the dl 
//				  operations.
//Return Value  : a NULL-terminated character string that gives the last error 
//				  that occurred due to any of the dl operations. If no errors 
//				  have occurred since the last call to dlerror(), dlerror() 
//				  returns NULL.
//-----------------------------------------------------------------------------
EXPORT_C char* dlerror(void)
	{
	return (char*) __dlerror_r();
	}


//-----------------------------------------------------------------------------
//Function Name : int dlclose(void* handle)
//Description   : Decrements the reference count corresponding to the handle.
//				  If refrence count becomes zero library is unloaded.
//Return Value  : 0 on success otherwise non zero value
//-----------------------------------------------------------------------------
EXPORT_C int dlclose(void* handle)
	{
	return __dlclose_r(handle);
	}
	
