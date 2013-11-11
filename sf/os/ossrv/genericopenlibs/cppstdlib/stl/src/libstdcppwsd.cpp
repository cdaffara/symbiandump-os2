// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : libstdcppwsd.cpp
// Part of     : standard c++ library.
// 
//

#include "stlport_prefix.h"
#include "stl/_threads.h"
#include <locale>
#include <libstdcppwsd.h>
#include "locale_impl.h"

#ifdef __SYMBIAN32__WSD__   //stuff for WSD

//constructor
EXPORT_C _Libcpp_wsd::_Libcpp_wsd()
	 : ios_base_Init_S_count(0), 
	    _IosInit(NULL),
		wsd_cin(NULL),
		wsd_cout(NULL),
		wsd_cerr(NULL),
		wsd_clog(NULL),
#ifndef _STLP_NO_WCHAR_T
		wsd_wcin(NULL),
		wsd_wcout(NULL),
		wsd_wcerr(NULL),
		wsd_wclog(NULL),
#endif	
		is_Initialized(false),
		_Stl_classic_locale(NULL),
		_Stl_global_locale(NULL)
		{ /* do nothing */ }
	
//destructor	
EXPORT_C _Libcpp_wsd::~_Libcpp_wsd()
		{
		//Free the internal allocations
		if( _IosInit != NULL )
			{
			_IosInit->~Init();
			WSDFree(_IosInit);
			}
		if( wsd_cin != NULL )
			{
			wsd_cin->~basic_istream();
			WSDFree(wsd_cin);
			}
		if( wsd_cout != NULL )
			{
			wsd_cout->~basic_ostream();
			WSDFree(wsd_cout);
			}
		if( wsd_cerr != NULL )
			{
			wsd_cerr->~basic_ostream();
			WSDFree(wsd_cerr);
			}
		if( wsd_clog != NULL )
			{
			wsd_clog->~basic_ostream();
			WSDFree(wsd_clog);
			}
		if( wsd_wcin != NULL )
			{
			wsd_wcin->~basic_istream();
			WSDFree(wsd_wcin);
			}
		if( wsd_wcout != NULL )
			{
			wsd_wcout->~basic_ostream();
			WSDFree(wsd_wcout);
			}
		if( wsd_wcerr != NULL )
			{
			wsd_wcerr->~basic_ostream();
			WSDFree(wsd_wcerr);
			}
		if( wsd_wclog != NULL )
			{
			wsd_wclog->~basic_ostream();
			WSDFree(wsd_wclog);
			}
		if( _Stl_global_locale != NULL )
			{
			_Stl_global_locale->~locale();
			WSDFree(_Stl_global_locale);
			}
		if( _Stl_classic_locale != NULL )
			{
			_Stl_classic_locale->~locale();
			WSDFree(_Stl_classic_locale);
			}
			
		if( _Loc_init_S_count != NULL )
			WSDFree(_Loc_init_S_count);	
		}

EXPORT_C void CleanupWSD() 
	{
	_Libcpp_wsd* wsdPtr = Pls<_Libcpp_wsd>(KLibstdcppUID, NULL);
	wsdPtr->~_Libcpp_wsd();
	}

EXPORT_C _Libcpp_wsd& get_libcpp_wsd()
	{
	//get the TLS pointer
	_Libcpp_wsd* g_libcpp_wsd = Pls<_Libcpp_wsd>(KLibstdcppUID, NULL);
	if(g_libcpp_wsd->is_Initialized != true)
		{
		//this has to be here to avoid infinite recursion
		g_libcpp_wsd->is_Initialized = true;

#ifdef _STLP_DEBUG
		get_Nameless() = new std::string("*");
#endif
		//initialize allocator static data
		stdcpp_allocators_init();

		//initialize the page size
		filebuf_page_size_init();

		//initialize _S_count of _Loc_init
		// Calling placement new on internally alloc'ed memory.
		g_libcpp_wsd->_Loc_init_S_count = new (WSDAlloc(sizeof(std::_Refcount_Base))) std::_Refcount_Base(0);

		std::_Locale_impl::Init::Init();
		// Calling placement new on internally alloc'ed memory.
		g_libcpp_wsd->_IosInit = new (WSDAlloc(sizeof(std::ios_base::Init))) std::ios_base::Init();
		}
	return (*g_libcpp_wsd);
	}


/******************** threads related *****************/
//
_STLP_DECLSPEC std::_STLP_STATIC_MUTEX& exp_get_threads_S_swap_lock() 
	{
	return get_libcpp_wsd()._threads_S_swap_lock;
	}

_STLP_DECLSPEC std::_STLP_STATIC_MUTEX& exp_get_threads_0_S_swap_lock() 
	{
  	return get_libcpp_wsd()._threads_0_S_swap_lock;
	}



// locale::id
//ACCESS FUNCTIONS
_STLP_DECLSPEC std::locale::id&
std::codecvt<char, char, mbstate_t>::GetFacetLocaleId()
    {
    	return get_libcpp_wsd().codecvt_char_char_mbstate_id;
    }


_STLP_DECLSPEC std::locale::id&
std::codecvt<wchar_t, char, mbstate_t>::GetFacetLocaleId()
    {
   	 	return get_libcpp_wsd().codecvt_wchar_char_mbstate_id;
    }

_STLP_DECLSPEC std::locale::id&
std::collate<char>::GetFacetLocaleId()
    {
   		return get_libcpp_wsd().collate_char_id;
    }

_STLP_DECLSPEC std::locale::id&
std::ctype<char>::GetFacetLocaleId()
    {
    	return get_libcpp_wsd().ctype_char_id;
    }

_STLP_DECLSPEC std::locale::id&
std::moneypunct<char, true>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().moneypunct_char_true_id;
	}

_STLP_DECLSPEC std::locale::id&
std::moneypunct<char, false>::GetFacetLocaleId()
    {
    	return get_libcpp_wsd().moneypunct_char_false_id;
    }

_STLP_DECLSPEC std::locale::id&
std::messages<char>::GetFacetLocaleId()
    {
    	return get_libcpp_wsd().messages_char_id;
    }

_STLP_DECLSPEC std::locale::id&
std::numpunct<char>::GetFacetLocaleId()
    {
    	return get_libcpp_wsd().numpunct_char_id;
    }

_STLP_DECLSPEC std::locale::id&
std::collate<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().collate_wchar_id;
	}

_STLP_DECLSPEC std::locale::id&
std::ctype<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().ctype_wchar_id;
	}

_STLP_DECLSPEC std::locale::id&
std::moneypunct<wchar_t, true>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().moneypunct_wchar_true_id;
	}

_STLP_DECLSPEC std::locale::id&
std::moneypunct<wchar_t, false>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().moneypunct_wchar_false_id;
	}

_STLP_DECLSPEC std::locale::id&
std::numpunct<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().numpunct_wchar_id;
	}

_STLP_DECLSPEC std::locale::id&
std::messages<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsd().messages_wchar_id;
	}

#endif  /* __SYMBIAN32__WSD__ */
