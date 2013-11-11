/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 *
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */ 
# include "stlport_prefix.h"

#include "locale_impl.h"
#include <locale>
#include <typeinfo>
#include <stdexcept>
#include "c_locale.h"
#include "aligned_buffer.h"
#include <exception>

#include "locale_impl.h"
#include <stl/_codecvt.h>
#include <stl/_collate.h>
#include <stl/_ctype.h>
#include <stl/_monetary.h>
#include "message_facets.h"

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
#include "libstdcppwsd.h"
_STLP_BEGIN_NAMESPACE
extern void filebuf_page_size_init();
extern void num_get_array_init();
extern void locale_mutex_lock_init();
extern void ios_base_S_index_init();
extern void ios_base_Init_S_count_init();
extern void ios_base_S_was_synced_init();
extern void numpunct_name_init();
extern void locale_catalog_category_hash_lock_init();
extern void complex_trig_limit_init();
extern void monetary_empty_string_init();
//forward declaraions
void ios_base_Loc_init_S_count_init();
void locale_impl_init();
void locale_impl_string_init();
_STLP_END_NAMESPACE
# endif

#ifdef __WINSCW__
extern void SetGlobalTlsData(void* aData);
#endif

_STLP_BEGIN_NAMESPACE


// #ifdef _STLP_USE_OWN_NAMESPACE
// using _STLP_VENDOR_EXCEPT_STD::bad_cast;
// #endif

_Locale_impl::_Locale_impl(const char* s) : name(s) {}
_Locale_impl::~_Locale_impl() {}
void _Locale_impl::incr() {}
void _Locale_impl::decr() {}

// _Locale_impl non-inline member functions.
void _STLP_CALL
_Locale_impl::_M_throw_bad_cast()
{
  _STLP_THROW(bad_cast());  
}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
static void global_iostream_init()
{
	_Libcpp_wsd* libwsd = &get_libcpp_wsdptr();
	//initialize the pointer members
	libwsd->cin = new istream(0);
	libwsd->cout = new ostream(0);
	libwsd->cerr = new ostream(0);
	libwsd->clog = new ostream(0);
#ifndef _STLP_NO_WCHAR_T
	libwsd->wcin = new wistream(0);
	libwsd->wcout = new wostream(0);
	libwsd->wcerr = new wostream(0);
	libwsd->wclog = new wostream(0);
#endif //_STLP_NO_WCHAR_T
}

//WSD class constructor
_Libcpp_wsd::_Libcpp_wsd()
	{
	ios_base_Init_S_count = 0;
	ios_base_Loc_init_S_count = 0;
	locale_catalog_ctype_hash = NULL;
	locale_catalog_numeric_hash = NULL;
	locale_catalog_time_hash = NULL;
	locale_catalog_collate_hash = NULL;
	locale_catalog_monetary_hash = NULL;
	locale_catalog_messages_hash = NULL;
	_LocInit = NULL;
	_IosInit = NULL;
	ios_pword_dummy = NULL;
	Locale_impl_S_global_impl = NULL;

	//initialize the global stream pointers
	cin  = NULL;
	cout = NULL;
	cerr = NULL;
	clog = NULL;
#ifndef _STLP_NO_WCHAR_T
	wcin  = NULL;
	wcout = NULL;
	wcerr = NULL;
	wclog = NULL;
#endif
    _new_handler = NULL;
	}

//WSd class destructor
_Libcpp_wsd::~_Libcpp_wsd()
	{
		delete _LocInit;
		delete _IosInit;
		delete cin;
		delete cout;
		delete cerr;
		delete clog;
# ifndef _STLP_NO_WCHAR_T		
		delete wcin;
		delete wcout;
		delete wcerr;
		delete wclog;
# endif		
	}

void CallCloseStdLib()
    {
    _Libcpp_wsd* wsd = &get_libcpp_wsdptr();
#ifdef __WINSCW__
    SetGlobalTlsData(NULL);
#endif //__WINSCW__
    delete wsd;
    }

# endif  //__LIBSTD_CPP_SYMBIAN32_WSD__

static void 
_Stl_loc_assign_ids() {

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  //initialize wsd obj pointer
  locale_impl_init();
# endif  
  
  // This assigns ids to every facet that is a member of a category,
  // and also to money_get/put, num_get/put, and time_get/put
  // instantiated using ordinary pointers as the input/output
  // iterators.  (The default is [io]streambuf_iterator.)

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)

    collate<char>::GetFacetLocaleId()._M_index = 1;

    ctype<char>::GetFacetLocaleId()._M_index = 2;

 # ifndef _STLP_NO_MBSTATE_T
    codecvt<char, char, mbstate_t>::GetFacetLocaleId()._M_index = 3;
  #  ifndef _STLP_NO_WCHAR_T
    codecvt<wchar_t, char, mbstate_t>::GetFacetLocaleId()._M_index = 22;
  #  endif//!_STLP_NO_WCHAR_T
 # endif//!_STLP_NO_MBSTATE_T

    moneypunct<char, true>::GetFacetLocaleId()._M_index = 4;
    moneypunct<char, false>::GetFacetLocaleId()._M_index = 5;
    messages<char>::GetFacetLocaleId()._M_index = 7;
    numpunct<char>::GetFacetLocaleId()._M_index = 6;

 # ifndef _STLP_NO_WCHAR_T
    collate<wchar_t>::GetFacetLocaleId()._M_index = 20;
    ctype<wchar_t>::GetFacetLocaleId()._M_index = 21;

    moneypunct<wchar_t, true>::GetFacetLocaleId()._M_index = 23;
    moneypunct<wchar_t, false>::GetFacetLocaleId()._M_index = 24;

    numpunct<wchar_t>::GetFacetLocaleId()._M_index = 25;
    messages<wchar_t>::GetFacetLocaleId()._M_index = 26;
 # endif //!_STLP_NO_WCHAR_T


  money_get<char, istreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index                     = 8;
  money_get<char, const char*>::GetFacetLocaleId()._M_index        = 9;
  money_put<char, ostreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index                     = 10;
  money_put<char, char*>::GetFacetLocaleId()._M_index              = 11;

  num_get<char, istreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index                       = 12;
  num_get<char, const char*>::GetFacetLocaleId()._M_index          = 13;
  num_put<char, ostreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index                       = 14;
  num_put<char, char*>::GetFacetLocaleId()._M_index                = 15;
  time_get<char, istreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index                      = 16;
  time_get<char, const char*>::GetFacetLocaleId()._M_index         = 17;
  time_put<char, ostreambuf_iterator<char, char_traits<char> > >::GetFacetLocaleId()._M_index                      = 18;
  time_put<char, char*>::GetFacetLocaleId()._M_index               = 19;
  //adding new iterator type
  num_put<char, back_insert_iterator<string> >::GetFacetLocaleId()._M_index  = 40;
#else
  money_get<char, istreambuf_iterator<char, char_traits<char> > >::id._M_index                     = 8;
  money_get<char, const char*>::id._M_index        = 9;
  money_put<char, ostreambuf_iterator<char, char_traits<char> > >::id._M_index                     = 10;
  money_put<char, char*>::id._M_index              = 11;

  num_get<char, istreambuf_iterator<char, char_traits<char> > >::id._M_index                       = 12;
  num_get<char, const char*>::id._M_index          = 13;
  num_put<char, ostreambuf_iterator<char, char_traits<char> > >::id._M_index                       = 14;
  num_put<char, char*>::id._M_index                = 15;
  time_get<char, istreambuf_iterator<char, char_traits<char> > >::id._M_index                      = 16;
  time_get<char, const char*>::id._M_index         = 17;
  time_put<char, ostreambuf_iterator<char, char_traits<char> > >::id._M_index                      = 18;
  time_put<char, char*>::id._M_index               = 19;
#endif

# ifndef _STLP_NO_WCHAR_T

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()._M_index          = 27;
  money_get<wchar_t, const wchar_t*>::GetFacetLocaleId()._M_index  = 28;
  money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()._M_index          = 29;
  money_put<wchar_t, wchar_t*>::GetFacetLocaleId()._M_index        = 30;

  num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()._M_index            = 31;
  num_get<wchar_t, const wchar_t*>::GetFacetLocaleId()._M_index    = 32;
  num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > ::GetFacetLocaleId()._M_index           = 33;
  num_put<wchar_t, wchar_t*>::GetFacetLocaleId()._M_index          = 34;
  time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()._M_index           = 35;
  time_get<wchar_t, const wchar_t*>::GetFacetLocaleId()._M_index   = 36;
  time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::GetFacetLocaleId()._M_index           = 37;
  time_put<wchar_t, wchar_t*>::GetFacetLocaleId()._M_index         = 38;
#else
  money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                  = 27;
  money_get<wchar_t, const wchar_t*>::id._M_index  = 28;
  money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                  = 29;
  money_put<wchar_t, wchar_t*>::id._M_index        = 30;

  num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                       = 31;
  num_get<wchar_t, const wchar_t*>::id._M_index    = 32;
  num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > ::id._M_index                      = 33;
  num_put<wchar_t, wchar_t*>::id._M_index          = 34;
  time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                   = 35;
  time_get<wchar_t, const wchar_t*>::id._M_index   = 36;
  time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >::id._M_index                   = 37;
  time_put<wchar_t, wchar_t*>::id._M_index         = 38;
#endif
  //  messages<wchar_t>::id._M_index                   = 38;
# endif

  //  locale::id::_S_max                               = 39;
}

# if !defined(__LIBSTD_CPP_SYMBIAN32_WSD__) && !defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
static _Stl_aligned_buffer<_Locale_impl> _S_classic_locale;

static _Stl_aligned_buffer<collate<char> > _S_collate_char;
static _Stl_aligned_buffer<ctype<char> > _S_ctype_char;

# ifndef _STLP_NO_MBSTATE_T
static _Stl_aligned_buffer<codecvt<char, char, mbstate_t> > _S_codecvt_char;
# endif

static _Stl_aligned_buffer<moneypunct<char, true> > _S_moneypunct_true_char;
static _Stl_aligned_buffer<moneypunct<char, false> > _S_moneypunct_false_char;
static _Stl_aligned_buffer<numpunct<char> > _S_numpunct_char;
static _Stl_aligned_buffer<messages<char> > _S_messages_char;

static _Stl_aligned_buffer<money_get<char, istreambuf_iterator<char, char_traits<char> > > > _S_money_get_char;
static _Stl_aligned_buffer<money_put<char, ostreambuf_iterator<char, char_traits<char> > > > _S_money_put_char;
static _Stl_aligned_buffer<num_get<char, istreambuf_iterator<char, char_traits<char> > > > _S_num_get_char;
static _Stl_aligned_buffer<num_put<char, ostreambuf_iterator<char, char_traits<char> > > > _S_num_put_char;
static _Stl_aligned_buffer<time_get<char, istreambuf_iterator<char, char_traits<char> > > > _S_time_get_char;
static _Stl_aligned_buffer<time_put<char, ostreambuf_iterator<char, char_traits<char> > > > _S_time_put_char;

static _Stl_aligned_buffer<money_get<char, const char*> > _S_money_get_char_const_char;
static _Stl_aligned_buffer<money_put<char, char*> > _S_money_put_char_char;
static _Stl_aligned_buffer<num_get<char, const char*> > _S_num_get_char_const_char;
static _Stl_aligned_buffer<num_put<char, char*> > _S_num_put_char_char;
static _Stl_aligned_buffer<time_get<char, const char*> > _S_time_get_char_const_char;
static _Stl_aligned_buffer<time_put<char, char*> > _S_time_put_char_char;

# ifndef _STLP_NO_WCHAR_T
static _Stl_aligned_buffer<collate<wchar_t> > _S_collate_wchar;
static _Stl_aligned_buffer<ctype<wchar_t> > _S_ctype_wchar;
# ifndef _STLP_NO_MBSTATE_T
static _Stl_aligned_buffer<codecvt<wchar_t, char, mbstate_t> > _S_codecvt_wchar;
# endif //!_STLP_NO_MBSTATE_T
static _Stl_aligned_buffer<moneypunct<wchar_t, true> > _S_moneypunct_true_wchar;
static _Stl_aligned_buffer<moneypunct<wchar_t, false> > _S_moneypunct_false_wchar;
static _Stl_aligned_buffer<numpunct<wchar_t> > _S_numpunct_wchar;
static _Stl_aligned_buffer<messages<wchar_t> > _S_messages_wchar;

static _Stl_aligned_buffer<money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > > _S_money_get_wchar;
static _Stl_aligned_buffer<money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > > _S_money_put_wchar;
static _Stl_aligned_buffer<num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > > _S_num_get_wchar;
static _Stl_aligned_buffer<num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > > _S_num_put_wchar;
static _Stl_aligned_buffer<time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > > _S_time_get_wchar;
static _Stl_aligned_buffer<time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > > _S_time_put_wchar;

static _Stl_aligned_buffer<money_get<wchar_t, const wchar_t*> > _S_money_get_wchar_const_wchar;
static _Stl_aligned_buffer<money_put<wchar_t, wchar_t*> > _S_money_put_wchar_wchar;
static _Stl_aligned_buffer<num_get<wchar_t, const wchar_t*> > _S_num_get_wchar_const_wchar;
static _Stl_aligned_buffer<num_put<wchar_t, wchar_t*> > _S_num_put_wchar_wchar;
static _Stl_aligned_buffer<time_get<wchar_t, const wchar_t*> > _S_time_get_wchar_const_wchar;
static _Stl_aligned_buffer<time_put<wchar_t, wchar_t*> > _S_time_put_wchar_wchar;
# endif //!_STLP_NO_WCHAR_T

static _Messages _Null_messages;

static locale::facet* _S_classic_facets[] = {
  __REINTERPRET_CAST(locale::facet*,0),
  __REINTERPRET_CAST(locale::facet*,&_S_collate_char),
  __REINTERPRET_CAST(locale::facet*,&_S_ctype_char), 
# ifndef _STLP_NO_MBSTATE_T
    __REINTERPRET_CAST(locale::facet*,&_S_codecvt_char),
# else
    __REINTERPRET_CAST(locale::facet*,0), 
# endif //!_STLP_NO_MBSTATE_T
  __REINTERPRET_CAST(locale::facet*,&_S_moneypunct_true_char),
  __REINTERPRET_CAST(locale::facet*,&_S_moneypunct_false_char),
  __REINTERPRET_CAST(locale::facet*,&_S_numpunct_char),
  __REINTERPRET_CAST(locale::facet*,&_S_messages_char),

  __REINTERPRET_CAST(locale::facet*,&_S_money_get_char),
  __REINTERPRET_CAST(locale::facet*,&_S_money_get_char_const_char),
  __REINTERPRET_CAST(locale::facet*,&_S_money_put_char),
  __REINTERPRET_CAST(locale::facet*,&_S_money_put_char_char),

  __REINTERPRET_CAST(locale::facet*,&_S_num_get_char),
  __REINTERPRET_CAST(locale::facet*,&_S_num_get_char_const_char),
  __REINTERPRET_CAST(locale::facet*,&_S_num_put_char),
  __REINTERPRET_CAST(locale::facet*,&_S_num_put_char_char),
  __REINTERPRET_CAST(locale::facet*,&_S_time_get_char),
  __REINTERPRET_CAST(locale::facet*,&_S_time_get_char_const_char),
  __REINTERPRET_CAST(locale::facet*,&_S_time_put_char),
  __REINTERPRET_CAST(locale::facet*,&_S_time_put_char_char),
# ifndef _STLP_NO_WCHAR_T
  __REINTERPRET_CAST(locale::facet*,&_S_collate_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_ctype_wchar), 
# ifndef _STLP_NO_MBSTATE_T
  __REINTERPRET_CAST(locale::facet*,&_S_codecvt_wchar),
# else 
  __REINTERPRET_CAST(locale::facet*,0)
# endif //!_STLP_NO_MBSTATE_T
  __REINTERPRET_CAST(locale::facet*,&_S_moneypunct_true_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_moneypunct_false_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_numpunct_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_messages_wchar),

  __REINTERPRET_CAST(locale::facet*,&_S_money_get_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_money_get_wchar_const_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_money_put_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_money_put_wchar_wchar),

  __REINTERPRET_CAST(locale::facet*,&_S_num_get_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_num_get_wchar_const_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_num_put_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_num_put_wchar_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_time_get_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_time_get_wchar_const_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_time_put_wchar),
  __REINTERPRET_CAST(locale::facet*,&_S_time_put_wchar_wchar),
# endif //!_STLP_NO_WCHAR_T
  0
};
# endif //!__LIBSTD_CPP_SYMBIAN32_WSD__

//Put all static variable initialization in this file
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)

#ifndef __WINSCW__
_STLP_STATIC_MEMBER_EXP_DECLSPEC _Libcpp_wsd* g_libcpp_wsd = NULL;
#endif

void locale_impl_init()
{
	int index = 0;

#ifdef __WINSCW__
	_Libcpp_wsd* g_libcpp_wsd = &get_libcpp_wsdptr();
#else
	g_libcpp_wsd  = &get_libcpp_wsdptr();
#endif

	if (!g_libcpp_wsd)
		return;

	//initialize _S_max
	get_locale_id_S_max() = 39;

	get_locale_impl_S_global_impl() = 0;

	//initialize the page size
	filebuf_page_size_init();

	//initialize the array in num_get
	num_get_array_init();

	//initialize the mutex lock in locale.cpp
	locale_mutex_lock_init();

	//initialize _S_index in ios.cpp
	ios_base_S_index_init();

	//initialize _S_count of _Loc_init
	ios_base_Loc_init_S_count_init();

	//initialize _S_count of Init
	ios_base_Init_S_count_init();

	//initialize S_was_synced of ios_base
	ios_base_S_was_synced_init();

	//initialize the mutex in locale_catalog.cpp
	locale_catalog_category_hash_lock_init();

	//initialize float and double limits in complex_trig.cpp
	complex_trig_limit_init();

	//initialize the string
	locale_impl_string_init();

	//initialize empty string in monetary.cpp
	monetary_empty_string_init();

	//initialize true/false name in numpunct.cpp
	numpunct_name_init();

	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,0);
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_collate_char());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_ctype_char());

# ifndef _STLP_NO_MBSTATE_T
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_codecvt_char());
# else
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,0);
# endif
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_moneypunct_true_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_moneypunct_false_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_numpunct_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_messages_char());

	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_get_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_get_char_const_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_put_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_put_char_char());

	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_get_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_get_char_const_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_put_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_put_char_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_get_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_get_char_const_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_put_char());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_put_char_char());
# ifndef _STLP_NO_WCHAR_T
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_collate_wchar());
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_ctype_wchar());
# ifndef _STLP_NO_MBSTATE_T
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_codecvt_wchar());
# else
	get_locale_impl_S_classic_facets()[index++] =  __REINTERPRET_CAST(locale::facet*,0);
# endif //!_STLP_NO_MBSTATE_T

	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_moneypunct_true_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_moneypunct_false_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_numpunct_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_messages_wchar());

	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_get_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_get_wchar_const_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_put_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_money_put_wchar_wchar());

	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_get_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_get_wchar_const_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_put_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_put_wchar_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_get_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_get_wchar_const_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_put_wchar());
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_time_put_wchar_wchar());
# endif  //!_STLP_NO_WCHAR_T
	//support for back_insert iterator
	get_locale_impl_S_classic_facets()[index++] = __REINTERPRET_CAST(locale::facet*,&get_locale_impl_S_num_put_char_back_insert_iterator());
	get_locale_impl_S_classic_facets()[index] = 0;
}
# endif //__LIBSTD_CPP_SYMBIAN32_WSD__

_Locale_impl* 
_Locale_impl::make_classic_locale() {
  // The classic locale contains every facet that belongs to a category.
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	_Locale_impl* classic = __REINTERPRET_CAST(_Locale_impl*, &get_locale_impl_S_classic_locale());
# else
	_Locale_impl* classic = __REINTERPRET_CAST(_Locale_impl*, &_S_classic_locale);
# endif //__LIBSTD_CPP_SYMBIAN32_WSD__
  
  new (classic) _Locale_impl("C");

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  classic->facets = get_locale_impl_S_classic_facets();
  classic->_M_size = get_locale_id_S_max();
# else
  classic->facets = _S_classic_facets;
  classic->_M_size = locale::id::_S_max;
# endif  //__LIBSTD_CPP_SYMBIAN32_WSD__

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  // ctype category
  new(&get_locale_impl_S_ctype_char()) ctype<char>(0, false, 1);
  // collate category
  new(&get_locale_impl_S_collate_char()) collate<char>(1);
  new(&get_locale_impl_S_codecvt_char()) codecvt<char, char, mbstate_t>(1);
  // numeric category
  new(&get_locale_impl_S_numpunct_char()) numpunct<char>(1);
  new (&get_locale_impl_S_num_get_char()) num_get<char, istreambuf_iterator<char, char_traits<char> > >(1);
  new (&get_locale_impl_S_num_put_char()) num_put<char, ostreambuf_iterator<char, char_traits<char> > >(1);
  new (&get_locale_impl_S_time_get_char()) time_get<char, istreambuf_iterator<char, char_traits<char> > >(1);
  new (&get_locale_impl_S_time_put_char()) time_put<char, ostreambuf_iterator<char, char_traits<char> > >(1);  
  new (&get_locale_impl_S_num_get_char_const_char()) num_get<char, const char*>(1);
  new (&get_locale_impl_S_num_put_char_char()) num_put<char, char*>(1);
  new (&get_locale_impl_S_num_put_char_back_insert_iterator()) num_put<char, back_insert_iterator<string> >(1);
  new (&get_locale_impl_S_time_get_char_const_char()) time_get<char, const char*>(1);
  new (&get_locale_impl_S_time_put_char_char()) time_put<char, char*>(1);

  // monetary category
  new (&get_locale_impl_S_moneypunct_true_char()) moneypunct<char, true>(1);
  new (&get_locale_impl_S_moneypunct_false_char()) moneypunct<char, false>(1);
  new (&get_locale_impl_S_money_get_char()) money_get<char, istreambuf_iterator<char, char_traits<char> > >(1);
  new (&get_locale_impl_S_money_put_char()) money_put<char, ostreambuf_iterator<char, char_traits<char> > >(1);
  new (&get_locale_impl_S_money_get_char_const_char()) money_get<char, const char*>(1);
  new (&get_locale_impl_S_money_put_char_char()) money_put<char, char*>(1);

  // messages category
  new (&get_locale_impl_S_messages_char())messages<char>(&get_locale_impl_messages());
# ifndef _STLP_NO_WCHAR_T
  // ctype category
  new(&get_locale_impl_S_ctype_wchar()) ctype<wchar_t>(1);
  // collate category
  new(&get_locale_impl_S_collate_wchar()) collate<wchar_t>(1);
  new(&get_locale_impl_S_codecvt_wchar()) codecvt<wchar_t, char, mbstate_t>(1);
  // numeric category
  new(&get_locale_impl_S_numpunct_wchar()) numpunct<wchar_t>(1);
  new (&get_locale_impl_S_num_get_wchar()) num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&get_locale_impl_S_num_put_wchar()) num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&get_locale_impl_S_time_get_wchar()) time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&get_locale_impl_S_time_put_wchar()) time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&get_locale_impl_S_messages_wchar())messages<wchar_t>(&get_locale_impl_messages());
  new (&get_locale_impl_S_num_get_wchar_const_wchar()) num_get<wchar_t, const wchar_t*>(1); 
  new (&get_locale_impl_S_num_put_wchar_wchar()) num_put<wchar_t, wchar_t*>(1); 
  new (&get_locale_impl_S_time_get_wchar_const_wchar()) time_get<wchar_t, const wchar_t*>(1); 
  new (&get_locale_impl_S_time_put_wchar_wchar()) time_put<wchar_t, wchar_t*>(1);   
  // monetary category
  new (&get_locale_impl_S_moneypunct_true_wchar()) moneypunct<wchar_t, true>(1);
  new (&get_locale_impl_S_moneypunct_false_wchar()) moneypunct<wchar_t, false>(1);
  new (&get_locale_impl_S_money_get_wchar()) money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&get_locale_impl_S_money_put_wchar()) money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&get_locale_impl_S_money_get_wchar_const_wchar()) money_get<wchar_t, const wchar_t*>(1);
  new (&get_locale_impl_S_money_put_wchar_wchar()) money_put<wchar_t, wchar_t*>(1);
# endif //!_STLP_NO_WCHAR_T
# else //!__LIBSTD_CPP_SYMBIAN32_WSD__
  // ctype category
  new(&_S_ctype_char) ctype<char>(0, false, 1);
  // collate category
  new(&_S_collate_char) collate<char>(1);
  new(&_S_codecvt_char) codecvt<char, char, mbstate_t>(1);
  // numeric category
  new(&_S_numpunct_char) numpunct<char>(1);
  new (&_S_num_get_char) num_get<char, istreambuf_iterator<char, char_traits<char> > >(1);
  new (&_S_num_put_char) num_put<char, ostreambuf_iterator<char, char_traits<char> > >(1);
  new (&_S_time_get_char) time_get<char, istreambuf_iterator<char, char_traits<char> > >(1);
  new (&_S_time_put_char) time_put<char, ostreambuf_iterator<char, char_traits<char> > >(1);
  new (&_S_num_get_char_const_char) num_get<char, const char*>(1);
  new (&_S_num_put_char_char) num_put<char, char*>(1);
  new (&_S_time_get_char_const_char) time_get<char, const char*>(1);
  new (&_S_time_put_char_char) time_put<char, char*>(1);
  // monetary category
  new (&_S_moneypunct_true_char) moneypunct<char, true>(1);
  new (&_S_moneypunct_false_char) moneypunct<char, false>(1);
  new (&_S_money_get_char) money_get<char, istreambuf_iterator<char, char_traits<char> > >(1);
  new (&_S_money_put_char) money_put<char, ostreambuf_iterator<char, char_traits<char> > >(1);
  new (&_S_money_get_char_const_char) money_get<char, const char*>(1);
  new (&_S_money_put_char_char) money_put<char, char*>(1);
  // messages category
  new (&_S_messages_char)messages<char>(&_Null_messages);

# ifndef _STLP_NO_WCHAR_T
  // ctype category
  new(&_S_ctype_wchar) ctype<wchar_t>(1);
  // collate category
  new(&_S_collate_wchar) collate<wchar_t>(1);
  new(&_S_codecvt_wchar) codecvt<wchar_t, char, mbstate_t>(1);
  // numeric category
  new(&_S_numpunct_wchar) numpunct<wchar_t>(1);
  new (&_S_num_get_wchar) num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&_S_num_put_wchar) num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&_S_time_get_wchar) time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&_S_time_put_wchar) time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&_S_messages_wchar)messages<wchar_t>(&_Null_messages);
  new (&_S_num_get_wchar_const_wchar) num_get<wchar_t, const wchar_t*>(1); 
  new (&_S_num_put_wchar_wchar) num_put<wchar_t, wchar_t*>(1); 
  new (&_S_time_get_wchar_const_wchar) time_get<wchar_t, const wchar_t*>(1); 
  new (&_S_time_put_wchar_wchar) time_put<wchar_t, wchar_t*>(1); 
  // monetary category
  new (&_S_moneypunct_true_wchar) moneypunct<wchar_t, true>(1);
  new (&_S_moneypunct_false_wchar) moneypunct<wchar_t, false>(1);
  new (&_S_money_get_wchar) money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&_S_money_put_wchar) money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >(1);
  new (&_S_money_get_wchar_const_wchar) money_get<wchar_t, const wchar_t*>(1);
  new (&_S_money_put_wchar_wchar) money_put<wchar_t, wchar_t*>(1);
# endif //!_STLP_NO_WCHAR_T

# endif //__LIBSTD_CPP_SYMBIAN32_WSD__
  return classic;
}


//----------------------------------------------------------------------

// Declarations of (non-template) facets' static data members
# if !defined(__LIBSTD_CPP_SYMBIAN32_WSD__) && !defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)

size_t locale::id::_S_max = 39;

_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id collate<char>::id = { 1 };

_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id ctype<char>::id = { 2 };

# ifndef _STLP_NO_MBSTATE_T
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id codecvt<char, char, mbstate_t>::id = { 3 };
#  ifndef _STLP_NO_WCHAR_T
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id codecvt<wchar_t, char, mbstate_t>::id = { 22 };
#  endif//!_STLP_NO_WCHAR_T
# endif//!_STLP_NO_MBSTATE_T

_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id moneypunct<char, true>::id = { 4 };
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id moneypunct<char, false>::id = { 5 };
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id messages<char>::id = { 7 };
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id numpunct<char>::id = { 6 } ;

# ifndef _STLP_NO_WCHAR_T
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id collate<wchar_t>::id = { 20 };
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id ctype<wchar_t>::id = { 21 };

_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id moneypunct<wchar_t, true>::id = { 23 } ;
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id moneypunct<wchar_t, false>::id = { 24 } ;

_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id numpunct<wchar_t>::id = { 25 };
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id messages<wchar_t>::id = { 26 };
# endif //!_STLP_NO_WCHAR_T
# endif //!__LIBSTD_CPP_SYMBIAN32_WSD__
//
// locale class
//

_STLP_EXP_DECLSPEC locale::facet::~facet() {}

# if ! defined ( _STLP_MEMBER_TEMPLATES ) || defined (_STLP_INLINE_MEMBER_TEMPLATES)
// members that fail to be templates 
bool locale::operator()(const string& __x,
                        const string& __y) const {
  return __locale_do_operator_call(this, __x, __y);
}

#  ifndef _STLP_NO_WCHAR_T
bool locale::operator()(const wstring& __x,
                  const wstring& __y) const {
  return __locale_do_operator_call(this, __x, __y);
}
#  endif
# endif

# if !defined(__LIBSTD_CPP_SYMBIAN32_WSD__) && !defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
_Locale_impl*   _Locale_impl::_S_global_impl    = 0;
static _Stl_aligned_buffer<locale> _S_b_classic;
_STLP_STATIC_MUTEX _Locale_impl::_S_global_locale_lock _STLP_MUTEX_INITIALIZER;
// locale          _Locale_impl::_S_classic(__REINTERPRET_CAST(_Locale_impl*, &_S_classic_locale));
# endif



  
//----------------------------------------------------------------------
// class locale

_STLP_EXP_DECLSPEC void _STLP_CALL
locale::_M_throw_runtime_error(const char* name)
{
  char buf[256];

  if (name) {
    const char* prefix = "bad locale name: ";
    strcpy(buf, prefix);
    strncat(buf, name, 256 - strlen(prefix));
    buf[255] = '\0';
  }
  else {
    strcpy(buf, "locale error");
  }
  _STLP_THROW(runtime_error(buf));
}

#if defined(__SYMBIAN32__) || (!( defined (__BORLANDC__) && defined(_RTLDLL)))

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
void ios_base_Loc_init_S_count_init()
{
	get_ios_base_Loc_init_S_count() = 0;	
}
# else
long ios_base::_Loc_init::_S_count = 0;
# endif

_STLP_EXP_DECLSPEC ios_base::_Loc_init::_Loc_init() {
  //  if (_S_count++ == 0)
      locale::_S_initialize();
}

_STLP_EXP_DECLSPEC ios_base::_Loc_init::~_Loc_init() {
  //    if (--_S_count == 0)
      locale::_S_uninitialize();
}

#endif /* _RTLDLL */

// Initialization of the locale system.  This must be called before
// any locales are constructed.  (Meaning that it must be called when
// the I/O library itself is initialized.)
void _STLP_CALL
locale::_S_initialize()
{
  // additional check for singleton count : linker may choose to alter the order of function calls on initialization
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	if (get_ios_base_Loc_init_S_count()++ > 0 )
    	return;
# else
	if (ios_base::_Loc_init::_S_count++ > 0 )
    	return;
# endif  
  
  _Stl_loc_assign_ids();
  
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	get_locale_impl_S_global_impl() = _Locale_impl::make_classic_locale();
	locale* loc = __REINTERPRET_CAST(locale*, &get_locale_impl_S_b_classic());
	new (loc) locale (get_locale_impl_S_global_impl());
	global_iostream_init();
# else
	_Locale_impl::_S_global_impl = _Locale_impl::make_classic_locale();
	locale* loc = __REINTERPRET_CAST(locale*, &_S_b_classic);
	new (loc) locale (_Locale_impl::_S_global_impl);
# endif  
  
}

void _STLP_CALL
locale::_S_uninitialize()
{
  // additional check for singleton count : linker may choose to alter the order of function calls on initialization
 #if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
 	if (--get_ios_base_Loc_init_S_count() != 0 )
    	return;
 # else
 	if (--ios_base::_Loc_init::_S_count != 0 )
    	return;
 # endif
  
 #if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
 	get_locale_impl_S_global_impl()->decr();
 # else
 	_Locale_impl::_S_global_impl->decr();
 # endif	  
}

// Default constructor: create a copy of the global locale.
_STLP_EXP_DECLSPEC locale::locale() : _M_impl(0) {
 #if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
 	_M_impl = _S_copy_impl(get_locale_impl_S_global_impl());
 # else
 	_M_impl = _S_copy_impl(_Locale_impl::_S_global_impl);
 # endif  
}

_STLP_EXP_DECLSPEC locale::locale(_Locale_impl* impl) : _M_impl(impl)
{}

// Copy constructor
_STLP_EXP_DECLSPEC locale::locale(const locale& L) _STLP_NOTHROW
  : _M_impl(0)
{
  _M_impl = _S_copy_impl(L._M_impl);
}

// Destructor.
_STLP_EXP_DECLSPEC locale::~locale() _STLP_NOTHROW
{
  if (_M_impl) {
  _M_impl->decr();
  _M_impl = 0;
  }
}

// Assignment operator.  Much like the copy constructor: just a bit of
// pointer twiddling.
_STLP_EXP_DECLSPEC const locale& locale::operator=(const locale& L) _STLP_NOTHROW
{
  if (this->_M_impl != L._M_impl) {
    this->_M_impl->decr();
    this->_M_impl = _S_copy_impl(L._M_impl);
  }
  return *this;
}

_STLP_EXP_DECLSPEC locale::facet* locale::_M_get_facet(const locale::id& n) const
{
  return n._M_index < _M_impl->size()
    ? __REINTERPRET_CAST(locale::facet*,_M_impl->facets[n._M_index])
    : __REINTERPRET_CAST(locale::facet*, 0);
}

_STLP_EXP_DECLSPEC locale::facet* locale::_M_use_facet(const locale::id& n) const
{
  locale::facet* f = (n._M_index < _M_impl->size()
		      ? __REINTERPRET_CAST(locale::facet*,_M_impl->facets[n._M_index])
		      : __REINTERPRET_CAST(locale::facet*, 0));
  if (!f)
    _M_impl->_M_throw_bad_cast();
  return f;
}

_STLP_EXP_DECLSPEC string locale::name() const {
  return _M_impl->name;
}

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
void locale_impl_string_init()
{
	get_locale_impl_nameless().copy("*", 1);
}
# else
static string _Nameless("*");
# endif

// Compare two locales for equality.
_STLP_EXP_DECLSPEC bool locale::operator==(const locale& L) const {

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  return this->_M_impl == L._M_impl ||
         (this->name() == L.name() && this->name() != get_locale_impl_nameless());
# else
  return this->_M_impl == L._M_impl ||
         (this->name() == L.name() && this->name() != _Nameless);
# endif  //__LIBSTD_CPP_SYMBIAN32_WSD__       
}

_STLP_EXP_DECLSPEC bool locale::operator!=(const locale& L) const {
  return !(*this == L);
}

// Static member functions.
_STLP_EXP_DECLSPEC const locale&  _STLP_CALL
locale::classic() {
  //  return _Locale_impl::_S_classic;
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  return *__REINTERPRET_CAST(const locale*, &get_locale_impl_S_b_classic());	
# else  
  return *__REINTERPRET_CAST(const locale*, &_S_b_classic);
# endif
}

_STLP_EXP_DECLSPEC locale  _STLP_CALL
locale::global(const locale& L) 
{
  locale old;                   // A copy of the old global locale.

  L._M_impl->incr();
  { 

#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    _STLP_auto_lock lock(get_locale_impl_S_global_locale_lock());\
    get_locale_impl_S_global_impl()->decr();     // We made a copy, so it can't be zero.
    get_locale_impl_S_global_impl() = L._M_impl;
# else
    _STLP_auto_lock lock(_Locale_impl::_S_global_locale_lock);\
	_Locale_impl::_S_global_impl->decr();     // We made a copy, so it can't be zero.
    _Locale_impl::_S_global_impl = L._M_impl;
# endif    
  }

                                // Set the global C locale, if appropriate.
#if !defined(_STLP_WINCE)
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
	if (L.name() != get_locale_impl_nameless())
# else
	if (L.name() != _Nameless)
# endif  
    setlocale(LC_ALL, L.name().c_str());
#endif

  return old;
}


// static data members.

# if !defined (_STLP_STATIC_CONST_INIT_BUG) && ! defined (_STLP_USE_DECLSPEC)

const locale::category locale::none;
const locale::category locale::collate;
const locale::category locale::ctype;
const locale::category locale::monetary;
const locale::category locale::numeric;
const locale::category locale::time; 
const locale::category locale::messages;
const locale::category locale::all;

# endif


#if defined(__SYMBIAN32__) && defined( __WINSCW__)

extern "C" bool __uncaught_exception(void);

_STLP_EXP_DECLSPEC bool uncaught_exception()
{
    // providing default implementation

#if __MWERKS__ > 0x3200
	return __uncaught_exception();
#else
	// no uncaught_exception() implementation on CW 2.4.7
	return false;
#endif

}
#endif

_STLP_END_NAMESPACE

//
// Facets included in classic locale :
//



