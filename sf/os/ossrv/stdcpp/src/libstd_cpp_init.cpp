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


# include "stlport_prefix.h"
#include <e32base.h>
#include <e32err.h>
//#include <typeinfo.h>
#include <memory>

#include "libstdcppwsd.h"
#ifdef __WINSCW__
#include <pls.h>
const TUid KLibstdcppUID = {0x10282872};
#endif

#ifdef	__WINSCW__
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}	// extern "C"
#endif
#include <e32std.h>
#endif //__WINSCW__
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
#include "sysif.h"
#endif




_STLP_BEGIN_NAMESPACE

void CallIosInit();
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
extern void CallCloseStdLib();
#endif

_STLP_EXP_DECLSPEC void LibStdCppInit()
    {
	//Create Cleanup Stack
	CTrapCleanup* pCleanup = CTrapCleanup::New();

	if( pCleanup )
		{
		CallIosInit();
		delete pCleanup;
		}
    }

#if 0
_STLP_EXP_DECLSPEC type_info::~type_info () {}


_STLP_EXP_DECLSPEC bad_cast::~bad_cast() throw() { }

_STLP_EXP_DECLSPEC bad_typeid::~bad_typeid() throw() { }
    

// return true if this is a type_info for a pointer type
_STLP_EXP_DECLSPEC bool type_info::__is_pointer_p () const
{
  return false;
}

_STLP_EXP_DECLSPEC bool type_info::__is_function_p () const
{
  return false;
}

_STLP_EXP_DECLSPEC bool type_info::__do_catch (const type_info *thr_type, void **, unsigned) const
{
  return *this == *thr_type;
}

#endif


_STLP_EXP_DECLSPEC new_handler set_new_handler(new_handler pnew) __NO_THROW
{
    new_handler& phandler = get_new_handler();
    new_handler phold = phandler;
    phandler = pnew;
    return phold;
}

_STLP_END_NAMESPACE

using namespace std;

int
init_libcpp_wsdvar(_Libcpp_wsd* g_libcpp_wsd)
{
	g_libcpp_wsd->is_Initialized = false;
	return KErrNone;
}

#ifdef __WINSCW__
void* GetGlobalTlsData()
    {
    return Pls<_Libcpp_wsd>(KLibstdcppUID, &init_libcpp_wsdvar);
    }

void SetGlobalTlsData(void* aData)
    {
    SetPls(aData, KLibstdcppUID);
    }
#endif
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
void* SetBackendHeap()
{

	RHeap* oldHeap = User::SwitchHeap( Backend()->Heap() ); // From now, new allocates memory form backend heap
	
	return (void*)oldHeap;
	
}

void ReSetUserHeap(void* oldHeap)
{
	
	User::SwitchHeap( (RHeap*)oldHeap);
}


#endif



_STLP_EXP_DECLSPEC void CloseStdLibCpp()
    {
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    std::CallCloseStdLib();
#endif
    }




_STLP_EXP_DECLSPEC void *operator new(unsigned int aSize)__NO_THROW
	{
	void* __y;

	__y = _STLP_NEW(aSize);

	if (__y == 0)
	    {
	    new_handler& phandler = get_new_handler();
	    if(phandler == NULL)
	        _STLP_THROW(bad_alloc());
	    else
	      phandler();
	    }
    return __y;
	}

_STLP_EXP_DECLSPEC void *operator new[](unsigned int aSize)__NO_THROW
    {
    return ::operator new(aSize);
    }

_STLP_EXP_DECLSPEC void operator delete(void* aPtr) __NO_THROW
    {

    ::free(aPtr);

    }

_STLP_EXP_DECLSPEC void operator delete[](void* aPtr) __NO_THROW
    {
    ::operator delete(aPtr);
    }

_STLP_EXP_DECLSPEC void* operator new(unsigned int aSize, const std::nothrow_t& /*aNoThrow*/) __NO_THROW
    { 
	void* __y = _STLP_NEW(aSize);
	if (__y == 0)
	    {
	    new_handler& phandler = get_new_handler();
	    if(phandler != NULL)
	      phandler();
	    }
    return __y;
    }

_STLP_EXP_DECLSPEC void* operator new[](unsigned int aSize, const std::nothrow_t& aNoThrow) __NO_THROW
    { 
    return operator new (aSize, aNoThrow); 
    }

_STLP_EXP_DECLSPEC void operator delete(void* aPtr, const std::nothrow_t& /*aNoThrow*/) __NO_THROW
    {
    operator delete(aPtr);
    }

_STLP_EXP_DECLSPEC void operator delete[](void* aPtr, const std::nothrow_t& /*aNoThrow*/) __NO_THROW
    {
    operator delete(aPtr);
    }


// end of file
