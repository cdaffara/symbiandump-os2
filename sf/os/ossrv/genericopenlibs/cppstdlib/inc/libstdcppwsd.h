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
* Name        : libcppwsd.h
* Part of     : standard c++ library (wsd definitions)
* 
*
*/



#ifndef LIBSTDCPPWSD_H_
#define LIBSTDCPPWSD_H_

#ifdef __SYMBIAN32__WSD__   //stuff for WSD


// ******************************  WSD RELATED ************************//
#include <e32std.h>
#include <pls.h> // For emulator WSD API
#include <typeinfo>
#include <locale>
#include <clocale>             
#include <vector>
#include <string>
#include <hash_map>
#include <cfloat>
#include <cmath>
#include <assert.h>
#include <stl/_new.h>
#include <stl/_map.h>
#include <stl/_locale.h>
#include <stl/_numpunct.h>
#include <stl/_alloc.h>
#include <stl/_fstream.h>
#include <stl/_codecvt.h>
#include <stl/_collate.h>
#include <stl/_ctype.h>
#include <stl/_pthread_alloc.h>
#include <stl/_monetary.h>
#include <stl/_threads.h>


// Global hash tables for category objects.
typedef std::hash_map<std::string, std::pair<void*, size_t>, std::hash<std::string>, std::equal_to<std::string> > Category_Map;

#define LOCALE_ID_MAX		39
/*
 * The following functions are used to make allocations internal to the 
 * C++ library.These are only required on the emulator as these are 
 * static variables in the actual STLPort implementation but in order to
 * save them on the ewsd, they need to allocated on heap.
 */
IMPORT_C void* WSDAlloc(unsigned int size);
IMPORT_C int   WSDFree(void*);

// ******************************************************//
//  ***   all WSD definitions here
// ******************************************************//
class _Libcpp_wsd
{
public:
	//_locale.h
	size_t locale_id_S_max;
	
	//locale_impl.h
	std::locale *_Stl_classic_locale;	
	std::locale *_Stl_global_locale;
	
	void* __Loc_init_buf;
	
	std::_Refcount_Base *_Loc_init_S_count;
	
	//locale.cpp
	__stl_atomic_t _S_index;
	std::_STLP_STATIC_MUTEX locale_Index_lock;

	//_ios_base.h
	bool ios_base_S_was_synced;
	long ios_base_Init_S_count; 	//ios_base::Init class	
	std::ios_base::Init *_IosInit;
	
	//ios.cpp
	int ios_base_xalloc_S_index;
	std::_STLP_STATIC_MUTEX ios_base_xalloc_lock; 

	//locale_catalog.cpp
	Category_Map *_S_ctype_hash;
	Category_Map *_S_numeric_hash;
	Category_Map *_S_time_hash;
	Category_Map *_S_collate_hash;
	Category_Map *_S_monetary_hash;
	Category_Map *_S_messages_hash;
	
	std::_STLP_STATIC_MUTEX locale_catalog_category_hash_lock;

	//iostream.cpp
	std::istream  *wsd_cin;
	std::ostream  *wsd_cout;
	std::ostream  *wsd_cerr;
	std::ostream  *wsd_clog;
#ifndef _STLP_NO_WCHAR_T
	std::wistream *wsd_wcin;
	std::wostream *wsd_wcout;
	std::wostream *wsd_wcerr;
	std::wostream *wsd_wclog;
#endif

	//numpunct.cpp
	std::string numpunct_char_m_truename;
	std::string numpunct_char_m_falsename;
	std::string numpunct_char_m_grouping;
# ifndef _STLP_NO_WCHAR_T
	std::wstring numpunct_wchar_m_truename;
	std::wstring numpunct_wchar_m_falsename;
	std::string  numpunct_wchar_m_grouping;
# endif
	
	//monetary.cpp
	std::string monetary_S_empty_string;	
# ifndef _STLP_NO_WCHAR_T
	std::wstring monetary_S_empty_wstring;
# endif //_STLP_NO_WCHAR_T

	//_fstream.h
	size_t fstream_Filebuf_base_M_page_size;
	
	//allocators.cpp
	std::__oom_handler_type wsd__oom_handler;
	std::_STLP_STATIC_MUTEX wsd_allocator_S_lock;
	size_t wsd__node_alloc_impl_S_heap_size;
	char* wsd_S_start_free;
	char* wsd_S_end_free;
	void* wsd_S_free_list[16];	
	
	std::_STLP_STATIC_MUTEX wsd_pt_S_chunk_allocator_lock;
	char *wsd_pt_S_start_free;
	char *wsd_pt_S_end_free;
	size_t wsd_pt_S_heap_size;
	stlp_priv::_Pthread_alloc::__state_type *wsd_pt_S_free_per_thread_states;
	pthread_key_t wsd_pt_S_key;
	bool wsd_pt_S_key_initialized;
	
	//_threads.h
	std::_STLP_STATIC_MUTEX _threads_S_swap_lock;
	std::_STLP_STATIC_MUTEX _threads_0_S_swap_lock;
	unsigned _mutex_max;
	unsigned _mutex_last;
	
# ifndef _STLP_NO_MBSTATE_T
	std::locale::id codecvt_char_char_mbstate_id;
# ifndef _STLP_NO_WCHAR_T
	std::locale::id codecvt_wchar_char_mbstate_id;
# endif
# endif		//_STLP_NO_MBSTATE_T
	std::locale::id collate_char_id;

	std::locale::id ctype_char_id;

	std::locale::id moneypunct_char_true_id;

	std::locale::id moneypunct_char_false_id;

	std::locale::id messages_char_id;

	std::locale::id numpunct_char_id;

# ifndef _STLP_NO_WCHAR_T
	std::locale::id collate_wchar_id;

	std::locale::id ctype_wchar_id;

	std::locale::id moneypunct_wchar_true_id;

	std::locale::id moneypunct_wchar_false_id;

	std::locale::id numpunct_wchar_id;

	std::locale::id messages_wchar_id;

	std::locale::id num_get_wchar_istreambuf_iterator_id;

	std::locale::id num_get_wchar_wchar_const_id;

	std::locale::id num_put_wchar_ostreambuf_iterator_id;

	std::locale::id num_put_wchar_wchar_id;

	std::locale::id time_get_wchar_istreambuf_iterator_id;

	std::locale::id time_get_wchar_wchar_const_id;

	std::locale::id time_put_wchar_ostreambuf_iterator_id;

	std::locale::id time_put_wchar_wchar_id;

	std::locale::id money_get_wchar_istreambuf_iterator_id;

	std::locale::id money_get_wchar_wchar_const_id;

	std::locale::id money_put_wchar_ostreambuf_iterator_id;

	std::locale::id money_put_wchar_wchar_id;
#endif

	std::locale::id time_get_char_istreambuf_iterator_id;

	std::locale::id time_get_char_char_const_id;

	std::locale::id time_put_char_ostreambuf_iterator_id;

	std::locale::id time_put_char_char_id;

	std::locale::id num_get_char_istreambuf_iterator_id;

	std::locale::id num_get_char_char_const_id;

	std::locale::id num_put_char_ostreambuf_iterator_id;

	std::locale::id num_put_char_char_id;

	std::locale::id num_put_char_back_insert_iterator_id;

	std::locale::id money_get_char_istreambuf_iterator_id;

	std::locale::id money_get_char_char_const_id;

	std::locale::id money_put_char_ostreambuf_iterator_id;

	std::locale::id money_put_char_char_id;

	std::map<std::string, std::locale::id> ctype_charT_ids; 		//for ctype<charT>
	std::map<std::string, std::locale::id> numpunct_charT_ids; 	//for numpunct<charT>
	std::map<std::string, std::locale::id> moneypunct_charT_ids;	//for moneypunct<charT>
		
#ifdef _STLP_DEBUG
	std::string *_Nameless;
#endif	
	// ****** end of static var definition
	
    bool is_Initialized;
    
    //constructor
    _Libcpp_wsd();
    
    //destructor	
    ~_Libcpp_wsd();

};


//declaraions for wsd initialization functions
void locale_impl_init();
void locale_index_lock_init();
void monetary_empty_string_init();
void global_iostream_init();
void stdcpp_allocators_init();
void filebuf_page_size_init();

#ifdef _STLP_DEBUG
inline std::string*& get_Nameless();
#endif

namespace _STLP_PRIV_NAME {
void locale_catalog_category_hash_lock_init();
void Category_Map_Init();
_STLP_END_NAMESPACE


const TUid KLibstdcppUID = {0x20017610};

//cleanup all the WSD allocations
IMPORT_C void CleanupWSD();

//return the global class pointer
IMPORT_C _Libcpp_wsd& get_libcpp_wsd();

#ifdef _STLP_DEBUG
inline std::string*& get_Nameless()
	{
	return get_libcpp_wsd()._Nameless;
	}
#endif


// ******************************************************//

//locale wsd functions
#include "wsd_locales.h"

//streams wsd functions
#include "wsd_streams.h"

// ******************************************************//
#endif

#endif /*LIBSTDCPPWSD_H_*/
