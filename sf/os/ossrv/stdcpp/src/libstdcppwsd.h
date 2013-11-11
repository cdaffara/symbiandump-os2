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



# ifndef LIBSTDCPPWSD_H
# define LIBSTDCPPWSD_H

# include "stlport_prefix.h"

#include "locale_impl.h"
#include <locale>
#include <typeinfo>
#include "c_locale.h"
#include "aligned_buffer.h"

#include <stl/_codecvt.h>
#include <stl/_collate.h>
#include <stl/_ctype.h>
#include <stl/_monetary.h>
#include "message_facets.h"

#include <clocale>             // C locale header file.
#include <vector>
#include <string>
#include <stl/_locale.h>
#include "c_locale.h"
#include <hash_map>

#include <stl/_alloc.h>
#include <stl/_fstream.h>

#include "complex_impl.h"

#include <cfloat>
#include <cmath>
# include "message_facets.h"

#include <assert.h>
#include <stl/_new.h>
#include <stl/_map.h>

#ifdef __WINSCW__
extern void* GetGlobalTlsData();
#endif
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
void* SetBackendHeap();
void ReSetUserHeap(void* oldHeap);
#endif


_STLP_BEGIN_NAMESPACE

#define MAX_LOCALE_CLASSIC_FACETS 128

//These are copied from locale_catalog.cpp
//Changing names to aviod compilation errors
struct __eqstr {
  bool operator()(const char* s1, const char* s2) const
    { return strcmp(s1, s2) == 0; }
};

class _Libcpp_wsd
{
public:

	//local_impl.cpp
	_Messages Locale_impl_Null_messages;

	locale::facet* Locale_impl_S_classic_facets[MAX_LOCALE_CLASSIC_FACETS];

	locale::id collate_char_id;

	locale::id ctype_char_id;

# ifndef _STLP_NO_MBSTATE_T
	locale::id codecvt_char_char_mbstate_id;
# ifndef _STLP_NO_WCHAR_T
	locale::id codecvt_wchar_char_mbstate_id;
# endif
# endif

	locale::id moneypunct_char_true_id;

	locale::id moneypunct_char_false_id;

	locale::id messages_char_id;

	locale::id numpunct_char_id;

# ifndef _STLP_NO_WCHAR_T
	locale::id collate_wchar_id;

	locale::id ctype_wchar_id;

	locale::id moneypunct_wchar_true_id;

	locale::id moneypunct_wchar_false_id;

	locale::id numpunct_wchar_id;

	locale::id messages_wchar_id;

	locale::id num_get_wchar_istreambuf_iterator_id;

	locale::id num_get_wchar_wchar_const_id;

	locale::id num_put_wchar_ostreambuf_iterator_id;

	locale::id num_put_wchar_wchar_id;

	locale::id time_get_wchar_istreambuf_iterator_id;

	locale::id time_get_wchar_wchar_const_id;

	locale::id time_put_wchar_ostreambuf_iterator_id;

	locale::id time_put_wchar_wchar_id;

	locale::id money_get_wchar_istreambuf_iterator_id;

	locale::id money_get_wchar_wchar_const_id;

	locale::id money_put_wchar_ostreambuf_iterator_id;

	locale::id money_put_wchar_wchar_id;
#endif

	locale::id time_get_char_istreambuf_iterator_id;

	locale::id time_get_char_char_const_id;

	locale::id time_put_char_ostreambuf_iterator_id;

	locale::id time_put_char_char_id;

	locale::id num_get_char_istreambuf_iterator_id;

	locale::id num_get_char_char_const_id;

	locale::id num_put_char_ostreambuf_iterator_id;

	locale::id num_put_char_char_id;

	locale::id num_put_char_back_insert_iterator_id;

	locale::id money_get_char_istreambuf_iterator_id;

	locale::id money_get_char_char_const_id;

	locale::id money_put_char_ostreambuf_iterator_id;

	locale::id money_put_char_char_id;

	_Stl_aligned_buffer<_Locale_impl> Locale_impl_S_classic_locale;

	_Stl_aligned_buffer<collate<char> > Locale_impl_S_collate_char;

	_Stl_aligned_buffer<ctype<char> > Locale_impl_S_ctype_char;

# ifndef _STLP_NO_MBSTATE_T
	_Stl_aligned_buffer<codecvt<char, char, mbstate_t> > Locale_impl_S_codecvt_char;
# endif

	_Stl_aligned_buffer<moneypunct<char, true> > Locale_impl_S_moneypunct_true_char;
	_Stl_aligned_buffer<moneypunct<char, false> > Locale_impl_S_moneypunct_false_char;
	_Stl_aligned_buffer<numpunct<char> > Locale_impl_S_numpunct_char;
	_Stl_aligned_buffer<messages<char> > Locale_impl_S_messages_char;

	_Stl_aligned_buffer<money_get<char, istreambuf_iterator<char, char_traits<char> > > > Locale_impl_S_money_get_char;
	_Stl_aligned_buffer<money_put<char, ostreambuf_iterator<char, char_traits<char> > > > Locale_impl_S_money_put_char;
	_Stl_aligned_buffer<num_get<char, istreambuf_iterator<char, char_traits<char> > > > Locale_impl_S_num_get_char;
	_Stl_aligned_buffer<num_put<char, ostreambuf_iterator<char, char_traits<char> > > > Locale_impl_S_num_put_char;
	_Stl_aligned_buffer<time_get<char, istreambuf_iterator<char, char_traits<char> > > > Locale_impl_S_time_get_char;
	_Stl_aligned_buffer<time_put<char, ostreambuf_iterator<char, char_traits<char> > > > Locale_impl_S_time_put_char;

	_Stl_aligned_buffer<money_get<char, const char*> > Locale_impl_S_money_get_char_const_char;
	_Stl_aligned_buffer<money_put<char, char*> > Locale_impl_S_money_put_char_char;
	_Stl_aligned_buffer<num_get<char, const char*> > Locale_impl_S_num_get_char_const_char;
	_Stl_aligned_buffer<num_put<char, char*> > Locale_impl_S_num_put_char_char;
	_Stl_aligned_buffer<num_put<char, back_insert_iterator<string> > > Locale_impl_S_num_put_char_back_insert_iterator;
	_Stl_aligned_buffer<time_get<char, const char*> > Locale_impl_S_time_get_char_const_char;
	_Stl_aligned_buffer<time_put<char, char*> > Locale_impl_S_time_put_char_char;

# ifndef _STLP_NO_WCHAR_T
	_Stl_aligned_buffer<collate<wchar_t> > Locale_impl_S_collate_wchar;
	_Stl_aligned_buffer<ctype<wchar_t> > Locale_impl_S_ctype_wchar;
# ifndef _STLP_NO_MBSTATE_T
	_Stl_aligned_buffer<codecvt<wchar_t, char, mbstate_t> > Locale_impl_S_codecvt_wchar;
# endif

	_Stl_aligned_buffer<moneypunct<wchar_t, true> > Locale_impl_S_moneypunct_true_wchar;
	_Stl_aligned_buffer<moneypunct<wchar_t, false> > Locale_impl_S_moneypunct_false_wchar;
	_Stl_aligned_buffer<numpunct<wchar_t> > Locale_impl_S_numpunct_wchar;
	_Stl_aligned_buffer<messages<wchar_t> > Locale_impl_S_messages_wchar;

	_Stl_aligned_buffer<money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > > Locale_impl_S_money_get_wchar;
	_Stl_aligned_buffer<money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > > Locale_impl_S_money_put_wchar;
	_Stl_aligned_buffer<num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > > Locale_impl_S_num_get_wchar;
	_Stl_aligned_buffer<num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > > Locale_impl_S_num_put_wchar;
	_Stl_aligned_buffer<time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > > Locale_impl_S_time_get_wchar;
	_Stl_aligned_buffer<time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > > Locale_impl_S_time_put_wchar;


	_Stl_aligned_buffer<money_get<wchar_t, const wchar_t*> > Locale_impl_S_money_get_wchar_const_wchar;
	_Stl_aligned_buffer<money_put<wchar_t, wchar_t*> > Locale_impl_S_money_put_wchar_wchar;
	_Stl_aligned_buffer<num_get<wchar_t, const wchar_t*> > Locale_impl_S_num_get_wchar_const_wchar;
	_Stl_aligned_buffer<num_put<wchar_t, wchar_t*> > Locale_impl_S_num_put_wchar_wchar;
	_Stl_aligned_buffer<time_get<wchar_t, const wchar_t*> > Locale_impl_S_time_get_wchar_const_wchar;
	_Stl_aligned_buffer<time_put<wchar_t, wchar_t*> > Locale_impl_S_time_put_wchar_wchar;
#endif

	_Stl_aligned_buffer<locale> Locale_impl_S_b_classic;

	string locale_impl_nameless;

	//_locale.h
	size_t locale_id_S_max;

	//locale_impl.h
	_Locale_impl*   Locale_impl_S_global_impl;
	_STLP_STATIC_MUTEX Locale_impl_S_global_locale_lock;

	//_fstream.h
	size_t fstream_Filebuf_base_M_page_size;

	//ios.cpp
	_STLP_STATIC_MUTEX ios_xalloc_L;
	long ios_iword_dummy;
	void* ios_pword_dummy;

	//complex_exp.cpp
	float complex_exp_float_ln10_inv;
	double complex_exp_double_ln10_inv;
	long double complex_exp_long_double_ln10_inv;

	//num_get.cpp
	char num_get_narrow_digits[11];
	char num_get_narrow_xdigits[13];

	//locale.cpp
	_STLP_STATIC_MUTEX locale_Index_lock;

	//_ios_base.h
	bool ios_base_S_was_synced;
	int ios_base_S_index;
	long ios_base_Init_S_count; //ios_base::Init class
	long ios_base_Loc_init_S_count; //ios_base::_Loc_init class
	ios_base::_Loc_init *_LocInit;
	ios_base::Init *_IosInit;


	//locale_catalog.cpp
	hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>* locale_catalog_ctype_hash;
	hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>* locale_catalog_numeric_hash;
	hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>* locale_catalog_time_hash;
	hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>* locale_catalog_collate_hash;
	hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>* locale_catalog_monetary_hash;
	hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>* locale_catalog_messages_hash;
	_STLP_STATIC_MUTEX locale_catalog_category_hash_lock;

	//numpunct.cpp
	string numpunct_char_m_truename;
	string numpunct_char_m_falsename;
	string numpunct_char_m_grouping;
# ifndef _STLP_NO_WCHAR_T
	wstring numpunct_wchar_m_truename;
	wstring numpunct_wchar_m_falsename;
	string  numpunct_wchar_m_grouping;
# endif

	//iostream.cpp
	istream* cin;
	ostream* cout;
	ostream* cerr;
	ostream* clog;

#ifndef _STLP_NO_WCHAR_T
	wistream* wcin;
	wostream* wcout;
	wostream* wcerr;
	wostream* wclog;
#endif

	//complex_trig.cpp
	float complex_trig_float_limit;
	double complex_trig_double_limit;

	//monetary.cpp
	string monetary_S_empty_string;
# ifndef _STLP_NO_WCHAR_T
	wstring monetary_S_empty_wstring;
# endif //_STLP_NO_WCHAR_T
    new_handler _new_handler;
    //variable to check whether lib is initialized
    bool is_Initialized;
	//constructor
	_Libcpp_wsd();

	//destructor
	~_Libcpp_wsd();
	map<string, locale::id> ctype_charT_ids; //for ctype<charT>
	map<string, locale::id> numpunct_charT_ids; //for numpunct<charT>
	map<string, locale::id> moneypunct_charT_ids;//for moneypunct<charT>
	
};
#ifndef __WINSCW__
extern _Libcpp_wsd* g_libcpp_wsd;
#endif


//return the global class pointer
inline _Libcpp_wsd&
get_libcpp_wsdptr()
	{
		//get the TLS pointer
#ifdef __WINSCW__
void* oldHeap = SetBackendHeap();

		_Libcpp_wsd* g_libcpp_wsd = (_Libcpp_wsd*)GetGlobalTlsData();		
#else
		void* oldHeap = SetBackendHeap();
		if(g_libcpp_wsd == NULL)
		{
			g_libcpp_wsd = new _Libcpp_wsd();
			g_libcpp_wsd->is_Initialized = false;			
		}
#endif //__WINSCW__

		if(g_libcpp_wsd->is_Initialized != true)
  		{
			//this has to be here to avoid infinite recursion
			g_libcpp_wsd->is_Initialized = true;
			g_libcpp_wsd->_LocInit = new ios_base::_Loc_init();
			g_libcpp_wsd->_IosInit = new ios_base::Init();
		}
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
		ReSetUserHeap(oldHeap);
#endif //__WINSCW__		
		return (*g_libcpp_wsd);
	}

#define LIBCPP_WSD_PTR ( get_libcpp_wsdptr())

inline _Locale_impl*&
get_locale_impl_S_global_impl()
	{
		return get_libcpp_wsdptr().Locale_impl_S_global_impl;
	}

inline _STLP_STATIC_MUTEX&
get_locale_impl_S_global_locale_lock()
	{
		return get_libcpp_wsdptr().Locale_impl_S_global_locale_lock;
	}

inline _Messages&
get_locale_impl_messages()
	{
		return get_libcpp_wsdptr().Locale_impl_Null_messages;
	}

inline locale::facet**
get_locale_impl_S_classic_facets()
	{
		return get_libcpp_wsdptr().Locale_impl_S_classic_facets;
	}

# ifndef _STLP_NO_MBSTATE_T
inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
codecvt<char, char, mbstate_t>::GetFacetLocaleId()
    {
    	return get_libcpp_wsdptr().codecvt_char_char_mbstate_id;
    }
#  ifndef _STLP_NO_WCHAR_T

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
codecvt<wchar_t, char, mbstate_t>::GetFacetLocaleId()
    {
   	 	return get_libcpp_wsdptr().codecvt_wchar_char_mbstate_id;
    }
#  endif//_STLP_NO_WCHAR_T
# endif//_STLP_NO_MBSTATE_T

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
collate<char>::GetFacetLocaleId()
    {
   		return get_libcpp_wsdptr().collate_char_id;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
ctype<char>::GetFacetLocaleId()
    {
    	return get_libcpp_wsdptr().ctype_char_id;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
moneypunct<char, true>::GetFacetLocaleId()
	{
		return get_libcpp_wsdptr().moneypunct_char_true_id;
	}

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
moneypunct<char, false>::GetFacetLocaleId()
    {
    	return get_libcpp_wsdptr().moneypunct_char_false_id;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
messages<char>::GetFacetLocaleId()
    {
    	return get_libcpp_wsdptr().messages_char_id;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
numpunct<char>::GetFacetLocaleId()
    {
    	return get_libcpp_wsdptr().numpunct_char_id;
    }

# ifndef _STLP_NO_WCHAR_T
inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
collate<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsdptr().collate_wchar_id;
	}

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
ctype<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsdptr().ctype_wchar_id;
	}

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
moneypunct<wchar_t, true>::GetFacetLocaleId()
	{
		return get_libcpp_wsdptr().moneypunct_wchar_true_id;
	}

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
moneypunct<wchar_t, false>::GetFacetLocaleId()
	{
		return get_libcpp_wsdptr().moneypunct_wchar_false_id;
	}

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
numpunct<wchar_t>::GetFacetLocaleId()
	{
		return get_libcpp_wsdptr().numpunct_wchar_id;
	}

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
messages<wchar_t>::GetFacetLocaleId()
	{
        return get_libcpp_wsdptr().messages_wchar_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_get<_CharT, _InputIter>::GetFacetLocaleId(istreambuf_iterator<wchar_t, char_traits<wchar_t> >* )
    {
        return get_libcpp_wsdptr().num_get_wchar_istreambuf_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_get<_CharT, _InputIter>::GetFacetLocaleId(const wchar_t**)
    {
        return get_libcpp_wsdptr().num_get_wchar_wchar_const_id;
    }

template <class _CharT, class _OutputIter>
 _STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_put<_CharT, _OutputIter>::GetFacetLocaleId(ostreambuf_iterator<wchar_t, char_traits<wchar_t> > *)
    {
        return get_libcpp_wsdptr().num_put_wchar_ostreambuf_iterator_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_put<_CharT, _OutputIter>::GetFacetLocaleId(wchar_t**)
    {
        return get_libcpp_wsdptr().num_put_wchar_wchar_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_get<_CharT, _InputIter>::GetFacetLocaleId(istreambuf_iterator<wchar_t, char_traits<wchar_t> >*)
    {
        return get_libcpp_wsdptr().time_get_wchar_istreambuf_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_get<_CharT, _InputIter>::GetFacetLocaleId(const wchar_t**)
    {
        return get_libcpp_wsdptr().time_get_wchar_wchar_const_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_put<_CharT, _OutputIter>::GetFacetLocaleId(ostreambuf_iterator<wchar_t, char_traits<wchar_t> >*)
    {
        return get_libcpp_wsdptr().time_put_wchar_ostreambuf_iterator_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_put<_CharT, _OutputIter>::GetFacetLocaleId(wchar_t**)
    {
        return get_libcpp_wsdptr().time_put_wchar_wchar_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_get<_CharT, _InputIter>::GetFacetLocaleId(istreambuf_iterator<wchar_t, char_traits<wchar_t> >*)
    {
        return get_libcpp_wsdptr().money_get_wchar_istreambuf_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_get<_CharT, _InputIter>::GetFacetLocaleId(const wchar_t**)
    {
        return get_libcpp_wsdptr().money_get_wchar_wchar_const_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_put<_CharT, _OutputIter>::GetFacetLocaleId(ostreambuf_iterator<wchar_t, char_traits<wchar_t> >*)
    {
        return get_libcpp_wsdptr().money_put_wchar_ostreambuf_iterator_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_put<_CharT, _OutputIter>::GetFacetLocaleId(wchar_t**)
    {
        return get_libcpp_wsdptr().money_put_wchar_wchar_id;
    }
# endif //_STLP_NO_WCHAR_T

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_get<_CharT, _InputIter>::GetFacetLocaleId()
    {
        _InputIter* dummyPtr = NULL;
        return GetFacetLocaleId(dummyPtr);
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_put<_CharT, _OutputIter>::GetFacetLocaleId()
    {
        _OutputIter* dummyPtr = NULL;
        return GetFacetLocaleId(dummyPtr);
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_get<_CharT, _InputIter>::GetFacetLocaleId()
    {
        _InputIter* dummyPtr = NULL;
        return GetFacetLocaleId(dummyPtr);
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_put<_CharT, _OutputIter>::GetFacetLocaleId()
    {
        _OutputIter* dummyPtr = NULL;
        return GetFacetLocaleId(dummyPtr);
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_get<_CharT, _InputIter>::GetFacetLocaleId()
    {
        _InputIter* dummyPtr = NULL;
        return GetFacetLocaleId(dummyPtr);
    }
template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_put<_CharT, _OutputIter>::GetFacetLocaleId()
    {
        _OutputIter* dummyPtr = NULL;
        return GetFacetLocaleId(dummyPtr);
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_get<_CharT, _InputIter>::GetFacetLocaleId(istreambuf_iterator<char, char_traits<char> >*)
    {
        return get_libcpp_wsdptr().money_get_char_istreambuf_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_get<_CharT, _InputIter>::GetFacetLocaleId(const char  **)
    {
        return get_libcpp_wsdptr().money_get_char_char_const_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_put<_CharT, _OutputIter>::GetFacetLocaleId(ostreambuf_iterator<char, char_traits<char> >*)
    {
        return get_libcpp_wsdptr().money_put_char_ostreambuf_iterator_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
money_put<_CharT, _OutputIter>::GetFacetLocaleId(char**)
    {
        return get_libcpp_wsdptr().money_put_char_char_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_get<_CharT, _InputIter>::GetFacetLocaleId(istreambuf_iterator<char, char_traits<char> >* )
    {
        return get_libcpp_wsdptr().num_get_char_istreambuf_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_get<_CharT, _InputIter>::GetFacetLocaleId(const char**)
    {
        return get_libcpp_wsdptr().num_get_char_char_const_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_put<_CharT, _OutputIter>::GetFacetLocaleId(ostreambuf_iterator<char, char_traits<char> > *)
    {
        return get_libcpp_wsdptr().num_put_char_ostreambuf_iterator_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_put<_CharT, _OutputIter>::GetFacetLocaleId(char**)
    {
        return get_libcpp_wsdptr().num_put_char_char_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
num_put<_CharT, _OutputIter>::GetFacetLocaleId(back_insert_iterator<string> *)
    {
        return get_libcpp_wsdptr().num_put_char_back_insert_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_get<_CharT, _InputIter>::GetFacetLocaleId(istreambuf_iterator<char, char_traits<char> >*)
    {
        return get_libcpp_wsdptr().time_get_char_istreambuf_iterator_id;
    }

template <class _CharT, class _InputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_get<_CharT, _InputIter>::GetFacetLocaleId(const char**)
    {
        return get_libcpp_wsdptr().time_get_char_char_const_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_put<_CharT, _OutputIter>::GetFacetLocaleId(ostreambuf_iterator<char, char_traits<char> >*)
    {
        return get_libcpp_wsdptr().time_put_char_ostreambuf_iterator_id;
    }

template <class _CharT, class _OutputIter>
_STLP_STATIC_MEMBER_EXP_DECLSPEC locale::id&
time_put<_CharT, _OutputIter>::GetFacetLocaleId(char**)
	{
		return get_libcpp_wsdptr().time_put_char_char_id;
	}

inline _STLP_EXP_DECLSPEC locale::id&
Ctype_charT_GetFacetLocaleId(const char* type)
    {
    		if((get_libcpp_wsdptr().ctype_charT_ids).find(type)!= (get_libcpp_wsdptr().ctype_charT_ids).end())
				return (get_libcpp_wsdptr().ctype_charT_ids)[type];
		else
			{
				locale::id id;
				id._M_index = (size_t)-1;
				//(get_libcpp_wsdptr().ctype_charT_ids)[type] = id;
				memmove((void*)&get_libcpp_wsdptr().ctype_charT_ids[type],&id, sizeof(id));

				return (get_libcpp_wsdptr().ctype_charT_ids)[type];
			}
    }

inline _STLP_EXP_DECLSPEC locale::id&
Numpunct_charT_GetFacetLocaleId(const char* type)
    {
    		if((get_libcpp_wsdptr().numpunct_charT_ids).find(type)!= (get_libcpp_wsdptr().numpunct_charT_ids).end())
				return (get_libcpp_wsdptr().numpunct_charT_ids)[type];
		else
			{
				locale::id id;
				id._M_index = (size_t)-1;
				//(get_libcpp_wsdptr().numpunct_charT_ids)[type] = id;
				memmove((void*)&get_libcpp_wsdptr().numpunct_charT_ids[type],&id, sizeof(id));

				return (get_libcpp_wsdptr().numpunct_charT_ids)[type];
			}
    }
inline _STLP_EXP_DECLSPEC locale::id&
Moneypunct_charT_GetFacetLocaleId(const char* type)
    {
    		if((get_libcpp_wsdptr().moneypunct_charT_ids).find(type)!= (get_libcpp_wsdptr().moneypunct_charT_ids).end())
				return (get_libcpp_wsdptr().moneypunct_charT_ids)[type];
		else
			{
				locale::id id;
				id._M_index = (size_t)-1;
				//(get_libcpp_wsdptr().moneypunct_charT_ids)[type] = id;
				memmove((void*)&get_libcpp_wsdptr().moneypunct_charT_ids[type],&id, sizeof(id));
				return (get_libcpp_wsdptr().moneypunct_charT_ids)[type];
			}
    }

inline _Stl_aligned_buffer<_Locale_impl>&
get_locale_impl_S_classic_locale()
	{
		return get_libcpp_wsdptr().Locale_impl_S_classic_locale;
	}

inline _Stl_aligned_buffer<collate<char> >&
get_locale_impl_S_collate_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_collate_char;
	}

inline _Stl_aligned_buffer<ctype<char> >&
get_locale_impl_S_ctype_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_ctype_char;
	}

# ifndef _STLP_NO_MBSTATE_T
inline _Stl_aligned_buffer<codecvt<char, char, mbstate_t> >&
get_locale_impl_S_codecvt_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_codecvt_char;
	}
# endif

inline _Stl_aligned_buffer<moneypunct<char, true> >&
get_locale_impl_S_moneypunct_true_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_moneypunct_true_char;
	}

inline _Stl_aligned_buffer<moneypunct<char, false> >&
get_locale_impl_S_moneypunct_false_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_moneypunct_false_char;
	}

inline _Stl_aligned_buffer<numpunct<char> >&
get_locale_impl_S_numpunct_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_numpunct_char;
	}

inline _Stl_aligned_buffer<messages<char> >&
get_locale_impl_S_messages_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_messages_char;
	}

inline 	_Stl_aligned_buffer<money_get<char, istreambuf_iterator<char, char_traits<char> > > >&
get_locale_impl_S_money_get_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_get_char;
	}

inline _Stl_aligned_buffer<money_put<char, ostreambuf_iterator<char, char_traits<char> > > >&
get_locale_impl_S_money_put_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_put_char;
	}

inline 	_Stl_aligned_buffer<num_get<char, istreambuf_iterator<char, char_traits<char> > > >&
get_locale_impl_S_num_get_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_get_char;
	}

inline _Stl_aligned_buffer<num_put<char, ostreambuf_iterator<char, char_traits<char> > > >&
get_locale_impl_S_num_put_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_put_char;
	}

inline _Stl_aligned_buffer<time_get<char, istreambuf_iterator<char, char_traits<char> > > >&
get_locale_impl_S_time_get_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_get_char;
	}

inline _Stl_aligned_buffer<time_put<char, ostreambuf_iterator<char, char_traits<char> > > >&
get_locale_impl_S_time_put_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_put_char;
	}

inline _Stl_aligned_buffer<money_get<char, const char*> >&
get_locale_impl_S_money_get_char_const_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_get_char_const_char;
	}

inline _Stl_aligned_buffer<money_put<char, char*> >&
get_locale_impl_S_money_put_char_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_put_char_char;
	}

inline _Stl_aligned_buffer<num_get<char, const char*> >&
get_locale_impl_S_num_get_char_const_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_get_char_const_char;
	}

inline _Stl_aligned_buffer<num_put<char, char*> >&
get_locale_impl_S_num_put_char_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_put_char_char;
	}

inline _Stl_aligned_buffer<num_put<char,back_insert_iterator<string> > >&
get_locale_impl_S_num_put_char_back_insert_iterator()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_put_char_back_insert_iterator;
	}

inline _Stl_aligned_buffer<time_get<char, const char*> >&
get_locale_impl_S_time_get_char_const_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_get_char_const_char;
	}

inline _Stl_aligned_buffer<time_put<char, char*> >&
get_locale_impl_S_time_put_char_char()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_put_char_char;
	}

# ifndef _STLP_NO_WCHAR_T
inline _Stl_aligned_buffer<collate<wchar_t> >&
get_locale_impl_S_collate_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_collate_wchar;
	}

inline _Stl_aligned_buffer<ctype<wchar_t> >&
get_locale_impl_S_ctype_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_ctype_wchar;
	}
# ifndef _STLP_NO_MBSTATE_T
inline _Stl_aligned_buffer<codecvt<wchar_t, char, mbstate_t> >&
get_locale_impl_S_codecvt_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_codecvt_wchar;
	}
# endif //!_STLP_NO_MBSTATE_T

inline _Stl_aligned_buffer<moneypunct<wchar_t, true> >&
get_locale_impl_S_moneypunct_true_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_moneypunct_true_wchar;
	}

inline _Stl_aligned_buffer<moneypunct<wchar_t, false> >&
get_locale_impl_S_moneypunct_false_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_moneypunct_false_wchar;
	}

inline _Stl_aligned_buffer<numpunct<wchar_t> >&
get_locale_impl_S_numpunct_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_numpunct_wchar;
	}

inline _Stl_aligned_buffer<messages<wchar_t> >&
get_locale_impl_S_messages_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_messages_wchar;
	}

inline _Stl_aligned_buffer<money_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > >&
get_locale_impl_S_money_get_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_get_wchar;
	}

inline _Stl_aligned_buffer<money_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > >&
get_locale_impl_S_money_put_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_put_wchar;
	}

inline _Stl_aligned_buffer<num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > >&
get_locale_impl_S_num_get_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_get_wchar;
	}

inline _Stl_aligned_buffer<num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > >&
get_locale_impl_S_num_put_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_put_wchar;
	}

inline _Stl_aligned_buffer<time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > > >&
get_locale_impl_S_time_get_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_get_wchar;
	}

inline _Stl_aligned_buffer<time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > > >&
get_locale_impl_S_time_put_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_put_wchar;
	}

inline _Stl_aligned_buffer<money_get<wchar_t, const wchar_t*> >&
get_locale_impl_S_money_get_wchar_const_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_get_wchar_const_wchar;
	}

inline _Stl_aligned_buffer<money_put<wchar_t, wchar_t*> >&
get_locale_impl_S_money_put_wchar_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_money_put_wchar_wchar;
	}

inline _Stl_aligned_buffer<num_get<wchar_t, const wchar_t*> >&
get_locale_impl_S_num_get_wchar_const_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_get_wchar_const_wchar;
	}

inline _Stl_aligned_buffer<num_put<wchar_t, wchar_t*> >&
get_locale_impl_S_num_put_wchar_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_num_put_wchar_wchar;
	}

inline _Stl_aligned_buffer<time_get<wchar_t, const wchar_t*> >&
get_locale_impl_S_time_get_wchar_const_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_get_wchar_const_wchar;
	}

inline _Stl_aligned_buffer<time_put<wchar_t, wchar_t*> >&
get_locale_impl_S_time_put_wchar_wchar()
	{
		return get_libcpp_wsdptr().Locale_impl_S_time_put_wchar_wchar;
	}
# endif //!_STLP_NO_WCHAR_T

inline _Stl_aligned_buffer<locale>&
get_locale_impl_S_b_classic()
	{
		return get_libcpp_wsdptr().Locale_impl_S_b_classic;
	}

//_locale.h
inline _STLP_STATIC_MEMBER_EXP_DECLSPEC size_t&
get_locale_id_S_max()
	{
		return get_libcpp_wsdptr().locale_id_S_max;
	}

//_fstream.h
inline _STLP_EXPORT_DECLSPEC size_t&
get_fstream_Filebuf_Base_GetPageSize()
	{
		return get_libcpp_wsdptr().fstream_Filebuf_base_M_page_size;
	}

//ios.cpp
inline _STLP_STATIC_MUTEX&
get_ios_xalloc_L()
	{
		return get_libcpp_wsdptr().ios_xalloc_L;
	}

inline long&
get_ios_iword_dummy()
	{
		return get_libcpp_wsdptr().ios_iword_dummy;
	}

inline void*&
get_ios_pword_dummy()
	{
		return get_libcpp_wsdptr().ios_pword_dummy;
	}

//complex_exp.cpp
inline float&
get_complex_exp_float_ln10_inv()
	{
		return get_libcpp_wsdptr().complex_exp_float_ln10_inv;
	}

inline double&
get_complex_exp_double_ln10_inv()
	{
		return get_libcpp_wsdptr().complex_exp_double_ln10_inv;
	}

inline long double&
get_complex_exp_long_double_ln10_inv()
	{
		return get_libcpp_wsdptr().complex_exp_long_double_ln10_inv;
	}

//num_get.cpp
inline char*
get_num_get_narrow_digits()
	{
		return get_libcpp_wsdptr().num_get_narrow_digits;
	}

inline char*
get_num_get_narrow_xdigits()
	{
		return get_libcpp_wsdptr().num_get_narrow_xdigits;
	}

//locale.cpp
inline _STLP_STATIC_MUTEX&
get_locale_Index_lock()
	{
		return get_libcpp_wsdptr().locale_Index_lock;
	}

//_ios_base.h
inline bool&
get_ios_base_S_was_synced()
	{
		return get_libcpp_wsdptr().ios_base_S_was_synced;
	}

inline int&
get_ios_base_S_index()
	{
		return get_libcpp_wsdptr().ios_base_S_index;
	}

inline long&
get_ios_base_Init_S_count()
	{
		return get_libcpp_wsdptr().ios_base_Init_S_count;
	}

inline long&
get_ios_base_Loc_init_S_count()
	{
		return get_libcpp_wsdptr().ios_base_Loc_init_S_count;
	}

//locale_catalog.cpp
inline hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>*&
get_locale_catalog_ctype_hash()
	{
		return get_libcpp_wsdptr().locale_catalog_ctype_hash;
	}

inline hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>*&
get_locale_catalog_numeric_hash()
	{
		return get_libcpp_wsdptr().locale_catalog_numeric_hash;
	}

inline hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>*&
get_locale_catalog_time_hash()
	{
		return get_libcpp_wsdptr().locale_catalog_time_hash;
	}

inline hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>*&
get_locale_catalog_collate_hash()
	{
		return get_libcpp_wsdptr().locale_catalog_collate_hash;
	}

inline hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>*&
get_locale_catalog_monetary_hash()
	{
		return get_libcpp_wsdptr().locale_catalog_monetary_hash;
	}

inline hash_map<const char*, pair<void*, size_t>, hash<const char*>, __eqstr>*&
get_locale_catalog_messages_hash()
	{
		return get_libcpp_wsdptr().locale_catalog_messages_hash;
	}

inline _STLP_STATIC_MUTEX&
get_locale_catalog_category_hash_lock()
	{
		return get_libcpp_wsdptr().locale_catalog_category_hash_lock;
	}

//numpunct.cpp
inline _STLP_STATIC_MEMBER_EXP_DECLSPEC string&
numpunct<char>::GetNumPunct_M_truename()
    {
    	return get_libcpp_wsdptr().numpunct_char_m_truename;
    }
inline _STLP_STATIC_MEMBER_EXP_DECLSPEC string&
numpunct<char>::GetNumPunct_M_falsename()
    {
    	return get_libcpp_wsdptr().numpunct_char_m_falsename;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC string&
numpunct<char>::GetNumPunct_M_grouping()
    {
    	return get_libcpp_wsdptr().numpunct_char_m_grouping;
    }

#  ifndef _STLP_NO_WCHAR_T
inline _STLP_STATIC_MEMBER_EXP_DECLSPEC wstring&
numpunct<wchar_t>::GetNumPunct_M_Wchar_truename()
    {
    	return get_libcpp_wsdptr().numpunct_wchar_m_truename;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC wstring&
numpunct<wchar_t>::GetNumPunct_M_Wchar_falsename()
    {
    	return get_libcpp_wsdptr().numpunct_wchar_m_falsename;
    }

inline _STLP_STATIC_MEMBER_EXP_DECLSPEC string&
numpunct<wchar_t>::GetNumPunct_M_Wchar_grouping()
    {
    	return get_libcpp_wsdptr().numpunct_wchar_m_grouping;
    }
# endif
//iostream.cpp
inline ostream*&
getCErrStream()
	{
		return get_libcpp_wsdptr().cerr;
	}

inline ostream*&
getCoutStream()
    {
	    return get_libcpp_wsdptr().cout;
    }

inline ostream*&
getClogStream()
    {
	    return get_libcpp_wsdptr().clog;
    }

inline istream*&
getCinStream()
    {
	    return get_libcpp_wsdptr().cin;
    }

#ifndef _STLP_NO_WCHAR_T
inline wostream*&
getWCErrStream()
    {
	    return get_libcpp_wsdptr().wcerr;
    }

inline wostream*&
getWCoutStream()
    {
	    return get_libcpp_wsdptr().wcout;
    }

inline wostream*&
getWClogStream()
    {
	    return get_libcpp_wsdptr().wclog;
    }

inline wistream*&
getWCinStream()
    {
 	   return get_libcpp_wsdptr().wcin;
    }
#endif //_STLP_NO_WCHAR_T
//complex_trig.cpp
inline float&
get_complex_trig_float_limit()
	{
		return get_libcpp_wsdptr().complex_trig_float_limit;
	}

inline double&
get_complex_trig_double_limit()
	{
		return get_libcpp_wsdptr().complex_trig_double_limit;
	}

//locale_impl.cpp
inline string&
get_locale_impl_nameless()
	{
		return get_libcpp_wsdptr().locale_impl_nameless;
	}

//monetary.cpp
inline string&
get_monetary_S_empty_string()
	{
		return get_libcpp_wsdptr().monetary_S_empty_string;
	}

# ifndef _STLP_NO_WCHAR_T
inline wstring&
get_monetary_S_empty_wstring()
	{
		return get_libcpp_wsdptr().monetary_S_empty_wstring;
	}
#endif //_STLP_NO_WCHAR_T
inline new_handler& get_new_handler()
    {
    return get_libcpp_wsdptr()._new_handler;
    }

_STLP_END_NAMESPACE
# endif //LIBSTDCPPWSD_H
